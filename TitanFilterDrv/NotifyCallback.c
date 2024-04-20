#include "NotifyCallback.h"
#include "FltRule.h"
#include "FltRuleMgr.h"
#include "Globals.h"

NTSTATUS ConnectNotify(PFLT_PORT ClientPort, PVOID ServerPortCookie, PVOID ConnectionContext, 
	ULONG SizeOfContext, PVOID* ConnectionPortCookie)
{
	NTSTATUS status = STATUS_SUCCESS;
	
	return status;
}

VOID DisconnectNotify(PVOID ConnectionCookie) 
{
	
}

NTSTATUS MessageNotify(PVOID PortCookie, PVOID InputBuffer, ULONG InputBufferLength, 
	PVOID OutputBuffer, ULONG OutputBufferLength, PULONG ReturnOutputBufferLength)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (InputBuffer == NULL || InputBufferLength == 0 || InputBufferLength != sizeof(DATA_PACKET)) {
		return STATUS_INVALID_PARAMETER;
	}

	DATA_PACKET* packet = (DATA_PACKET*)InputBuffer;
	FLT_RULE_LIST* ruleList = GetRuleListInstance();
	
	switch (packet->Type)
	{
	case PACKET_START: 
	{
		InterlockedExchange(&g_isEnableFilter, TRUE);
		break;
	}
	case PACKET_STOP:
	{
		InterlockedExchange(&g_isEnableFilter, FALSE);
		break;
	}
	case PACKET_ADD_PATH:
	{
		FLT_RULE rule;
		rule.FilePathType = packet->FilePathType;
		rule.ProtectFlag = packet->ProtectFlag;
		
		memset(rule.ProcessName, 0, sizeof(rule.ProcessName));
		memset(rule.FilePath, 0, sizeof(rule.FilePath));

		memcpy(rule.ProcessName, packet->ProcessName, sizeof(rule.ProcessName));
		memcpy(rule.FilePath, packet->FilePath, sizeof(rule.FilePath));

		AddRuleToList(ruleList, &rule);

		break;
	}
	case PACKET_DEL_PATH:
	{
		DeleteRuleFromList(ruleList, packet->ProcessName, packet->FilePath);
		break;
	}

	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}
	

	return status;
}