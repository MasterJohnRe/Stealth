#pragma once
#ifndef NAMEDPIPEEXCEPTION_H
#define NAMEDPIPEEXCEPTION_H

#include <exception>
#include <string>

// Base class for Named Pipe exceptions
class NamedPipeException : public std::exception {
public:
    explicit NamedPipeException(const std::string& message) : message(message) {}
    virtual const char* what() const noexcept override {
        return message.c_str();
    }

protected:
    std::string message;
};

// Exception for pipe creation errors
class PipeCreationException : public NamedPipeException {
public:
    explicit PipeCreationException(const std::string& message) : NamedPipeException(message) {}
};

// Exception for pipe write errors
class PipeWriteException : public NamedPipeException {
public:
    explicit PipeWriteException(const std::string& message) : NamedPipeException(message) {}
};

// Exception for pipe read errors
class PipeReadException : public NamedPipeException {
public:
    explicit PipeReadException(const std::string& message) : NamedPipeException(message) {}
};

// Exception for operations when pipe is not open
class PipeNotOpenException : public NamedPipeException {
public:
    explicit PipeNotOpenException(const std::string& message) : NamedPipeException(message) {}
};

#endif // NAMEDPIPEEXCEPTION_H
