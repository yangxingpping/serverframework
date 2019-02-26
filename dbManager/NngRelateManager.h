#pragma once

#include "CustomForwardDeclare.h"

#include <memory>
#include <string>
#include <thread>

class NngRelateManager
{
public:
	NngRelateManager(std::string pubaddr = "tcp://127.0.0.1:100", std::string pulladdr = "tcp://127.0.0.1:101");
	bool startNngRelateManager();
	void stopNngRelateManager();
	void addPubMessage(std::shared_ptr<std::string> msg);

	virtual ~NngRelateManager();

	void pubThreadFunc();
	void pullThreadFunc();

private:
	std::shared_ptr<moodycamel::ConcurrentQueue<std::shared_ptr<std::string>, moodycamel::ConcurrentQueueDefaultTraits>> _pubQueue;
	std::shared_ptr<std::thread> _pubThread = nullptr;
	std::string _pubAddress = "tcp://127.0.0.1:100";
	std::shared_ptr<std::thread> _pullThread = nullptr;
	std::string _pullAddress = "tcp://127.0.0.1:101";
	std::shared_ptr<DBManager> _dbManager = nullptr;
};

