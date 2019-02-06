
#include <librobosub/robosub.h>
#include "readout.h"

const char* connectionAddr = 
	//"192.168.1.1:8081"
	"0.0.0.0:8081"
;

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

const double MOTOR_POWER = 100.0;
const double MOTOR_DEADBAND = 50.0; //minimum motor power

int convertMotorValuesToRobot(double value) {
    double power = value * MOTOR_POWER;
    if (abs(power) < MOTOR_DEADBAND) power = 0;

    return (int)(power + 1500.0);
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

void network(ReadoutData* readout) {
	DataBucket current;
	DataBucket toRobot;
	
	DataBucket clientConnectionData;
	ConnectionState clientConnectionState;
	bool clientConnected = false;
	
	WsClient client(connectionAddr);
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
				//cout << "Client message: \"" << message_str << "\"" << endl;
				try
				{
						DataBucket temp = DataBucket(message_str);
						current["robot_rtt"] = temp["rtt"];
						current["pin"] = temp["pin"];
						current["robotCpu"] = Util::round<double>((double)temp["cpu"]);
						current["robotRam"] = Util::round<double>((double)temp["ram"]);
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
	
	DataBucket previousState;
	
	int i=0;
	while(true) {
		current["index"] = (i++ / 1000) % 1000; //force refresh approx every second
		current["robot_connected"] = clientConnected;
		
		try{
			readout->rtt = current["robot_rtt"];
			readout->cpu = current["robotCpu"];
			readout->ram = current["robotRam"];
			
			readout->accel_x = current["imu"]["ax"];
			readout->accel_y = current["imu"]["ay"];
			readout->accel_z = current["imu"]["az"];
			
			readout->valid = true;
		}catch(exception e){
			cout<<"exception reading telemetry: "<<e.what()<<endl;
			readout->valid = false;
		}
		
		robosub::Time::waitMillis(1);
		
		unsigned long milliseconds_since_epoch = robosub::Time::millis();
		
		controller1->controllerDataBucket(current,"controller1");
		controller2->controllerDataBucket(current,"controller2");
		/*
		toRobot["motors"] = current["motors"];
		try {
			toRobot["motors"]["br"] = convertMotorValuesToRobot(current["motors"]["br"]);
			toRobot["motors"]["ul"] = convertMotorValuesToRobot(current["motors"]["ul"]);
			toRobot["motors"]["ur"] = convertMotorValuesToRobot(current["motors"]["ur"]);
			toRobot["motors"]["bl"] = convertMotorValuesToRobot(current["motors"]["bl"]);
		}catch(exception e){
			cout<<"exception reading motor values: "<<e.what()<<endl;
		}
		*/
		//send update to all active connections
		if(clientConnected)
			send<WsClient>(client.connection,clientConnectionState,clientConnectionData,toRobot,milliseconds_since_epoch,false);
		
		
	}
}
