#pragma once
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

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

// Exception for operations when pipe is not open
class PipeNotConnectedException : public NamedPipeException {
public:
    explicit PipeNotConnectedException(const std::string& message) : NamedPipeException(message) {}
};


// Base class for Named Pipe exceptions
class OsHandlerException : public std::exception {
public:
    explicit OsHandlerException(const std::string& message) : message(message) {}
    virtual const char* what() const noexcept override {
        return message.c_str();
    }

protected:
    std::string message;
};


// Exception for failing run command
class OsHandlerRunCommandException : public OsHandlerException {
public:
    explicit OsHandlerRunCommandException(const std::string& message) : OsHandlerException(message) {}
};

#endif
