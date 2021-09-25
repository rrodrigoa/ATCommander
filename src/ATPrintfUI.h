#ifndef ATPRINTFUI_H_
#define ATPRINTFUI_H_

#include "IATUI.h"
#include <string>

using namespace std;

class ATPrintfUI : public IATUI{
	public:
		ATPrintfUI();
		void Message(ATUIMessageType type, char* message);
		void CarrierName(char* carrierName);
};

#endif
