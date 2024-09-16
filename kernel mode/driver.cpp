#include "defines.h"
#include "controller.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(DriverObject);

    return IoCreateDriver(NULL, initialize_driver);
}