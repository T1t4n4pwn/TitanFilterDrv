#include "Main.h"
#include "Globals.h"
#include "DrvLog.h"

#include "NotifyCallback.h"
#include "FltPreCallbacks.h"
#include "FltPostCallBacks.h"

#include "FltRuleMgr.h"

FLT_OPERATION_REGISTRATION FltCallbacks[] = {
	{IRP_MJ_CREATE, 0, FltPreCreate, FltPostCreate},
	{IRP_MJ_DIRECTORY_CONTROL, 0, FltPreDirectoryControl, FltPostDirectoryControl },
	{IRP_MJ_OPERATION_END}
};

const FLT_REGISTRATION FltRegistration = {
	sizeof(FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	0,
	NULL,
	FltCallbacks,
	FltUnload,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) 
{
	NTSTATUS status = STATUS_SUCCESS;

	__try {
		status = FltRegisterFilter(DriverObject, &FltRegistration, &g_pFilter);
		if (!NT_SUCCESS(status)) {
			ErrorLog("FltRegisterFilter() Failed! %p\n", status);
			return status;
		}

		status = FltStartFiltering(g_pFilter);
		if (!NT_SUCCESS(status)) {
			ErrorLog("FltStartFiltering() Failed!\n");
			return status;
		}

		PSECURITY_DESCRIPTOR pSD;
		OBJECT_ATTRIBUTES objAttri = { 0 };
		UNICODE_STRING uniPortName = { 0 };

		status = FltBuildDefaultSecurityDescriptor(&pSD, FLT_PORT_ALL_ACCESS);
		if (!NT_SUCCESS(status)) {
			return status;
		}

		RtlInitUnicodeString(&uniPortName, L"\\TitanCommunicationPort");

		InitializeObjectAttributes(&objAttri, &uniPortName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, pSD);

		status = FltCreateCommunicationPort(g_pFilter, &g_pServerPort, &objAttri, NULL, ConnectNotify, DisconnectNotify, MessageNotify, 1);
		FltFreeSecurityDescriptor(pSD);
		if (!NT_SUCCESS(status)) {
			return status;
		}
	}
	__finally {
		if (!NT_SUCCESS(status)) {
			if (g_pServerPort != NULL) {
				FltCloseCommunicationPort(g_pServerPort);
			}

			if (g_pFilter != NULL) {
				FltUnregisterFilter(g_pFilter);
			}
		}
	}

	return status;
}

NTSTATUS FltUnload(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	NTSTATUS status = STATUS_SUCCESS;

	CleanAllRule(GetRuleListInstance());

	FltCloseCommunicationPort(g_pServerPort);

	FltUnregisterFilter(g_pFilter);

	ErrorLog("FltUnload()!");

	return status;
}
