#include "Filter.h"
#include <Windows.h>
#include <fltUser.h>

MiniFilterCommunication::MiniFilterCommunication() 
{

}

MiniFilterCommunication::~MiniFilterCommunication() 
{
	if (m_hPort != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hPort);
	}
}

bool MiniFilterCommunication::Connect(const std::wstring& PortName) 
{
	HRESULT hResult = FilterConnectCommunicationPort(PortName.c_str(), 0, NULL, 0, NULL, &m_hPort);
	if (hResult != S_OK) {
		return false;
	}

	return true;
}

bool MiniFilterCommunication::SendMessageToFlt(DataPacket Packet)
{

	DWORD outputSize = 0;



	HRESULT hResult = FilterSendMessage(m_hPort, &Packet, sizeof(Packet), &Packet, sizeof(Packet), &outputSize);
	if (hResult != S_OK) {
		return false;
	}

	return true;
}

bool MiniFilterCommunication::SendMessageToFlt(PACKET_TYPE Type, ULONG Flag, const wchar_t* FilePath, const wchar_t* ProcessName, FLT_PATH_TYPE PathType)
{
	DataPacket packet{Type, Flag, FilePath, ProcessName, PathType};

	DWORD outputSize = 0;

	HRESULT hResult = FilterSendMessage(m_hPort, &packet, sizeof(packet), &packet, sizeof(packet), &outputSize);
	if (hResult != S_OK) {
		return false;
	}

	return true;
}
