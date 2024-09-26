#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/time.h>

/* A linked list node data structure to maintain application
 * information related to a connected socket */
struct node {
	int	socket;
	struct sockaddr_in	client_addr;
	int	pending_data; /* flag to indicate whether there is more data to send */
	/* you will need to introduce some variables here to record
	 * all the information regarding this socket.
	 * e.g. what data needs to be sent next */
	char *msg;
	struct node	*next;
};


/* remove the data structure associated with a connected socket
 * used when tearing down the connection */
void dump( struct node *head, int socket )
{
	struct node *current, *temp;
	current = head;
	while ( current->next )
	{
		if ( current->next->socket == socket )
		{
			/* remove */
			temp = current->next;
			current->next	= temp->next;
			free( temp ); /* don't forget to free memory */
			return;
		} else {
			current = current->next;
		}
	}
}


/* create the data structure associated with a connected socket */
void add( struct node *head, int socket, struct sockaddr_in addr )
{
	struct node *new_node;

	new_node = (struct node *) malloc( sizeof(struct node) );
	new_node->socket	= socket;
	new_node->client_addr	= addr;
	new_node->pending_data	= 0;
	/* new_node->msg = ; //TODO */
	new_node->next	= head->next;
	head->next	= new_node;
}

uint64_t htobe64(uint64_t host_64bits);
uint64_t be64toh(uint64_t big_endian_64bits);

/*****************************************/
/* main program                          */
/*****************************************/

/* simple server, takes one parameter, the server port number */
int main(int argc, char **argv) {
    /* check the correctness of parameters*/
    if (argc != 2)
    {
        printf("Incorrect number of arguments\n");
        exit(0); 
    }
     /* socket and option variables */
    int sock, new_sock, max;
    int optval = 1;

    /* server socket address variables */
    struct sockaddr_in sin, addr;
    unsigned short server_port = atoi(argv[1]);

    /* make sure port range from 18000-18200 */ 
    if(server_port < 18000 || server_port > 18200)
    {
    printf("Error: port number should be within 18000 and 18200 but received %d\n", server_port);
    exit(1);
    }

    /* socket address variables for a connected client */
    socklen_t addr_len = sizeof(struct sockaddr_in);

	/* maximum number of pending connection requests */
	int BACKLOG = 5;

	/* variables for select */
	fd_set	read_set, write_set;
	struct timeval	time_out;
	int	select_retval;

    char *message;
	/* number of bytes sent/received */
	int count;

	FILE *fp = NULL;
	fp = malloc( 1000 );
	long	lSize;
	size_t	result;

	char* filename;
	filename = (char *) malloc( sizeof(char) * 1024 );

	/* linked list for keeping track of connected sockets */
	struct node	head;
	struct node	*current, *next;

	/* a buffer to read data */
	char	*buf;
	int	BUF_LEN = 65535;

	buf = (char *) malloc( BUF_LEN );

	/* initialize  head node  */
	head.socket	= -1;
	head.next	= 0;


  /* create a server socket to listen for TCP connection requests */
    if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror ("Error: creating socket error");
        abort ();
    }

    /* set option so we can reuse the port number quickly after a restart */
    if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) <0)
    {
        perror ("Error: setting TCP socket option Failed");
        abort ();
    }

    /* fill in the address of the server socket */
    memset (&sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons (server_port);

    /* bind server socket to the address */
    if (bind(sock, (struct sockaddr *) &sin, sizeof (sin)) < 0)
    {
        perror("binding socket to address");
        abort();
    }

    /* put the server socket in listen mode */
    if (listen (sock, BACKLOG) < 0)
    {
        perror ("listen on socket failed");
        abort();
    }

	int ex_cnt = 0;
	/*Handling all I/O operations from client*/
	while ( 1 )
	{
		/* set up the file descriptor bit map that select should be watching */
		FD_ZERO( &read_set );           
		FD_ZERO( &write_set );          

		FD_SET( sock, &read_set );      /* put the listening socket in */
		max = sock;                     /* initialize max */

		/* put connected sockets into the read and write sets to monitor them */
		for ( current = head.next; current; current = current->next )
		{
			FD_SET( current->socket, &read_set );

			if ( current->pending_data )
			{
				/* there is data pending to be sent, monitor the socket
				 *       in the write set so we know when it is ready to take more
				 *       data */
				FD_SET( current->socket, &write_set );
			}

			if ( current->socket > max )
			{
				/* update max if necessary */
				max = current->socket;
			}
		}

		time_out.tv_usec	= 100000; /* 1-tenth of a second timeout */
		time_out.tv_sec		= 0;

		/* invoke select, to check if the fd ready or not */
		
		select_retval = select( max + 1, &read_set, &write_set, NULL, &time_out );
		if ( select_retval < 0 )
		{
			perror( "select failed" );
			abort();
		}

		if ( select_retval == 0 )
		{
			/* no descriptor ready, timeout happened */
			continue;
		}

		if ( select_retval > 0 )                        /* at least one file descriptor is ready */
		{
			if ( FD_ISSET( sock, &read_set ) )      /* check the server socket */
			{
				/* there is an incoming connection, try to accept it */
				new_sock = accept( sock, (struct sockaddr *) &addr, &addr_len );

				if ( new_sock < 0 )
				{
					perror( "error accepting connection" );
					abort();
				}


				/* make the socket non-blocking so send and recv will
				 *       return immediately if the socket is not ready.
				 *       this is important to ensure the server does not get
				 *       stuck when trying to send data to a socket that
				 *       has too much data to send already.
				 */
				if ( fcntl( new_sock, F_SETFL, O_NONBLOCK ) < 0 )
				{
					perror( "making socket non-blocking" );
					abort();
				}

				/* the connection is made, everything is ready */
				printf( "Accepted connection. Client IP address is: %s\n",
					inet_ntoa( addr.sin_addr ) );

				/* add this client connection  as node into linked list */
				add( &head, new_sock, addr );
			}

			/* check other connected sockets, see if there is
			 *       anything to read or some socket is ready to send
			 *       more pending data */
			
			for ( current = head.next; current; current = next )
			{
				next = current->next;

				if ( FD_ISSET( current->socket, &read_set ) )
				{
					/* we have data from a client */
					/* recieve once */
					count = recv( current->socket, buf, BUF_LEN, 0 );

					if ( count <= 0 )
					{
						/* something is wrong */
						if ( count == 0 )
						{
							// printf( "Client closed connection." );
							ex_cnt = 0;
							printf( "Client closed connection. Client IP address is: %s\n", inet_ntoa( current->client_addr.sin_addr ) );
						} else {
							perror( "error receiving from a client" );
						}

						/* connection is closed, clean up */
						close( current->socket );
						dump( &head, current->socket );
					} else {
						uint16_t size = ntohs(*(uint16_t *) (buf));
						/* size should range from 18~65535 */
						if (size < 18 || size > 65535){
							perror( "error receiving from a client" );
						}
						int tempcount = 0;
						// continue recieving from client
						while (count < size){
							tempcount = recv( current->socket, buf + count, size - count, 0 );
							if (tempcount <= 0){
								continue;
							}
							count += tempcount;
							printf( "Count is %i\n", count );
						}

						printf( "%d bytes were received from client\n", count );

						// /* caculate the time and embed it into the message */
						// struct timeval tv;
						// uint64_t server_tv_sec, server_tv_usec;
						// if (gettimeofday(&tv, NULL) == 0)
						// {
						// 	server_tv_sec = (uint64_t)tv.tv_sec;
						// 	server_tv_usec = (uint64_t)tv.tv_usec;
						// }

						// *(uint64_t *)(buf + 18) = (uint64_t)htobe64(server_tv_sec);
						// *(uint64_t *)(buf + 26) = (uint64_t)htobe64(server_tv_usec);

						// /* Pong  */

						count = send( current->socket, buf, size, 0 );
						ex_cnt ++;
						while ( count < size )
						{
							tempcount = send( current->socket, buf + count, size - count, 0 );	
							if ( tempcount <= 0 )
								continue;
							
							count += tempcount;

						}
						printf( "%d bytes were sent to  client\n", count );
						printf( "Exchange Count %d\n ", ex_cnt);
					
					}

					
				}
			}
			
        }
		
    }
}

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
