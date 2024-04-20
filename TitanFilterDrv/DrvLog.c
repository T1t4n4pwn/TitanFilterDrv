#include "DrvLog.h"

void ErrorLog(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vDbgPrintExWithPrefix(DRIVER_LOG_PREFIX, DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, va);
	va_end(va);
}

void ErrorPrint(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vDbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, va);
	va_end(va);
}