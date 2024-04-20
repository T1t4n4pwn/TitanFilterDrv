#pragma once
#include <ntifs.h>

#define FLT_RULE_FILE_DENY_ACESS 0x1
#define FLT_RULE_FILE_HIDE 0x2

typedef enum {
	FLT_PATH_TYPE_EQUAL,
	FLT_PATH_TYPE_INCLUDE,
}FLT_PATH_TYPE;

typedef enum {
	PACKET_NONE,
	PACKET_START,
	PACKET_STOP,
	PACKET_ADD_PATH,
	PACKET_DEL_PATH,
}PACKET_TYPE;

#pragma pack(1)
typedef struct {
	PACKET_TYPE Type;
	wchar_t ProcessName[260];
	wchar_t FilePath[260];
	ULONG ProtectFlag;
	FLT_PATH_TYPE FilePathType;
}DATA_PACKET;
#pragma pack()

typedef struct _FLT_RULE 
{
	LIST_ENTRY List;
	wchar_t ProcessName[260];
	wchar_t FilePath[260];
	ULONG ProtectFlag;
	FLT_PATH_TYPE FilePathType;
}FLT_RULE, *PFLT_RULE;
