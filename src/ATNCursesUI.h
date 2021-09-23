#ifndef ATNCURSESUI_H_ _
#define ATNCURSESUI_H_ _

#include "IATUI.h"
#include <string>

using namespace std;

class ATNCursesUI : public IATUI{
	public:
		ATNCursesUI();
		void StartUpMessage(ATUIMessageType type, std::string message);
};

#endif
