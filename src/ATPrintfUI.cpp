#include "ATPrintfUI.h"
#include <string>
#include <stdio.h>

using namespace std;

ATPrintfUI::ATPrintfUI(){}

void ATPrintfUI::Message(ATUIMessageType type, char* message){
	switch (type)
	{
		case ATUIMessageType::Message:
			printf("Message: %s\n", message);
			break;
		case Error:
			printf("Error: %s\n", message);
			break;
		case Success:
			printf("Success: %s\n", message);
			break;
		case SendCommand:
			printf(">>%s\n", message);
			break;
		case ReadCommand:
			printf("<<%s\n", message);
			break;
	}
}

void ATPrintfUI::CarrierName(char* carrierName){
	printf("Carrier: %s\n", carrierName);
}

