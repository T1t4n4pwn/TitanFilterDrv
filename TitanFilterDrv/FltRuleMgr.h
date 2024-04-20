#pragma once
#include "FltRule.h"
#include "Main.h"



typedef struct _FLT_RULE_LIST 
{
	LIST_ENTRY HeadList;
	KSPIN_LOCK SpinLock;
	PFLT_RULE FltRule;
}FLT_RULE_LIST, *PFLT_RULE_LIST;

PFLT_RULE_LIST GetRuleListInstance();

void AddRuleToList(PFLT_RULE_LIST Self, PFLT_RULE Rule);

void DeleteRuleFromList(PFLT_RULE_LIST Self, const wchar_t* ProcessName, const wchar_t* FilePath);

BOOLEAN IsRuleExistFromList(PFLT_RULE_LIST Self, const wchar_t* ProcessName, const wchar_t* FilePath, PFLT_RULE* Rule);

void CleanAllRule(PFLT_RULE_LIST Self);