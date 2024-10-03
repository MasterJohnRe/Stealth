#include "StealthService.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

auto logger = spdlog::basic_logger_mt("file_logger", "logfile.txt");
//spdlog::set_pattern("[%Y-%m-%d%H:%M:%S] [%l] %v");

void StealthService::findWhatAccessThisAddress(DWORD processName) {
	try {
		NamedPipesHandler namedPipesHandler = NamedPipesHandler();
		namedPipesHandler.createPipe("FindWhatAccessThisAddressNamedPipe");
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
