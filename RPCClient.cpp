/**
  * RPCClient.cpp - Json RPC server client for Redbear Duo / Particle boards (or any board with TCPServer / TCPClient)
  * (c)  2017 frmdstryr@gmail.com, see accompanying license for details.
*/
#include "RPCClient.h"
#include "ArduinoJson.h"

RPCClient::RPCClient(): TCPClient() {
	wasConnected = false;
}

/**
 * Create an RPCClient from a TCPClient
 */
RPCClient::RPCClient(TCPClient & client): TCPClient((client.*(&RPCClient::sock_handle))()) {
	wasConnected = false;
}


/**
 * Send a JsonObject
 */
void RPCClient::send(JsonObject & msg) {
	char buf[msg.measureLength()+1];
	Serial.println("ClientResponse:");
	msg.printTo(Serial); // TODO: only do this for debugging
	msg.printTo(buf,sizeof(buf));
	print(buf); // Send to client
}
