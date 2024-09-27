# COMP556 Project 1 - Ping-Pong Client/Server

## Team Members

1. **Aathmika Neelakanta (S01469766)** - Responsible for Client development and measurement tasks.
2. **Bo Sung Kim(S01361291)** - Responsible for Client development and measurement tasks.
3. **Tzuhan Su(S01469920)&Jingke Zou(S01505152)** - Responsible for Server development,testing, packaging, and project documentation.

## Project Overview

This project implements a Ping-Pong Client and Server to measure the network's performance by transmitting messages of various sizes. The Client sends ping messages to the Server, and the Server responds with pong messages without modifying the content. The Client measures the latency of each exchange and calculates the average latency.

## Responsibilities

- **Client Development**: 
  - Aathmika and Bo Sung were responsible for developing the client-side of the application, including the implementation of the ping-pong message exchanges and measurement of latency.
  
- **Server Development**: 
  - Tzuhan and Jingke was responsible for developing the server-side of the application, handling multiple concurrent connections and responding to client messages, preparing the Makefile, testing the project on cloud, packaging the project.
  

## How to Build & Run
To build the project, ensure you have a working C compiler on your system. Navigate to the project directory and run:
This guide explains how to build and run the project, which includes a server, a client, and a latencies calculation program. The project uses `gcc` for compiling and `make` to automate the build process.

### Prerequisites


1. **GCC (GNU Compiler Collection)**: Used to compile the C code.
    Install on Linux-based systems:
     ```bash
     sudo apt-get install gcc
     ```

2. **Make**: A tool for managing builds.
     ```bash
     sudo apt-get install make
     ```

### Directory Structure

The project contains the following files and directories:

- `source/server.c`: Source file for the server.
- `source/client_num.c`: Source file for the client.
- `source/latencies.c`: Source file for the latencies calculation program.
- `headers/`: Directory containing any required header files.

### Building the Project

#### 1. Build All Targets

To compile the server, client, and latencies program at once, use the following command:

```bash
make
```

You can also build individual components as needed:

```bash
make server | make client_num | make latencies
```

#### 2. Clean Build Files

```bash
make clean
```

### Run the Executables
After building the project, you can run the executables as follows:
```bash
./server <port>  //server
```
```bash
./client_num <server_ip> <port> <size> <count> //client_num
```
```bash
./latencies <server_ip> <port> <size> <count> //latencies
```

## Functional Testing
Note:\
The **size** refers to the number of bytes in each message sent by the client, and the **count** refers to the number of message exchanges to be performed.

### 1. Basic Functional Testing
#### Start with a small message size (size = 18 bytes) and a small number of exchanges (count = 10), verify the following:
- The client **connects** to the server.
- The server **accepts** the connection from the client.
- The client **sends a ping message** to the server.
- The server **receives the ping message** from the client.
- The server **sends the pong message** back to the client.
- The client **receives the pong message** from the server.
- The client **prints the RTT** correctly for that iteration.
- The client and the server **exchange messages** for **count** times of iterations.
- The client **prints the average RTT** correctly.
- The client **terminates** after completing the message exchanges.
- The server **observes** the connection was closed by the client.

### 2. Edge Cases Testing
#### For the client, verify the following:
- **Size** with a value of 18 (min) and 65535 (max) should function properly.
- **Count** with a value of 1 (min) and 10000 (max) should function properly.

### 3. Concurrency Testing
#### For the following scenarios, ensure monitoring of both the client and server behavior as follows:
- The server responds to each client without crashing.
- Each client receives the pong message back from the server and prints the average RTT.
- The latency measurements from each client are within acceptable ranges (i.e., no unusual delays due to multiple clients).

#### Test the following scenarios:
Note:\
The concurrency testing here involves the stress testing by increasing the number of clients, the size, or the count.

- For **number of clients** = 2 or 10, perform the following test cases:\
  A. Increase the size
  - **size** = 18; **count** = 10
  - **size** = 5000; **count** = 10
  - **size** = 65535; **count** = 10

  B. Increase the count
  - **size** = 18; **count** = 5000
  - **size** = 18; **count** = 10000
  - **size** = 65535; **count** = 5000
  - **size** = 65535; **count** = 10000

### 4. Error Handling Testing
#### For the client, verify the following:
- **Port** outside the range [18000, 18200] should trigger an error.
- **Size** outside the range [18, 65535] should trigger an error.
- **Count** outside the range [1, 10000] should trigger an error.

#### For the server, verify the following:
- **Port** outside the range [18000, 18200] should trigger an error.