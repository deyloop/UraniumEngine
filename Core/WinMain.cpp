#include <Windows.h>
#include "WindowsOSFramework.h"
#include "Core.h"
#include "ErrorCode.h"
using namespace u92;
using namespace u92::core;

/*-----------------------------------------------------------------------------
Starting point of the Application. Initializes the OSFramework and Core of the
Engine, then sets it off. Performs cleanup afterwords.
PARAMETERS:
[IN] intance		-	Handle to the current instnace of the application.
[IN] prevInstance	-	Handle to the previous instance of the application.
[IN] cmdLine		-	Contents of the command line arguments.
[IN] cmdShow		-	Mode in which the application window should be opened
in.
RETURNS:
0	successfull exit
1	Initial Memory Allocation Failed
2	Initialisation of OS Framework failed
3	Initialisation of the Engine Core failed
-----------------------------------------------------------------------------*/
int WINAPI WinMain (HINSTANCE instance,HINSTANCE prevInstance,
					LPSTR cmdLine,int cmdShow) {
	WindowsOSFramework* pOSFramework = WindowsOSFramework::getWindowsInstance ( );
	Core*				pCore = new Core;

	//neccessary checks
	if (!pOSFramework||!pCore) return 1;

	int result;
	result = pOSFramework->init (instance);
	if (result!=E_CODE_SUCCESS) {
		if (result==E_CODE_REGISTERCLASSFAILED) {
			MessageBox (NULL,"Registration of Window Class failed","Fatal Error",NULL);
		}
		return 2;
	}

	result = pCore->init ();
	if (result!=E_CODE_SUCCESS) {
		return 3;
	}

	//Off we go!
	pCore->run ( );

	//We reach here only when the game is closing
	pCore->release ( );
	delete pCore;
	return 0;
}
