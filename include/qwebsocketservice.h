#ifndef QWEBSOCKETSERVICE_H
#define QWEBSOCKETSERVICE_H

#include "connectionprv.h"
#include "jspacketprv.h"

#include <QDebug>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWebSocketServer>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>


template <class Cmd, class Act, class Err, class Field, class UserTypeFlag>
class QWebSocketService
{

public:
    static QVector<QPair<Cmd, QString>>   cmdstrs;
    static QVector<QPair<Act, QString>>   actstrs;
    static QVector<QPair<Err, QString>>   errstrs;
    static QVector<QPair<Field, QString>> fldstrs;

    using THIS    = QWebSocketService<Cmd, Act, Err, Field, UserTypeFlag>;
    using UsrType = int;
    enum SpecialCases { ConnectionStablished, BadRequest, AccessDenied };
    using JsPacket =
        JsPacketPrv<Cmd, Act, Err, Field, cmdstrs, actstrs, errstrs, fldstrs>;
    using Connection = ConnectionPrv<JsPacket, Err>;
    using CallId     = std::pair<Cmd, Act>;

private:
    using CallBack = std::function<void(Connection&, const JsPacket&)>;
    using RouteObj = std::pair<CallBack, UsrType>;


public:
    QWebSocketService() {}
    void start(quint16 wsPort = 0) {
        if (m_wsserver.listen(QHostAddress::Any, wsPort == 0 ? 1441 : wsPort))
            qDebug() << "WsServer started on port" << m_wsserver.serverPort();
        else {
            qDebug() << "***Err: ws server not started";
            return;
        }

        QObject::connect(
            &m_wsserver, &QWebSocketServer::newConnection, [this]() {
                while (m_wsserver.hasPendingConnections()) {
                    auto* socket     = m_wsserver.nextPendingConnection();
                    auto* connection = new Connection{socket};
                    connection->init(*this, m_routemap, m_specialRouteMap);
                    QObject::connect(
                        socket,
                        &QWebSocket::disconnected,
                        [this, connection]() {
                            m_connections.remove(*connection);
                        });
                    m_connections.push_back(*connection);
                }
            });
    }
    QWebSocketService& route(Cmd c, Act a, CallBack f, UsrType userType = 0) {
        CallId cid{c, a};
        m_routemap.insert(cid, std::make_pair(f, userType));
        return *this;
    }

    QWebSocketService& route(SpecialCases page, CallBack f) {
        m_specialRouteMap.insert(page, f);
        return *this;
    }
    QWebSocketServer& server() {
        return m_connections;
    }

private:
    std::list<Connection> m_connections;
    QWebSocketServer m_wsserver{"wsserver", QWebSocketServer::NonSecureMode};
    QMap<CallId, RouteObj>       m_routemap;
    QMap<SpecialCases, CallBack> m_specialRouteMap{
        {SpecialCases::ConnectionStablished,
         [](const Connection&, const JsPacket&) {}},
        {SpecialCases::BadRequest,
         [](const Connection& c, const JsPacket&) {
             c.sendMessage("Bad request - register bad request using "
                           "server.registerSpecial()");
         }},
        {SpecialCases::AccessDenied, [](const Connection& c, const JsPacket&) {
             c.sendMessage("Access denied - register access denied using "
                           "server.registerSpecial()");
         }}};
};

template <class Cmd, class Act, class Err, class Field, class UserTypeFlag>
QVector<QPair<Cmd, QString>>
    QWebSocketService<Cmd, Act, Err, Field, UserTypeFlag>::cmdstrs;

template <class Cmd, class Act, class Err, class Field, class UserTypeFlag>
QVector<QPair<Act, QString>>
    QWebSocketService<Cmd, Act, Err, Field, UserTypeFlag>::actstrs;

template <class Cmd, class Act, class Err, class Field, class UserTypeFlag>
QVector<QPair<Err, QString>>
    QWebSocketService<Cmd, Act, Err, Field, UserTypeFlag>::errstrs;

template <class Cmd, class Act, class Err, class Field, class UserTypeFlag>
QVector<QPair<Field, QString>>
    QWebSocketService<Cmd, Act, Err, Field, UserTypeFlag>::fldstrs;

#endif // QWEBSOCKETSERVICE_H
