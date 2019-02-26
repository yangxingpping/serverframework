
#include "WebManagerPair.h"
#include "NngRelateManager.h"
#include "MessageQueueManager.h"
#include "DumpManager.h"

void custom_main()
{
	std::shared_ptr<MessageQueueManager> msgqueue = std::make_shared<MessageQueueManager>();
	WebManagerPair wmanager;
	NngRelateManager nngmanager;
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