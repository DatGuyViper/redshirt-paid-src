#pragma once
#include "defines.h"
#include "memory.h"

INT32 get_winver() {
    RTL_OSVERSIONINFOW ver = { 0 };
    RtlGetVersion(&ver);
    switch (ver.dwBuildNumber)
    {
    case WIN_1803:
        return 0x0278;
    case WIN_1809:
        return 0x0278;
    case WIN_1903:
        return 0x0280;
    case WIN_1909:
        return 0x0280;
    case WIN_2004:
        return 0x0388;
    case WIN_20H2:
        return 0x0388;
    case WIN_21H1:
        return 0x0388;
    case WIN_22H2:
        return 0x0388;
    default:
        return 0x0388;
    }
}

NTSTATUS io_controller(PDEVICE_OBJECT device_obj, PIRP irp) {
    SPOOF_FUNC;
    UNREFERENCED_PARAMETER(device_obj);

    NTSTATUS status = { };
    ULONG bytes = { };
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

    ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;
    ULONG size = stack->Parameters.DeviceIoControl.InputBufferLength;

    if (code == RDWCode) {
        if (size == sizeof(RD)) {
            RDW req = (RDW)(irp->AssociatedIrp.SystemBuffer);

            status = PSD(req);
            bytes = sizeof(RD);
        }
        else
        {
            status = STATUS_INFO_LENGTH_MISMATCH;
            bytes = 0;
        }
    }
    else if (code == SHACode) {
        if (size == sizeof(SH)) {
            SHA req = (SHA)(irp->AssociatedIrp.SystemBuffer);

            status = BSD(req);
            bytes = sizeof(SH);
        }
        else
        {
            status = STATUS_INFO_LENGTH_MISMATCH;
            bytes = 0;
        }
    }
    else if (code == FGACode) {
        if (size == sizeof(FA)) {
            FGA req = (FGA)(irp->AssociatedIrp.SystemBuffer);

            status = guardedregion(req);
            bytes = sizeof(FA);
        }
        else
        {
            status = STATUS_INFO_LENGTH_MISMATCH;
            bytes = 0;
        }
    }
    else if (code == CR3Code) {
        PMEMORY_OPERATION_DATA req = (PMEMORY_OPERATION_DATA)(irp->AssociatedIrp.SystemBuffer);

        status = findcr3(req);
    }

    irp->IoStatus.Status = status;
    irp->IoStatus.Information = bytes;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS unsupported_dispatch(PDEVICE_OBJECT device_obj, PIRP irp) {
    UNREFERENCED_PARAMETER(device_obj);

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return irp->IoStatus.Status;
}

NTSTATUS dispatch_handler(PDEVICE_OBJECT device_obj, PIRP irp) {
    UNREFERENCED_PARAMETER(device_obj);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

    switch (stack->MajorFunction) {
    case IRP_MJ_CREATE:
        break;
    case IRP_MJ_CLOSE:
        break;
    default:
        break;
    }

    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return irp->IoStatus.Status;
}

void unload_drv(PDRIVER_OBJECT drv_obj) {
    NTSTATUS status = { };

    status = IoDeleteSymbolicLink(&link);

    if (!NT_SUCCESS(status))
        return;

    IoDeleteDevice(drv_obj->DeviceObject);
}

NTSTATUS NTAPI IopInvalidDeviceRequest(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_INVALID_DEVICE_REQUEST;
}

void HideDriver(PDRIVER_OBJECT DriverObject) {
    PLDR_DATA_TABLE_ENTRY entry = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
    if (!entry) {
        return;
    }
    RemoveEntryList(&entry->InLoadOrderLinks);
    RemoveEntryList(&entry->InMemoryOrderLinks);
    RemoveEntryList(&entry->InInitializationOrderLinks);
    InitializeListHead(&entry->InLoadOrderLinks);
    InitializeListHead(&entry->InMemoryOrderLinks);
    InitializeListHead(&entry->InInitializationOrderLinks);
}

NTSTATUS initialize_driver(_In_ PDRIVER_OBJECT drv_obj, _In_ PUNICODE_STRING path) {
    UNREFERENCED_PARAMETER(path);

    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_OBJECT device_obj = NULL;

    UNICODE_STRING name, link;
    RtlInitUnicodeString(&name, L"\\Device\\securecomms");
    RtlInitUnicodeString(&link, L"\\DosDevices\\securecomms");

    status = IoCreateDevice(drv_obj, 0, &name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_obj);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    HideDriver(drv_obj); // hide from psloadedmodulelist

    InitializeIoDriverObjectType();

    status = LoadDriverIntoSignedMemory(drv_obj);
    if (!NT_SUCCESS(status)) {
        IoDeleteSymbolicLink(&link);
        IoDeleteDevice(device_obj);
        return status;
    }

    status = IoCreateSymbolicLink(&link, &name);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(device_obj);
        return status;
    }

    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        drv_obj->MajorFunction[i] = IopInvalidDeviceRequest;
    }

    drv_obj->MajorFunction[IRP_MJ_CREATE] = dispatch_handler;
    drv_obj->MajorFunction[IRP_MJ_CLOSE] = dispatch_handler;
    drv_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = io_controller;
    drv_obj->DriverUnload = unload_drv;

    device_obj->Flags |= DO_BUFFERED_IO;
    device_obj->Flags &= ~DO_DEVICE_INITIALIZING;

    status = load_dynamic_functions();
    if (!NT_SUCCESS(status)) {
        IoDeleteSymbolicLink(&link);
        IoDeleteDevice(device_obj);
        return status;
    }

    drv_obj->DriverSection = NULL; // avoid etw logging

    return status;
}