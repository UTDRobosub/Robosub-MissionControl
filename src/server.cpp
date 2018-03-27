#include <librobosub/robosub.h>

using namespace robosub;
using WsServer = robosub::ws::SocketServer<robosub::ws::WS>;
using WsClient = robosub::ws::SocketClient<robosub::ws::WS>;


#include <stdio.h>
//#include <sys/ioctl.h> // For FIONREAD
//#include <termios.h>
#include <stdbool.h>
#include <stdlib.h> //rand
#include "main.h"

struct ConnectionState {
public:
  bool ready = false;
  long lastSend = 0;
  long rtt = 0;
};

template <class T>
void send(shared_ptr<typename T::Connection> connection, 
          ConnectionState &connectionState,
          DataBucket &connectionData,
          DataBucket &current,
          unsigned long milliseconds_since_epoch,
          bool compress
){
      if (!connectionState.ready) return;

      //compress data
      DataBucket previousState = connectionData;
      DataBucket compressed = current.compress(previousState);

      //skip if no changes
      if (compressed.toJson().empty()) return;

      //update time-dependent values and recompress
      DataBucket sentState = current;
      sentState["time"] = milliseconds_since_epoch;
      sentState["rtt"] = connectionState.rtt;
      sentState["controllerTime"] = controllerTime;
      compressed = sentState.compress(previousState);

      //set new connection state
      connectionData = current;

      //update connection state
      connectionState.ready = false;
      connectionState.lastSend = robosub::Time::millis();

      //check if better to send as compressed or uncompressed
      //cout << current.toString().length() << " " << compressed.toString().length() << endl;
      //cout << "sending" << current << endl;
      if (!compress || current.toString().length() < compressed.toString().length())
      {
        //better to send uncompressed
        auto send_stream = make_shared<typename T::SendStream>();
        *send_stream << sentState;
        connection->send(send_stream);
      } else {
        //send compressed
        auto send_stream = make_shared<typename T::SendStream>();
        *send_stream << compressed;
        connection->send(send_stream);
      }
}

void server() {
  //prepare buckets to store data
  DataBucket current;
  DataBucket toRobot;

  //initialize server
  WsServer server;
  server.config.port = 8080;
  server.config.thread_pool_size = 1;
  //server.config.address = "0.0.0.0";

  //endpoint state storage
  std::map<shared_ptr<WsServer::Connection>, DataBucket> serverConnectionData;
  std::map<shared_ptr<WsServer::Connection>, ConnectionState> serverConnectionState;
  DataBucket clientConnectionData;
  ConnectionState clientConnectionState;
  bool clientConnected = false;

  //GET "/": root endpoint
  auto &root = server.endpoint["^/?$"];
  //GET "/": on open
  //When opened, server should send initial state message
  root.on_open = [&serverConnectionState,&serverConnectionData,&current](shared_ptr<WsServer::Connection> connection) {
    cout << "Server: Opened connection " << connection.get() << endl;
    //put current state into state storage
    serverConnectionData[connection] = current;
    serverConnectionState[connection] = ConnectionState();
    serverConnectionState[connection].lastSend = robosub::Time::millis();

    //convert current state to stream
    auto send_stream = make_shared<WsServer::SendStream>();
    *send_stream << serverConnectionData[connection];
    //send current state
    connection->send(send_stream, [](const robosub::ws::error_code &ec) {
      if(ec) {
        cout << "Server: Error sending message. " <<
            // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
            "Error: " << ec << ", error message: " << ec.message() << endl;
      }
    });
  };
  //GET "/": on message received
  //TODO send options information to another server
  root.on_message = [&serverConnectionState](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
    //Display message received
    auto message_str = message->string();
    if (message_str == "\x06") {
      serverConnectionState[connection].ready = true;
      serverConnectionState[connection].rtt = robosub::Time::millis() - serverConnectionState[connection].lastSend;
    } else if (message_str == "reset-controllers") {
      cout << "Resetting controllers" << endl;
      refresh = true;
    } else {
      cout << "Server: Message received: " << message_str << endl;
    }
  };
  //GET "/": on close
  root.on_close = [&serverConnectionState,&serverConnectionData](shared_ptr<WsServer::Connection> connection, int status, const string & /*reason*/) {
    serverConnectionState.erase(connection);
    serverConnectionData.erase(connection);
    // See RFC 6455 7.4.1. for status codes
    cout << "Server: Closed connection " << connection.get() << " with status code " << status << endl;
  };
  //GET "/": on error
  root.on_error = [&serverConnectionState,&serverConnectionData](shared_ptr<WsServer::Connection> connection, const robosub::ws::error_code &ec) {
    // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    serverConnectionState.erase(connection);
    serverConnectionData.erase(connection);
    cout << "Server: Error in connection " << connection.get() << ". "
         << "Error: " << ec << ", error message: " << ec.message() << endl;
  };

  thread server_thread([&server]() {
    // Start WS-server
    server.start();
  });






    //CLIENT
  //WsClient client("localhost:8081/echo");
  WsClient client("169.254.85.1:8081/");
  client.on_open = [&clientConnectionState,&clientConnectionData,&toRobot,&clientConnected](shared_ptr<WsClient::Connection> connection) {
    clientConnected = true;
    clientConnectionData = toRobot;
    clientConnectionState = ConnectionState();
    clientConnectionState.lastSend = robosub::Time::millis();
    cout << "Client: Opened connection" << endl;
    //convert current state to stream
    auto send_stream = make_shared<WsClient::SendStream>();
    *send_stream << clientConnectionData;
    //send current state
    connection->send(send_stream, [](const robosub::ws::error_code &ec) {
      if(ec) {
        cout << "Client: Error sending message. " <<
            "Error: " << ec << ", error message: " << ec.message() << endl;
      }
      });
  };

  client.on_message = [&current,&clientConnectionState](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::Message> message) {
    auto message_str = message->string();
    if (message_str == "\x06") {
      clientConnectionState.ready = true;
      clientConnectionState.rtt = robosub::Time::millis() - clientConnectionState.lastSend;
    } else {
        cout << "Client: Message received: \"" << message_str << "\"" << endl;
        DataBucket temp = DataBucket(message_str);
        current["rand"] = temp["rand"];
        current["robot_rtt"] = temp["rtt"];
        auto send_stream = make_shared<WsClient::SendStream>();
        *send_stream << "\x06";
        connection->send(send_stream);
    }
    //TODO handle telemetry from robot
    //parse json into fromRobot data bucket (or maybe current?)
  };


  client.on_close = [&clientConnected](shared_ptr<WsClient::Connection> /*connection*/, int status, const string & /*reason*/) {
    cout << "Client: Closed connection with status code " << status << endl;
    clientConnected = false;
  };

  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  client.on_error = [&clientConnected](shared_ptr<WsClient::Connection> /*connection*/, const robosub::ws::error_code &ec) {
    cout << "Client: Error: " << ec << ", error message: " << ec.message() << endl;
    clientConnected = false;
  };

  thread client_thread([&client]() {
    // Start WS-server
    client.start();
  });








  //wait one second for server to start
  robosub::Time::waitMillis(1000);
  cout << "Server started" << endl;

  int i=0;
  while(true) {
    current["index"] = (i++ / 1000) % 1000; //force refresh approx every second

    robosub::Time::waitMillis(100);

    unsigned long milliseconds_since_epoch = robosub::Time::millis();

    current["controller1"] = { };
    current["controller1"]["lx"] = controllerData[0];
    current["controller1"]["ly"] = controllerData[1];
    current["controller1"]["rx"] = controllerData[2];
    current["controller1"]["ry"] = controllerData[3];

    //current["controller1"]["dpad"] = controllerData[6];
    if(controllerData[6] == 1 || controllerData[6] == 3 || controllerData[6] == 9)
        current["controller1"]["up"] = 1;
    else
        current["controller1"]["up"] = 0;
    if(controllerData[6] == 2 || controllerData[6] == 3 || controllerData[6] == 6)
        current["controller1"]["right"] = 1;
    else
        current["controller1"]["right"] = 0;
    if(controllerData[6] == 4 || controllerData[6] == 6 || controllerData[6] == 12)
        current["controller1"]["down"] = 1;
    else
        current["controller1"]["down"] = 0;
    if(controllerData[6] == 8 || controllerData[6] == 12 || controllerData[6] == 9)
        current["controller1"]["left"] = 1;
    else
        current["controller1"]["left"] = 0;


    current["controller1"]["a"] = controllerData[7];
    current["controller1"]["b"] = controllerData[8];
    current["controller1"]["x"] = controllerData[9];
    current["controller1"]["y"] = controllerData[10];
    current["controller1"]["lb"] = controllerData[11];
    current["controller1"]["rb"] = controllerData[12];
    current["controller1"]["select"] = controllerData[13];
    current["controller1"]["start"] = controllerData[14];
    current["controller1"]["ldown"] = controllerData[15];
    current["controller1"]["rdown"] = controllerData[16];

    switch(controllerData[17]){
        case 1:
            current["controller1"]["mode"] = "D";
            current["controller1"]["lt"] = controllerData[4];
            current["controller1"]["rt"] = controllerData[5];
            current["controller1"]["connected"] = 1;
            break;
        case 2:
            current["controller1"]["connected"] = 1;
            current["controller1"]["mode"] = "X";
            current["controller1"]["t"] = controllerData[5]-controllerData[4];
            if(controllerData[5]-controllerData[4] > 2){
                current["controller1"]["lt"] = 0;
                current["controller1"]["rt"] = 1;
            }
            else if(controllerData[5]-controllerData[4] < -2){
                current["controller1"]["lt"] = 1;
                current["controller1"]["rt"] = 0;
            }
            else{
                current["controller1"]["lt"] = 0;
                current["controller1"]["rt"] = 0;
            }
            break;
        default:
            current["controller1"]["connected"] = 0;
            current["controller1"]["mode"] = "";
            break;
    }

    current["controller2"] = { };
    current["controller2"]["lx"] = controllerData[18];
    current["controller2"]["ly"] = controllerData[19];
    current["controller2"]["rx"] = controllerData[20];
    current["controller2"]["ry"] = controllerData[21];

    //current["controller2"]["dpad"] = controllerData[24];
    if(controllerData[24] == 1 || controllerData[24] == 3 || controllerData[24] == 9)
        current["controller2"]["up"] = 1;
    else
        current["controller2"]["up"] = 0;
    if(controllerData[24] == 2 || controllerData[24] == 3 || controllerData[24] == 6)
        current["controller2"]["right"] = 1;
    else
        current["controller2"]["right"] = 0;
    if(controllerData[24] == 4 || controllerData[24] == 6 || controllerData[24] == 12)
        current["controller2"]["down"] = 1;
    else
        current["controller2"]["down"] = 0;
    if(controllerData[24] == 8 || controllerData[24] == 12 || controllerData[24] == 9)
        current["controller2"]["left"] = 1;
    else
        current["controller2"]["left"] = 0;


    current["controller2"]["a"] = controllerData[25];
    current["controller2"]["b"] = controllerData[26];
    current["controller2"]["x"] = controllerData[27];
    current["controller2"]["y"] = controllerData[28];
    current["controller2"]["lb"] = controllerData[29];
    current["controller2"]["rb"] = controllerData[30];
    current["controller2"]["select"] = controllerData[31];
    current["controller2"]["start"] = controllerData[32];
    current["controller2"]["ldown"] = controllerData[33];
    current["controller2"]["rdown"] = controllerData[34];

    switch(controllerData[35]){
        case 1:
            current["controller2"]["mode"] = "D";
            current["controller2"]["lt"] = controllerData[22];
            current["controller2"]["rt"] = controllerData[23];
            current["controller2"]["connected"] = 1;
            break;
        case 2:
            current["controller2"]["mode"] = "X";
            current["controller2"]["t"] = controllerData[23]-controllerData[22];
            current["controller2"]["connected"] = 1;
            if(controllerData[23]-controllerData[22] > 2){
                current["controller2"]["lt"] = 0;
                current["controller2"]["rt"] = 1;
            }
            else if(controllerData[23]-controllerData[22] < -2){
                current["controller2"]["lt"] = 1;
                current["controller2"]["rt"] = 0;
            }
            else{
                current["controller2"]["lt"] = 0;
                current["controller2"]["rt"] = 0;
            }
            break;
        default:
            current["controller2"]["connected"] = 0;
            current["controller2"]["mode"] = "";
            break;
    }
    toRobot["controller1"] = current["controller1"];
    toRobot["controller2"] = current["controller2"];

    // cout << current << endl;

    //send update to all active connections
    for(auto &connection : server.get_connections())
        send<WsServer>(connection,serverConnectionState[connection],serverConnectionData[connection],current,milliseconds_since_epoch,true);
    if(clientConnected)
        send<WsClient>(client.connection,clientConnectionState,clientConnectionData,toRobot,milliseconds_since_epoch,false);
  }

  return;
}

