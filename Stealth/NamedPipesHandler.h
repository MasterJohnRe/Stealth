#ifndef NAMEDPIPESHANDLER_H
#define NAMEDPIPESHANDLER_H

#include <Windows.h>
#include <string>
#include "Exceptions.h"
#include <future>
#include <mutex>

#pragma once
class NamedPipesHandler
{
public:

	std::mutex pipeMutex;  // Mutex to protect shared resources

	std::shared_future<std::string> dataFuture;
	NamedPipesHandler();
	NamedPipesHandler(const std::string& name);
	~NamedPipesHandler();

	void createPipe(const std::string& pipeName);
	void connectToPipe();
	void closePipe();
	void writeData(const std::string& data, DWORD timeoutMs = 5000);
	void readData(std::promise<std::string>&& dataPromise, DWORD timeoutMs=5000);
	std::shared_future<std::string> readDataAsync(DWORD timeoutMs = 5000);

private:
	HANDLE hPipe; //Handle to the named pipe
	std::thread readThread;// Thread for reading data
	std::string pipeName; //Name of the pipe
};



#endif