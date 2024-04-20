#pragma once
#include <ntifs.h>
#include <fltKernel.h>

PCHAR PsGetProcessImageFileName(PEPROCESS Process);

NTSTATUS VolumePathToDosPath(const PFLT_VOLUME Volume, const PFLT_FILE_NAME_INFORMATION FileInfo, PUNICODE_STRING Output);

NTSTATUS GetProcesssNameByFltData(PFLT_CALLBACK_DATA Data, PUNICODE_STRING ProcessName);