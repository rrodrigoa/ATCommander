#ifndef ATTERMINAL_H_
#define ATTERMINAL_H_

#include "IATUI.h"
#include <string>
#include <termios.h>

class IATTerminal {
public:
	virtual bool OpenSerial(char* serialName, speed_t baud) = 0;
	virtual bool CloseSerial() = 0;
	virtual bool Write(char* command) = 0;
	virtual char* Read(int seconds) = 0;

	virtual void SetUI(IATUI* ui) = 0;
};

#endif
