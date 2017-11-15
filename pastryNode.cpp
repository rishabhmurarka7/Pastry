#include "pastry.h"

string delim = "$#$";

string extractPublicIP (void){
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
	  string ret;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;
        if (ifa->ifa_addr->sa_family == AF_INET)
        { // IP4
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      		if(ifa->ifa_name[0] == 'w')
            {
  				ret = string(addressBuffer);
  				if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  				return ret;
      			}
        }
    }
    return ret;
}

string md5(string a) {

	const char*c=a.c_str();

	/* Length of message to encrypt */
	int msg_len =strlen(c);

	/* Length of resulting sha1 hash - gcry_md_get_algo_dlen
	* returns digest lenght for an algo */
	int hash_len = gcry_md_get_algo_dlen( GCRY_MD_SHA1 );

	/* output sha1 hash - this will be binary data */
	unsigned char hash[ hash_len ];

	/* output sha1 hash - converted to hex representation
	* 2 hex digits for every byte + 1 for trailing \0 */
	char *out = (char *) malloc( sizeof(char) * ((hash_len*2)+1) );
	char *p = out;

	/* calculate the SHA1 digest. This is a bit of a shortcut function
	* most gcrypt operations require the creation of a handle, etc. */
	gcry_md_hash_buffer( GCRY_MD_SHA1, hash, static_cast<void*>(&c), msg_len );

	/* Convert each byte to its 2 digit ascii
	* hex representation and place in out */
	int i;
	for ( i = 0; i < hash_len; i++, p += 2 ) {
	snprintf ( p, 3, "%02x", hash[i] );
	}

	return out;
}

// Function to connect the node to pastry network
void connectToNetwork(string ip, int port, int toDo) {
	int client_socket;
    struct sockaddr_in node_addr;

    // Creating the client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Error checking for creation of socket
    if (client_socket < 0) {
        cout << "\nError establishing socket..." << endl;
        close(client_socket);
        exit(1);
    }

    // Specifies that we are using ipv4 and assigning the port number for CRS.
    node_addr.sin_family = AF_INET;
    node_addr.sin_port = htons(port);

    // Converts the IP to binary format and puts it into the server address structure.
    // We use #include <arpa/inet.h> to use this function.
    if (inet_pton(AF_INET, ip.c_str(), &node_addr.sin_addr) != 1) {
        perror("inet_pton failed");
        exit(1);
    }

    // Establishing the connection
    if (connect(client_socket,(struct sockaddr *)&node_addr, sizeof(node_addr)) == 0) {
        cout << "\n------Connection Established with other node through Port: " << port << "------"<<endl;
    }
    else {
        cout << "Could not establish connection\n";
        close(client_socket);
        exit(1);
    }

    int bufSize = 1024;
    string buf;

    if(toDo == LEAF_SET || toDo == NEIGHBOUR_SET || toDo == ROUTING_TABLE) {
    	buf += "join";
    	buf += delim;
    	buf += message;
    	send(client_socket, buf.c_str(), bufSize, 0);
    	cout << "\nNew Node added to Pastry" << endl;
    	close(client_socket);
    }
    else if(toDo == NOTHING){
    	buf += "nothing";
    	buf += delim;

    	buf += connection.IP;
    	buf += delim;
    	buf += to_string(connection.port);
    	buf += delim;

    	buf += "connection";
    	buf += delim;

    	// Send the full routing table 
    	for (auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
    		buf += (*i).first;
    		buf += delim;
    		buf += to_string((*i).second);
    		buf += delim;
    	}

    	buf += "LSend";
    	buf += delim;

    	for (auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
    		buf += (*i).first;
    		buf += delim;
    		buf += to_string((*i).second);
    		buf += delim;
    	}

    	buf += "NSend";
    	buf += delim;

    	for(auto i = node.routing_set.begin(); i != node.routing_set.end(); i++) {
    		buf += i->first;
    		buf += delim;
    		for (auto j = (i->second).begin(); j != (i->second).end(); j++) {
	    		buf += (*j).first;
	    		buf += delim;
	    		buf += to_string((*j).second);
	    		buf += delim;
	    	}
	    	buf += "RTRowEnd";
	    	buf += delim;
    	}

    	buf += "RTEnd";
    	buf += delim;

    	send(client_socket, buf.c_str(), bufSize, 0);
    	close(client_socket);    	
    }   
    else if(toDo == LSP || toDo == RTP || toDo == NSP) {
    	buf += "put";
    	buf += delim;
    	buf += message;
    	cout << buf << endl;
    	send(client_socket, buf.c_str(), bufSize, 0);
    	close(client_socket);  	
    } 
    else if(toDo == LSG || toDo == RTG || toDo == NSG) {
    	buf += "get";
    	buf += delim;
    	buf += message;
    	cout << buf << endl;
    	send(client_socket, buf.c_str(), bufSize, 0);
    	close(client_socket);  		
    }
}

// Function to handle the join request
void handleJoin(string remaining) {
	string join_nodeID, join_IP, join_port;

    int p = remaining.find_first_of(" ");
    join_nodeID = remaining.substr(0,p);
    remaining = remaining.substr(p+1);
    p = remaining.find_first_of(" ");
    join_IP = remaining.substr(0, p);
    join_port = remaining.substr(p+1);
   

    message =join_nodeID + delim + join_IP + delim + join_port + delim;


    int where = NOTHING;
    
    connectToNetwork(join_IP, stoi(join_port), NOTHING);

    map<string,vector<pair<string,int> > >::iterator it;
    if(join_nodeID.substr(0, join_nodeID.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
    	//  connect and send
    	where = LEAF_SET;
        node.leaf_set.push_back(make_pair(join_IP, stoi(join_port)));        
    }
    else if(where != LEAF_SET){
        // searching for first 2 chars

        if(join_nodeID.substr(0, join_nodeID.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(join_nodeID.substr(0, 2));
        	if(it == node.routing_set.end()) {	        		
        		node.routing_set[join_nodeID.substr(0, join_nodeID.length() - 2)].push_back(make_pair(join_IP, stoi(join_port)));
        	}
        	else {
          		(it->second).push_back(make_pair(join_IP, stoi(join_port)));
          	}
        }
        else if(join_nodeID.substr(0, join_nodeID.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(join_nodeID.substr(0, 1));
        	if(it == node.routing_set.end()) {	        		
        		node.routing_set[join_nodeID.substr(0, join_nodeID.length() - 3)].push_back(make_pair(join_IP, stoi(join_port)));
        	}
        	else {
          		(it->second).push_back(make_pair(join_IP, stoi(join_port)));
          	}
        }
        else {
        	where = NEIGHBOUR_SET;
        	node.neighbour_set.push_back(make_pair(join_IP, stoi(join_port)));
        }
    }
    if(where == LEAF_SET) {
    	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, LEAF_SET);
    	}
	}
	else if(where == ROUTING_TABLE) {
    	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
    		connectToNetwork((*i).first, (*i).second, ROUTING_TABLE);
    	}
	}
	else if(where == NEIGHBOUR_SET) {
		for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, NEIGHBOUR_SET);
    	}
	}
}

// Function to handle the put request
void handlePut(string remaining) {
	int where = SELF;
	int pos = remaining.find_first_of(" ");
	string key = remaining.substr(0, pos);
	string value = remaining.substr(pos + 1);
	string key_Id = key.substr(0, 4);
	message = key + delim + value + delim;

	map<string,vector<pair<string,int> > >::iterator it;
	if(key_Id == nodeID) {
		where = SELF;
		cout << "---------Key-Value Pair Inserted into Distributed Hash Table---------\n\n";
		cout << key << " " << value << endl;
		distributedHashTable.insert(make_pair(key, value));
	}
    else if(key_Id.substr(0, key_Id.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
        where = LSP;      
    }
    else if(where != LSP) {
        if(key_Id.substr(0, key_Id.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	
        	it = node.routing_set.find(key_Id.substr(0, 2));
        	if(it != node.routing_set.end()) {	
          		where = RTP;
          	}
        }
        else if(key_Id.substr(0, key_Id.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	
        	it = node.routing_set.find(key_Id.substr(0, 1));
        	if(it != node.routing_set.end()) {
          		where = RTP;
          	}
        }
        else {
        	where = NSP;        	
        }
    }
    if(where == LSP) {
    	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, LSP);
    	}
	}
	else if(where == RTP) {
    	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
    		connectToNetwork((*i).first, (*i).second, RTP);
    	}
	}
	else if(where == NSP) {
		for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
			connectToNetwork((*i).first, (*i).second, NSP);
    	}
	}
}

// Function to handle the get request
void handleGet(string remaining) {
	int where = SELFG;
	string value;
	string key = remaining;
	string key_Id = key.substr(0, 4);
	message = key + delim;
	cout<<key<<"  __________  "<<endl;

	map<string,vector<pair<string,int> > >::iterator it;
	map<string, string>::iterator dt;
	if(key_Id == nodeID) {
		where = SELFG;
		value = distributedHashTable[key];
		cout << "---------Key-Value Pair Found in the Distributed Hash Table---------\n\n";
		cout << key << " " << value << endl;
	}
    else if(key_Id.substr(0, key_Id.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
        where = LSG;      
    }
    else if(where != LSG) {
        if(key_Id.substr(0, key_Id.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {        	
        	it = node.routing_set.find(key_Id.substr(0, 2));
        	if(it != node.routing_set.end()) {	
          		where = RTG;
          	}
        }
        else if(key_Id.substr(0, key_Id.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {        	
        	it = node.routing_set.find(key_Id.substr(0, 1));
        	if(it != node.routing_set.end()) {	
          		where = RTG;
          	}
        }
        else {
        	where = NSG;        	
        }
    }
    if(where == LSG) {
    	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, LSG);
    	}
	}
	else if(where == RTG) {
    	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
    		connectToNetwork((*i).first, (*i).second, RTG);
    	}
	}
	else if(where == NSG) {
		for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, NSG);
    	}
	}
}

// Function to print the leaf set
void printLeafSet() {
	cout << "--------------Leaf Set Data------------------\n\n";
	for(unsigned int i = 0; i < node.leaf_set.size(); i++) {
		cout << node.leaf_set[i].first << " " << node.leaf_set[i].second << endl;
	}
}

// Function to print the neighbour set
void printNeighbourSet() {
	cout << "--------------Neighbour Set Data------------------\n\n";
	for(unsigned int i = 0; i < node.neighbour_set.size(); i++) {
		cout << node.neighbour_set[i].first << " " << node.neighbour_set[i].second << endl;
	}
}

// Function to print the routing set
void printRoutingSet() {
	cout << "--------------Routing Set Data------------------\n\n";
	for(auto i = node.routing_set.begin(); i != node.routing_set.end(); i++) {
		cout << i->first << endl;
		for(auto j = i->second.begin(); j != i->second.end(); j++) {
			cout << (*j).first << " " << (*j).second << endl;
		}
	}
}

// Function to print the routing set
void printHashTable() {
	cout << "--------------DHT Data------------------\n\n";
	for(auto i = distributedHashTable.begin(); i != distributedHashTable.end(); i++) {
		cout << i->first << " " << i->second << endl;		
	}
}

// Function to handle inputs from user
void inputHandling() {
	
	string input;

	while(1) {
		getline(cin, input);
		int pos = input.find_first_of(" ");
		string command = input.substr(0, pos);
		string remaining = input.substr(pos+1);

		if(command == "put") {
			handlePut(remaining);
		}
		else if(command == "get") {
			handleGet(remaining);
		}
		else if(command == "join") {
	      	handleJoin(remaining);
		}
		else if(command == "lset") {
	      	printLeafSet();
		}
		else if(command == "nset") {
	      	printNeighbourSet();
		}
		else if(command == "routetable") {
	      	printRoutingSet();
		}
		else if(command == "hashtable") {
	      	printHashTable();
		}
	}
}

int main(int argc, char* argv[]) {

	string input;
	getline(cin, input);
	connection.IP = extractPublicIP();

	cout << "My IP: " << connection.IP << endl;

	int pos = input.find_first_of(" ");
	string command = input.substr(0, pos);
	string remaining = input.substr(pos+1);
	string temp,port;

	if(command == "port") {
		connection.port = stoi(remaining);
		temp += connection.IP;
		temp += remaining;
		nodeID = md5(temp);
		nodeID = nodeID.substr(0, 4);
		cout << "-------------NodeID generated after MD5 Hashing----------------------\n\n";
		cout << "Node ID " << nodeID << endl;
	}

	int returnSer;
    pthread_t newServerThread;
	returnSer = pthread_create(&newServerThread, NULL, startServer, (void *)&connection);
	if(returnSer != 0) {
		perror("Server Thread Creation failed\n");
		exit(EXIT_FAILURE);
	}

	inputHandling();

	return 0;
}
