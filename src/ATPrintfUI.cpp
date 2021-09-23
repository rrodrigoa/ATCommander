#include "ATPrintfUI.h"
#include <string>
#include <stdio.h>

using namespace std;

ATPrintfUI::ATPrintfUI(){}

void ATPrintfUI::StartUpMessage(ATUIMessageType type, std::string message){
	switch (type)
	{
		case Message:
			printf("Message: %s\n", message.c_str());
			break;
		case Error:
			printf("Error: %s\n", message.c_str());
			break;
		case Success:
			printf("Success: %s\n", message.c_str());
			break;
		case Command:
			printf("Command: %s\n", message.c_str());
			break;
		case Read:
			printf("Read: %s\n", message.c_str());
			break;
	}
}

