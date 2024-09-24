#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>


/* simple client, takes two parameters, the server domain name,
   and the server port number */

uint64_t htobe64(uint64_t host_64bits);
uint64_t be64toh(uint64_t big_endian_64bits);

int main(int argc, char **argv) {
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
  uint16_t server_port = (uint16_t)(atoi(argv[2]));

  char *buffer, *sendbuffer;
  int recvcount;
  int interval = 0;

  /* size in bytes of the message */
  uint16_t size = (uint16_t)atoi(argv[3]);

  /* number of message exchanges to perform */
  int count = atoi(argv[4]);
  
  double elapsed = 0.0;
  struct timeval tv;
  uint64_t client_send_tv_sec, client_send_tv_usec, client_recv_tv_sec, client_recv_tv_usec;
  uint64_t server_tv_sec, server_tv_usec;

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

  // initialize the buffer for receiving
  buffer = (char *) malloc(size);
  if (!buffer) {
    perror("failed to allocated buffer");
    abort();
  }

  // initialize the sendbuffer to 0s
  sendbuffer = (char *)calloc(size, sizeof(char));
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

  while (interval < count) {
    // set the size in message
    *(uint16_t *)(sendbuffer) = (uint16_t) htons(size);

    // set the 1st timestamp in message
    if(gettimeofday(&tv,NULL) == 0){
      client_send_tv_sec = (uint64_t)tv.tv_sec;
      client_send_tv_usec = (uint64_t)tv.tv_usec;
    }
    *(uint64_t *)(sendbuffer + 2) = (uint64_t) htobe64(client_send_tv_sec);
    *(uint64_t *)(sendbuffer + 10) = (uint64_t) htobe64(client_send_tv_usec);

    // Nothing to do - Rest of data is 0

    // Send the message
    int sentSizeCount = 0;
    int tempSent = 0;
    
    while (sentSizeCount != size){
      tempSent = send(sock, sendbuffer + sentSizeCount, size - sentSizeCount, 0);
      if (tempSent <= 0) {
        continue;
      }
      sentSizeCount += tempSent;
    }



    // Wait for receive
    int recSizeCount = 0;
    int tempRec = 0;
    
    while (recSizeCount != size){
      tempRec = recv(sock, buffer + recSizeCount, size - recSizeCount, 0);
      if (tempRec <= 0) {
        continue;
      }
      recSizeCount += tempRec;
    }
    // recvcount = recv(sock, buffer, size, 0);
    // if (recvcount != size) {
    //     perror("receive failure");
    //     abort();
    // }

    // calculate the total latency = (client send to server) + (server send to client)
    server_tv_sec = be64toh(*(uint64_t *)(buffer + 18));
    server_tv_usec = be64toh(*(uint64_t *)(buffer + 26));

    double ctos_sec_diff = server_tv_sec - client_send_tv_sec;
    double ctos_usec_diff = server_tv_usec - client_send_tv_usec;
    double ctos_msec_diff = (double)(ctos_sec_diff * 1000 + ((double)ctos_usec_diff) / 1000);

    if (gettimeofday(&tv, NULL) == 0)
    {
      client_recv_tv_sec = (uint64_t)tv.tv_sec;
      client_recv_tv_usec = (uint64_t)tv.tv_usec;
    }

    double stoc_sec_diff = client_recv_tv_sec - server_tv_sec;
    double stoc_usec_diff = client_recv_tv_usec - server_tv_usec;
    double stoc_msec_diff = (double)(stoc_sec_diff * 1000 + ((double)stoc_usec_diff) / 1000);

    double total_msec_diff = ctos_msec_diff + stoc_msec_diff;

    printf("Iteration %d:\n", interval + 1);
    printf("- Client-to-server latency: %.3lf ms\n", ctos_msec_diff);
    printf("- Server-to-client latency: %.3lf ms\n", stoc_msec_diff);
    printf("- Total latency: %.3lf ms\n\n", total_msec_diff);

    // calculate the total latency over iterations
    elapsed += total_msec_diff;

    // Iter update
    interval = interval + 1;
  }

  // Final latency value
  printf("Average latency of %d iterations: %.3f\n", count, (elapsed / count));

  // Free resources 
  close(sock);
  free(buffer);
  free(sendbuffer);

  return 0;
}

// uint64_t htobe64(uint64_t host_64bits)
// {
//   uint64_t result = 0;
//   result |= (host_64bits & 0x00000000000000FF) << 56;
//   result |= (host_64bits & 0x000000000000FF00) << 40;
//   result |= (host_64bits & 0x0000000000FF0000) << 24;
//   result |= (host_64bits & 0x00000000FF000000) << 8;
//   result |= (host_64bits & 0x000000FF00000000) >> 8;
//   result |= (host_64bits & 0x0000FF0000000000) >> 24;
//   result |= (host_64bits & 0x00FF000000000000) >> 40;
//   result |= (host_64bits & 0xFF00000000000000) >> 56;
//   return result;
// }

// uint64_t be64toh(uint64_t big_endian_64bits)
// {
//   uint64_t result = 0;

//   result |= (big_endian_64bits & 0x00000000000000FFULL) << 56;
//   result |= (big_endian_64bits & 0x000000000000FF00ULL) << 40;
//   result |= (big_endian_64bits & 0x0000000000FF0000ULL) << 24;
//   result |= (big_endian_64bits & 0x00000000FF000000ULL) << 8;
//   result |= (big_endian_64bits & 0x000000FF00000000ULL) >> 8;
//   result |= (big_endian_64bits & 0x0000FF0000000000ULL) >> 24;
//   result |= (big_endian_64bits & 0x00FF000000000000ULL) >> 40;
//   result |= (big_endian_64bits & 0xFF00000000000000ULL) >> 56;

//   return result;
// }