# EE450 Project Report

> First Name: Haoyu
>
> Last Name: Li
>
> USC Username: hli12442
>
> Student ID: 7947629427



[toc]



## Abstract

In this project, I successfully built up the model with **one TCP link between one main server with client** and **two UDP links between one main server with two backend servers** using socket in **C++**. I defined several types of data struct to **store, process and transmit** two important data in the project, **username and time availability**. In addition, there are a number of functions designed for these three major processes of **storing data, data processing and transmitting within these model**. The following will be detailed code introduction.



## Code Introduction

>There are 4 .cpp files and 4 .h files with two .txt files, one readme.md files and one makefile.

### serverM.cpp serverM.h

> Main server works at three stage: **data storage, data processing and data transmission**.

- Both file contribute to be a main server which have the following functions:

  - Establish TCP connection with client and UDP data transmission with two backend servers.

    ***createServerTCP(), listenClientTCP(), acceptClientTCP(),*** 

    ***createSockUDP(), initSockUDP(), bindSockUDP()***

  - Get the exists name list from backend servers.

    ***getList()***

  - Get the input from client.

    ***recvList()***

  - Examine the input from client.

    ***examineList()***

  - Send "no exist" notification to client and Send request to backend servers.

    ***replyClient(), requestEndServer()***

  - Get the response from backend servers.

    ***getResult()***

  - Processing the result from both backend servers, get their intersection.

    ***getIntersection()***

  - Send the final result back to client.

    ***replyResult()***

### serverA.cpp serverA.h & serverB.cpp serverB.h

>Two Backend servers works at three stage: **data storage, data processing and data transmission**. Interact with txt files to get the basic data and also get the request from main server, finally send the response back.

- Establish UDP socket and ready to send data.

  ***createSockUDP(), initSockUDP(), bindSockUDP()***

- Read txt files and store the data.

  ***readFile()***

- Sending username list to main server.

  ***sendList()***

- Receive the request list from the main server and get the request information.

  ***getRequestInfo()***

- Get the interaction of time availability according to the request.

  ***getIntersection()***

### client.cpp client.h

> client input the name list, then check the validation of input, after stages of **data storage, data processing and data transmission**. Finally get the result of time availability.

- Establish TCP connection with main server.

  ***createClientTCP(), connectServerTCP()***

- Get and check the invalidated input.

  ***checkInput()***

- Send them to main server.

  ***sendList()***

- Get the "no_exist" username list.

  ***recvNoExist()***

- Get the final result of time availability intersection.

  ***recvResult()***

### makefile

> the makefile contains two commands:
>
> - ***make all***: automatically compile serverM.cpp, serverA.cpp, serverB.cpp and client.cpp into four .exe files, serverM, serverA, serverB and client.
> - ***make clean***: automatically clean all the exe files



## Data Format

>At different stages of model, I designed different types of data struct to store usernames and time availability, also the incoming client and backend servers information stored in main server.

- ***string getNameStr(vector\<string\> list)***

  - This function converts the **vector\<string\>** **type** of name list into **string type**.

  - For **string type**, I **add a space to split each name** and the whole data struct will be:

    ***username1\<space\>username2\<space\>...usernameN\<space\>***

- ***vector\<string\> getVector(string str)***

  - This function converts the **string type **of name list into **vector\<string\>** **type**.
  - ***int countSpaces(const string& str)*** used to count the number of <space> in str to for ***getVector()*** to restore them into a string type vector.

### Username

- **vector\<string\> list** type to store the list of usernames.

- Convert to ***string*** type with function ***getNameStr()***, when processing the whole list with the function.
- **String type** will be used to transmit the username list between servers and client.

### Time Availability

#### Data Storage and Data Processing

```c++
typedef struct user{
    string name;                     
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
```

- When backend server first read txt files, I design a **User** data struct to store the usernames and time availability.
- At the stage of data processing, I design **TimeInterval** type to store each time in one time list and use a **TimeInterval** type vector to store the whole time list for each users.
- In order to satisfy each Query from client which includes multiple user information. I defined a **Query** type to store username and their corresponding time list. 

#### Transmitting

```cpp
//sending side
//vector<string> type -> string type
//eg. ServerM.h requestEndServer()
vector<string> list;
string list = getNameStr(list);

//recving side
//string type -> vector<string> type
//eg. ServerM.h recvList()
string str(recv_buf, strlen(recv_buf));
client_input = getVector(str);

//transmitting time availability
vector<char> serialize(vector<TimeInterval> time_list);
vector<TimeInterval> deserialize(vector<char>&& data);
```



### Main Server Connection

```cpp
typedef struct udp_server{
    string id;
    string ip = LOCAL_HOST;
    int port;
    struct sockaddr_in addr;
    socklen_t addr_len;
    vector<string> list;
}Server;

typedef struct tcp_client{
    string id;
    vector<string> list;
}Client;
```

- When main server received the connection from backend servers via UDP, main server store the basic UDP information in the **Server** type of data struct, which also identify each backend server with corresponding username list.
- When main server received the connection from client via TCP, main server store the id of client and the request name list from client into a string type of vector.



## Idiosyncrasy of Project

> Generally good strategy in the stage of data processing

- ***printIntersection()***

  At the phase 3, we need to schedule and get the intersection of client validated input. The following chart is an example to help readers have a good understanding of ***printIntersection()***

  eg. The intersection of [12,16] and [13,17], we can finally get the result [13,16]

  |            |    Max    |   Min   |
  | :--------: | :-------: | :-----: |
  |   **t1**   |    12     |   16    |
  |   **t2**   |    13     |   17    |
  |            | **Start** | **End** |
  | **Result** |    13     |   16    |



> Good data struct design for username and time availability

- In processing the data with client request. At the stage, the main server will process with the username and corresponding list of time availability. A good design of **TimeInterval** to store the time and struct **Query** includes each username and **TimeInterval ** type vector, which greatly improve the efficiency of data processing.

  ```cpp
  typedef struct TimeInterval {
      int start;
      int end;
  }TimeInterval;
  
  typedef struct Query {
      string name;
      vector<TimeInterval> time;
  }Query;
  ```

  



## Reused Code

- To consider how to transmit the **TimeInterval** type of vector, which represents the **entire list of time availability for one user**. The original way is to use **char[]** type but too complicated. So I used **serialize at sending side** and **deserialize at receiving side**

  ```cpp
  vector<char> serialize(vector<TimeInterval> time_list){
      vector<char> data;
      for(auto& interval : time_list){
          char* start_ptr = reinterpret_cast<char*>(&interval.start);
          char* end_ptr = reinterpret_cast<char*>(&interval.end);
  
          data.insert(data.end(), start_ptr, start_ptr + sizeof(interval.start));
          data.insert(data.end(), end_ptr, end_ptr + sizeof(interval.end));
      }
      return data;
  }
  
  vector<TimeInterval> deserialize(vector<char>&& data){
      vector<TimeInterval> time_list;
       for (size_t i = 0; i < data.size(); i += sizeof(TimeInterval)){
          TimeInterval interval;
          char* start_ptr = &data[i];
          char* end_ptr = &data[i + sizeof(interval.start)];
          memcpy(&interval.start, start_ptr, sizeof(interval.start));
          memcpy(&interval.end, end_ptr, sizeof(interval.end));
          if(interval.start > 0 && interval.end > 0)
              time_list.push_back(interval);
       }
      return time_list;
  }
  ```

  I rewrite **serialize** and **deserialize** with this idea to convert the **vector\<TimeInterval\>** into **vector\<char\>** type, which improved the efficiency and feasibility at the stage of transmitting time availability.
