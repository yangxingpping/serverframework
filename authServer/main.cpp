
#include "HttpManagerPair.h"
#include "AuthProcThread.h"
#include "MessageQueueManager.h"
#include "DumpManager.h"
#include "ProcManager.h"
#include "ProcessMessageImpl.h"
#include "LogInit.h"

void custom_main()
{
	InitLogMoudle();
	std::shared_ptr<MessageQueueManager> msgqueue = std::make_shared<MessageQueueManager>();
	std::shared_ptr<ProcManagerInterface> procmanager = std::make_shared<ProcManager>();
	HttpManagerPair wmanager;
	procmanager->InitProcManager(4);
	for (int i = 0; i < 1; ++i)
	{
		auto logicunit = new AuthProcThread(std::make_shared<ProcessMessageImpl>());
		procmanager->AddBackendUnit(logicunit);
	}
	msgqueue->InitMessageQueueManager(procmanager);
	wmanager.InitWebManager("127.0.0.1", 1000, msgqueue);

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