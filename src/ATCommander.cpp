#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <wchar.h>
#include <queue>
#include <ncurses.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <chrono>
#include <thread>

#include "ATResponses.h"
#include "ATNCursesUI.h"
#include "ATPrintfUI.h"
#include "IATUI.h"
#include "IATTerminal.h"
#include "ATTermios.h"

using namespace std;


class ATCommand {
public:
	virtual bool ExecuteCommand(IATTerminal* terminal, IATUI* ui) = 0;
};

class SendSMS : public ATCommand{
public:
	SendSMS(char* phone, char* message){
		memcpy(this->phone,  phone,  strlen(phone));
		memcpy(this->message, message, strlen(message));
	}

	bool ExecuteCommand(IATTerminal* terminal, IATUI* ui)
	{
		char at_CMGF[20];
		memset(at_CMGF, 0, 20);
		sprintf(at_CMGF, "%s=1\r\n", AT_ATCMGF);


		char at_CMGS[50];
		memset(at_CMGS, 0, 50);

		// Clear buffer
		terminal->Read(0);

		// Write change SMS to text mode
		terminal->Write(at_CMGF);
		char* actualResponse = terminal->Read(100);
		if (actualResponse == NULL || !strstr(actualResponse, AT_OK)){
			return false;
		}

		// Write SMS to phone number
		sprintf(at_CMGS, "AT+CMGS=\"%s\"\r", this->phone);
		terminal->Write(at_CMGS);
		actualResponse = terminal->Read(100);
		if (actualResponse == NULL || !strstr(actualResponse, AT_LARGER)){
			return false;
		}

		// Write SMS message and ctrl+z to end message
		terminal->Write(this->message);
		char breakLine[2] = { 0x1A, 0 };
		terminal->Write(breakLine);
		actualResponse = terminal->Read(5000);
		if (actualResponse == NULL || !strstr(actualResponse, AT_OK)){
			return false;
		}

		// Message was sent successfully
		return true;
	}

private:
	char phone[50];
	char message[256];
};

class GetCarrierName : public ATCommand{
public:
	GetCarrierName(){}
	bool ExecuteCommand(IATTerminal* terminal, IATUI* ui)
	{
		char at_COPS[20];
		memset(at_COPS, 0, 20);
		sprintf(at_COPS, "%s?\r\n", AT_ATCOPS);

		// Clear buffer
		terminal->Read(0);

		// Get carrier name
		terminal->Write(at_COPS);
		char* actualResponse = terminal->Read(100);
		if (actualResponse == NULL || !strstr(actualResponse, "\""))
		{
			return false;
		}

		string actualResponseString = string(actualResponse);
		size_t start = actualResponseString.find_first_of('\"');
		size_t end = actualResponseString.find_last_of('\"');
		string carrierName = actualResponseString.substr(start, end);

		ui->CarrierName(const_cast<char*>(carrierName.c_str()));
	}
};

class MakeCall : public ATCommand{
public:
	MakeCall(char* phoneNumber){
		this->phoneNumber = phoneNumber;
	}
	bool ExecuteCommand(IATTerminal* terminal, IATUI* ui){
		char at_ATD[50];
		memset(at_ATD, 0, 50);
		sprintf(at_ATD, "%s%s;\r\n", AT_ATD, this->phoneNumber);

		// Clear buffer
		terminal-> Read(0);

		// Make call
		terminal->Write(at_ATD);
		char* actualResponse = terminal->Read(5000);
		if (actualResponse == NULL || !strstr(actualResponse, AT_OK)){
			return false;
		}

		return true;
	}
private:
	char* phoneNumber;
};

class HangUpCall : public ATCommand{
public:
	HangUpCall(){}
	bool ExecuteCommand(IATTerminal* terminal, IATUI* ui){
		char at_CHUP[50];
		memset(at_CHUP, 0, 50);
		sprintf(at_CHUP, AT_ATCHUP);

		// Clear buffer
		terminal->Read(0);

		// Hang Up Call
		terminal->Write(at_CHUP);
		char* actualResponse = terminal->Read(1000);
		if(actualResponse == NULL || !strstr(actualResponse, AT_OK)){
			return false;
		}

		return true;
	}
};

class MuteCall : public ATCommand{
public:
	MuteCall(){}
	bool ExecuteCommand(IATTerminal* terminal, IATUI* ui){
		return true;
	}
};

class ATCommander{
public:
	ATCommander(){
	}

	void SetUI(IATUI* ui)
	{
		this->ui = ui;
	}

	void SetTermios(IATTerminal* terminal)
	{
		this->terminal = terminal;
	}

	bool PushCommand(ATCommand* command){
		this->commandQueue.push(command);
		return true;
	}

	bool ExecuteTopCommand(){
		if (!this->commandQueue.empty()){
			ATCommand* command = this->commandQueue.front();
			this->commandQueue.pop();

			command->ExecuteCommand(this->terminal, this->ui);

		}
	}

private:
	IATUI* ui;
	IATTerminal* terminal;
	queue<ATCommand*> commandQueue;
};

int main()
{
	ATCommander* commander = new ATCommander();
	IATUI* ui = new ATPrintfUI();
	IATTerminal* terminal = new ATTermios();

	terminal->SetUI(ui);
	terminal->OpenSerial("/dev/ttyUSB0", B115200);

	commander->SetUI(ui);
	commander->SetTermios(terminal);

	char noum[50];
	char verb[50];
	char subject[50];

	printf("@");
	while(scanf("%s %s", noum, verb) && !strstr(noum, "exit")){
		// CALL
		if (strstr(noum, "call")){
			// call make
			if(strstr(verb, "make")){
				scanf("%s", subject);
				ATCommand* command = new MakeCall(subject);
				commander->PushCommand(command);
				commander->ExecuteTopCommand();
			}

			// call hangup
			if(strstr(verb, "hangup") || strstr(verb, "hang") || strstr(verb, "stop")){
				ATCommand* command = new HangUpCall();
				commander->PushCommand(command);
				commander->ExecuteTopCommand();
			}

			// call mute
			if(strstr(verb, "mute")){
				ATCommand* command = new MuteCall();
				commander->PushCommand(command);
				commander->ExecuteTopCommand();
			}
		}

		// SMS
		if (strstr(noum, "sms")){
			// sms write
			// sms read
		}

		printf("@");
	}

	terminal->CloseSerial();
}
