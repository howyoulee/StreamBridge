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

#include "serverM.h"

using namespace std;


int main(void) {
    
    boot_up();

    while(true){

        //1. get list from client
        client_list = recvList();
        if(client_list.size() <= 0)
            continue;
        //check list
        cout << "check the whole input list:" << endl;
        // cout << "input list size: " << client_list.size() << endl;
            for(int i = 0; i < client_list.size(); i++)
                cout << client_list[i] << endl;

        //2. check list with the stored name list from backend server A and B
        
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
        // cout << "no_exist.size(): " << no_exist.size() << endl;
        replyClient(no_exist);
        
        //3.2 reply to the backend server A and B with RequestA and RequestB
        requestEndServer(RequestA, ServerA);
        //requestEndServer(RequestB, ServerB);

        //4. get time availbility from backend server and reply client
        // Server ServerA;
        // vector<string> ServerA.list (username list)
        // vector<string> RequestA.list (username list)
        // string ServerA.id = 'A'
        // 
        //
        //4.1 waiting for information from response from backend server

        //4.2 get the intersection of two list from 

        //4.3 reply the information to client


        //init
        RequestA.list.clear();
        no_exist.clear();

    }


    close(udp_sockfd_M);
    close(tcp_sockfd_S);
    close(tcp_sockfd_child);
    return 0;
}