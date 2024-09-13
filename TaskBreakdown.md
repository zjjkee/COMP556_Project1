# Project Breakdown and Task Assignment:

## Task 1: Client Program Development
### Assigned to []  
Responsibilities:  
1.Develop the ping-pong client program.Implement the command line parameter handling for hostname, port, size, and count.
Code the logic for sending ping messages and receiving pong messages.  
2.Use gettimeofday() to measure latency and print average latency results.  
3.Deliverables:  
client.c file with implemented client program.
Ensure it handles edge cases for message sizes and counts.

## Task 2: Server Program Development
### Assigned to []  
Responsibilities:  
1.Develop the ping-pong server program.
Implement command line parameter handling for the port.
Code the server logic to accept connections and respond to incoming ping messages with pong messages.  
2.Ensure the server handles multiple concurrent connections.  
3.Deliverables:  
server.c file with implemented server program.
Ensure the server can manage concurrent client connections properly.

## Task 3: Measurement and Analysis (Part II)
### Assigned to []  
Responsibilities:  
1.Design a method to measure the bandwidth-independent delay across two servers.    
2.Conduct the measurements using the developed client and server programs. Calculate the estimated bandwidth-independent delay and network link bandwidth.  
3.Deliverables:  
A detailed write-up explaining the measurement method, results, and calculations.
Submit as part2.pdf with clear reasoning and precise wording.

## Task 4: Testing, Documentation, and Submission
### Assigned to []
Responsibilities:  
1.Create and maintain the Makefile to compile both client and server programs.  
2.Test the client and server programs on the CLEAR servers to ensure they work as expected.  
3.Prepare the README file documenting group members, testing procedures, known issues, and any important notes for graders.  
4.Package all files (client.c, server.c, part2.pdf, Makefile, README) into project1.tar.gz.  
5.Deliverables:  
Ensure the entire project compiles and runs correctly on CLEAR.
Manage the submission to Canvas, ensuring it meets all administrative requirements.