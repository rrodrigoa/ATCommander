#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <wchar.h>
#include <queue>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <chrono>
#include <thread>

#include "ATResponses.h"

using namespace std;

enum ATUIMessageType
{
	Message = 0,
	Error = 1,
	Success = 2,
	Command = 3,
	Read = 4
};

class IATUI{
public:
	virtual void StartUpMessage(ATUIMessageType type, std::string message) = 0;
};

class ATUI : public IATUI{
	public:
		void StartUpMessage(ATUIMessageType type, std::string message)
		{
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
};

class ATCommand {
public:
	virtual std::string Command() = 0;
	virtual bool ProcessMessage(std::string message) = 0;
};

class ATOK : public ATCommand{
public:
	std::string Command(){
		return AT_AT + AT_ENTER;
	}

	bool ProcessMessage(std::string message){}
};

class ATD : public ATCommand{
public:
	ATD(std::string number){
		this->number = number;
	}

	std::string Command(){
		return AT_ATD + this->number + ";" + AT_ENTER;
	}

	bool ProcessMessage(std::string message){}

private:
	std::string number;
};

class ATCHLD : public ATCommand{
public:
	std::string Command(){
		return AT_ATCHLD + AT_ENTER;
	}

	bool ProcessMessage(std::string message){}
};

class ATCMUT : public ATCommand{
	std::string Command(){
		return AT_ATCMUT + AT_ENTER;
	}

	bool ProcessMessage(std::string message){}
};

class IATTerminal {
public:
	virtual bool OpenSerial(std::string serialName, speed_t baud) = 0;
	virtual bool CloseSerial() = 0;
	virtual bool Write(ATCommand* command) = 0;
	virtual std::string Read() = 0;

	virtual void SetUI(IATUI* ui) = 0;
};

class ATTermios : public IATTerminal{
public:
	bool OpenSerial(std::string serialName, speed_t baud){
		char messageBuffer[200];
		int fd;
		this->terminalFile = -1;
				
		sprintf(messageBuffer, "Initializing serial file %s", serialName.c_str());
		this->ui->StartUpMessage(ATUIMessageType::Message, messageBuffer);
		fd = open(serialName.c_str(), O_RDWR | O_NOCTTY);
		
		this->ui->StartUpMessage(ATUIMessageType::Message, "Opening terminal");
		this->terminal.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
		this->terminal.c_iflag=IGNPAR;
		this->terminal.c_oflag=0;
		this->terminal.c_lflag=0;
		this->terminal.c_cc[VMIN]=1;
		this->terminal.c_cc[VTIME]=0;
		cfsetospeed(&this->terminal,baud);
		cfsetispeed(&this->terminal,baud);
		tcsetattr(fd, TCSANOW, &this->terminal);
		
		if (tcflush(fd,TCIFLUSH)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType::Error, "Serial failed with TCIFLUSH");
			return false;
		}
		
		if (tcflush(fd,TCOFLUSH)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType::Error, "Serial failed with TCOFLUSH");
			return false;
		}
		
		if (tcsetattr(fd,TCSANOW,&this->terminal)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType::Error, "Serial failed with TCSANOW");
			return false;
		}
		
		this->ui->StartUpMessage(ATUIMessageType::Success, "Serial is open");
		this->terminalFile = fd;
		
		return true;
	}

	bool CloseSerial(){
		if (this->terminalFile != -1)
		{
			this->ui->StartUpMessage(ATUIMessageType::Message, "Closing serial file");
			close(this->terminalFile);
		}
	}

	bool Write(ATCommand* command)
	{
		this->ui->StartUpMessage(ATUIMessageType::Message, "Sending AT");
		this->ui->StartUpMessage(ATUIMessageType::Command, command->Command());
		const char* message = command->Command().c_str();
		write(this->terminalFile, command->Command().c_str(), command->Command().length());
		this->ui->StartUpMessage(ATUIMessageType::Message, "Sent");

		return true;
	}

	std::string Read()
	{
		memset(data, 0, size);
		this->ui->StartUpMessage(ATUIMessageType::Message, "Reading AT");
		int result = read(this->terminalFile, data, size);
		this->ui->StartUpMessage(ATUIMessageType::Read, data);

		return string(data, result);
	}

	void SetUI(IATUI* ui){
		this->ui = ui;
	}
private:
	struct termios terminal;
	int terminalFile;
	IATUI* ui;
	char data[256];
	int size = 256;
	int timeout_usec = 10000;
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

			this->terminal->Write(command);
			std::string response = this->terminal->Read();

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
	IATUI* ui = new ATUI();
	IATTerminal* terminal = new ATTermios();

	terminal->SetUI(ui);
	terminal->OpenSerial("/dev/ttyUSB0", B115200);

	commander->SetUI(ui);
	commander->SetTermios(terminal);
	
	ATCommand* command;

	command = new ATOK();
	commander->PushCommand(command);

	commander->ExecuteTopCommand();

	command = new ATD(std::string("4252096207"));
	commander->PushCommand(command);

	commander->ExecuteTopCommand();

	printf(">Press any key to drop call");
	getchar();

	command = new ATCHLD();
	commander->PushCommand(command);

	commander->ExecuteTopCommand();

	terminal->CloseSerial();
}
