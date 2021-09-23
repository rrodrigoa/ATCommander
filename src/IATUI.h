#ifndef IATUI_H_ _
#define IATUI_H_ _

#include <string>

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

#endif
