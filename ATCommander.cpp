#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

class ATCommand {
public:
	VerifySignal();
	VerifyService();
	MessageSend();
	MessageReceive();
	CallMake();
	CallReceive();
	CallHangUp();
private:
	
}

enum ATUIMessageType
{
	Message = 0,
	Error = 1,
	Success = 2
}

class ATUI{
public:
	StartUpMessage(ATUIMessageType type, char* message)
	{
		switch (type)
		{
			case ATUIMessageType.Message:
				printf("Message: %s\n", message);
				break;
			case ATUIMessageType.Error:
				printf("Error: %s\n", message);
				break;
			case ATUIMessageType.Success:
				printf("Success: %s", message);
				break;
		}
	}	
}

class ATCommander{
public:
	void CreateUI()
	{
		this->ui = new ATUI();
	}
	
	int OpenSerial(char* serialName, speed_t baud)
	{
		this->terminalFile = -1;
				
		this->ui->StartUpMessage(ATUIMessageType.Message, "Initializing serial file " + serialName);		
		this->terminalFile = open(serialName, O_RDWR | O_NOCTTY);
		
		this->ui->StartupMessage(ATUIMessageType.Message, "Opening terminal");
		this->terminal.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
		this->terminal.c_iflag=IGNPAR;
		this->terminal.c_oflag=0;
		this->terminal.c_lflag=0;
		this->terminal.c_cc[VMIN]=1;
		this->terminal.c_cc[VTIME]=0;
		cfsetospeed(&this->terminal,baud);
		cfsetispeed(&this->terminal,baud);
		
		if (tcflush(fd,TCIFLUSH)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType.Error, "Serial failed with TCIFLUSH");
			return -1;
		}
		
		if (tcflush(fd,TCOFLUSH)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType.Error, "Serial failed with TCOFLUSH");
		}
		
		if (tcsetattr(fd,TCSANOW,&this->terminal)==-1)
		{
			this->ui->StartUpMessage(ATUIMessageType.Error, "Serial failed with TCSANOW");
		}
		
		this->ui->StartUpMessage(ATUIMessageType.Success, "Serial is open");
		this->terminalFile = fd;
		
		return fd;
		
	}
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
	}
private:
	ATUI ui;
	struct termios terminal;
	fd terminalFile;
	std::thread ATQueueThread (QueueMainLoop);
	bool CancelationToken;
	std::queue<ATCommand> ATQueueCommands;
}

int main()
{
	ATCommander commander = new ATCommander();
	
	commander->CreateUI();
	commander->OpenSerial("/dev/ttyUSB0", B115200);
}