/*
  RPCServer.cpp - Simple JSON-RPC Server for Arduino
  Created by Meir Tseitlin, March 5, 2014. This code is based on https://code.google.com/p/ajson-rpc/
  Modified by frmdstryr, March 2017 to use ArduinoJson
  Released under GPLv2 license.
*/
#include "Arduino.h"
#include "ArduinoJson.h"
#include "RPCServer.h"

RPCServer::RPCServer(int port) : TCPServer(port) {
}

void RPCServer::setup(int maxMethods, int maxClients) {
	initRegistry(maxMethods);
	initServer(maxClients);
}

void RPCServer::initRegistry(int maxMethods) {
	FuncMap &reg = registry;
    reg.capacity = maxMethods;
    reg.used = 0;
    reg.mappings = new Mapping[maxMethods];
	
	// Register all json procedures (pure virtual - implemented in derivatives with macros)
	registerProcs();
}

void RPCServer::initServer(int maxClients) {
	RPCServer &self = *this;
	self.maxClients  = maxClients;
	self.clients = new RPCClient[maxClients];
}


void RPCServer::registerMethod(String methodName, JSON_PROC_STATIC_T callback, JSON_RPC_RET_TYPE type) {
    // only write keyvalue pair if we allocated enough memory for it
	FuncMap &reg = registry;
    if (reg.used < reg.capacity) {
	    Mapping &mapping = reg.mappings[reg.used++];
	    mapping.name = methodName;
	    mapping.callback = callback;
		mapping.retType = type;
    }
}

void RPCServer::onClientConnected(RPCClient &client) {
	client.wasConnected = true;
	Serial.println("ClientConnected.");
    client.flush();
}

void RPCServer::onClientMessage(RPCClient &client, JsonObject &msg) {
	Serial.println("ClientMessage:");
	msg.printTo(Serial);
	Serial.println("\n");
	DynamicJsonBuffer jsonBuffer;
	// Create response
	JsonObject& response = jsonBuffer.createObject();
    response["jsonrpc"] = "2.0";

    // Check required fields
    if (!msg.containsKey("method") || !msg.containsKey("id")) {
    	// Not a valid Json-RPC 2.0 message
    	JsonObject& error = response.createNestedObject("error");
    	error[ "code"] = -32600;
    	error[ "message"] = "Invalid Request.";

    	if (!msg.containsKey("id")) {
    		response["id"] = NULL;
    		error[ "data"] = "Missing id.";
    	} else {
    		response["id"] = msg["id"];
    		error[ "data"] ="Missing method.";
    	}

    	client.send(response);
        return;
    }
    
    // Get the params
    JsonObject& params = msg.containsKey("params")? msg["params"] : jsonBuffer.createObject();

    // Add the id
    response["id"] = msg["id"];
    FuncMap &reg = registry;
    String method = (const char*)  msg["method"];

    for (unsigned int i=0; i<reg.used; i++) {
        Mapping &mapping = reg.mappings[i];
        if (method.equals(mapping.name)) {
			switch (mapping.retType) {
				case JSON_RPC_RET_TYPE_NONE: {
					mapping.callback(this, params);
					break;
				}
				case JSON_RPC_RET_TYPE_INT: {
					JSON_PROC_INT_STATIC_T callback = (JSON_PROC_INT_STATIC_T) mapping.callback;
					response["result"] = callback(this, params);
					break;
				}
				case JSON_RPC_RET_TYPE_BOOL: {
					JSON_PROC_BOOL_STATIC_T callback = (JSON_PROC_BOOL_STATIC_T) mapping.callback;
					response["result"] = callback(this, params);
					break;
				}
				case JSON_RPC_RET_TYPE_FLOAT: {
					JSON_PROC_FLOAT_STATIC_T callback = (JSON_PROC_FLOAT_STATIC_T) mapping.callback;
					response["result"] = callback(this, params);
					break;
				}
				case JSON_RPC_RET_TYPE_DOUBLE: {
					JSON_PROC_DOUBLE_STATIC_T callback = (JSON_PROC_DOUBLE_STATIC_T) mapping.callback;
					response["result"] = callback(this, params);
					break;
				}
				case JSON_RPC_RET_TYPE_STRING: {
					JSON_PROC_STRING_STATIC_T callback = (JSON_PROC_STRING_STATIC_T) mapping.callback;
					response["result"] = callback(this, params);
					break;
				}
				case JSON_RPC_RET_TYPE_OBJECT: {
					JSON_PROC_OBJECT_STATIC_T callback = (JSON_PROC_OBJECT_STATIC_T) mapping.callback;
					response["result"] = callback(this, params);
					break;
				}
			}
			client.send(response);
			return;
		}
    }

    // If we get here... the Method is not found
	JsonObject& error = response.createNestedObject("error");
	error[ "code"] = -32601;
	error[ "message"] = "Method not found.";
	client.send(response);
	return;
}

void RPCServer::onClientDisconnected(RPCClient &client) {
	client.wasConnected = false;
	Serial.println("ClientDisconnected.");
}

void RPCServer::processClient(RPCClient &client) {
	DynamicJsonBuffer  requestBuffer;
	// Fire connect
	//Serial.println("Client connected..");
	if (!client.wasConnected) {
		onClientConnected(client);
	}

	// Process messages
	if (client.available() > 0) {
		 JsonObject &msg = requestBuffer.parse(client);
		 onClientMessage(client,msg);
	 }
}

void RPCServer::process() {
	RPCServer &self = *this;
	for(unsigned int i = 0; i < self.maxClients; i++) { // TODO: Impl use of  meta->used
		RPCClient &client = self.clients[i];
		if (client.connected()) {
			processClient(client);
		} else {
			if (client.wasConnected) {
					onClientDisconnected(client);
			}
			// Check for new client
			TCPClient c = available();
			client = RPCClient(c);
			//= new RPCClient(c); // issue here i htink..
		}
	}
}
