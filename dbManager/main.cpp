
#include "WebManagerPair.h"
#include "NngRelateManager.h"
#include "MessageQueueManager.h"
#include "DumpManager.h"
#include "ProcManager.h"
#include "LogInit.h"

void custom_main()
{
	InitLogMoudle();
	std::shared_ptr<MessageQueueManager> msgqueue = std::make_shared<MessageQueueManager>();
	std::shared_ptr<ProcManagerInterface> procmanager = std::make_shared<ProcManager>();
	WebManagerPair wmanager;
	NngRelateManager nngmanager;
	procmanager->InitProcManager(&wmanager, 4);
	msgqueue->InitMessageQueueManager(procmanager);
	wmanager.InitWsManagerPair("127.0.0.1", 1000, msgqueue);
	nngmanager.startNngRelateManager();
	char ch;
	std::cin >> ch;
}

int main(int argc, char* argv[])
{
	DumpManager dump;
	custom_main();
	return 0;
}

#include "winmain-inl.h"