#include "FltUtils.h"

NTSTATUS VolumePathToNtPath(const PCFLT_RELATED_OBJECTS FltObjects, const PFLT_FILE_NAME_INFORMATION FltFileInfo, PUNICODE_STRING Path)
{
	NTSTATUS status = STATUS_SUCCESS;

	UNICODE_STRING dosName = { 0 };
	wchar_t path_buf[260] = { 0 };
	PDEVICE_OBJECT pDiskDevObj = { NULL };

	RtlInitEmptyUnicodeString(&dosName, path_buf, sizeof(path_buf));

	status = FltGetDiskDeviceObject(FltObjects->Volume, &pDiskDevObj);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	__try {
		status = IoVolumeDeviceToDosName(pDiskDevObj, &dosName);
		if (!NT_SUCCESS(status)) {
			return status;
		}
	}
	__finally {
		ObDereferenceObject(pDiskDevObj);
	}

	size_t need_size = dosName.Length + FltFileInfo->ParentDir.Length + FltFileInfo->FinalComponent.Length;

	if (Path->MaximumLength < need_size) {
		ExFreePool(dosName.Buffer);
		return STATUS_BUFFER_TOO_SMALL;
	}

	RtlAppendUnicodeStringToString(Path, &dosName);
	RtlAppendUnicodeStringToString(Path, &FltFileInfo->ParentDir);
	RtlAppendUnicodeStringToString(Path, &FltFileInfo->FinalComponent);

	ExFreePool(dosName.Buffer);

	return status;
}

NTSTATUS GetProcesssNameByFltData(PFLT_CALLBACK_DATA Data, PUNICODE_STRING ProcessName) {

	PEPROCESS pProcess = NULL;
	pProcess = FltGetRequestorProcess(Data);
	if (!pProcess) {
		return STATUS_INVALID_PARAMETER;
	}

	PCHAR processName = NULL;
	processName = PsGetProcessImageFileName(pProcess);
	if (!processName) {
		return STATUS_INVALID_PARAMETER;
	}

	ANSI_STRING processNameAnsi = { 0 };
	RtlInitAnsiString(&processNameAnsi, processName);

	size_t needLen = strlen(processName) << 1;
	if (ProcessName->MaximumLength < needLen) {
		return STATUS_BUFFER_TOO_SMALL;
	}

	return RtlAnsiStringToUnicodeString(ProcessName, &processNameAnsi, FALSE);
}