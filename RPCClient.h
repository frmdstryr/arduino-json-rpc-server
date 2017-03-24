/**
  * RPCClient.h - Json RPC server client for Redbear Duo / Particle boards (or any board with TCPServer / TCPClient)
  * (c)  2017 frmdstryr@gmail.com, see accompanying license for details.
*/

#ifndef RPCClient_h
#define RPCClient_h

#include "Arduino.h"
#include "ArduinoJson.h"

/**
 * Add connected state variable
 * and helper to send objects.
 */
class RPCClient: public TCPClient {
public:
	RPCClient();
	RPCClient(TCPClient &client);
	void send(JsonObject & msg);
	bool wasConnected;
};

#endif
