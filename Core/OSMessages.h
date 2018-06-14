#pragma once
#ifndef _OS_MESSAGES_H_
#define _OS_MESSAGES_H_
namespace u92 {

	enum WindowCommandType {
		WINDOWCOMMAND_NONE = 0,
		WINDOWCOMMAND_CREATEWINDOW
	};

	struct CreateWindowCommand {
		char title[20];
		int width,height;
		bool fullscreen;
	};

	struct WindowCommand {
		WindowCommandType type;
		union {

			CreateWindowCommand createWindow;

		} command;
	};

	struct InputCommand {

	};
}

#endif  