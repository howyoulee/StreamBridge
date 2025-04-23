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

#include <algorithm>
#include <set>


#define MAX_SIZE_TA 10
#define MAX_SIZE_USER 10
#define LOCAL_HOST "127.0.0.1" // Host address
#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define FAIL -1
#define MAX_FILE_SIZE 1000
#define LOCAL_HOST "127.0.0.1" // Host address
#define ServerM_UDP_PORT 23427 // Main Server port number
#define ServerA_UDP_PORT 21427 // Server A port number
#define ServerB_UDP_PORT 22427

using namespace std;

//global variable
vector<string> input_list;
int tcp_sockfd_C;
struct sockaddr_in tcp_server_addr;


void createClientTCP(){

    tcp_sockfd_C = socket(AF_INET, SOCK_STREAM, 0); // Create TCP socket

    if (tcp_sockfd_C == FAIL) {
        perror("[ERROR] main server: fail to create socket for client");
        exit(1);
    }

    // Initialize IP address, port number
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr)); //  make sure the struct is empty

    tcp_server_addr.sin_family = AF_INET; // Use IPv4 address family
    tcp_server_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST); // Host IP address
    tcp_server_addr.sin_port = ServerM_UDP_PORT; // Port number for client

    // // Bind socket for client with IP address and port number for client
    // if (::bind(tcp_sockfd_C, (struct sockaddr *) &tcp_server_addr, sizeof(tcp_server_addr)) == FAIL) {
    //     perror("[ERROR] main server: fail to bind client socket");
    //     exit(1);
    // }

}

void connectServerTCP(){

    if (connect(tcp_sockfd_C, (struct sockaddr *) &tcp_server_addr, sizeof(tcp_server_addr)) == FAIL) {
        perror("[ERROR] client: fail to connect with main server");
        close(tcp_sockfd_C);
        exit(1); // If connection failed, we cannot continue
    }

    cout << "Client is up and running." << endl;
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

vector<string> getList(string str){
    vector<string> list;
    string a;
    char b[str.length() + 1];
    strcpy(b, str.c_str());

    a = strtok(b, " ");
    list.push_back(a);

    for(int i = 0; i < (countSpaces(str)); i++){
            a = strtok(NULL, " ");

            list.push_back(a);
        }

    return list;
}

bool checkCase(string str){

    for (int i = 0; i < str.length(); i++) {
        if (isupper(str[i])) {
            return false;
        }
    }
    return true;
}

bool checkSpecials(string str){

    for (int i = 0; i < str.length(); i++) {
        if (!isspace(str[i]) && !isalnum(str[i])) {
            return false;
        }
    }
    return true;
}

string eraseSpaces(string str){

    auto new_end = unique(str.begin(), str.end(), [](char a, char b){
        return a == ' ' && b == ' ';
    });

    str.erase(new_end, str.end()); // erase extra spaces

    return str;
}

bool checkNum(vector<string> list){

    if(list.size() <= MAX_SIZE_USER)
        return true;
    else
        return false;
}

bool checkUnique(vector<string> list){
    set<string> unique_strings(list.begin(), list.end());

    if (unique_strings.size() == list.size())
        return true;
    else
        return false;
}

bool checkInput(string str){
    //1. erase extra spaces
    str = eraseSpaces(str);
    // cout << "done: 1" << endl;

    //2. check all lower case
    if(!checkCase(str)){
        cout << "Input is not all Lowercases." << endl;
        return false;
    }
    // cout << "done: 2" << endl;

    //3. check no specials
    if(!checkSpecials(str)){
        cout << "Input has Specials." << endl;
        return false;
    }
    // cout << "done: 3" << endl;

    //4. get name list, string -> vector<string>
    input_list = getList(str);
    // cout << "done: 4" << endl;

    //5. check num of name < MAX_SIZE_USER
    if(!checkNum(input_list)){
        cout << "Input number of name exceeded the MAX." << endl;
        return false;
    }
    // cout << "done: 5" << endl;

    //6. check input name unique
    if(!checkUnique(input_list)){
        cout << "Input has duplicated name." << endl;
        return false;
    }
    // cout << "done: 6" << endl;

    return true;
}

void boot_up(){

    
        //1. create TCP connection
        createClientTCP();
        connectServerTCP(); //screen print successful connection

    while(true){
        cout << "Please enter the usernames to check schedule availability: " << endl;
        
        char input_buf[100];
        if (!fgets(input_buf, sizeof(input_buf), stdin)) {
            cout << "Input is empty." << endl;  //check empty
            continue;
        }

        string input(input_buf);
        // cout << "string input: " << input << endl;
        //2 check input
        //2.1 check name format
        if(!checkInput(input)){
            cout << "-----Start a new request-----" << endl;
            continue;
        }
        //check name in the list? no, main server's job

        //2.2 store, string -> vector<string>
        //check "input_list"
        cout << "check the whole input list:" << endl;
        for(int i = 0; i < input_list.size(); i++)
            cout << input_list[i] << endl;

        //3. send to main server the list (string)
        //string input


    }
}

