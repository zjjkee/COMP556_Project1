#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* simple client, takes two parameters, the server domain name,
   and the server port number */

int main(int argc, char** argv) {

  /* our client socket */
  int sock;

  /* variables for identifying the server */
  unsigned int server_addr;
  struct sockaddr_in sin;
  struct addrinfo *getaddrinfo_result, hints;

  /* convert server domain name to IP address */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; /* indicates we want IPv4 */

  if (getaddrinfo(argv[1], NULL, &hints, &getaddrinfo_result) == 0) {
    server_addr = (unsigned int) ((struct sockaddr_in *) (getaddrinfo_result->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(getaddrinfo_result);
  }

  /* server port number */
  unsigned short server_port = atoi (argv[2]);

  char *buffer, *sendbuffer;
  // int size = 500;
  int recvcount;
  int iterval = 0;

  int size = argv[3];
  int count = argv[4];
  
  double elapsed = 0.0;
  struct timeval tv;
  struct timezone tz;
  long int sent_sec, sent_usec;
  
  if (size < 18 || size > 65535) {
      perror("Invalid size, terminating the program");
      abort();
  }

  if (count < 1 || count > 10000) {
      perror("Invalid count, terminating the program");
      abort();
  }

  /* allocate a memory buffer in the heap */
  /* putting a buffer on the stack like:

         char buffer[500];

     leaves the potential for
     buffer overflow vulnerability */
  buffer = (char *) malloc(size);
  if (!buffer) {
      perror("failed to allocated buffer");
      abort();
  }

  sendbuffer = (char *) malloc(size);
  if (!sendbuffer) {
      perror("failed to allocated sendbuffer");
      abort();
  }

  /* create a socket */
  if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror ("opening TCP socket");
      abort ();
  }

  /* fill in the server's address */
  memset (&sin, 0, sizeof (sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = server_addr;
  sin.sin_port = htons(server_port);

  /* connect to the server */
  if (connect(sock, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
      perror("connect to server failed");
      abort();
  }

  while (iterval < count) {
      // Size in header
      *(short*)(sendbuffer) = (short)htons(size);

      // Timestamp in header
      gettimeofday(&tv, &tz);
      *(long int*)(sendbuffer + 2) = (long int)htonl(tv.tv_sec);
      *(long int*)(sendbuffer + 10) = (long int)htonl(tv.tv_usec);

      // Nothing to do - Rest of data is 0

      // Send the message
      send(sock, sendbuffer, size, 0);

      // Wait for receive
      recvcount = recv(sock, buffer, size, 0);
      if (recvcount <= 0) {
          perror("receive failure");
          abort();
      }

      // Latency calculation    
      if (recvcount != size) {
          printf("Message incomplete, something is still being transmitted\n");
          // Todo: How to handle this?
      }
      else {
          gettimeofday(&tv, &tz);
          sent_sec = (long int)ntohl(*(long int*)(buffer + 2));
          sent_usec = (long int)ntohl(*(long int*)(buffer + 10));
          elapsed = elapsed + ((tv.tv_sec - sent_sec) * 1000 + ((tv.tv_usec - sent_usec) / 1000));
      }

      // Iter update
      iterval = iterval + 1;
  }

  // Final latency value
  printf("Latency: %.3f", (elapsed / count));

  // Free resources 
  close(sock);
  free(buffer);
  free(sendbuffer);

  return 0;
}
