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
#define ServerM_TCP_PORT 24427
#define ServerA_UDP_PORT 21427 // Server A port number
#define ServerB_UDP_PORT 22427

using namespace std;

typedef struct TimeInterval {
    int start;
    int end;
}TimeInterval;

//global variable
vector<string> input_list;
int tcp_sockfd_C;
struct sockaddr_in tcp_server_addr;
char send_buf[1024];
char recv_buf[1024];
vector<string> nonlist;
vector<string> no_exist;
vector<TimeInterval> result_final{};

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
    tcp_server_addr.sin_port = ServerM_TCP_PORT; // Port number for client

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
    vector<string> list{};
    string a;
    char b[str.length() + 1];
    strcpy(b, str.c_str());

    a = strtok(b, " ");
    list.push_back(a);

    for(int i = 0; i < (countSpaces(str)); i++){
            a = strtok(NULL, " ");

            auto pos = a.find_last_not_of('\n');
            if (pos != std::string::npos) {
                a.erase(pos + 1);
            }

            list.push_back(a);
        }

    return list;
}

//string -> vector
vector<string> getVector(string str){
    vector<string> list;
    string a;
    char b[1024];
    strcpy(b, str.c_str());

    a = strtok(b, " ");
    list.push_back(a);

    if(countSpaces(str) <= 1)
        return list;

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
    input_list = getVector(str);
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

vector<TimeInterval> deserialize(vector<char>&& data){
    vector<TimeInterval> time_list;
    // cout << "deserialize started" << endl;
    // cout << "data size: " << data.size() << endl;
     for (size_t i = 0; i < data.size(); i += sizeof(TimeInterval)){
        TimeInterval interval;

        char* start_ptr = &data[i];
        char* end_ptr = &data[i + sizeof(interval.start)];

        memcpy(&interval.start, start_ptr, sizeof(interval.start));
        memcpy(&interval.end, end_ptr, sizeof(interval.end));
        // cout << "interval.start: " << interval.start << endl;
        // cout << "interval.end: " << interval.end << endl;

        if(interval.start > 0 && interval.end > 0)
            time_list.push_back(interval);

     }
    // cout << "deserialize ended" << endl;

    return time_list;
}

void printIntersection(vector<TimeInterval> time_list){
    cout << "[";

    for(int i = 0; i < time_list.size(); i++){

        cout << "[" << time_list[i].start << "," << time_list[i].end << "]";

        if(i != (time_list.size() - 1))
            cout << ", ";
    }

    cout << "]";
}

void sendList(vector<string> list){

    string str = getNameStr(list);
    // cout << "client send input to server: " << str << "*" << endl;
    memset(send_buf, 0, 1024);
    
    strcpy(send_buf, str.c_str());

    if(send(tcp_sockfd_C, send_buf, sizeof(send_buf), 0) == FAIL){
        perror("Message Send Error");
		close(tcp_sockfd_C);
		exit(1);
    }

    cout << "Client finished sending the usernames to Main Server." << endl;
}


vector<string> recvNoExist(){
    
    memset(recv_buf, 0, 1024);
    nonlist.clear();
    cout << "recvNoExist(): waiting for no_exist list from Main Server..." << endl;
    int a = recv(tcp_sockfd_C, recv_buf, sizeof(recv_buf), 0);
    if(strlen(recv_buf) == 0){
        cout << "No no_exist list sending back" << endl;
        return nonlist;
    }
    // cout << "**" << endl;
    string str(recv_buf);

    //str.erase(remove(str.begin(), str.end(), '\n'));
    // cout << "str: " << str << "*"<< endl;
    // cout << "str.length() = " << str.length() << endl;

    nonlist = getVector(str);

    if(str.length() != 0)
        cout << "Client received the reply from Main Server using TCP over port " << ServerM_TCP_PORT << ":\n" << str << "do not exist." << endl;

    return nonlist;
}

vector<TimeInterval> recvResult(){

    vector<TimeInterval> result{};
    vector<char> buffer(1024);
    cout << "recvResult(): waiting for final result from Main Server..." << endl;

    ssize_t a = recv(tcp_sockfd_C, buffer.data(), buffer.size(), 0);
    // cout << "a = " << a << endl;
    if(a <= 0 || strlen(buffer.data()) == 0){
        // cout << "No intersection result is sending back" << endl;
        return result;
    }

    result = deserialize(vector<char>(buffer.data(), buffer.data() + buffer.size()));

    return result;
}

vector<string> getFinalUser(vector<string> list, vector<string> nonlist){
    vector<string> result;

    set_difference(list.begin(), list.end(), nonlist.begin(), nonlist.end(), std::back_inserter(result));

    return result;
}


void boot_up(){

    //phase 1
    //1. create TCP connection
    createClientTCP();
    connectServerTCP(); //screen print successful connection

    while(true){
        cout << "Please enter the usernames to check schedule availability: " << endl;
        
        char input_buf[1024];
        if (!fgets(input_buf, sizeof(input_buf), stdin)) {
            // cout << "Input is empty." << endl;  //check empty
            continue;
        }

        string input(input_buf, strlen(input_buf));
        //input.erase(remove(input.begin(), input.end(), '\n'));
        replace(input.begin(), input.end(), '\n', ' ');
        // cout << "string input: " << input << "*" << endl;
        // cout << "input.length() = " << input.length() << endl;

        //2 check input
        //2.1 check name format
        if(!checkInput(input)){
            cout << "-----Start a new request-----" << endl;
            continue;
        }
        //check name in the list? no, main server's job

        //2.2 store, string -> vector<string>
        //check "input_list"
        // cout << "check the whole input list:" << endl;
        // for(int i = 0; i < input_list.size(); i++)
        //     cout << input_list[i] << endl;
        
        //phase 2
        //forwarding
        //1. send to main server the list (string)  "vector<string> input_list"
        sendList(input_list);

        //2. get the no_exist list from main server
        no_exist = recvNoExist();
        //check no_exist
        
        // if(no_exist.size() != 0){
        //     cout << "no_exist: ";
        //     for(int i = 0; i < no_exist.size(); i++)
        //         cout << no_exist[i] << " ";
        //     cout << endl;
        // }
        // // else
        // //     cout << "All user exist!" << endl;
        
        //phase 4
        //get the final result from main server
        result_final = recvResult();
        vector<string> user_final = getFinalUser(input_list, no_exist);
        string list_print = getNameStr(user_final);
        cout << "Client received the reply from Main Server using TCP over port: " << ServerM_TCP_PORT << ":\nTime intervals ";
        printIntersection(result_final);
        cout << " works for " << list_print << endl;
        
        
        //init
        no_exist.clear();
        result_final.clear();
        // cout << "no_exist cleared" << endl;

        cout << "-----Start a new request-----" << endl;
    }

    close(tcp_sockfd_C);

}

void signalHandler(int signum) {
//    cout << "\nInterrupt signal (" << signum << ") received.\n";
   exit(signum);  
}