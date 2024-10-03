#include "OsHandler.h"
#include "Exceptions.h"

int OsHandler::runCommand(std::string command) {
	int result = system(command.c_str());
	if (result != 0) {
		throw OsHandlerRunCommandException("Error: failed running the following command: " + std::string(command));
	}
	return 0;
}