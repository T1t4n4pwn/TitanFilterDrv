#include "FltPostCallBacks.h"
#include "FltUtils.h"
#include "Globals.h"

#include "FltRuleMgr.h"

FLT_POSTOP_CALLBACK_STATUS FltPostCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, 
	PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	
	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_POSTOP_CALLBACK_STATUS FltPostDirectoryControl(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, 
	PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	PFLT_FILE_NAME_INFORMATION pFileNameInfo = { 0 };

	if (!g_isEnableFilter) {
		return FLT_POSTOP_FINISHED_PROCESSING;
	}

	//获取文件相关信息
	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &pFileNameInfo);
	if (!NT_SUCCESS(status)) {
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
	UNICODE_STRING processName = { 0 };
	wchar_t processNameBuffer[260] = { 0 };

	RtlInitEmptyUnicodeString(&processName, processNameBuffer, sizeof(processNameBuffer));

	status = GetProcesssNameByFltData(Data, &processName);
	if (!NT_SUCCESS(status)) {
		return FLT_POSTOP_FINISHED_PROCESSING;
	}

	__try {

		status = FltParseFileNameInformation(pFileNameInfo);
		if (!NT_SUCCESS(status)) {
			return FLT_POSTOP_FINISHED_PROCESSING;
		}

		UNICODE_STRING filePath = { 0 };
		wchar_t filePathBuffer[260] = { 0 };

		RtlInitEmptyUnicodeString(&filePath, filePathBuffer, sizeof(filePathBuffer));

		status = VolumePathToNtPath(FltObjects, pFileNameInfo, &filePath);
		if (!NT_SUCCESS(status)) {
			return FLT_POSTOP_FINISHED_PROCESSING;
		}

		PFLT_RULE_LIST ruleList = GetRuleListInstance();
		PFLT_RULE pRule = NULL;
		if (!IsRuleExistFromList(ruleList, processName.Buffer, filePath.Buffer, &pRule)) {
			return FLT_POSTOP_FINISHED_PROCESSING;
		}

		if (pRule->ProtectFlag & FLT_RULE_FILE_HIDE) {
			Data->IoStatus.Status = STATUS_NO_MORE_FILES;
			Data->IoStatus.Information = 0;
			return FLT_POSTOP_FINISHED_PROCESSING;
		}

	}
	__finally {
		FltReleaseFileNameInformation(pFileNameInfo);
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}
