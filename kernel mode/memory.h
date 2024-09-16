#pragma once
#include "defines.h"

NTSTATUS read(PVOID target_address, PVOID buffer, SIZE_T size, SIZE_T* bytes_read) {
    SPOOF_FUNC;
    MM_COPY_ADDRESS to_read = { 0 };
    to_read.PhysicalAddress.QuadPart = (LONGLONG)target_address;

    SIZE_T bytes_copied = 0;
    PVOID result = DynamicMmCopyMemory(buffer, to_read, size, MM_COPY_MEMORY_PHYSICAL, &bytes_copied);

    if (bytes_read) {
        *bytes_read = bytes_copied;
    }

    return (result != NULL && bytes_copied == size) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

NTSTATUS write(PVOID target_address, PVOID buffer, SIZE_T size, SIZE_T* bytes_read) {
    SPOOF_FUNC;
    if (!target_address)
        return STATUS_UNSUCCESSFUL;

    PHYSICAL_ADDRESS AddrToWrite = { 0 };
    AddrToWrite.QuadPart = LONGLONG(target_address);

    PVOID pmapped_mem = DynamicMmMapIoSpaceEx(AddrToWrite, size, PAGE_READWRITE);

    if (!pmapped_mem)
        return STATUS_UNSUCCESSFUL;

    memcpy(pmapped_mem, buffer, size);

    *bytes_read = size;
    DynamicMmUnmapIoSpace(pmapped_mem, size);
    return STATUS_SUCCESS;
}

volatile uint64_t g_MmPfnDatabase = 0;
volatile uint64_t g_PXE_BASE = 0;
volatile uint64_t g_idx = 0;
uintptr_t dirBase = 0;

void initDefinesCR3() {
    KDDEBUGGER_DATA64 kdBlock = { 0 };
    CONTEXT context = { 0 };
    context.ContextFlags = CONTEXT_FULL;
    (RtlCaptureContext)(&context);

    PDUMP_HEADER dumpHeader = (PDUMP_HEADER)ExAllocatePool(NonPagedPool, DUMP_BLOCK_SIZE);
    if (dumpHeader) {
        (KeCapturePersistentThreadState)(&context, NULL, 0, 0, 0, 0, 0, dumpHeader);
        RtlCopyMemory(&kdBlock, (PUCHAR)dumpHeader + KDDEBUGGER_DATA_OFFSET, sizeof(kdBlock));

        ExFreePool(dumpHeader);

        g_MmPfnDatabase = *(ULONG64*)(kdBlock.MmPfnDatabase);

        // Calculate base addresses for page table entries
        ULONG64 g_PTE_BASE = kdBlock.PteBase;
        ULONG64 g_PDE_BASE = g_PTE_BASE + ((g_PTE_BASE & 0xffffffffffff) >> 9);
        ULONG64 g_PPE_BASE = g_PTE_BASE + ((g_PDE_BASE & 0xffffffffffff) >> 9);
        g_PXE_BASE = g_PTE_BASE + ((g_PPE_BASE & 0xffffffffffff) >> 9);
        g_idx = (g_PTE_BASE >> 39) - 0x1FFFE00;
    }
}

uintptr_t get_kernel_base() {
    const auto idtbase = *reinterpret_cast<uint64_t*>(__readgsqword(0x18) + 0x38);
    const auto descriptor_0 = *reinterpret_cast<uint64_t*>(idtbase);
    const auto descriptor_1 = *reinterpret_cast<uint64_t*>(idtbase + 8);
    const auto isr_base = ((descriptor_0 >> 32) & 0xFFFF0000) + (descriptor_0 & 0xFFFF) + (descriptor_1 << 32);
    auto align_base = isr_base & 0xFFFFFFFFFFFFF000;

    for (; ; align_base -= 0x1000) {
        for (auto* search_base = reinterpret_cast<uint8_t*>(align_base); search_base < reinterpret_cast<uint8_t*>(align_base) + 0xFF9; search_base++) {
            if (search_base[0] == 0x48 &&
                search_base[1] == 0x8D &&
                search_base[2] == 0x1D &&
                search_base[6] == 0xFF) {
                const auto relative_offset = *reinterpret_cast<int*>(&search_base[3]);
                const auto address = reinterpret_cast<uint64_t>(search_base + relative_offset + 7);
                if ((address & 0xFFF) == 0) {
                    if (*reinterpret_cast<uint16_t*>(address) == 0x5A4D) {
                        return address;
                    }
                }
            }
        }
    }
}

intptr_t search_pattern(void* module_handle, const char* section, const char* signature_value) {
    static auto in_range = [](auto x, auto a, auto b) { return (x >= a && x <= b); };
    static auto get_bits = [](auto  x) { return (in_range((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (in_range(x, '0', '9') ? x - '0' : 0)); };
    static auto get_byte = [](auto  x) { return (get_bits(x[0]) << 4 | get_bits(x[1])); };

    const auto dos_headers = reinterpret_cast<PIMAGE_DOS_HEADER>(module_handle);
    const auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(module_handle) + dos_headers->e_lfanew);
    const auto section_headers = reinterpret_cast<PIMAGE_SECTION_HEADER>(nt_headers + 1);

    auto range_start = 0ui64;
    auto range_end = 0ui64;
    for (auto cur_section = section_headers; cur_section < section_headers + nt_headers->FileHeader.NumberOfSections; cur_section++) {
        if (strcmp(reinterpret_cast<const char*>(cur_section->Name), section) == 0) {
            range_start = reinterpret_cast<uintptr_t>(module_handle) + cur_section->VirtualAddress;
            range_end = range_start + cur_section->Misc.VirtualSize;
        }
    }

    if (range_start == 0)
        return 0u;

    auto first_match = 0ui64;
    auto pat = signature_value;
    for (uintptr_t cur = range_start; cur < range_end; cur++) {
        if (*pat == '\0') {
            return first_match;
        }
        if (*(uint8_t*)pat == '\?' || *reinterpret_cast<uint8_t*>(cur) == get_byte(pat)) {
            if (!first_match)
                first_match = cur;

            if (!pat[2])
                return first_match;

            if (*(uint16_t*)pat == 16191 || *(uint8_t*)pat != '\?') {
                pat += 3;
            }
            else {
                pat += 2;
            }
        }
        else {
            pat = signature_value;
            first_match = 0;
        }
    }
    return 0u;
}

#pragma warning(push)
#pragma warning(disable:4201)

typedef union {
    struct {
        uint64_t reserved1 : 3;
        uint64_t page_level_write_through : 1;
        uint64_t page_level_cache_disable : 1;
        uint64_t reserved2 : 7;
        uint64_t address_of_page_directory : 36;
        uint64_t reserved3 : 16;
    };
    uint64_t flags;
} cr3;
static_assert(sizeof(cr3) == 0x8);

typedef union {
    struct {
        uint64_t present : 1;
        uint64_t write : 1;
        uint64_t supervisor : 1;
        uint64_t page_level_write_through : 1;
        uint64_t page_level_cache_disable : 1;
        uint64_t accessed : 1;
        uint64_t dirty : 1;
        uint64_t large_page : 1;
        uint64_t global : 1;
        uint64_t ignored_1 : 2;
        uint64_t restart : 1;
        uint64_t page_frame_number : 36;
        uint64_t reserved1 : 4;
        uint64_t ignored_2 : 7;
        uint64_t protection_key : 4;
        uint64_t execute_disable : 1;
    };

    uint64_t flags;
} pt_entry_64;
static_assert(sizeof(pt_entry_64) == 0x8);
#pragma warning(pop)

static uint64_t pte_base = 0;
static uint64_t pde_base = 0;
static uint64_t ppe_base = 0;
static uint64_t pxe_base = 0;
static uint64_t self_mapidx = 0;
static uint64_t mm_pfn_database = 0;

uint64_t get_dirbase() {
    return __readcr3() & 0xFFFFFFFFFFFFF000;
}

void* phys_to_virt(uint64_t phys) {
    PHYSICAL_ADDRESS phys_addr = { .QuadPart = (int64_t)(phys) };
    return reinterpret_cast<void*>(MmGetVirtualForPhysical(phys_addr));
}

void init_pte_base() {
    cr3 system_cr3 = { .flags = get_dirbase() };
    uint64_t dirbase_phys = system_cr3.address_of_page_directory << 12;
    pt_entry_64* pt_entry = reinterpret_cast<pt_entry_64*>(phys_to_virt(dirbase_phys));
    for (uint64_t idx = 0; idx < 0x200; idx++) {
        if (pt_entry[idx].page_frame_number == system_cr3.address_of_page_directory) {
            pte_base = (idx + 0x1FFFE00ui64) << 39ui64;
            pde_base = (idx << 30ui64) + pte_base;
            ppe_base = (idx << 30ui64) + pte_base + (idx << 21ui64);
            pxe_base = (idx << 12ui64) + ppe_base;
            self_mapidx = idx;
            break;
        }
    }
}

uintptr_t init_mmpfn_database() {
    auto search = search_pattern(reinterpret_cast<void*>(get_kernel_base()), ".text", "B9 ? ? ? ? 48 8B 05 ? ? ? ? 48 89 43 18") + 5;
    auto resolved_base = search + *reinterpret_cast<int32_t*>(search + 3) + 7;
    mm_pfn_database = *reinterpret_cast<uintptr_t*>(resolved_base);
    return mm_pfn_database;
}

UINT64 findcr3(PMEMORY_OPERATION_DATA x) {
    if (!pte_base) init_pte_base();
    if (!mm_pfn_database) init_mmpfn_database();
    auto mem_range_count = 0;
    auto mem_range = MmGetPhysicalMemoryRanges();

    auto cr3_ptebase = self_mapidx * 8 + pxe_base;

    for (mem_range_count = 0; mem_range_count < 200; ++mem_range_count)
    {
        if (mem_range[mem_range_count].BaseAddress.QuadPart == 0 && mem_range[mem_range_count].NumberOfBytes.QuadPart == 0)
            break;

        auto start_pfn = mem_range[mem_range_count].BaseAddress.QuadPart >> 12;
        auto end_pfn = start_pfn + (mem_range[mem_range_count].NumberOfBytes.QuadPart >> 12);

        for (auto i = start_pfn; i < end_pfn; ++i)
        {
            auto cur_mmpfn = reinterpret_cast<_MMPFN*>(mm_pfn_database + 0x30 * i);

            if (cur_mmpfn->flags)
            {
                if (cur_mmpfn->flags == 1)
                    continue;

                if (cur_mmpfn->pte_address != cr3_ptebase)
                    continue;

                auto decrypted_eprocess = ((cur_mmpfn->flags | 0xF000000000000000) >> 0xd) | 0xFFFF000000000000;
                dirBase = i << 12;
                PEPROCESS process;
                PsLookupProcessByProcessId((HANDLE)x->pid, &process);

                if (MmIsAddressValid(reinterpret_cast<void*>(decrypted_eprocess)) && reinterpret_cast<PEPROCESS>(decrypted_eprocess) == process)
                {
                    if (dirBase) {
                        RtlCopyMemory((void*)x->cr3, &dirBase, sizeof(dirBase));
                        return STATUS_SUCCESS;
                    }

                    break;
                }
            }
        }
    }
}

UINT64 translate_linear(UINT64 directoryTableBase, UINT64 virtualAddress) {
    directoryTableBase &= ~0xf;

    UINT64 pageOffset = virtualAddress & ~(~0ul << PAGE_OFFSET_SIZE);
    UINT64 pte = ((virtualAddress >> 12) & (0x1ffll));
    UINT64 pt = ((virtualAddress >> 21) & (0x1ffll));
    UINT64 pd = ((virtualAddress >> 30) & (0x1ffll));
    UINT64 pdp = ((virtualAddress >> 39) & (0x1ffll));

    SIZE_T readsize = 0;
    UINT64 pdpe = 0;
    read(PVOID(directoryTableBase + 8 * pdp), &pdpe, sizeof(pdpe), &readsize);
    if (~pdpe & 1)
        return 0;

    UINT64 pde = 0;
    read(PVOID((pdpe & PMASK) + 8 * pd), &pde, sizeof(pde), &readsize);
    if (~pde & 1)
        return 0;

    if (pde & 0x80)
        return (pde & (~0ull << 42 >> 12)) + (virtualAddress & ~(~0ull << 30));

    UINT64 pteAddr = 0;
    read(PVOID((pde & PMASK) + 8 * pt), &pteAddr, sizeof(pteAddr), &readsize);
    if (~pteAddr & 1)
        return 0;

    if (pteAddr & 0x80)
        return (pteAddr & PMASK) + (virtualAddress & ~(~0ull << 21));

    virtualAddress = 0;
    read(PVOID((pteAddr & PMASK) + 8 * pte), &virtualAddress, sizeof(virtualAddress), &readsize);
    virtualAddress &= PMASK;

    if (!virtualAddress)
        return 0;

    return virtualAddress + pageOffset;
}

ULONG64 find_min(INT32 g, SIZE_T f) {
    INT32 h = (INT32)f;
    ULONG64 result = 0;

    result = (((g) < (h)) ? (g) : (h));

    return result;
}

NTSTATUS PSD(RDW x) {
    if (x->security != SecurityCode)
        return STATUS_UNSUCCESSFUL;

    if (!x->process_id)
        return STATUS_UNSUCCESSFUL;

    PEPROCESS process = NULL;
    PsLookupProcessByProcessId((HANDLE)x->process_id, &process);
    if (!process)
        return STATUS_UNSUCCESSFUL;

    ULONGLONG process_base = dirBase;
    if (!process_base)
        return STATUS_UNSUCCESSFUL;
    ObDereferenceObject(process);

    SIZE_T this_offset = NULL;
    SIZE_T total_size = x->size;

    INT64 physical_address = translate_linear(process_base, (ULONG64)x->address + this_offset);
    if (!physical_address)
        return STATUS_UNSUCCESSFUL;

    ULONG64 final_size = find_min(PAGE_SIZE - (physical_address & 0xFFF), total_size);
    SIZE_T bytes_trough = NULL;

    if (x->write) {
        write(PVOID(physical_address), (PVOID)((ULONG64)x->buffer + this_offset), final_size, &bytes_trough);
    }
    else {
        read(PVOID(physical_address), (PVOID)((ULONG64)x->buffer + this_offset), final_size, &bytes_trough);
    }

    return STATUS_SUCCESS;
}

NTSTATUS BSD(SHA x) {
    ULONGLONG image_base = NULL;
    if (x->security != SecurityCode)
        return STATUS_UNSUCCESSFUL;

    if (!x->process_id)
        return STATUS_UNSUCCESSFUL;

    PEPROCESS process = NULL;
    if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)x->process_id, &process))) {
        return STATUS_UNSUCCESSFUL;
    }

    image_base = (ULONGLONG)PsGetProcessSectionBaseAddress(process);

    if (!image_base) {
        ObDereferenceObject(process);
        return STATUS_UNSUCCESSFUL;
    }

    RtlCopyMemory(x->address, &image_base, sizeof(image_base));
    ObDereferenceObject(process);

    return STATUS_SUCCESS;
}

NTSTATUS guardedregion(FGA x) {
    if (x->security != SecurityCode)
        return STATUS_UNSUCCESSFUL;

    ULONG infoLen = 0;
    NTSTATUS status = ZwQuerySystemInformation(SystemBigPoolInformation, &infoLen, 0, &infoLen);
    PSYSTEM_BIGPOOL_INFORMATION pPoolInfo = 0;

    while (status == STATUS_INFO_LENGTH_MISMATCH)
    {
        if (pPoolInfo)
            ExFreePool(pPoolInfo);

        pPoolInfo = (PSYSTEM_BIGPOOL_INFORMATION)ExAllocatePool(NonPagedPool, infoLen);
        status = ZwQuerySystemInformation(SystemBigPoolInformation, pPoolInfo, infoLen, &infoLen);
    }

    if (pPoolInfo)
    {
        for (unsigned int i = 0; i < pPoolInfo->Count; i++)
        {
            SYSTEM_BIGPOOL_ENTRY* Entry = &pPoolInfo->AllocatedInfo[i];
            PVOID VirtualAddress;
            VirtualAddress = (PVOID)((uintptr_t)Entry->VirtualAddress & ~1ull);
            SIZE_T SizeInBytes = Entry->SizeInBytes;
            BOOLEAN NonPaged = Entry->NonPaged;

            if (Entry->NonPaged && Entry->SizeInBytes == 0x200000) {
                UCHAR expectedTag[] = "TnoC";  // Tag should be a string, not a ulong
                if (memcmp(Entry->Tag, expectedTag, sizeof(expectedTag)) == 0) {
                    RtlCopyMemory((void*)x->address, &Entry->VirtualAddress, sizeof(Entry->VirtualAddress));
                    return STATUS_SUCCESS;
                }
            }

        }

        ExFreePool(pPoolInfo);
    }

    return STATUS_SUCCESS;
}

void InitializeIoDriverObjectType() {
    UNICODE_STRING driverTypeName;
    RtlInitUnicodeString(&driverTypeName, L"Driver");
    ObReferenceObjectByName(&driverTypeName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, (PVOID*)&IoDriverObjectType);
}

NTSTATUS LoadDriverIntoSignedMemory(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING signedDriverName;
    RtlInitUnicodeString(&signedDriverName, L"\\Driver\\ACPI"); // can be changed

    PDRIVER_OBJECT signedDriverObject;
    NTSTATUS status = ObReferenceObjectByName(&signedDriverName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, (PVOID*)&signedDriverObject);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (signedDriverObject->DriverSection) {
        DriverObject->DriverSection = signedDriverObject->DriverSection;
    }
    else {
        ObDereferenceObject(signedDriverObject);
        return STATUS_UNSUCCESSFUL;
    }

    ObDereferenceObject(signedDriverObject);
    return STATUS_SUCCESS;
}