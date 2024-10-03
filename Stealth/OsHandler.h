#pragma once
#ifndef OSHANDLER_H
#define OSHANDLER_H

#include <cstdlib>
#include <string>

class OsHandler
{
public:
	int runCommand(std::string command);
};


#endif // !OSHANDLER_H