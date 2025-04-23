#include<vector>
#include<unistd.h>
#include<iostream>
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<limits.h>
#include<malloc.h>
#include<wait.h>
#include<sys/times.h>
#include<err.h>
#include<fcntl.h>
#include <algorithm>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

using namespace std;

#define MAX_SIZE_TA 10
#define MAX_SIZE_USER 10

#define LOCAL_HOST "127.0.0.1" // Host address
#define ServerM_UDP_PORT 23427 // Main Server port number
#define ServerM_TCP_PORT 24427
#define ServerA_UDP_PORT 21427 // Server A port number
#define ServerB_UDP_PORT 22427

#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define FAIL -1
#define MAX_FILE_SIZE 1000

#define MAX_TCP_LINK 10 

typedef struct user{
    string name;                     //name check
    int time_ava[10][2];
}User;

typedef struct udp_server{
    string id;
    string ip = LOCAL_HOST;
    int port;
    struct sockaddr_in addr;
    socklen_t addr_len;
    vector<string> list;

}Server;

typedef struct tcp_client{
    string id;
    vector<string> list;
}Client;

//global variable
int udp_sockfd_M, tcp_sockfd_S, tcp_sockfd_child;
struct sockaddr_in serverM_addr, tcp_server_addr, tcp_client_addr, tcp_child_addr;
char recv_buf[1024];
char send_buf[1024];
Server ServerA;
Server ServerB;
Client RequestA{};
Client RequestB{};

vector<string> client_list;
vector<string> no_exist{};

void createSockUDP(){
    // Create a UDP socket
    udp_sockfd_M = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp_sockfd_M == FAIL) {
        perror("Error: main server socket open denied!\n");
        exit(1);
    }
}

void initSockUDP(){
    // Initialize server A IP address, port number
    memset(&serverM_addr, 0, sizeof(serverM_addr)); //  make sure the struct is empty

    serverM_addr.sin_family = AF_INET; // Use IPv4 address family
    serverM_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    serverM_addr.sin_port = htons(ServerM_UDP_PORT); // Main Server port number
}

void bindSockUDP(){
    if (::bind(udp_sockfd_M, (struct sockaddr *) &serverM_addr, sizeof(serverM_addr)) == FAIL) {
        perror("Error: Main Server failed to bind UDP socket");
        exit(1);
    }

    cout << "Server M is up and running using UDP on port " << ServerM_UDP_PORT << endl;
}

int countSpaces(const string& str) {
    int count = 0;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == ' ') {
            count++;
        }
    }
    return count;
}

vector<string> getList(Server end_server){
    vector<string> username_list;
    string a;

    end_server.addr_len = sizeof(end_server.addr);
    ssize_t recv_bytes = recvfrom(udp_sockfd_M, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&end_server.addr, &end_server.addr_len);

    string list_str(recv_buf, recv_bytes);

    if(recv_bytes < 0){
        perror("recvfrom failed");
        exit(1);
    }

    if(recv_bytes > 0){
        string a;

        a = strtok(recv_buf, " ");
        username_list.push_back(a);

        // cout << "countSpaces: " << countSpaces(list_str) << endl;

        for(int i = 0; i < (countSpaces(list_str) - 1); i++){
            a = strtok(NULL, " ");

            username_list.push_back(a);
        }

        //screen print for getting list from backend server
        cout << "Main Server received the username list from server " + end_server.id + " using UDP over port " << end_server.port << endl;

    }
    
    return username_list;
}

void createServerTCP(){

    tcp_sockfd_S = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (tcp_sockfd_S == FAIL) {
        perror("[ERROR] main server: fail to create socket for monitor");
        exit(1);
    }

    // Initialize IP address, port number
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr)); //  make sure the struct is empty
    tcp_server_addr.sin_family = AF_INET; // Use IPv4 address family
    tcp_server_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    tcp_server_addr.sin_port = ServerM_TCP_PORT; // Port number for monitor
    
    // Bind socket
    if (::bind(tcp_sockfd_S, (struct sockaddr *) &tcp_server_addr, sizeof(tcp_server_addr)) == FAIL) {
        perror("[ERROR] main server: fail to bind monitor socket");
        exit(1);
    }
}

void listenClientTCP() {
    if (listen(tcp_sockfd_S, MAX_TCP_LINK) == FAIL) {
        perror("[ERROR] main server: fail to listen for client socket");
        exit(1);
    }
}

void acceptClientTCP(){
    socklen_t client_address_len = sizeof(tcp_child_addr);

    cout << "accept..." << endl;
    tcp_sockfd_child = accept(tcp_sockfd_S, (struct sockaddr*)&tcp_child_addr, &client_address_len);
    if(tcp_sockfd_child)
        cout << "Main Server received the request from client using TCP over port " << ServerM_TCP_PORT << endl;

}

//vector -> string
string getNameStr(vector<string> list){
    string name_str = "";
    for(int i = 0; i < list.size(); i++){
        name_str += list[i];

        name_str += " ";
    }
    //cout << name_str << endl;
    return name_str;
}

//string -> vector
vector<string> getVector(string str){
    vector<string> list;
    string a;
    char b[1024];
    strcpy(b, str.c_str());

    a = strtok(b, " ");
    list.push_back(a);

    for(int i = 0; i < (countSpaces(str) - 1); i++){
            a = strtok(NULL, " ");

            auto pos = a.find_last_not_of('\n');
            if (pos != std::string::npos) {
                a.erase(pos + 1);
            }

            list.push_back(a);
        }

    return list;
}

vector<string> recvList(){
    vector<string> client_input;

    memset(recv_buf, 0, 1024);

    if(recv(tcp_sockfd_child, recv_buf, sizeof(recv_buf), 0) == FAIL){
        perror("Receive Error");
		close(tcp_sockfd_child);
		exit(1);
    }
    
    // //char -> string
    string str(recv_buf);

    cout << "recv the message: " << str << endl;

    //string -> vector<string>
    client_input = getVector(str);

    return client_input;
}

bool checkIfdata(Server end_server, string str){
    auto it = find(end_server.list.begin(), end_server.list.end(), str);

    if(it != end_server.list.end()){
        // cout << "111: " << str << endl;
        // cout << "222: " << end_server.id << endl;
        if(end_server.id == "A"){
            cout << "The str: " << str << " is in Server A" << endl;
            RequestA.list.push_back(end_server.list[it - end_server.list.begin()]);
            return true;
        }

        if(end_server.id == "B"){
            cout << "The user: " << str << " is in Server B" << endl;
            RequestB.list.push_back(end_server.list[it - end_server.list.begin()]);
            return true;
        }
    }

    return false;
}

void examineList(vector<string> list){

    for(int i = 0; i < list.size(); i++){
        cout << list[i] << endl;
        if(checkIfdata(ServerA, list[i]))
            continue;

        else if(checkIfdata(ServerB, list[i]))
            continue;
        
        else{
            cout << list[i] << " is not exist" << endl;
            no_exist.push_back(list[i]);
        }

    }

}

void replyClient(vector<string> list){
    
    if(list.size() == 0) 
        exit(0);
    cout << "no_exist is not empty\n";
    string str = getNameStr(list);

    memset(send_buf, 0, 1024);
    
    strcpy(send_buf, str.c_str());
    cout << "*" << strlen(send_buf) << endl;
    if(send(tcp_sockfd_child, send_buf, strlen(send_buf), 0) == FAIL){
        perror("Message Send Error");
		close(tcp_sockfd_child);
		exit(1);
    }

    cout << "< " << str << ">" << " do not exist. Send a reply to the client."  << endl;
    
}

void boot_up(){
    //1. boot-up print
    cout << "Main Server is up and running." << endl;

    //2. get username list from both backend servers
    //2.1 local information
    createSockUDP();
    initSockUDP();
    bindSockUDP();
    //2.2 assign server information and get list with getList()
    ServerA.id = 'A';
    ServerB.id = 'B';
    ServerA.port = ServerA_UDP_PORT;

    // cout << "Waiting for User List..." << endl;
    //3. get list from backend server
    ServerA.list = getList(ServerA);
    //ServerB.list = getList(ServerB);

    //check list
    // cout << "check the whole name list:" << endl;
    // for(int i =0; i < ServerA.list.size(); i++)
    //     cout << ServerA.list[i] << endl;

    //4. get message from client
    cout << "waiting for client..." << endl;
    //4.1 create server and client socket
    createServerTCP();
    //createClientTCP();
    listenClientTCP();
    acceptClientTCP();
    
    //4.2
    
}

void forward(){
    //1. get list from main server
    client_list = recvList();
    //check list
    cout << "check the whole input list:" << endl;
    cout << "input list size: " << client_list.size() << endl;
        for(int i = 0; i < client_list.size(); i++)
            cout << client_list[i] << endl;

    //2. check list with the stored name list from backend server A and B
    RequestA.id = 'A';
    RequestB.id = 'B';
    examineList(client_list);
    // //check all splited list
    // cout << "***************" << endl;
    // cout << "RequestA: " << endl;
    // for(int i = 0; i < RequestA.list.size(); i++)
    //     cout << RequestA.list[i] << " ";
    // cout << "\nRequestB: " << endl;
    // for(int i = 0; i < RequestB.list.size(); i++)
    //     cout << RequestB.list[i] << " ";
    // cout << "\nno_exist: " << endl;
    // for(int i = 0; i < no_exist.size(); i++)
    //     cout << no_exist[i] << " ";
    // cout << endl;
    
    //3. send the processed list back to client, backend server
    //3.1 reply to client with the no_exist list if have no exist name
    replyClient(no_exist);
    
    //3.2 reply to the backend server A and B with RequestA and RequestB

    

        
}