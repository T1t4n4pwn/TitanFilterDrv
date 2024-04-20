#pragma once
#include <iostream>
#include <Windows.h>
#pragma comment(lib, "FltLib.lib")

#define FLT_RULE_FILE_DENY_ACESS 0x1
#define FLT_RULE_FILE_HIDE 0x2

#define MINIFILTER_PORT_NAME L"\\TitanCommunicationPort"

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
class DataPacket {
public:
	DataPacket() : m_type(PACKET_TYPE::PACKET_NONE), m_pathType(FLT_PATH_TYPE::FLT_PATH_TYPE_EQUAL)
	{
		memset(m_filePath, 0, sizeof(m_filePath));
		memset(m_processName, 0, sizeof(m_processName));
	}

	DataPacket(PACKET_TYPE Type, ULONG Flag, const wchar_t* FilePath, const wchar_t* ProcessName, FLT_PATH_TYPE PathType)
		: m_type(Type), m_flags(Flag), m_pathType(PathType)
	{
		memset(m_filePath, 0, sizeof(m_filePath));
		memset(m_processName, 0, sizeof(m_processName));

		memcpy(m_filePath, FilePath, wcslen(FilePath) << 1);
		memcpy(m_processName, ProcessName, wcslen(ProcessName) << 1);
	}

	PACKET_TYPE getType()
	{
		return m_type;
	}

	void setType(PACKET_TYPE Type)
	{
		m_type = Type;
	}

	ULONG getProtectFlags()
	{
		return m_flags;
	}

	void setProtectFlags(ULONG Options)
	{
		m_flags = Options;
	}

	std::wstring getFilePath() 
	{
		return m_filePath;
	}

	void setFilePath(const std::wstring& FilePath)
	{
		memset(m_filePath, 0, sizeof(m_filePath));
		memcpy(m_filePath, FilePath.c_str(), FilePath.size() << 1);
	}

	std::wstring getProcessName() 
	{
		return m_processName;
	}

	void setProcessName(const std::wstring& ProcessName)
	{
		memset(m_processName, 0, sizeof(m_processName));
		memcpy(m_processName, ProcessName.c_str(), ProcessName.size() << 1);
	}

	FLT_PATH_TYPE getPathType()
	{
		return m_pathType;
	}

	void setPathType(FLT_PATH_TYPE PathType)
	{
		m_pathType = PathType;
	}

private:
	PACKET_TYPE m_type;
	wchar_t m_processName[260];
	wchar_t m_filePath[260];
	ULONG m_flags;
	FLT_PATH_TYPE m_pathType;
};
#pragma pack()

class MiniFilterCommunication {
public:
	MiniFilterCommunication();
	~MiniFilterCommunication();

	bool Connect(const std::wstring& PortName);

	bool SendMessageToFlt(DataPacket Packet);

	bool SendMessageToFlt(PACKET_TYPE Type, ULONG Flag, const wchar_t* FilePath, const wchar_t* ProcessName, FLT_PATH_TYPE PathType);

private:
	HANDLE m_hPort;
};