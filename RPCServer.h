/*
  RPCServer.h - Simple JSON-RPC Server for Arduino
  Created by Meir Tseitlin, March 5, 2014. This code is based on https://code.google.com/p/ajson-rpc/
  Modified by frmdstryr, March 2017 to use ArduinoJson
  Released under GPLv2 license.
*/

#ifndef RPCServer_h
#define RPCServer_h

#include "Arduino.h"
#include "ArduinoJson.h"
#include "RPCClient.h"

enum JSON_RPC_RET_TYPE {
	JSON_RPC_RET_TYPE_NONE,
	JSON_RPC_RET_TYPE_BOOL,
	JSON_RPC_RET_TYPE_INT,
	JSON_RPC_RET_TYPE_FLOAT,
	JSON_RPC_RET_TYPE_DOUBLE,
	JSON_RPC_RET_TYPE_STRING,
	JSON_RPC_RET_TYPE_OBJECT
};

class RPCServer;

typedef void (RPCServer::*JSON_PROC_T)(JsonObject&);
typedef void (*JSON_PROC_STATIC_T)(RPCServer*, JsonObject&);

typedef bool (RPCServer::*JSON_PROC_BOOL_T)(JsonObject&);
typedef bool (*JSON_PROC_BOOL_STATIC_T)(RPCServer*, JsonObject&);

typedef int (RPCServer::*JSON_PROC_INT_T)(JsonObject&);
typedef int (*JSON_PROC_INT_STATIC_T)(RPCServer*, JsonObject&);

typedef float (RPCServer::*JSON_PROC_FLOAT_T)(JsonObject&);
typedef float (*JSON_PROC_FLOAT_STATIC_T)(RPCServer*, JsonObject&);

typedef double (RPCServer::*JSON_PROC_DOUBLE_T)(JsonObject&);
typedef double (*JSON_PROC_DOUBLE_STATIC_T)(RPCServer*, JsonObject&);

typedef String (RPCServer::*JSON_PROC_STRING_T)(JsonObject&);
typedef String (*JSON_PROC_STRING_STATIC_T)(RPCServer*, JsonObject&);

typedef JsonObject& (RPCServer::*JSON_PROC_OBJECT_T)(JsonObject&);
typedef JsonObject& (*JSON_PROC_OBJECT_STATIC_T)(RPCServer*, JsonObject&);


struct Mapping {
    String name;
	JSON_PROC_STATIC_T callback;
	JSON_RPC_RET_TYPE retType;
};

struct FuncMap {
    Mapping* mappings;
    unsigned int capacity;
    unsigned int used;
};

#define Declare(server, method, returnType) \
	returnType method(JsonObject& params); \
	static returnType _static##method(server &container, JsonObject& params) { \
		return container.method(params); \
	}


#define Register(method, returnType) \
	registerMethod(#method, (JSON_PROC_STATIC_T) &_static##method, returnType)


class RPCServer: public TCPServer {
public:
	RPCServer(int port);
	void setup(int maxMethods, int maxClients);
	void process();
protected:
	void initRegistry(int maxMethods);
	void initServer(int maxClients);
	void registerMethod(String methodName, JSON_PROC_STATIC_T callback, JSON_RPC_RET_TYPE type = JSON_RPC_RET_TYPE_NONE);
	virtual void registerProcs() = 0;

	void processClient(RPCClient &client);
	void onClientConnected(RPCClient &client);
	void onClientMessage(RPCClient &client, JsonObject &msg);
	void onClientDisconnected(RPCClient &client);

private:
	FuncMap registry;
	RPCClient* clients;
	unsigned int maxClients;
};

#endif
