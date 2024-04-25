#include "FltPreCallbacks.h"

#include "FltUtils.h"

#include "DrvLog.h"

#include "FltRuleMgr.h"
#include "Globals.h"

FLT_PREOP_CALLBACK_STATUS FltPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, 
	PVOID* CompletionContext)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFLT_FILE_NAME_INFORMATION pFileNameInfo = { 0 };

	if (!g_isEnableFilter) {
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}

	//获取文件相关信息
	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &pFileNameInfo);
	if (!NT_SUCCESS(status)) {
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	UNICODE_STRING processName = { 0 };
	wchar_t processNameBuffer[260] = { 0 };
	
	RtlInitEmptyUnicodeString(&processName, processNameBuffer, sizeof(processNameBuffer));
	
	__try {

		status = GetProcesssNameByFltData(Data, &processName);
		if (!NT_SUCCESS(status)) {
			return FLT_PREOP_SUCCESS_WITH_CALLBACK;
		}

		status = FltParseFileNameInformation(pFileNameInfo);
		if (!NT_SUCCESS(status)) {
			return FLT_PREOP_SUCCESS_WITH_CALLBACK;
		}

		UNICODE_STRING filePath = { 0 };
		wchar_t filePathBuffer[260] = { 0 };

		RtlInitEmptyUnicodeString(&filePath, filePathBuffer, sizeof(filePathBuffer));

		status = VolumePathToDosPath(FltObjects->Volume, pFileNameInfo, &filePath);
		if (!NT_SUCCESS(status)) {
			return FLT_PREOP_SUCCESS_WITH_CALLBACK;
		}

		PFLT_RULE_LIST ruleList = GetRuleListInstance();
		PFLT_RULE pRule = NULL;
		if (!IsRuleExistFromList(ruleList, processName.Buffer, filePath.Buffer, &pRule)) {
			return FLT_PREOP_SUCCESS_WITH_CALLBACK;
		}

		if (pRule->ProtectFlag & FLT_RULE_FILE_DENY_ACESS) {
			Data->IoStatus.Status = STATUS_ACCESS_DENIED;
			Data->IoStatus.Information = 0;
			return FLT_PREOP_COMPLETE;
		}
		
	}
	__finally {
		FltReleaseFileNameInformation(pFileNameInfo);
	}

	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS FltPreDirectoryControl(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, 
	PVOID* CompletionContext)
{
	
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}