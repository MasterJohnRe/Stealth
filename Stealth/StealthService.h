#pragma once
#ifndef STEALTHSERVICE_H
#define STEALTHSERVICE_H

#include <iostream>
#include "MemoryAccess.h"
#include "OsHandler.h"
#include "Consts.h"
#include "Exceptions.h"

class StealthService
{
private:
	DWORD attachedProcessPid;
	MemoryAccess memoryHandler = MemoryAccess();
	OsHandler osHandler = OsHandler();
public:
	void attachToProcess(std::string processName);
	void findWhatAccessThisAddress();
	void stopFindWhatAccessThisAddress();
};

#endif