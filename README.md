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
