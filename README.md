# QWebSocketService

Header only lib for create websocket service easily with simple user management.

### Depends on
- Qt
 - C++17
### Build

 Just include .pri file in your project .pro file.
```c++
include(QWebSocketService/QWebSocketService.pri)
```

### How to use
##
```c++
//-- create enums for Command Action Error and Fields you want use.
enum class Command { Get, Auth };
enum class Action { User, Page, UserData, AllUsersData, DemoVideo, Nonce };
enum class Error { AccessDenied, BadRequest };
enum class Field { Auth, Nonce, Name, Family, Users };
enum UserType {
    Any              = 0,
    DemoUser         = 1,
    SuperUser        = 2,
    Admin            = 4,
    AnyAuthenticated = DemoUser | SuperUser | Admin
};

//-- Create your server type using this context.
using WebSocketService =
    QWebSocketService<Command, Action, Error, Field, UserType>;

//-- Make a pair between your types enums and an string like this.
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
                                 {Field::Nonce, "nonce"},
                                 {Field::Name, "name"},
                                 {Field::Family, "family"},
                                 {Field::Users, "users"}};
}

//-- Some function with void (Connection& c, const JsPacket& req) signature
void
connectionEstablished(
    const WebSocketService::Connection& c,
    const WebSocketService::JsPacket&   req) {
    WebSocketService::JsPacket res(req);
    res.insert("message", "wellcome");
    c.sendPacket(res);
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

int
main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    init();
    WebSocketService service;
    // route special states and functoins to tell what to do when special cmd act recieved.
    service
        .route(WebSocketService::ConnectionEstablished, connectionEstablished)
        .route(
            WebSocketService::AccessDenied,
            [](WebSocketService::Connection& c,
               const WebSocketService::JsPacket&) {
                c.sendError(Error::AccessDenied);
            })
        .route(
            Command::Get,
            Action::UserData,
            userSelfData,               // <<- route function
            UserType::AnyAuthenticated) // <<- last parameter of route is users access.
                                        // you can see how to automate check user access
                                        // in example project.
        .route(
            Command::Get,
            Action::AllUsersData,
            otherUsersData,
            UserType::AnyAuthenticated);

    service.start();
    return a.exec();
}
```

##### Example
Connecting to ws://127.0.0.1:1441
Request:
```json
{
  "cmd":"get",
  "act":"userdata"
}
```
Response:
```json
{
  "act": "userdata",
  "cmd": "get",
  "content": {
    "family": "Shobeyri",
    "name": "Mehrdad"
  },
  "status": 1
}
```
### Todos

 - Write Unit test
 - Make it crossplatform service 

License
----

MIT


