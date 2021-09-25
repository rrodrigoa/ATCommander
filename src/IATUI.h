#ifndef IATUI_H_ _
#define IATUI_H_ _

#include <string>

using namespace std;

enum ATUIMessageType
{
	Message = 0,
	Error = 1,
	Success = 2,
	SendCommand = 3,
	ReadCommand = 4
};

class IATUI{
public:
	virtual void Message(ATUIMessageType type, char* message) = 0;
	virtual void CarrierName(char* carrierName) = 0;
};

#endif
