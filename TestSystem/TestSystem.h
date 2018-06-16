#pragma once
#ifndef _TEST_SYSTEM_H_
#define _TEST_SYSTEM_H_

#include <System.h>
#include <WindowEvent.h>
#include <TickMessage.h>

using namespace u92;

class TestSystem : public System {
public:
	void init ( OSFramework* pOS );
	void release ( );

	void threadInit ( );
private:
	void handleWindowMessage (const WindowEvent event);
	void render (const TickMessage msg);

	OSFramework* m_pOS;
};

#ifdef URANIUM_EXPORTS
#define U92_API __declspec(dllexport) 
#else
#define U92_API __declspec(dllimport) 
#endif

extern "C" {
U92_API System* getSystemInstance ( );
}
#endif