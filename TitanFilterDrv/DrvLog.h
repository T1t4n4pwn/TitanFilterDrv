#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>

#define DRIVER_LOG_PREFIX "[TitanDrv]: "

void ErrorLog(const char* fmt, ...);

void ErrorPrint(const char* fmt, ...);