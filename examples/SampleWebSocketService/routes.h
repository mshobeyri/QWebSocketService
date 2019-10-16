#ifndef ROUTES_H
#define ROUTES_H

#include "init.h"
#include <QJsonArray>

void
authenticate(
    WebSocketService::Connection& c, const WebSocketService::JsPacket& data) {
    auto hashAgorithem = [](QString pass, QString salt, QString nonce) {
        return pass + salt + nonce; // user better hash algorithm
    };
    auto getUserData = [](const QString& username) {
        // query to db to find user data using username
        if (username == "")
            return QVector{"passwordOfSomeUser", "saltOfSomeUser", "userType"};
        else
            return QVector{"passwordOfSomeUser", "saltOfSomeUser", "userType"};
    };

    auto userDate = getUserData(data.value("username").toString());
    if (data.value("hash").toString() ==
        hashAgorithem(
            userDate[0], userDate[1], QString::number(c.user.nonce))) {
        c.user.permitions = QString(userDate[2]).toInt();
    }
    WebSocketService::JsPacket res;
    res.insert(Field::Auth, true);
    c.sendRespond(res);
}

void
connectionEstablished(
    const WebSocketService::Connection& c,
    const WebSocketService::JsPacket&   req) {
    WebSocketService::JsPacket res(req);
    res.insert("message", "wellcome");
    c.sendPacket(res);
}
void
accessDenied(
    WebSocketService::Connection& c, const WebSocketService::JsPacket&) {
    c.sendError(Error::AccessDenied);
}
void
getDemoVideo(
    const WebSocketService::Connection& c,
    const WebSocketService::JsPacket&   req) {
    WebSocketService::JsPacket res{req};
    // return static path
    res.insert("video_address", "https://some_host.com/some_address");
    c.sendRespond(res);
}
void
userSelfData(
    const WebSocketService::Connection& c,
    const WebSocketService::JsPacket&   req) {
    WebSocketService::JsPacket res{req};
    // query db for user data
    res.insert(Field::Name, "Mehrdad");
    res.insert(Field::Family, "Shobeyri");
    c.sendRespond(res);
}
void
otherUsersData(
    const WebSocketService::Connection& c,
    const WebSocketService::JsPacket&   req) {
    // query db for other users data
    QJsonArray                 users;
    WebSocketService::JsPacket user;
    user.insert(Field::Name, "Mehrdad");
    user.insert(Field::Family, "Shobeyri");
    // and so on

    users.append(user);

    WebSocketService::JsPacket res{req};
    res.insert(Field::Users, users);
    c.sendRespond(res);
}

#endif // ROUTES_H

