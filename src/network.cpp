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

const double MOTOR_POWER = 50.0;

int convertMotorValuesToRobot(double value) {
    return (int)((value * MOTOR_POWER) + 1500.0);
}

template <class T>
void send(shared_ptr<typename T::Connection> connection,
          ConnectionState &connectionState,
          DataBucket &connectionData,
          DataBucket &current,
          unsigned long milliseconds_since_epoch,
          bool compress
){
      if (!connectionState.ready) return;

      try {
          //compress data
          DataBucket previousState = connectionData;

          //remove time-dependent variables
          DataBucket timeRemoved = current;
          timeRemoved.remove("robot_rtt");
          timeRemoved.remove("robotCpu");
          timeRemoved.remove("robotRam");

          DataBucket compressed = timeRemoved.compress(previousState);
          previousState = current;
          previousState.remove("robot_rtt");
          previousState.remove("robotCpu");
          previousState.remove("robotRam");
          connectionData = previousState;

          //skip if no changes
          if (compressed.toJson().empty()) return;

          //update time-dependent values and recompress
          DataBucket sentState = current;
          sentState["time"] = milliseconds_since_epoch;
          sentState["rtt"] = connectionState.rtt;
          sentState["controllerTime"] = controllerTime;
          compressed = sentState.compress(previousState);

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
      } catch (exception e) {
          cout << "Failed to send to robot: " << e.what() << endl;
      }
}

void network() {
  //prepare buckets to store data
  DataBucket current;
  DataBucket toRobot;






  //initialize server for web browser communication
  WsServer server;
  server.config.port = 8080;
  server.config.thread_pool_size = 1;
  server.config.address = "0.0.0.0";

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
//  WsClient client("localhost:8081/");
  WsClient client("192.168.1.1:8081/");
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
        cout << "Client message: \"" << message_str << "\"" << endl;
        try
        {
            DataBucket temp = DataBucket(message_str);
            // current["rand"] = temp["rand"];
            current["robot_rtt"] = temp["rtt"];
            current["pin"] = temp["pin"];
            current["robotCpu"] = Util::round<double>((double)temp["cpu"]);
            current["robotRam"] = Util::round<double>((double)temp["ram"]);
//            current["sensor"] = temp["sensor"];
            current["imu"] = temp["imu"];
        } catch (exception e) {
            cout << e.what() << endl;
        }

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
    while(true){
        client.start();
        cout << "Not connected to robot" << endl;
        robosub::Time::waitMillis(1000);
    }
  });



  //wait one second for server to start
  robosub::Time::waitMillis(1000);
  cout << "Server started" << endl;

  DataBucket previousState;

  int i=0;
  while(true) {
    current["index"] = (i++ / 1000) % 1000; //force refresh approx every second
    current["robot_connected"] = clientConnected;

    robosub::Time::waitMillis(1);

    unsigned long milliseconds_since_epoch = robosub::Time::millis();

    controller1->controllerDataBucket(current,"controller1");
    controller2->controllerDataBucket(current,"controller2");

    toRobot["motors"] = current["motors"];
    toRobot["motors"]["bl"] = convertMotorValuesToRobot(current["motors"]["bl"]);
    toRobot["motors"]["br"] = convertMotorValuesToRobot(current["motors"]["br"]);
    toRobot["motors"]["ul"] = convertMotorValuesToRobot(current["motors"]["ul"]);
    toRobot["motors"]["ur"] = convertMotorValuesToRobot(current["motors"]["ur"]);

    //send update to all active connections
    for(auto &connection : server.get_connections())
        send<WsServer>(connection,serverConnectionState[connection],serverConnectionData[connection],current,milliseconds_since_epoch,false);
    if(clientConnected)
        send<WsClient>(client.connection,clientConnectionState,clientConnectionData,toRobot,milliseconds_since_epoch,false);
  }
}
