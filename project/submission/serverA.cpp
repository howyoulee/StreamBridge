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
#include <csignal>

#include "serverA.h"

using namespace std;

int main(void) {
    signal(SIGINT, signalHandler);  

    boot_up();
    
    // cout << list_A[2].name << endl;
    // //check username list
    // for(auto& row : list_A[0].time_ava) {
    //     for(auto& element : row) {
    //         if(element != 0)
    //         cout << element << " ";
    //     }
    //     cout << endl;
    // }

    //check name
    // for(int i = 0; i < list_A.size(); i++)
    //     cout << list_A[i].name << endl;


    //file input
    while(true){

        initSockUDP();
        
        memset(recv_buf, 0, 1024);
        socklen_t serverA_addr_len = sizeof(serverA_addr);

        /**********************************************************************/
        //1. recvice the request list from the main server
        // cout << "while(): waiting for request from Main Server..." << endl;

        ssize_t recv_bytes = recvfrom(udp_sockfd_A, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&serverA_addr, &serverA_addr_len);
        if(recv_bytes <= 0){
            // cout << "no request from main server\n";
            vector<char> empty = serialize(result_1);
            
            sendto(udp_sockfd_A, empty.data(), empty.size(), 0, (struct sockaddr*)&serverM_addr, sizeof(serverM_addr));

            continue;
        }
        
        string list_str(recv_buf, recv_bytes);
        // cout << "get the request list: " << list_str << endl;
        cout << "Server A received the usernames from Main Server using UDP over port " << ServerA_UDP_PORT << endl;

        request = getVector(list_str);

        /**********************************************************************/
        //2. process list
        // vector<string> name_list_A       all users name in vector
        // vector<string> request           request username from main server
        // vector<User> list_A              each user's information
        /*
        typedef struct user{
            string name;                     //name check
            int time_ava[10][2];
        }User;
        */
       result_1 = schedule();

    //    //check the result
    //     printIntersection(result_1);


       
        /**********************************************************************/
        //3. send back list
        memset(send_buf, 0, 1024);

        // cout << "while(): send back result time list..." << endl;

        vector<char> serialized_data = serialize(result_1);

        ssize_t send_bytes = sendto(udp_sockfd_A, serialized_data.data(), serialized_data.size(), 0, (struct sockaddr*)&serverM_addr, sizeof(serverM_addr));
        
        // cout << "send_bytes: " << send_bytes << endl;
        if(send_bytes < 0){
            continue;
        }
        
        // string x(serialized_data.data(), send_bytes);
        // cout << "send the response list: " << x << endl;

        //init
        request.clear();
        result_1.clear();
        
    }
    //

    close(udp_sockfd_A);
    close(udp_sockfd_M);

    return 0;
}

