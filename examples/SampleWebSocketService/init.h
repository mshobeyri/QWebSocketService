#ifndef INIT_H
#define INIT_H

#include "qwebsocketservice.h"

enum class Command { Get, Auth };
enum class Action { User, Page, UserData, AllUsersData, DemoVideo, Nonce };
enum class Error { AccessDenied, BadRequest };
enum class Field { Auth, Nonce };
enum UserType {
    Any              = 0,
    DemoUser         = 1,
    SuperUser        = 2,
    Admin            = 4,
    AnyAuthenticated = DemoUser | SuperUser | Admin
};

enum class Page {
    Home,
    First,
};


using WebSocketService =
    QWebSocketService<Command, Action, Error, Field, UserType>;

void
init() {
    WebSocketService::cmdstrs = {{Command::Get, "get"},
                                 {Command::Auth, "auth"}};

    WebSocketService::actstrs = {{Action::User, "user"},
                                 {Action::Page, "page"},
                                 {Action::UserData, "userdata"},
                                 {Action::AllUsersData, "allusersdata"},
                                 {Action::DemoVideo, "demoVideo"},
                                 {Action::Nonce, "nonce"}};

    WebSocketService::errstrs = {{Error::AccessDenied, "access denied"},
                                 {Error::BadRequest, "bad request"}};

    WebSocketService::fldstrs = {{Field::Auth, "auth"},
                                 {Field::Nonce, "nonce"}};
}

#endif // INIT_H
