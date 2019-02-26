#ifndef NETWORKRELATELOGIC_H
#define NETWORKRELATELOGIC_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include <QNetworkReply>


class NetWorkRelateLogic : public QObject
{
    Q_OBJECT
public:
    explicit NetWorkRelateLogic(QObject *parent = nullptr);

	QString genLoginurl(QString user, QString pass);

signals:

public slots:
    void onLogin(QString user, QString pass);
    void httpRequestFinish(QNetworkReply* reply);

private:
    QNetworkAccessManager _manager;
	QString _loginUrl = "http://127.0.0.1:1000/?";
};

#endif // NETWORKRELATELOGIC_H
