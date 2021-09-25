#ifndef ATTERMIOS_H_
#define ATTERMIOS_H_

#include "IATUI.h"
#include <string>
#include <termios.h>

class ATTermios : public IATTerminal {
public:
	bool OpenSerial(char* serialName, speed_t baud);
	bool CloseSerial();
	bool Write(char* command);
	char* Read(int seconds);

	virtual void SetUI(IATUI* ui);
private:
	struct termios terminal;
	int terminalFile;
	IATUI* ui;
	char data[256];
	int size = 256;
	int timeout_usec = 10000;
};

#endif
