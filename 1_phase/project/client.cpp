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

#include "client.h"

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
string operation;

int main(int argc, char ** argv) {
    //1. client boot up
    boot_up();

    //OPERATIONS
    operation = argv[1];
    

    return 0;
}

