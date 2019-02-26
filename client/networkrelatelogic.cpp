
#include "networkrelatelogic.h"
#include "share_def.h"
#include "PackUtils.h"

#include "nlohmann/json.hpp"
#include "cppcodec/base32_crockford.hpp"
#include "cppcodec/base64_rfc4648.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <QNetworkRequest>
#include <QUrl>

NetWorkRelateLogic::NetWorkRelateLogic(QObject *parent) : QObject(parent)
{
    connect(&_manager, SIGNAL(finished(QNetworkReply*)),
                     SLOT(httpRequestFinish(QNetworkReply*)));
}

QString NetWorkRelateLogic::genLoginurl(QString user, QString pass)
{
	using base64 = cppcodec::base64_rfc4648;
	QString ret = _loginUrl + "?";// "login?";
	//ret += "user=" + user + "&pass=" + pass;
	using json = nlohmann::json;
	EPackType type = EPackType::JSONPACK;
	MainClassify maincmd = MainClassify::HallLogic;
	HallAssistRequest asscmd = HallAssistRequest::UserLogin;
	json juser;
	bool bConvert = false;
	int userid = user.toInt(&bConvert);
	juser["userid"] = userid;
	juser["password"] = pass.toStdString();
	//ret += QString::fromStdString(std::to_string((int)type));
	std::string cmds = std::to_string((int)type);
	cmds += PackUtils::PackMainAssCmd(MainCmdType(maincmd), UndefineAssist(asscmd));
	cmds += juser.dump();
	auto decodedata = base64::encode(cmds);
	
	//ret += cmds.data();
	//ret += QString::fromStdString(juser.dump());
	ret += decodedata.data(); //cmds.data();
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
