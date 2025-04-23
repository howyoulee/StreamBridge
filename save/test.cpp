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
using namespace std;

char file[10] = "a.txt";

int main() {
    // char read_line[100];

    // FILE *fp;

    // fp = fopen(file, "r");
    //   if(fp == NULL) puts("Error");

    // while(fgets(read_line,sizeof(read_line),fp) != NULL){
    //   cout<<read_line<<endl;
    //   char *a;
    //   a = strtok(read_line, ";");
    //   a = strtok(NULL, ";");
    //   cout << "***" << a[strlen(a) - 1];
    // }

    string a[5];
    a[0] = "abc";
    a[1] = "bcd";
    a[2] = "cbd";

    cout << a[4] <<endl;

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(12345); // 远程服务器的端口号
    remote_addr.sin_addr.s_addr = inet_addr("192.168.0.100"); // 远程服务器的IP地址

    const char* message = "Hello, server!";
    ssize_t sent_bytes = sendto(sock_fd, message, strlen(message), 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (sent_bytes > 0) {
        std::cout << "Sent " << sent_bytes << " bytes to " << inet_ntoa(remote_addr.sin_addr) << ":" << ntohs(remote_addr.sin_port) << std::endl;
    }

    close(sock_fd);
    return 0;
}



  return 0;
}
