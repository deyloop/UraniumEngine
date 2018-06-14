#pragma once
#ifndef _ERROR_CODE_H_
#define _ERROR_CODE_H_
namespace u92 {
	enum ErrorCode {
		E_CODE_SUCCESS = 0,

		//codes for OSFramework
		E_CODE_UNKOWN_SUBSYSTEM,
		E_CODE_QUIT_MESSAGE,
		E_CODE_REGISTERCLASSFAILED
	};
}

#endif