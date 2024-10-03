#include "StealthService.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

static auto logger = spdlog::get("file_logger");  // Static logger reference for the file

void StealthService::findWhatAccessThisAddress(DWORD processName) {
	try {
		std::string command = Consts::INJECTOR_64_EXE_FILE_PATH + "/dll" + Consts::FIND_WHAT_ACCESS_THIS_ADDRESS_DLL_FILE_PATH + "/target" + std::to_string(this->attachedProcessPid) + "/action 1";
		this->osHandler.runCommand(command);
	}
	catch (const OsHandlerRunCommandException& ex) {
		logger->error("failed injecting Dll: ", ex.what());
	}
}

void StealthService::stopFindWhatAccessThisAddress() {
	try{
	std::string command = Consts::INJECTOR_64_EXE_FILE_PATH + "/dll" + Consts::FIND_WHAT_ACCESS_THIS_ADDRESS_DLL_FILE_PATH + "/target" + std::to_string(this->attachedProcessPid) + "/action 2";
	this->osHandler.runCommand(command);
	}
	catch (const OsHandlerRunCommandException& ex) {
		logger->error("failed unloading Dll: ", ex.what());
	}
}

void StealthService::attachToProcess(std::string processName) {
	this->attachedProcessPid = this->memoryHandler.getGamePID((LPCSTR)processName.c_str());
}
