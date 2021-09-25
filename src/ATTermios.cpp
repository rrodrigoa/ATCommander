#include "IATTerminal.h"
#include <termios.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include "ATTermios.h"
#include <string.h>
#include <chrono>
#include <termios.h>
#include <fcntl.h>

bool ATTermios::OpenSerial(char* serialName, speed_t baud){
	char messageBuffer[200];
	int fd;
	this->terminalFile = -1;

	sprintf(messageBuffer, "Initializing serial file %s", serialName);
	this->ui->Message(ATUIMessageType::Message, messageBuffer);
	fd = open(serialName, O_RDWR | O_NOCTTY);

	this->ui->Message(ATUIMessageType::Message, "Opening terminal");
	this->terminal.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
	this->terminal.c_iflag=IGNPAR;
	this->terminal.c_oflag=0;
	this->terminal.c_lflag=0;
	this->terminal.c_cc[VMIN]=0;
	this->terminal.c_cc[VTIME]=0;
	cfsetospeed(&this->terminal,baud);
	cfsetispeed(&this->terminal,baud);
	tcsetattr(fd, TCSANOW, &this->terminal);

	if (tcflush(fd,TCIFLUSH)==-1)
	{
		this->ui->Message(ATUIMessageType::Error, "Serial failed with TCIFLUSH");
		return false;
	}

	if (tcflush(fd,TCOFLUSH)==-1)
	{
		this->ui->Message(ATUIMessageType::Error, "Serial failed with TCOFLUSH");
		return false;
	}

	if (tcsetattr(fd,TCSANOW,&this->terminal)==-1)
	{
		this->ui->Message(ATUIMessageType::Error, "Serial failed with TCSANOW");
		return false;
	}

	this->ui->Message(ATUIMessageType::Success, "Serial is open");
	this->terminalFile = fd;

	return true;
}

bool ATTermios::CloseSerial(){
	if (this->terminalFile != -1)
	{
		this->ui->Message(ATUIMessageType::Message, "Closing serial file");
		close(this->terminalFile);
	}
}

bool ATTermios::Write(char* command)
{
	this->ui->Message(ATUIMessageType::SendCommand, command);
	write(this->terminalFile, command, strlen(command));

	return true;
}

char* ATTermios::Read(int miliseconds)
{
	char* response;
	char buff = 0;
	int chars = 0;
	int index = 0;

	response = (char*)malloc(256);
	memset(response, 0, 256);

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds;

	do {
		chars = read(this->terminalFile, &buff, 1);
		if (chars == 0){
			end = std::chrono::system_clock::now();
			elapsed_seconds = end - start;

			continue;
		}

		sprintf(&response[index], "%c", buff);
		index += chars;
	} while(chars > 0 || elapsed_seconds.count()*1000 < miliseconds);

	this->ui->Message(ATUIMessageType::ReadCommand, response);

	return response;
}

void ATTermios::SetUI(IATUI* ui){
	this->ui = ui;
}
