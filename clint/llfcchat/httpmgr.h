#ifndef HTTPMGR_H
#define HTTPMGR_H

#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

//CRTP
class HttpMgr:public QObject, public Singleton<HttpMgr>,
              public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private:
    /*_instance = shared_ptr<T>(new T);
    使Singleton可以访问子类的构造函数*/
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;

public slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

signals:
    //连接http请求完成信号
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    //注册模块http相关请求完成发送此信号
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    //重置模块http相关请求完成发送此信号
    void sig_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    //登录模块http相关请求完成发送此信号
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMGR_H
