#pragma once
#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "windows.h"
#include <string>
#include <sstream>    // For std::stringstream
#include <iomanip>    // For std::hex, std::dec

class HelperFunctions
{
public:
	std::string ConvertAddressToString(DWORD address);
	DWORD ConvertStringToAddress(const std::string& addressStr);
};

#endif