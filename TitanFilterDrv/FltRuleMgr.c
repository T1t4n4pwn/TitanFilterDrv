#include "FltRuleMgr.h"

static void InitRuleList(PFLT_RULE_LIST Self)
{
	InitializeListHead(&Self->HeadList);
	KeInitializeSpinLock(&Self->SpinLock);
}

PFLT_RULE_LIST GetRuleListInstance()
{
	static PFLT_RULE_LIST pRuleList = NULL;

	if (!pRuleList) {
		pRuleList = ExAllocatePoolWithTag(NonPagedPool, sizeof(FLT_RULE_LIST), DRIVER_MEM_TAG);
		InitRuleList(pRuleList);
	}
	
	return pRuleList;
}

void AddRuleToList(PFLT_RULE_LIST Self, PFLT_RULE Rule)
{
	if (!Self || !Rule) {
		return;
	}

	PFLT_RULE pRule = NULL;
	if (IsRuleExistFromList(Self, Rule->ProcessName, Rule->FilePath, &pRule)) {
		return;
	}

	PFLT_RULE pRuleMem = (PFLT_RULE)ExAllocatePoolWithTag(NonPagedPool, sizeof(FLT_RULE), DRIVER_MEM_TAG);

	memset(pRuleMem, 0, sizeof(pRuleMem));

	pRuleMem->ProtectFlag = Rule->ProtectFlag;
	memcpy(pRuleMem->ProcessName, Rule->ProcessName, sizeof(pRuleMem->ProcessName));
	memcpy(pRuleMem->FilePath, Rule->FilePath, sizeof(pRuleMem->FilePath));

	KIRQL kIrql;
	KeAcquireSpinLock(&Self->SpinLock, &kIrql);
	InsertTailList(&Self->HeadList, &pRuleMem->List);
	KeReleaseSpinLock(&Self->SpinLock, kIrql);

}

void DeleteRuleFromList(PFLT_RULE_LIST Self, const wchar_t* ProcessName, const wchar_t* FilePath)
{
	if (!Self || !ProcessName || !FilePath) {
		return;
	}

	PLIST_ENTRY pList = Self->HeadList.Flink;
	while (pList != &Self->HeadList) {

		PFLT_RULE pRule = (PFLT_RULE)CONTAINING_RECORD(pList, FLT_RULE, List);

		if (wcscmp(pRule->ProcessName, ProcessName) == 0 && wcscmp(pRule->FilePath, FilePath) == 0) {
			KIRQL kIrql;
			KeAcquireSpinLock(&Self->SpinLock, &kIrql);
			RemoveEntryList(&pRule->List);
			KeReleaseSpinLock(&Self->SpinLock, kIrql);
			ExFreePoolWithTag(pRule, DRIVER_MEM_TAG);

			break;
		}

		pList = pList->Flink;
	}
}


BOOLEAN IsTargetFile(PFLT_RULE Rule, const wchar_t* FilePath)
{

	BOOLEAN isTarget = FALSE;

	if (Rule->FilePathType == FLT_PATH_TYPE_EQUAL) {
		isTarget = wcscmp(FilePath, Rule->FilePath) == 0;
	}
	else {
		isTarget = wcsstr(FilePath, Rule->FilePath) != NULL;
	}

	return isTarget;
}

BOOLEAN IsTargetProcess(PFLT_RULE Rule, const wchar_t* ProcessName)
{
	if (!ProcessName) {
		return FALSE;
	}

	if (wcscmp(Rule->ProcessName, L"*.exe") == 0) {
		return TRUE;
	}

	if (wcscmp(Rule->ProcessName, ProcessName) == 0) {
		return TRUE;
	}

	return FALSE;
}

BOOLEAN IsRuleExistFromList(PFLT_RULE_LIST Self, const wchar_t* ProcessName, const wchar_t* FilePath, PFLT_RULE* Rule)
{
	PLIST_ENTRY pList = Self->HeadList.Flink;
	while (pList != &Self->HeadList) {
		PFLT_RULE pRule = CONTAINING_RECORD(pList, FLT_RULE, List);

		if (IsTargetProcess(pRule, ProcessName) && IsTargetFile(pRule, FilePath)) {
			*Rule = pRule;
			return TRUE;
		}

		pList = pList->Flink;
	}

	return FALSE;
}

void CleanAllRule(PFLT_RULE_LIST Self)
{
	if (!Self) {
		return;
	}

	PLIST_ENTRY pList = Self->HeadList.Flink;
	while (pList != &Self->HeadList) {
		PFLT_RULE pRule = (PFLT_RULE)CONTAINING_RECORD(pList, FLT_RULE, List);


		pList = pList->Flink;

		KIRQL kIrql;
		KeAcquireSpinLock(&Self->SpinLock, &kIrql);
		RemoveEntryList(&pRule->List);
		KeReleaseSpinLock(&Self->SpinLock, kIrql);

		ExFreePoolWithTag(pRule, DRIVER_MEM_TAG);

	}
	

}
