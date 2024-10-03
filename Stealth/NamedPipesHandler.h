#ifndef NAMEDPIPESHANDLER_H
#define NAMEDPIPESHANDLER_H

#include <Windows.h>
#include <string>
#include "NamedPipesException.h"

#pragma once
class NamedPipesHandler
{
public:
	NamedPipesHandler();
	~NamedPipesHandler();

	void createPipe(const std::string& pipeName);
	void closePipe();
	void writeData(const std::string& data);
	void readData(std::string& outData);

private:
	HANDLE hPipe; //Handle to the named pipe
	std::string pipeName; //Name of the pipe
};



#endif