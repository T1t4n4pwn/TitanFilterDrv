#include "FltUtils.h"

NTSTATUS VolumePathToDosPath(const PFLT_VOLUME Volume, const PFLT_FILE_NAME_INFORMATION FileInfo, PUNICODE_STRING Output)
{
	UNICODE_STRING dosName = { 0 };
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT diskDeviceObject = NULL;

	__try {
		status = FltGetDiskDeviceObject(Volume, &diskDeviceObject);
		if (!NT_SUCCESS(status)) {
			return status;
		}

		if (!diskDeviceObject->AttachedDevice) {
			status =  STATUS_INVALID_DEVICE_OBJECT_PARAMETER;
			return status;
		}

		status = IoVolumeDeviceToDosName(diskDeviceObject->AttachedDevice, &dosName);
		if (!NT_SUCCESS(status)) {
			return status;
		}

		size_t remain = FileInfo->Name.Length - FileInfo->Volume.Length;
		size_t needSize = remain + dosName.Length;

		if (Output->MaximumLength < needSize) {
			status =  STATUS_BUFFER_TOO_SMALL;
			return status;
		}

		wchar_t* target = strstr(FileInfo->Name.Buffer, FileInfo->Volume.Buffer);
		if (!target) {
			status =  STATUS_INVALID_PARAMETER_2;
			return status;
		}

		RtlCopyMemory(Output->Buffer, dosName.Buffer, dosName.Length);
		RtlCopyMemory((char*)Output->Buffer + dosName.Length, (char*)target + FileInfo->Volume.Length, remain);

		Output->Length = needSize;

	}
	__finally {
		if (dosName.Buffer) {
			ExFreePool(dosName.Buffer);
		}
		if (diskDeviceObject) {
			ObDereferenceObject(diskDeviceObject);
		}
	}

	return status;
}

NTSTATUS GetProcesssNameByFltData(PFLT_CALLBACK_DATA Data, PUNICODE_STRING ProcessName) {

	PEPROCESS pProcess = NULL;
	pProcess = FltGetRequestorProcess(Data);
	if (!pProcess) {
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}

	PCHAR processName = NULL;
	processName = PsGetProcessImageFileName(pProcess);
	if (!processName) {
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}

	ANSI_STRING processNameAnsi = { 0 };
	RtlInitAnsiString(&processNameAnsi, processName);

	size_t needLen = strlen(processName) << 1;
	if (ProcessName->MaximumLength < needLen) {
		return STATUS_BUFFER_TOO_SMALL;
	}

	return RtlAnsiStringToUnicodeString(ProcessName, &processNameAnsi, FALSE);
}