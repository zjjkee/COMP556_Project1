#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <math.h>


/* simple client, takes two parameters, the server domain name,
   and the server port number */

// uint64_t htobe64(uint64_t host_64bits);
// uint64_t be64toh(uint64_t big_endian_64bits);

int main(int argc, char** argv) {
    /* our client socket */
    int sock;

    /* variables for identifying the server */
    unsigned int server_addr;
    struct sockaddr_in sin;
    struct addrinfo* getaddrinfo_result, hints;

    /* convert server domain name to IP address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* indicates we want IPv4 */

    if (getaddrinfo(argv[1], NULL, &hints, &getaddrinfo_result) == 0) {
        server_addr = (unsigned int)((struct sockaddr_in*)(getaddrinfo_result->ai_addr))->sin_addr.s_addr;
        freeaddrinfo(getaddrinfo_result);
    }

    /* server port number */
    uint16_t server_port = (uint16_t)(atoi(argv[2]));

    char* buffer, * sendbuffer;
    int interval = 0;

    /* size in bytes of the message */
    uint16_t size = (uint16_t)atoi(argv[3]);

    /* number of message exchanges to perform */
    int count = atoi(argv[4]);

    double elapsed = 0.0;
    struct timeval tv;

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
    /*buffer = (char*)malloc(size);
    if (!buffer) {
        perror("failed to allocated buffer");
        abort();
    }

    // initialize the sendbuffer to 0s
    sendbuffer = (char*)calloc(size, sizeof(char));
    if (!sendbuffer) {
        perror("failed to allocated sendbuffer");
        abort();
    }*/

    /* create a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("opening TCP socket");
        abort();
    }

    /* fill in the server's address */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = server_addr;
    sin.sin_port = htons(server_port);

    /* connect to the server */
    if (connect(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("connect to server failed");
        abort();
    }

    double bandwidths, delay_ind = 0.0;
    int testiters = 50;
    int x;

    for (x = 0; x < testiters; x++) {
        uint16_t msgsize[2];
        double latencies[2];
        int k;

        srand(time(0));
        msgsize[0] = (rand() % (9000 - 20 + 1)) + 20;
        msgsize[1] = 65535 - msgsize[0];

        for (k = 0; k < 2; k++) {
            printf("\nFor message size: %d\n", msgsize[k]);
            size = msgsize[k];
            interval = 0;

            // initialize the buffer for receiving
            buffer = (char*)malloc(size);
            if (!buffer) {
                perror("failed to allocated buffer");
                abort();
            }

            // initialize the sendbuffer to 0s
            sendbuffer = (char*)calloc(size, sizeof(char));
            if (!sendbuffer) {
                perror("failed to allocated sendbuffer");
                abort();
            }

            elapsed = 0.0;

            while (interval < count) {
                // set the size in message
                *(uint16_t*)(sendbuffer) = (uint16_t)htons(size);

                // set the 1st timestamp in message
                double client_send_tv_sec, client_send_tv_usec, recv_tv_sec, recv_tv_usec;
                if (gettimeofday(&tv, NULL) == 0) {
                    client_send_tv_sec = (uint64_t)tv.tv_sec;
                    client_send_tv_usec = (uint64_t)tv.tv_usec;
                }
                *(uint64_t*)(sendbuffer + 2) = (uint64_t)htobe64(client_send_tv_sec);
                *(uint64_t*)(sendbuffer + 10) = (uint64_t)htobe64(client_send_tv_usec);

                // Nothing to do - Rest of data is 0

                // Send the message
                int sentSizeCount = 0;
                int tempSent = 0;

                while (sentSizeCount != size) {
                    tempSent = send(sock, sendbuffer + sentSizeCount, size - sentSizeCount, 0);
                    if (tempSent <= 0) {
                        continue;
                    }
                    sentSizeCount += tempSent;
                }

                // Wait for receive
                int recSizeCount = 0;
                int tempRec = 0;

                while (recSizeCount != size) {
                    tempRec = recv(sock, buffer + recSizeCount, size - recSizeCount, 0);
                    if (tempRec <= 0) {
                        continue;
                    }
                    recSizeCount += tempRec;
                }

                double sent_tv_sec = be64toh(*(uint64_t*)(buffer + 2));
                double sent_tv_usec = be64toh(*(uint64_t*)(buffer + 10));

                if (gettimeofday(&tv, NULL) == 0)
                {
                    recv_tv_sec = (uint64_t)tv.tv_sec;
                    recv_tv_usec = (uint64_t)tv.tv_usec;
                }

                double lat_sec_diff = recv_tv_sec - sent_tv_sec;
                double lat_usec_diff = recv_tv_usec - sent_tv_usec;
                double total_lat = (double)(lat_sec_diff * 1000 + ((double)lat_usec_diff) / 1000);

                printf("Iteration %d:\n", interval + 1);
                printf("- Total latency: %.3lf ms\n\n", total_lat);

                // calculate the total latency over iterations
                elapsed += total_lat;

                // Iter update
                interval = interval + 1;
            }

            // Final latency value
            printf("Average latency of %d iterations: %.3f\n", count, (elapsed / count));
            latencies[k] = (elapsed / count);

            free(buffer);
            free(sendbuffer);
        }

        double bw_dep = fabs(((msgsize[1] - msgsize[0]) * 2) / (latencies[1] - latencies[0]));
        printf("Dependent bandwidth: %.3lf bytes/msec\n", bw_dep);
        double t_ind = fabs(latencies[0] - (msgsize[0] * 2)) / bw_dep;
        printf("Independent latency: %.3lf ms\n", t_ind);
        double bw_ind = (msgsize[0] * 2) / t_ind;
        printf("Independent bandwidth: %.3lf bytes/msec\n", bw_ind);
        bandwidths = bandwidths + bw_ind;
        delay_ind = delay_ind + t_ind;
    }

    printf("\nAverage Independent latency: %.3lf ms\n", (delay_ind / testiters));
    printf("\nAverage Independent Bandwidth: %.3lf bytes/msec\n", (bandwidths / testiters));

    // Free resources
    close(sock);
    // free(buffer);
    // free(sendbuffer);

    return 0;
}

/*
uint64_t htobe64(uint64_t host_64bits)
{
    uint64_t result = 0;
    result |= (host_64bits & 0x00000000000000FF) << 56;
    result |= (host_64bits & 0x000000000000FF00) << 40;
    result |= (host_64bits & 0x0000000000FF0000) << 24;
    result |= (host_64bits & 0x00000000FF000000) << 8;
    result |= (host_64bits & 0x000000FF00000000) >> 8;
    result |= (host_64bits & 0x0000FF0000000000) >> 24;
    result |= (host_64bits & 0x00FF000000000000) >> 40;
    result |= (host_64bits & 0xFF00000000000000) >> 56;
    return result;
}

uint64_t be64toh(uint64_t big_endian_64bits)
{
    uint64_t result = 0;

    result |= (big_endian_64bits & 0x00000000000000FFULL) << 56;
    result |= (big_endian_64bits & 0x000000000000FF00ULL) << 40;
    result |= (big_endian_64bits & 0x0000000000FF0000ULL) << 24;
    result |= (big_endian_64bits & 0x00000000FF000000ULL) << 8;
    result |= (big_endian_64bits & 0x000000FF00000000ULL) >> 8;
    result |= (big_endian_64bits & 0x0000FF0000000000ULL) >> 24;
    result |= (big_endian_64bits & 0x00FF000000000000ULL) >> 40;
    result |= (big_endian_64bits & 0xFF00000000000000ULL) >> 56;

    return result;
}
*/