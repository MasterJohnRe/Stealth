#include "OsHandler.h"
#include "Exceptions.h"

int OsHandler::runCommand(const char* command) {
	int result = system(command);
	if (result != 0) {
		throw OsHandlerRunCommandException("Error: failed running the following command: " + std::string(command));
	}
	return 0;
}