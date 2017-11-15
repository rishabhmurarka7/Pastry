#include "pastry.h"

string nodeID;
routingTable node;
string message;
string remaining;
connectionInfo connection;
string delim1 = "$#$";

vector<string> split(string s){
    vector<string> list;
    //string s = string(received_string);
    size_t pos;
    string token;
    while ((pos = s.find(delim1)) != string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delim1.length());
    }

    if(s.find('\0') != string::npos){
        token = s.substr(0, s.find('\0'));
        list.push_back(token);
    }
    else if(s.find('\0') == string::npos){
        list.push_back(s);
    }
    return list;
}

void checkAndInsert(string hashedDetails, string receivedIP, string receivedPort) {
	int where;
	map<string,vector<pair<string,int> > >::iterator it;
    if(hashedDetails.substr(0, hashedDetails.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
    //  connect and send

    	where = LEAF_SET;
        node.leaf_set.push_back(make_pair(receivedIP, stoi(receivedPort)));
        
    }
    else if(where != LEAF_SET){
        // searching for first 2 chars

        if(hashedDetails.substr(0, hashedDetails.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(hashedDetails.substr(0, 2));
        	if(it == node.routing_set.end()) {	        		
        		//temp.push_back(make_pair(receivedIP, stoi(receivedPort)));
        		node.routing_set[hashedDetails.substr(0, hashedDetails.length() - 2)].push_back(make_pair(receivedIP, stoi(receivedPort)));
        	}
        	else {
          		(it->second).push_back(make_pair(receivedIP, stoi(receivedPort)));
          	}
        }
        else if(hashedDetails.substr(0, hashedDetails.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	where = ROUTING_TABLE;
        	it = node.routing_set.find(hashedDetails.substr(0, 1));
        	if(it == node.routing_set.end()) {	        		
        		//temp.push_back(make_pair(receivedIP, stoi(receivedPort)));
        		node.routing_set[hashedDetails.substr(0, hashedDetails.length() - 3)].push_back(make_pair(receivedIP, stoi(receivedPort)));
        	}
        	else {
          		(it->second).push_back(make_pair(receivedIP, stoi(receivedPort)));
          	}
        }
        else {
        	where = NEIGHBOUR_SET;
        	node.neighbour_set.push_back(make_pair(receivedIP, stoi(receivedPort)));        	
        }
    }
}


void *handleJoinThread(void *) {

	// string &sp = *static_cast<string *>(remain);
	// string remaining = sp;

	//cout << "%%%%%%%%%% " << &sp << endl;

	string join_nodeID, join_IP, join_port;

	      //  join nodeID IP port :: join command parsing into noideID, IP, port
	      //int p = remaining.find_first_of("$");
		// for(unsigned int i = 0; i < msg.size() ;i++) {
		// 	cout << msg[i] << "\n";
		// }

		int pos = remaining.find_first_of('$');
		join_nodeID = remaining.substr(0, pos);
		//cout << "1. " << pos << " " << nodeID << endl;
		remaining = remaining.substr(pos+delim1.length());
		pos = remaining.find_first_of('$');
		join_IP = remaining.substr(0, pos);
		//cout << "2. " << pos << " " << IP << endl;
		remaining = remaining.substr(pos+delim1.length());
		pos = remaining.find_first_of('$');
		join_port = remaining.substr(0, pos);
		cout << "join Ip " << join_IP << endl;
	    cout << "join port " << join_port << endl;
	    cout << "join node id " << join_nodeID << endl;



	      message = join_nodeID + delim1 + join_IP + delim1 + join_port + delim1;


	      int where = NOTHING;
		  cout<<join_nodeID.substr(0, join_nodeID.length() - 1)<<"  jjjjjj   "<<endl;
		  cout<<nodeID.substr(0,nodeID.length() - 1)<<"   this n   "<<endl;
          int me=0;
	      map<string,vector<pair<string,int> > >::iterator it;
	      if(join_nodeID == nodeID) {
	      		cout << "ITS ME" << endl;
	      		me=1;
	      }

	      else if(join_nodeID.substr(0, join_nodeID.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
	        //  connect and send
	        cout << "inside if" << endl;
	        where = LEAF_SET;
	        node.leaf_set.push_back(make_pair(join_IP, stoi(join_port)));
	        cout << "Pushed back" << endl;
	      }
	      else if(where != LEAF_SET){
	        // searching for first 2 chars

	        if(join_nodeID.substr(0, join_nodeID.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
	        	where = ROUTING_TABLE;
	        	it = node.routing_set.find(join_nodeID.substr(0, 2));
	        	if(it == node.routing_set.end()) {	        		
	        		//temp.push_back(make_pair(join_IP, stoi(join_port)));
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
	        		//temp.push_back(make_pair(join_IP, stoi(join_port)));
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

   if(!me)
   {
        if(where == LEAF_SET) {
        	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
        		cout << "Goin to connect leaf" << endl;
        		connectToNetwork((*i).first, (*i).second, LEAF_SET);
        		cout << "Finish" << endl;
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
		connectToNetwork(join_IP, stoi(join_port), NOTHING);
	}		
	pthread_exit(0);

}

void *handlePutThread(void *) {

	// string *sp = static_cast<std::string*>(remain);
	// // You could use 'sp' directly, or this, which does a copy.
	// remaining = *sp;

	int where = SELF;
	int pos = remaining.find_first_of(" ");
	string key = remaining.substr(0, pos);
	string value = remaining.substr(pos + 1);
	string key_Id = key.substr(0, 4);
	message = key + delim1 + value + delim1;

	map<string,vector<pair<string,int> > >::iterator it;
	if(key_Id == nodeID) {
		where = SELF;
		cout << "aioueufeiufsnersif\n";
		cout << key_Id << " " << value << endl;
		distributedHashTable.insert(make_pair(key, value));
	}
    else if(key_Id.substr(0, key_Id.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
        where = LS;      
    }
    else if(where != LS) {
        if(key_Id.substr(0, key_Id.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	
        	it = node.routing_set.find(key_Id.substr(0, 2));
        	if(it == node.routing_set.end()) {	        		
        		
        	}
        	else {
          		where = RT;
          	}
        }
        else if(key_Id.substr(0, key_Id.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	
        	it = node.routing_set.find(key_Id.substr(0, 1));
        	if(it == node.routing_set.end()) {	        		
        		
        	}
        	else {
          		where = RT;
          	}
        }
        else {
        	where = NS;        	
        }
    }
    if(where == LS) {
    	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, LS);
    	}
	}
	else if(where == RT) {
    	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
    		connectToNetwork((*i).first, (*i).second, RT);
    	}
	}
	else if(where == NS) {
		for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, NS);
    	}
	} 
}


void *handleGetThread(void *) {

	// string *sp = static_cast<std::string*>(remain);
	// // You could use 'sp' directly, or this, which does a copy.
	// remaining = *sp;

	int where = SELF;
	int pos = remaining.find_first_of(" ");
	string key = remaining.substr(0, pos);
	string value = remaining.substr(pos + 1);
	string key_Id = key.substr(0, 4);
	message = key + delim1 + value + delim1;

	map<string,vector<pair<string,int> > >::iterator it;
	if(key_Id == nodeID) {
		where = SELF;
		cout << key_Id << " " << value << endl;
		message = distributedHashTable[key];
	}
    else if(key_Id.substr(0, key_Id.length() - 1) == nodeID.substr(0,nodeID.length() - 1)) {
        where = LS;      
    }
    else if(where != LS) {
        if(key_Id.substr(0, key_Id.length() - 2) == nodeID.substr(0,nodeID.length() - 2)) {
        	
        	it = node.routing_set.find(key_Id.substr(0, 2));
        	if(it == node.routing_set.end()) {	        		
        		
        	}
        	else {
          		where = RT;
          	}
        }
        else if(key_Id.substr(0, key_Id.length() - 3) == nodeID.substr(0,nodeID.length() - 3)) {
        	
        	it = node.routing_set.find(key_Id.substr(0, 1));
        	if(it == node.routing_set.end()) {	        		
        		
        	}
        	else {
          		where = RT;
          	}
        }
        else {
        	where = NS;        	
        }
    }
    if(where == LS) {
    	for(auto i = node.leaf_set.begin(); i != node.leaf_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, LS);
    	}
	}
	else if(where == RT) {
    	for(auto i = (it->second).begin(); i != (it->second).end(); i++) {
    		connectToNetwork((*i).first, (*i).second, RT);
    	}
	}
	else if(where == NS) {
		for(auto i = node.neighbour_set.begin(); i != node.neighbour_set.end(); i++) {
    		connectToNetwork((*i).first, (*i).second, NS);
    	}
	} 
}

// Thread handler function
void *startServer(void *connection) {

	cout << "Server started" << endl;
	connectionInfo con = *(struct connectionInfo *)connection;

	// Identifier of the socket getting created.
	int server_socket;

    struct sockaddr_in server_addr;
    socklen_t size;

    // Creating the client socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Error checking for creation of socket
    if (server_socket < 0) {
        perror("\nError establishing socket...\n");
        exit(1);
    }

    cout << "Server Socket creation Successful" << endl;

    // Initializing the CRS address data structure with the CRS IP and port number.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(con.IP.c_str());
    server_addr.sin_port = htons(con.port);
    size = sizeof(server_addr);

    // Binding the client port to the CRS
    if ((bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {
        perror("Error Binding connection in Server\n");
        exit(1);
    }

    // Listening for clients
    cout << "Listening for clients on port" << con.port << endl;
    listen(server_socket, 5);


	//clientRequestData clReq;   
    //int returnVal[noOfClients];
	//int clientCount = 1;
	int returnVal;

	while(1) {    	
    	
    	// Accept the connection from other clients for download
	    returnVal = accept(server_socket,(struct sockaddr *)&server_addr, &size);

	    // Error checking for accept() call
	    if (returnVal < 0) 
	        perror("Error on accepting the connection");

	    int bufSize = 1024;
	    char buffer[bufSize];
	    string buf;

	    	bzero(buffer, bufSize);	   
	    	cout<<" i m back here  "<<endl; 	
	    	recv(returnVal, buffer, bufSize, 0);
	    	cout << buffer << endl;

	    	buf = buffer;
	    	string token;

	    	int pos = buf.find_first_of(delim1);
	    	token = buf.substr(0, pos);
	    	remaining = buf.substr(pos+delim1.length());

	    	// Receive a Join Message
	    	if(token == "join") {

	    		// get the remaining string from the buffer and pass it to handleJoin.
	    		pthread_t newJoinThread;
	    		int threadJoin;

	    		cout << "awioefnweifjeoifje" << endl;
	    		cout << remaining << endl;
	    		threadJoin = pthread_create(&newJoinThread, NULL, handleJoinThread, NULL);
	    		if(threadJoin != 0) {
					perror("Thread Join Creation failed\n");
					exit(EXIT_FAILURE);
				}

				void* thread_result;
				threadJoin = pthread_join(newJoinThread, &thread_result);
				if (threadJoin != 0) {
					perror("Thread join failed");
					exit(EXIT_FAILURE);
				}

				cout << "Finished handling join" << endl;

	    	}
	    	else if(token == "put") {
	    		pthread_t newPutThread;
	    		int threadPut;

	    		threadPut = pthread_create(&newPutThread, NULL, handlePutThread, static_cast<void*>(&remaining));
	    		if(threadPut != 0) {
					perror("Thread Put Creation failed\n");
					exit(EXIT_FAILURE);
				}

				void* thread_result_put;
				threadPut = pthread_join(newPutThread, &thread_result_put);
				if (threadPut != 0) {
					perror("Thread Put failed");
					exit(EXIT_FAILURE);
				}

				cout << "Finished handling Put" << endl;
	    	}
	    	else if(token == "get") {
	    		pthread_t newGetThread;
	    		int threadGet;

	    		threadGet = pthread_create(&newGetThread, NULL, handleGetThread, static_cast<void*>(&remaining));
	    		if(threadGet != 0) {
					perror("Thread Get Creation failed\n");
					exit(EXIT_FAILURE);
				}

				void* thread_result_get;
				threadGet = pthread_join(newGetThread, &thread_result_get);
				if (threadGet != 0) {
					perror("Thread get failed");
					exit(EXIT_FAILURE);
				}

				cout << "Finished handling get" << endl;
	    	}

	    	else if(token == "nothing") {
				// accept the full routing table.
				// make temp vectors and map and insert them into node.
				
				string con = "connection";
				int posCon = remaining.find_first_of('c');
				string details = remaining.substr(0, posCon);
				cout << "My Details -" << details << endl;

				//remaining = remaining.substr(posCon + con.length() + delim.length());
				int pos1 = details.find_first_of('$');
				string receivedIP = details.substr(0, pos1);
				details = details.substr(pos1+delim1.length());
				pos1 = details.find_first_of('$');
				string receivedPort = details.substr(0, pos1);

				cout << "My IP " << receivedIP << " My Port " << receivedPort << endl;
                
                string hashedDetails = md5(receivedIP + receivedPort);
               	hashedDetails = hashedDetails.substr(0, 4);

               	checkAndInsert(hashedDetails, receivedIP, receivedPort);

				remaining = remaining.substr(posCon + con.length() + delim1.length());
				cout << remaining << endl;

				int posLSend = remaining.find_first_of("LSend");
				remaining = remaining.substr(0, posLSend);
				if(remaining != "") {
				
				vector<string> data = split(remaining);


				cout << "Size" << data.size() << endl; 
				for(unsigned int i = 0; i < data.size()-1; i+=2) {
					cout << "--------------Leaf Set Data------------------\n";
					cout << data[i] << " " << stoi(data[i+1]) << endl;
					//if(data[i] != connection.IP && stoi(data[i+1]) != connection.port)
					node.leaf_set.push_back(make_pair(data[i], stoi(data[i+1])));
				}
				
				int newpos = buf.find_first_of("L");
				string remain = buf.substr(newpos+8);
				int posNSend = remain.find_first_of("N");
				remain = remain.substr(0, posNSend);

					if(remain != "") {
						vector<string> data2 = split(remain);
						cout << "--------------Neighbour Set Data------------------\n";
						for(unsigned int i = 0; i < data2.size()-1; i+=2) {
							cout << data2[i] << endl << " " << stoi(data2[i+1]) << endl;
							//if(data2[i] != connection.IP && stoi(data2[i+1]) != connection.port)
							node.neighbour_set.push_back(make_pair(data2[i], stoi(data2[i+1])));
						}


						for(unsigned int i = 0; i < node.leaf_set.size(); i++) {
							cout << node.leaf_set[i].first << "**********" << node.leaf_set[i].second << endl;
						}

						for(unsigned int i = 0; i < node.neighbour_set.size(); i++) {
							cout << node.neighbour_set[i].first << "^^^^^^^^^^^^^" << node.neighbour_set[i].second << endl;
						}


						cout << remain << endl;
						int newerpos = buf.find_first_of("N");
						int Tpos = buf.find_first_of("T");
						string rtstring = buf.substr(newerpos+8,Tpos-newerpos);
						
						if(rtstring != "") {

							vector<string> data3 = split(rtstring);

							for(unsigned int i = 0; i < data3.size();) {
								int rowend_detect = find(data3.begin()+i, data3.end(), "RowEnd")-data3.begin();

								cout << rowend_detect << endl;
								vector<pair<string, int>> temp;

								cout << "map index " << data3[i];

								int j = i+1;
								for(; j < rowend_detect; j+=2) {
									cout << "\n IP " << data3[j] << " ### Port " << data3[j+1] << endl;
									//if(data3[j] != connection.IP && stoi(data3[j+1]) != connection.port) 
									temp.push_back(make_pair(data3[j], stoi(data3[j+1])));
								}

								node.routing_set.insert(make_pair(data3[i], temp));

								i = rowend_detect+1;
								if(data3[i] == "TEnd")
									break;

							}
							cout << "--------------Routing Set Data------------------\n";
							for(auto i = node.routing_set.begin(); i != node.routing_set.end(); i++) {
								cout << i->first << endl;
								for(auto j = i->second.begin(); j != i->second.end(); j++) {
									cout << (*j).first << " *** " << (*j).second << endl;
								}
							}
						}
					}
				
			}
		}

	}
}