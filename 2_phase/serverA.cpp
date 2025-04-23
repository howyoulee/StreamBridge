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

#include "serverA.h"

using namespace std;

int main(void) {
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

        //1. recvice the request list from the main server
        ssize_t recv_bytes = recvfrom(udp_sockfd_A, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&serverA_addr, &serverA_addr_len);
        if(recv_bytes < 0){
            continue;
        }
        
        string list_str(recv_buf, recv_bytes);
        cout << "get the request list: " << list_str << endl;

        request = getVector(list_str);
        
        //2. process list
        // vector<string> name_list_A       all users name in vector
        // vector<User> list_A              each user's information
        /*
        typedef struct user{
            string name;                     //name check
            int time_ava[10][2];
        }User;
        */
        



        //3. send back list


        //init
        request.clear();
    }
    //

    close(udp_sockfd_A);
    close(udp_sockfd_M);

    return 0;
}

