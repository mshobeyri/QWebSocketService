#include "init.h"
#include "routes.h"
#include <QCoreApplication>
#include <QTimer>

int
main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    init();
    WebSocketService service;

    service.route(WebSocketService::ConnectionEstablished, connectionEstablished)
        .route(WebSocketService::AccessDenied, accessDenied)
        .route(
            WebSocketService::BadRequest,
            [](WebSocketService::Connection& c,
               const WebSocketService::JsPacket&) {
                c.sendError(Error::BadRequest);
            })
        .route(
            Command::Get,
            Action::Nonce,
            [](WebSocketService::Connection& c,
               const WebSocketService::JsPacket&) {
                int nonce    = 75757; // randomNumber
                c.user.nonce = nonce;
                WebSocketService::JsPacket res;
                res.insert(Field::Nonce, nonce).insert("nonce", 10, nonce == 5);
                c.sendRespond(res);
            })
        .route(
            Command::Auth,
            Action::User,
            authenticate,
            UserType::DemoUser | UserType::SuperUser)
        .route(
            Command::Get, Action::DemoVideo, getDemoVideo, UserType::DemoUser)
        .route(
            Command::Get,
            Action::UserData,
            userSelfData,
            UserType::AnyAuthenticated)
        .route(
            Command::Get,
            Action::AllUsersData,
            otherUsersData,
            UserType::AnyAuthenticated);

    service.start();

    return a.exec();
}
