#pragma once
#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "windows.h"
#include <string>
#include <sstream>    // For std::stringstream
#include <iomanip>    // For std::hex, std::dec


struct AccessEntry {
    DWORD64 RAX;
    DWORD64 RBX;
    DWORD64 RCX;
    DWORD64 RDX;
    DWORD64 RSI;
    DWORD64 RDI;
    DWORD64 RBP;
    DWORD64 RSP;
    DWORD64 RIP;
    int count;
};

class HelperFunctions
{
public:
	std::string ConvertAddressToString(uintptr_t address);
	uintptr_t ConvertStringToAddress(const std::string& addressStr);
	AccessEntry parseAccessEntry(const std::string& addressStr);
};

#endif