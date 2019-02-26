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

signals:

public slots:
    void onLogin(QString user, QString pass);
    void httpRequestFinish(QNetworkReply* reply);

private:
    QNetworkAccessManager _manager;
};

#endif // NETWORKRELATELOGIC_H
