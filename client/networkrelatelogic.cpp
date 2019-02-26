
#include "networkrelatelogic.h"
#include "share_def.h"
#include "PackUtils.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <string>
#include <QNetworkRequest>
#include <QUrl>

NetWorkRelateLogic::NetWorkRelateLogic(QObject *parent) : QObject(parent)
{
    connect(&_manager, SIGNAL(finished(QNetworkReply*)),
                     SLOT(httpRequestFinish(QNetworkReply*)));
}

QString NetWorkRelateLogic::genLoginurl(QString user, QString pass)
{
	QString ret=_loginUrl;
	using json = nlohmann::json;
	EPackType type = EPackType::JSONPACK;
	MainClassify maincmd = MainClassify::HallLogic;
	HallAssistRequest asscmd = HallAssistRequest::UserLogin;
	json juser;
	juser["user"] = user.toStdString();
	juser["pass"] = pass.toStdString();
	ret += QString::fromStdString(std::to_string((int)type));
	
	auto cmds = PackUtils::PackMainAssCmd(MainCmdType(maincmd), UndefineAssist(asscmd));
	ret += cmds.data();
	ret += QString::fromStdString(juser.dump());
	return ret;
}

void NetWorkRelateLogic::onLogin(QString user, QString pass)
{
    using namespace std;
    cout << user.toStdString() << ":" << pass.toStdString() << endl;
	auto strRequest = genLoginurl(user, pass);
    QUrl url(strRequest);
    QNetworkRequest request(url);
    _manager.get(request);
}

void NetWorkRelateLogic::httpRequestFinish(QNetworkReply* reply)
{
	using namespace std;
	std::string strResp = reply->readAll().toStdString();
	cout << strResp << endl;
}
