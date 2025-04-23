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

typedef struct TimeInterval {
    int start;
    int end;
}TimeInterval;

typedef struct Query {
    string name;
    vector<TimeInterval> time;
}Query;


//global variable
char file[10] = "a.txt";
vector<User> list_A;

int udp_sockfd_M; // Server A datagram socket
int udp_sockfd_A;
struct sockaddr_in serverA_addr; // AWS address as a server & as a client
struct sockaddr_in serverM_addr;

vector<string> name_list_A;
string list_buf;
char send_buf[1024];
vector<string> request;
char recv_buf[1024];
vector<TimeInterval> result_1;

vector<User> readFile(char *file){
    char read_line[100];
    vector<User> username;
    FILE *fp;

	fp = fopen(file, "r");
    if(fp == NULL) puts("Error");

    while(fgets(read_line,sizeof(read_line),fp) != NULL){
        //
        User user_info;

        //init
        for (int i = 0; i < 10; i++) {
            user_info.time_ava[i][0] = 0;
            user_info.time_ava[i][1] = 0;
        }
        
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

int countSpaces(const string& str) {
    int count = 0;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == ' ') {
            count++;
        }
    }
    return count;
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


void boot_up(){
    //1. local information
    //1.1 create socket
    createSockUDP();
    //1.2 Create sockaddr_in struct
    initSockUDP();
    //1.3 bind socket with specified IP address and port number in server A
    bindSockUDP();
    
    //2. read file
    //vector<User> list_A for each user
    list_A = readFile(file);

    //check the whole list in server A
    // for (const auto& user : list_A) {
    //     cout << "User " << user.name << ": " << endl;

    //     for (int i = 0; i < 10; i++) {
    //         for (int j = 0; j < 2; j++) {
    //             if (user.time_ava[i][j] != 0) { // 
    //                 cout << "time_ava[" << i << "][" << j << "]: " << user.time_ava[i][j] << endl;
    //             }
    //         }
    //     }
    // }

    // for(int i = 0; i < list_A.size(); i++){
    //     cout << list_A[i].name << ": ";

    //     for(auto& row : list_A[i].time_ava){
    //         cout << "[ ";
    //         for(auto& element : row) {
    //             if(element != 0)
    //             cout << element << " ";
    //         }
    //         cout << "],";
    //     }
    //     cout << endl;
    // }

    //3. send username to main server via UDP
    //3.1 get str name list (both string type and vector<string> type)
    name_list_A = getName(list_A);          //vector<string> type
    list_buf = getNameStr(name_list_A);     //vector -> string
    //cout << list_buf << endl;

    //3.2 sending list to main server
    sendList(list_buf);

}

// void forward(){
//     //1. recvice the request list from the main server
//     request = recvRequest();



//     //init()
//     request.clear();
// }

//  getIntersection(){

// }

vector<TimeInterval> getTimeInterval(int time_ava[10][2]){

    vector<TimeInterval> time_list;

    for(int i = 0; i < 10; i++){
        
        if(time_ava[i][0] == 0 || time_ava[i][1] == 0)
            break;
        
        TimeInterval time;

        time.start = time_ava[i][0];
        time.end = time_ava[i][1];

        time_list.push_back(time);

    }

    return time_list;
}


vector<Query> getRequestInfo(vector<string> REQ, vector<User> data){
    vector<Query> RES;

    for(int i = 0; i < REQ.size(); i++){
        for(int j = 0; j < data.size(); j++){

            if(REQ[i] == data[j].name){
                Query info;

                info.name = data[j].name;
                info.time = getTimeInterval(data[j].time_ava);

                RES.push_back(info);
            }
            else
                continue;
        }
    }

    return RES;
}



// vector<TimeInterval> getIntersection(vector<Query> name_time){
//     if(name_time.empty())
//         return vector<TimeInterval>();

//     vector<TimeInterval> intersection = name_time[0].time;
    

//     for(int i = 0; i < name_time.size(); i++){
//         vector<TimeInterval> time_list = name_time[i].time;

//         vector<TimeInterval> newIntersection;
//         for(TimeInterval interval : intersection){
//             for(TimeInterval newInterval : time_list){
//                 if(interval.start <= newInterval.end && newInterval.start <= interval.end){
//                     newIntersection.push_back({
//                         max(interval.start, newInterval.start),
//                         min(interval.end, newInterval.end)
//                     });
//                 }
//             }
//         }

//         intersection = newIntersection;

//         if(intersection.empty())
//             break;

//     }
    
//     return intersection;
    
// }
void printIntersection(vector<TimeInterval> time_list){
    cout << "Found the intersection result: <[";

    for(int i = 0; i < time_list.size(); i++){

        cout << "[" << time_list[i].start << "," << time_list[i].end << "]";

        if(i != (time_list.size() - 1))
            cout << ", ";
    }

    cout << "]>";
}

void printName(vector<Query> user_info){
    cout << " for <";

    for(int i = 0; i < user_info.size(); i++){
        cout << user_info[i].name;

        if(i != (user_info.size() - 1))
            cout << ", ";
    }

    cout << ">." << endl;
}


Query getIntersection(Query timelist_1, Query timelist_2){
    Query intersection;
    vector<TimeInterval> result;

    for(int i = 0; i < timelist_1.time.size(); i++){
        TimeInterval t1 = timelist_1.time[i];

        for(int j = 0; j < timelist_2.time.size(); j++){
            TimeInterval t2 = timelist_2.time[j];

            int overlap_start = max(t1.start, t2.start);
            int overlap_end = min(t1.end, t2.end);
            
            // cout << "i = " << i << " j = " << j << endl;
            // cout << "overlap_start: " << overlap_start << endl;
            // cout << "overlap_end: " << overlap_end << endl;
            
            if(overlap_end > overlap_start){
                // cout << "push back!\n";
                TimeInterval overlap = {overlap_start, overlap_end};
                result.push_back(overlap);
            }
        }
    }

    intersection.time = result;

    return intersection;

}

vector<TimeInterval> schedule(){

    //1. get the users' information that we will response to the main server
    vector<Query> response;
    response = getRequestInfo(request, list_A);

    int user_num = response.size();
    cout << user_num << endl;
    Query result;

    //response.name     string name <-> vector<TimeInterval> time
    //[response.time.start , response.time.end]

    if(user_num == 0)
        return vector<TimeInterval>();

    if(user_num == 1){
        result.time = response[0].time;
    }

    if(user_num >= 2){
        result = response[0];

        for(int i = 1; i < user_num; i++){
            result = getIntersection(result, response[i]);
        }
    }

    //screen print
    printIntersection(result.time);
    printName(response);

    return result.time;
}

vector<char> serialize(vector<TimeInterval> time_list){

    // cout << "serialize start\n";
    vector<char> data;

    for(auto& interval : time_list){
        char* start_ptr = reinterpret_cast<char*>(&interval.start);
        char* end_ptr = reinterpret_cast<char*>(&interval.end);

        data.insert(data.end(), start_ptr, start_ptr + sizeof(interval.start));
        data.insert(data.end(), end_ptr, end_ptr + sizeof(interval.end));
    }
    // cout << "serialize end\n";

    return data;
}

