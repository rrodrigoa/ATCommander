#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <wchar.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <chrono>
#include <thread>

using namespace std;

enum ATUIMessageType
{
	Message = 0,
	Error = 1,
	Success = 2,
	Command = 3,
	Read = 4
};

class ATUI{
	public:
		void StartUpMessage(ATUIMessageType type, char* message)
		{
			switch (type)
			{
				case Message:
					printf("Message: %s\n", message);
					break;
				case Error:
					printf("Error: %s\n", message);
					break;
				case Success:
					printf("Success: %s\n", message);
					break;
				case Command:
					printf("Command: %s\n", message);
					break;
				case Read:
					printf("Read: %s\n", message);
					break;
			}
		}

		void GeneralMessage(ATUIMessageType type, char* message)
		{
		}
};

class ATCommand {
public:
	static ATCommand* ATOK()
	{
		ATCommand* newCommand = new ATCommand();
		newCommand->SendMessage = "AT\r\n";
		newCommand->ReceiveMessage = "OK";
		newCommand->DelimiterMessage = NULL;
		return newCommand;
	}

	static ATCommand* ATD(char* number)
	{
		char* callMessageNumber = (char*)malloc(strlen(number)+7);
		callMessageNumber [0] = 0;
		strcat(callMessageNumber, "ATD");
		strcat(callMessageNumber, number);
		strcat(callMessageNumber, ";\r\n");

		ATCommand* newCommand = new ATCommand();
		newCommand->SendMessage = callMessageNumber;
		newCommand->ReceiveMessage = "OK";
		newCommand->DelimiterMessage = NULL;
		return newCommand;
	}
	
	static ATCommand* ATH()
	{
		ATCommand* newCommand = new ATCommand();
		newCommand->SendMessage = "ATH\r\n";
		newCommand->DelimiterMessage = NULL;
		return newCommand;
	}

	char* SendMessage;
	char* ReceiveMessage;
	char DelimiterMessage;

};

class ATCommander{
public:
	void CreateUI()
	{
		this->ui = new ATUI();
	}
	
	int OpenSerial(char* serialName, speed_t baud)
	{
		char messageBuffer[200];
		int fd;
		this->terminalFile = -1;
				
		sprintf(messageBuffer, "Initializing serial file %s", serialName);
		this->ui->StartUpMessage(ATUIMessageType::Message, messageBuffer);
		fd = open(serialName, O_RDWR | O_NOCTTY);
		
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
			return -1;
		}
		
		if (tcflush(fd,TCOFLUSH)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType::Error, "Serial failed with TCOFLUSH");
		}
		
		if (tcsetattr(fd,TCSANOW,&this->terminal)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType::Error, "Serial failed with TCSANOW");
		}
		
		this->ui->StartUpMessage(ATUIMessageType::Success, "Serial is open");
		this->terminalFile = fd;
		
		return fd;
		
	}

	void CloseSerial()
	{
		if (this->terminalFile != -1)
		{
			this->ui->StartUpMessage(ATUIMessageType::Message, "Closing serial file");
			close(this->terminalFile);
		}
	}

	void WriteATCommand(ATCommand* command)
	{
		this->ui->StartUpMessage(ATUIMessageType::Message, "Sending AT");
		this->ui->StartUpMessage(ATUIMessageType::Command, command->SendMessage);
		write(this->terminalFile, command->SendMessage, strlen(command->SendMessage));
		this->ui->StartUpMessage(ATUIMessageType::Message, "Sent");
	}

	int ReadATResponse(char* data, int size, int timeout_usec)
	{
		memset(data, 0, size);
		this->ui->StartUpMessage(ATUIMessageType::Message, "Reading AT");
		int result = read(this->terminalFile, data, size);
		this->ui->StartUpMessage(ATUIMessageType::Read, data);
	}
	/*
	CloseSerial();
	EnqueueCommand();
	DequeueCommand();
	void QueueMainLoop()
	{
		while(this->CancelationToken == false)
		{
			// Main queue loop
			// Read serial and enqueue any command
			// top command and execute
			// read result and pop
		}
	}*/
private:
	ATUI* ui;
	struct termios terminal;
	int terminalFile;
	//std::thread ATQueueThread (QueueMainLoop);
	bool CancelationToken;
	//std::queue<ATCommand> ATQueueCommands;
};

int main()
{
	ATCommander* commander = new ATCommander();
	
	commander->CreateUI();
	commander->OpenSerial("/dev/ttyUSB0", B115200);

	ATCommand* command;

	command = ATCommand::ATOK();
	commander->WriteATCommand(command);

	char data[256];
	int read;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	read = commander->ReadATResponse(data, sizeof(data), 10000);

	command = ATCommand::ATD("");
	commander->WriteATCommand(command);
std::this_thread::sleep_for(std::chrono::seconds(2));
	read = commander->ReadATResponse(data,  sizeof(data), 10000);

	printf(">press any key to hang up\n");
	getchar();

	command = ATCommand::ATH();
	commander->WriteATCommand(command);

	printf("Call finished\n");

	commander->CloseSerial();
}
