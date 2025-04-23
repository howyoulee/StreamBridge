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


#define MAX_SIZE_TA 10
#define MAX_SIZE_USER 10

#define LOCAL_HOST "127.0.0.1" // Host address
#define ServerA_UDP_PORT 21427 // Server A port number
#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define FAIL -1
#define MAX_FILE_SIZE 1000
using namespace std;

typedef struct user{
    string name;                     //name check
    int time_ava[10][2];
}User;

//global variable
char file[10] = "a.txt";
vector<User> list_A;

int udp_sockfd_M; // Server A datagram socket
int udp_sockfd_A;
struct sockaddr_in serverA_addr; // AWS address as a server & as a client
struct sockaddr_in serverM_addr;

vector<string> name_list_A;
string list_buf;
string send_buf;

vector<User> readFile(char *file){
    char read_line[100];
    vector<User> username;
    FILE *fp;

	fp = fopen(file, "r");
    if(fp == NULL) puts("Error");

    while(fgets(read_line,sizeof(read_line),fp) != NULL){
        //
        User user_info{};
        
        // for(auto& row : user_info.time_ava) {
        //     for(auto& element : row) {
        //         cout << element << " ";
        //     }
        //     cout << endl;
        // }
        
        // puts(read_line);
        string a;
        string num_str;     num_str = "";
        int num = 0;
        int ta_count = 0;

        //"name" -> user_info.name
        user_info.name = strtok(read_line, ";");    //puts(user_info.name);

        //after ";" "data" -> user_info.time_ava
        a = strtok(NULL, ";");                      //puts(a);

        //storeTime(a);
        // cout << "string length: " << strlen(a) << endl;

        for(int i = 0; i < a.length(); i++){
            // cout << "i = " << i << endl;
            // cout << "a[i] = " << a[i] << " isdigit? " << isdigit(a[i]) << endl;
            // cout << "a[i+1] = " << a[i+1] << " isdigit? " << isdigit(a[i+1]) << endl;

            if(isdigit(a[i+1])){
                num_str += a[i+1];
                // cout << "add a num: " << num_str << endl;
                continue;
            }

            if(a[i+1] == '[')
                continue;

            if(a[i+1] == ',' && isdigit(a[i])){     //store start_time
                // cout << "store a start_time " << num_str << endl;
                
                num = stoi(num_str);
                user_info.time_ava[ta_count][0] = num;

                num_str.clear();
                continue;
            }
            else if(a[i+1] == ',')
                continue;

            if(a[i+1] == ']' && isdigit(a[i])){     //store end_time
                // cout << "store a end_time " << num_str << endl;

                num = stoi(num_str);
                user_info.time_ava[ta_count][1] = num;

                ta_count += 1;
                num_str.clear();
                continue;
            }
            else if(a[i+1] == ']')
                break;
        }

        //push user_info -> vector username
        username.push_back(user_info);
        //cout << user_info.name << endl;
        
    }
    // for(int i = 0; i < username.size(); i++)
    //     cout << username[i].name << endl;

    return username;
}

vector<string> getName(vector<User> list){
    vector<string> username_list;
    string a;

    for(int i = 0; i < list.size(); i++){
            a = list[i].name;
            //cout << list[i].name << endl;
            username_list.push_back(a);
    }

    return username_list;
}

string getNameStr(vector<string> list){
    string name_str = "";
    for(int i = 0; i < list.size(); i++){
        name_str += list[i];

        name_str += " ";
    }
    //cout << name_str << endl;
    return name_str;
}

void createSockUDP(){
    // Create a UDP socket
    udp_sockfd_A = socket(AF_INET, SOCK_DGRAM, 0);

    if (udp_sockfd_A == FAIL) {
        perror("Error: server A socket open denied!\n");
        exit(1);
    }
}

void initSockUDP(){
    // Initialize server A IP address, port number
    memset(&serverA_addr, 0, sizeof(serverA_addr)); //  make sure the struct is empty

    serverA_addr.sin_family = AF_INET; // Use IPv4 address family
    serverA_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    serverA_addr.sin_port = htons(ServerA_UDP_PORT); // Server A port number
}

void bindSockUDP(){
    if (::bind(udp_sockfd_A, (struct sockaddr *) &serverA_addr, sizeof(serverA_addr)) == FAIL) {
        perror("Error: Server A failed to bind UDP socket");
        exit(1);
    }

    cout << "Server A is up and running using UDP on port " << ServerA_UDP_PORT << endl;
}

void sendList(string list){

    memset(&serverM_addr, 0, sizeof(serverM_addr));

    serverM_addr.sin_family = AF_INET;
    serverM_addr.sin_port = htons(23427);
    serverM_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    ssize_t sent_bytes = sendto(udp_sockfd_A, list.c_str(), list.length(), 0, (struct sockaddr*)&serverM_addr, sizeof(serverM_addr));
    if (sent_bytes > 0) {
        // cout << "Sent " << sent_bytes << " bytes to " << inet_ntoa(serverM_addr.sin_addr) << ":" << ntohs(serverM_addr.sin_port) << endl;

        //screen print for finishing send list to main server
        cout << "Server A finished sending a list of usernames to Main Server." << endl;
    }

    close(udp_sockfd_A);
}

void boot_up(){
    //1. local information
    //1.1 create socket
    createSockUDP();
    //1.2 Create sockaddr_in struct
    initSockUDP();
    //1.3 bind socket with specified IP address and port number in server A
    bindSockUDP();
    
    //2. read file
    list_A = readFile(file);

    //3. send username to main server via UDP
    //3.1 get str name list (both string type and vector<string> type)
    name_list_A = getName(list_A);          //vector<string> type
    list_buf = getNameStr(name_list_A);     //string type (one line)
    //cout << list_buf << endl;

    //3.2 sending list to main server
    sendList(list_buf);

}
