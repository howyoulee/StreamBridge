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

#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

using namespace std;

#define MAX_SIZE_TA 10
#define MAX_SIZE_USER 10

#define LOCAL_HOST "127.0.0.1" // Host address
#define ServerM_UDP_PORT 23427 // Main Server port number
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

//global variable
int udp_sockfd_M, tcp_sockfd_S, tcp_sockfd_C, tcp_sockfd_child;
struct sockaddr_in serverM_addr, tcp_server_addr, tcp_client_addr, tcp_child_addr;
char recv_buf[1024];
Server ServerA;
Server ServerB;

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
    tcp_server_addr.sin_port = ServerM_UDP_PORT; // Port number for monitor

    // Bind socket
    if (::bind(tcp_sockfd_S, (struct sockaddr *) &tcp_server_addr, sizeof(tcp_server_addr)) == FAIL) {
        perror("[ERROR] main server: fail to bind monitor socket");
        exit(1);
    }
}

void createClientTCP(){

    tcp_sockfd_C = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket
    if (tcp_sockfd_C == FAIL) {
        perror("[ERROR] main server: fail to create socket for client");
        exit(1);
    }

    // Initialize IP address, port number
    memset(&tcp_client_addr, 0, sizeof(tcp_client_addr)); //  make sure the struct is empty
    tcp_client_addr.sin_family = AF_INET; // Use IPv4 address family
    tcp_client_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    tcp_client_addr.sin_port = 0; // Port number for client

    // Bind socket for client with IP address and port number for client
    if (::bind(tcp_sockfd_C, (struct sockaddr *) &tcp_client_addr, sizeof(tcp_client_addr)) == FAIL) {
        perror("[ERROR] main server: fail to bind client socket");
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
        cout << "accept client successfully!" << endl;

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