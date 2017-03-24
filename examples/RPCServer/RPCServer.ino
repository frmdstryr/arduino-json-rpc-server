#if defined(ARDUINO)
SYSTEM_MODE(MANUAL);
#endif

#include <ArduinoJson.h>
#include <RPCServer.h>

/* Modify the bellowing definitions for your AP/Router.*/
#define SSID     "SSID"
#define PASSWORD "PASSWORD"

class ExampleServer: public RPCServer {
public:
	ExampleServer(int port);

	Declare(ExampleServer, getInt, int);
	Declare(ExampleServer, setInt, bool);
protected:
	void registerProcs() {
		Register(getInt, JSON_RPC_RET_TYPE_INT);
		Register(setInt, JSON_RPC_RET_TYPE_BOOL);
	}
private:
	int value;

};

ExampleServer::ExampleServer(int port): RPCServer(port) {}


int ExampleServer::getInt(JsonObject &params) {
	return value;
}

bool ExampleServer::setInt(JsonObject &params) {
	value = params["value"];
	return true;
}


ExampleServer server(8888);

void setup() {
   char addr[16];

  // start up the serial interface
  Serial.begin(115200);
  while (!Serial) {
    // wait serial port initialization
  }
  Serial.println("Initializing JSON RPC server");

  WiFi.on();
  WiFi.setCredentials(SSID, PASSWORD,WPA2,WLAN_CIPHER_AES);
  WiFi.connect();

  Serial.println("Waiting for an IP address...\n");
  while (!WiFi.ready()) {
      delay(1000);
  }

  // Wait IP address to be updated.
  IPAddress localIP = WiFi.localIP();
  while (localIP[0] == 0) {
	  localIP = WiFi.localIP();
      delay(1000);
  }

    sprintf(addr, "%u.%u.%u.%u", localIP[0], localIP[1], localIP[2], localIP[3]);
    Serial.println(addr);

  /* add setup code here */
  server.setup(2,10);
  server.begin();
  Serial.println("Server started");
}

void loop() {
	// Handle RPC requests
	server.process();
}
