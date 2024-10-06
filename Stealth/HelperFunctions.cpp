#include <vector>
#include "HelperFunctions.h"



std::string HelperFunctions::ConvertAddressToString(uintptr_t address) {
    std::stringstream stream;
    stream << "0x" << std::hex << address;
    return stream.str();
}

uintptr_t HelperFunctions::ConvertStringToAddress(const std::string& addressStr) {
    return std::stoull(addressStr, nullptr, 16);
}

// Helper function to extract a label and value pair from a substring like "RAX:1140"
void extractLabelAndValue(const std::string& field, std::string& label, DWORD64& value) {
    size_t colonPos = field.find(':');
    if (colonPos != std::string::npos) {
        label = field.substr(0, colonPos);  // Extract the label part
        value = std::stoull(field.substr(colonPos + 1));  // Convert the value part to a DWORD64
    }
}

AccessEntry HelperFunctions::parseAccessEntry(const std::string& addressStr) {
    AccessEntry exceptionInfo = { 0 };  // Initialize all fields to zero

    size_t start = 0;
    size_t end = addressStr.find(',');  // Find the position of the first comma

    // Split the string manually using the comma as a delimiter
    while (end != std::string::npos) {
        std::string field = addressStr.substr(start, end - start);  // Extract the substring for this field

        // Extract label and value from the current field
        std::string label;
        DWORD64 value = 0;
        extractLabelAndValue(field, label, value);

        // Set the corresponding field in the PExceptionInfo structure based on the label
        if (label == "RAX") exceptionInfo.RAX = value;
        else if (label == "RBX") exceptionInfo.RBX = value;
        else if (label == "RCX") exceptionInfo.RCX = value;
        else if (label == "RDX") exceptionInfo.RDX = value;
        else if (label == "RSI") exceptionInfo.RSI = value;
        else if (label == "RDI") exceptionInfo.RDI = value;
        else if (label == "RBP") exceptionInfo.RBP = value;
        else if (label == "RSP") exceptionInfo.RSP = value;
        else if (label == "RIP") exceptionInfo.RIP = value;
        else if (label.find("count") != std::string::npos) exceptionInfo.count = value;

        start = end + 1;  // Move the start to the next character after the comma
        end = addressStr.find(',', start);  // Find the next comma position
    }

    // Process the last field (after the last comma)
    std::string field = addressStr.substr(start);
    std::string label;
    DWORD64 value = 0;
    extractLabelAndValue(field, label, value);

    if (label == "RAX") exceptionInfo.RAX = value;
    else if (label == "RBX") exceptionInfo.RBX = value;
    else if (label == "RCX") exceptionInfo.RCX = value;
    else if (label == "RDX") exceptionInfo.RDX = value;
    else if (label == "RSI") exceptionInfo.RSI = value;
    else if (label == "RDI") exceptionInfo.RDI = value;
    else if (label == "RBP") exceptionInfo.RBP = value;
    else if (label == "RSP") exceptionInfo.RSP = value;
    else if (label == "RIP") exceptionInfo.RIP = value;
    else if (label.find("count") != std::string::npos) exceptionInfo.count = value;

    return exceptionInfo;  // Return the populated structure
}