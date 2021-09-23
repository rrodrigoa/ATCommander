#ifndef ATPRINTFUI_H_
#define ATPRINTFUI_H_

#include "IATUI.h"
#include <string>

using namespace std;

class ATPrintfUI : public IATUI{
	public:
		ATPrintfUI();
		void StartUpMessage(ATUIMessageType type, std::string message);
};

#endif
