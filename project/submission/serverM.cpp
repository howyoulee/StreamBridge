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
#include <csignal>

#include "serverM.h"

using namespace std;

int main(void) {
    
    signal(SIGINT, signalHandler);  

    boot_up();

    while(true){

        no_exist.clear();

        //1. get list from client
        client_list = recvList();
        if(client_list.size() <= 0)
            continue;
        //check list
        // cout << "check the whole input list: " << endl;
        // // cout << "input list size: " << client_list.size() << endl;
        // for(int i = 0; i < client_list.size(); i++)
        //     cout << client_list[i] << " ";
        // cout << endl;

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
        requestEndServer(RequestB, ServerB);

        //4. get time availbility from backend server and reply client
        // Server ServerA;
        // vector<string> ServerA.list (username list)
        // vector<string> RequestA.list (username list)
        // string ServerA.id = 'A'

        //4.1 waiting for information of response from backend server
        vector<TimeInterval> result_1 = getResult(ServerA);
        vector<TimeInterval> result_2 = getResult(ServerB);

        if(result_1.empty() && result_2.empty()){
            vector<TimeInterval> result_final{};
            replyResult(result_final);

            // no_exist.clear();
            continue;
        }
            
        
        if(!(result_1.empty())){
            cout << "Main Server received from server A the intersection result using UDP over port " << ServerA_UDP_PORT << " :\n";
            printIntersection(result_1);
            cout << endl;
        }
        
        if(!(result_2.empty())){
            cout << "Main Server received from server B the intersection result using UDP over port " << ServerB_UDP_PORT << " :\n";
            printIntersection(result_2);
            cout << endl;
        }

        //4.2 get the intersection of two list from
        vector<TimeInterval> result_final{};

        if(result_1.empty()){
            // cout << "11\n";
            for(int i = 0; i < result_2.size(); i++)
                result_final.push_back(result_2[i]);
        }
        else if(result_2.empty()){
            // cout << "22\n";
            for(int i = 0; i < result_1.size(); i++)
                result_final.push_back(result_1[i]);
        }
        else{
            // cout << "33\n";
            result_final = getIntersection(result_1, result_2);
        }
        cout << "Found the intersection between the results from server A and B:\n";
        printIntersection(result_final);
        cout << "\n";
        
        //4.3 reply the information to client
        replyResult(result_final);

        //init
        RequestA.list.clear();
        RequestB.list.clear();
        no_exist.clear();

    }


    close(udp_sockfd_M);
    close(tcp_sockfd_S);
    close(tcp_sockfd_child);
    return 0;
}