// HEADER FILES -----------------------------------------------
#ifndef HEAD
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <sstream>
#include <errno.h>
#include <fcntl.h>
#include <ctime>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <gcrypt.h>
#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;

// DATA STRUCTURES --------------------------------------------

static unordered_map<string, string> distributedHashTable;

struct routingTable {
	vector< pair<string,int> > neighbour_set;
	vector< pair<string,int> > leaf_set;
	map<string,vector<pair<string,int> > >routing_set;
};

struct connectionInfo {
	string IP;
	int port;
};

extern connectionInfo connection;
extern string nodeID;
extern routingTable node;

// FUNCTION PROTOTYPES-----------------------------------------
void *startServer(void *connection);
void *handleJoinThread(void *remaining);
void handleJoin(string remaining);
void connectToNetwork(string ip, int port, int toDo);
string md5(string a);

extern string message;

enum ToDo {NOTHING, LEAF_SET, ROUTING_TABLE, NEIGHBOUR_SET};
enum ToWhere {SELF = 8, LS = 9, RT = 10, NS = 11};

#endif
