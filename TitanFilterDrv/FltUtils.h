#pragma once
#include <ntifs.h>
#include <fltKernel.h>

PCHAR PsGetProcessImageFileName(PEPROCESS Process);

NTSTATUS VolumePathToNtPath(const PCFLT_RELATED_OBJECTS FltObjects, const PFLT_FILE_NAME_INFORMATION FltFileInfo, PUNICODE_STRING Path);

NTSTATUS GetProcesssNameByFltData(PFLT_CALLBACK_DATA Data, PUNICODE_STRING ProcessName);