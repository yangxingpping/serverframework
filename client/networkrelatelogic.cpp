#include "networkrelatelogic.h"

#include <iostream>
#include <string>
#include <QNetworkRequest>
#include <QUrl>

NetWorkRelateLogic::NetWorkRelateLogic(QObject *parent) : QObject(parent)
{
    connect(&_manager, SIGNAL(finished(QNetworkReply*)),
                     SLOT(httpRequestFinish(QNetworkReply*)));
}

void NetWorkRelateLogic::onLogin(QString user, QString pass)
{
    using namespace std;
    cout << user.toStdString() << ":" << pass.toStdString() << endl;
    QUrl url("http://127.0.0.1:1000/?0{}");
    QNetworkRequest request(url);
    _manager.get(request);
}

void NetWorkRelateLogic::httpRequestFinish(QNetworkReply* reply)
{
	using namespace std;
	std::string strResp = reply->readAll().toStdString();
	cout << strResp << endl;
}
