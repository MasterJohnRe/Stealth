#include "NamedPipesHandler.h"
#include <iostream>

// Constructor
NamedPipesHandler::NamedPipesHandler() : hPipe(INVALID_HANDLE_VALUE), pipeName("") {}

// Destructor
NamedPipesHandler::~NamedPipesHandler() {
    closePipe();  // Ensure the pipe is closed when the object is destroyed
}

// Creates a named pipe with the specified name
void NamedPipesHandler::createPipe(const std::string& pipeName) {
    this->pipeName = "\\\\.\\pipe\\" + pipeName;

    hPipe = CreateNamedPipeA(
        this->pipeName.c_str(), PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 1024, 1024, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        throw PipeCreationException("Error: Could not create named pipe. Error code: " + std::to_string(GetLastError()));
    }
}

// Closes the named pipe
void NamedPipesHandler::closePipe() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

// Writes data to the named pipe
void NamedPipesHandler::writeData(const std::string& data) {
    if (hPipe == INVALID_HANDLE_VALUE) {
        throw PipeNotOpenException("Error: Pipe is not open. Cannot write data.");
    }

    DWORD bytesWritten;
    bool result = WriteFile(hPipe, data.c_str(), data.length(), &bytesWritten, NULL);

    if (!result || bytesWritten != data.length()) {
        throw PipeWriteException("Error: Could not write to the pipe. Error code: " + std::to_string(GetLastError()));
    }
}

// Reads data from the named pipe
void NamedPipesHandler::readData(std::string& outData) {
    if (hPipe == INVALID_HANDLE_VALUE) {
        throw PipeNotOpenException("Error: Pipe is not open. Cannot read data.");
    }

    char buffer[1024];
    DWORD bytesRead;
    bool result = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

    if (!result || bytesRead == 0) {
        throw PipeReadException("Error: Could not read from the pipe. Error code: " + std::to_string(GetLastError()));
    }

    buffer[bytesRead] = '\0';
    outData = buffer;
}
