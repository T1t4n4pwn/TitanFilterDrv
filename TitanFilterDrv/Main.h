#pragma once
#include <ntifs.h>
#include <fltKernel.h>

#define DRIVER_MEM_TAG 'T1T4'

NTSTATUS FltUnload(FLT_FILTER_UNLOAD_FLAGS Flags);