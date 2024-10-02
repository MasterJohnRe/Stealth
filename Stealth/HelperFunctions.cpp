#include "HelperFunctions.h"

std::string HelperFunctions::ConvertAddressToString(DWORD address) {
    std::stringstream stream;
    stream << "0x" << std::hex << address;
    return stream.str();
}

DWORD HelperFunctions::ConvertStringToAddress(const std::string& addressStr) {
    return std::stoul(addressStr, nullptr, 16);
}
