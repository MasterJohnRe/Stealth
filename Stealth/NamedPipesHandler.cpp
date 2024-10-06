#include "NamedPipesHandler.h"
#include <chrono>
#include <thread>  // For std::this_thread::sleep_for
#include <iostream>

// Constructor
NamedPipesHandler::NamedPipesHandler() : hPipe(INVALID_HANDLE_VALUE), pipeName("") {}

NamedPipesHandler::NamedPipesHandler(const std::string& name) : hPipe(INVALID_HANDLE_VALUE) {
    this->pipeName = "\\\\.\\pipe\\" + name;
}


// Destructor
NamedPipesHandler::~NamedPipesHandler() {
    closePipe();  // Ensure the pipe is closed when the object is destroyed
    if (readThread.joinable()) {
        readThread.join();    // Wait for the thread to finish
    }
}

// Creates a named pipe with the specified name
void NamedPipesHandler::createPipe(const std::string& pipeName) {
    this->pipeName = "\\\\.\\pipe\\" + pipeName;

    hPipe = CreateNamedPipeA(
        this->pipeName.c_str(), PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
        1, 1024, 1024, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        throw PipeCreationException("Error: Could not create named pipe. Error code: " + std::to_string(GetLastError()));
    }
}

// Connect to the named pipe
void NamedPipesHandler::connectToPipe() {
    hPipe = CreateFileA(
        pipeName.c_str(),           // Pipe name
        GENERIC_READ | GENERIC_WRITE, // Read and write access
        0,                          // No sharing
        NULL,                       // Default security attributes
        OPEN_EXISTING,              // Opens existing pipe
        0,                          // Default attributes
        NULL                        // No template file
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error == ERROR_PIPE_BUSY) {
            // Retry if the pipe is busy
            std::cout << "Pipe is busy, retrying connection..." << std::endl;
            if (!WaitNamedPipeA(pipeName.c_str(), 5000)) {  // Wait for up to 5 seconds
                throw std::runtime_error("Could not open pipe. Error code: " + std::to_string(GetLastError()));
            }
            connectToPipe();  // Retry connection
        }
        else {
            throw std::runtime_error("Error: Could not connect to named pipe. Error code: " + std::to_string(error));
        }
    }

    std::cout << "Successfully connected to the pipe." << std::endl;
}

// Closes the named pipe
void NamedPipesHandler::closePipe() {
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

// Writes data to the named pipe with timeout and retry mechanism, using custom exceptions
void NamedPipesHandler::writeData(const std::string& data, DWORD timeoutMs) {
    if (hPipe == INVALID_HANDLE_VALUE) {
        throw PipeNotOpenException("Error: Pipe is not open. Cannot write data.");
    }

    DWORD bytesWritten;
    DWORD startTime = GetTickCount();  // Record start time

    while (true) {
        BOOL result = WriteFile(hPipe, data.c_str(), data.length(), &bytesWritten, NULL);

        // Break the loop if write is successful
        if (result && bytesWritten == data.length()) {
            std::cout << "Data written successfully: " << data << std::endl;
            return;
        }

        // Calculate elapsed time
        DWORD elapsed = GetTickCount() - startTime;
        if (elapsed >= timeoutMs) {
            throw PipeWriteException("Error: Could not write to the pipe within the timeout period. Error code: " + std::to_string(GetLastError()));
        }

        // Wait a short time before retrying (to avoid busy looping)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void NamedPipesHandler::readData(std::promise<std::string>&& dataPromise, DWORD timeoutMs) {
    if (hPipe == INVALID_HANDLE_VALUE) {
        throw PipeNotOpenException("Error: Pipe is not open. Cannot read data.");
    }

    char buffer[1024];
    DWORD bytesRead;
    DWORD startTime = GetTickCount();  // Record start time

    while (true) {
        BOOL result = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

        // Break the loop if read is successful
        if (result && bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate the buffer
            dataPromise.set_value(buffer);  // Set the value in the promise
            std::cout << "Data read successfully: " << buffer << std::endl;
            return;
        }

        // Calculate elapsed time
        DWORD elapsed = GetTickCount() - startTime;
        if (elapsed >= timeoutMs) {
            throw PipeReadException("Error: Could not read from the pipe within the timeout period. Error code: " + std::to_string(GetLastError()));
        }

        // Wait a short time before retrying (to avoid busy looping)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Reads data from the named pipe with timeout and retry mechanism, using custom exceptions
std::future<std::string> NamedPipesHandler::readDataAsync(DWORD timeoutMs) {
    std::promise<std::string> dataPromise;
    std::future<std::string> dataFuture = dataPromise.get_future();
    readThread = std::thread(&NamedPipesHandler::readData, this, std::move(dataPromise), timeoutMs);  // Launch the readData function in a new thread
    return dataFuture;
}
