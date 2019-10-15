#ifndef JSPACKETPRV_H
#define JSPACKETPRV_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

template <class T>
T
findIn(const QString& string, const QVector<QPair<T, QString>>& typestrs) {
    for (auto typestr : typestrs) {
        if (typestr.second == string)
            return typestr.first;
    }
    return static_cast<T>(-1);
}

template <class T>
QString
findIn(const T& type, const QVector<QPair<T, QString>>& typestrs) {
    for (auto typestr : typestrs) {
        if (typestr.first == type)
            return typestr.second;
    }
    return "";
}

enum class Status { Request = 0, Response = 1, Event = 2, Error = -1 };

template <
    class Cmd,
    class Act,
    class Err,
    class Field,
    auto& cmdstrs,
    auto& actstrs,
    auto& errstrs,
    auto& fldstrs>
struct JsPacketPrv : public QJsonObject {
    JsPacketPrv(const QJsonObject& jsobj) : QJsonObject(jsobj) {}
    JsPacketPrv(const JsPacketPrv& request) : QJsonObject() {
        insert("cmd", request.value("cmd"));
        insert("act", request.value("act"));
        insert("stan", request.value("stan"));
    }
    JsPacketPrv() {}
    JsPacketPrv&
    insert(const QString& field, const QJsonValue& val, bool insertIf = true) {
        if (!insertIf)
            return *this;
        auto content = value("content").toObject();
        content.insert(field, val);
        QJsonObject::insert("content", content);
        return *this;
    }
    JsPacketPrv& insert(Cmd command) {
        auto str = findIn(command, cmdstrs);
        QJsonObject::insert("cmd", str.isEmpty() ? "unknown_cmd" : str);
    }
    JsPacketPrv& insert(Act action) {
        auto str = findIn(action, actstrs);
        QJsonObject::insert("act", str.isEmpty() ? "unknown_act" : str);
    }
    JsPacketPrv& insert(Err error) {
        auto str = findIn(error, fldstrs);
        insert(Status::Error);
        QJsonObject::insert("error_code", error);
        QJsonObject::insert("error_message", str == "" ? "unknown_err" : str);
    }
    JsPacketPrv& insert(Status status) {
        QJsonObject::insert("status", static_cast<int>(status));
        return *this;
    }
    JsPacketPrv& stan(QString stan) {
        QJsonObject::insert("stan", stan);
        return *this;
    }

    JsPacketPrv&
    insert(Field field, const QJsonValue& val, bool insertIf = true) {
        if (!insertIf)
            return *this;
        auto str = findIn(field, fldstrs);
        return insert(str == "" ? "unknown_field" : str, val);
    }
    QJsonValue contentValue(const QString& str) {
        return QJsonObject::value("content").toObject().value(str);
    }
    QJsonValue contentValue(Field f) {
        return contentValue(findIn(f, fldstrs));
    }
};

#endif // JSPACKETPRV_H
