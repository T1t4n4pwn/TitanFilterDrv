#pragma once
#include <ntifs.h>
#include <fltKernel.h>

NTSTATUS ConnectNotify(PFLT_PORT ClientPort, PVOID ServerPortCookie, PVOID ConnectionContext, ULONG SizeOfContext, PVOID* ConnectionPortCookie);

VOID DisconnectNotify(PVOID ConnectionCookie);

NTSTATUS MessageNotify(PVOID PortCookie, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength, PULONG ReturnOutputBufferLength);