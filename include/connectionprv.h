#ifndef CONNECTIONPRV_H
#define CONNECTIONPRV_H

#include "jspacketprv.h"
#include <QJsonArray>
#include <QWebSocket>


template <class JsPacket, class Err> struct ConnectionPrv {
    ConnectionPrv(QWebSocket* ws) : m_ws(ws) {
        //        static quint32 id = 0;
        //        m_id              = id;
        //        id++;
        //        if (id > std::numeric_limits<quint16>::max())
        //            id = 0;
    }
    bool operator==(const ConnectionPrv& lhd) {
        return lhd.id() == this->id();
    }
    ~ConnectionPrv() {}
    template <class WSService, class Map, class SMap>
    void init(WSService&, Map& routemap, SMap& sroutemap) {
        QObject::connect(
            m_ws,
            &QWebSocket::textMessageReceived,
            [this, &routemap, &sroutemap](const QString& textmessage) {
                auto handleReq = [this, &routemap, &sroutemap](
                                     const QJsonObject& mObj) {
                    typename WSService::CallId cid = std::make_pair(
                        findIn(
                            mObj.value("cmd").toString(), WSService::cmdstrs),
                        findIn(
                            mObj.value("act").toString(), WSService::actstrs));

                    if (!routemap.contains(cid)) {
                        sroutemap.value(WSService::SpecialCases::BadRequest)(
                            *this, JsPacket{});
                        return;
                    }

                    auto& f = routemap.value(cid);
                    auto  j = JsPacket{mObj};
                    if ((f.second == 0 || (f.second & this->user.permitions)) <=
                        0) {
                        sroutemap.value(WSService::SpecialCases::AccessDenied)(
                            *this, JsPacket{});
                        return;
                    }

                    f.first(*this, j);
                };

                auto mVars = QJsonDocument::fromJson(textmessage.toUtf8());
                if (mVars.isArray()) {
                    for (auto mVar : mVars.array()) {
                        handleReq(mVar.toObject());
                    }
                } else {
                    handleReq(mVars.object());
                }
            });
    }
    void sendRespond(JsPacket& packet) const {
        packet.insert(Status::Response);
        sendPacket(packet);
    }
    void sendEvent(JsPacket& packet) const {
        packet.insert(Status::Event);
        sendPacket(packet);
    }
    void sendError(Err error) {
        JsPacket packet;
        packet.insert(error);
        sendPacket(packet);
    }
    void sendPacket(const JsPacket& packet) const {
        sendMessage(QJsonDocument{packet}.toJson());
    }
    void sendMessage(const QString& data) const {
        m_ws->sendTextMessage(data);
    }

    QString stan() {
        if (user.stan > std::numeric_limits<uint>::max())
            user.stan = 0;
        user.stan++;
        return QString::number(user.stan);
    }
    quint32 id() const {
        return m_id;
    }

    struct User {
        QString username;
        int     permitions = 0;
        int     nonce;
        uint    stan = 0;
    } user;

private:
    QWebSocket* m_ws;
    quint32     m_id;
};


#endif // CONNECTIONPRV_H
