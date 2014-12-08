/******************************************************************************
* @file client.c
* @brief The client source file for a chatroom application which is meant to
*        work with server.c
* @author Matt Anderson <mia2n4@mst.edu>
*         Matt Gualdoni <mjg6y5@mst.edu>
* @version 1.0
******************************************************************************/

#include "chatroom.h" 

void *recieveHandle( void* fd ); 

int main( int argc, char* argv[])
{
	int sockDesc; //Used to store the descriptor that references the socket
	struct sockaddr_in serverAddr = { AF_INET, htons( SERVER_PORT ) };
	char buf[MAX_BUFFER];
	char msg[MAX_BUFFER];
	struct hostent *hp;
    pthread_t reciever;

	if( argc != 3 )
	{
		printf( "Usage: %s hostname username\n", argv[0] );
		exit(1);
	}
	
	//Get host
	if( ( hp = gethostbyname( argv[1] ) ) == NULL ) //host not found
	{
		printf( "%s: %s is unknown host", argv[0], argv[1] );
		exit(1);
	}
	bcopy( hp->h_addr_list[0], (char*)&serverAddr.sin_addr, hp->h_length ); //Places first address from hp into serverAddr host address

	//Create socket
	if( ( sockDesc = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) //attempts to create stream socket connection of TCP/IP protocol
	{
		perror( "Client: Socket could not be created" );
		exit(1);
	}
	
	//Connect a socket
	if( connect( sockDesc, (struct sockAddr*)&serverAddr, sizeof(serverAddr) ) == -1)
	{
		perror( "Client: Connect() failed:" );
		exit(1);
	}
    
    pthread_create(&reciever, NULL, &recieveHandle, (void*)sockDesc);
	
	printf( "Connect successful! Messages can now be sent to server\n" );

	while( gets(&buf) != EOF )
	{
		strcpy(msg, argv[2]);
		strcat(msg, ": ");
		strcat(msg, buf);
		write( sockDesc, msg, sizeof(msg) );
	}
	
	close(sockDesc);
	return(0);
}

void *recieveHandle( void* fd )
{
    char buffer[MAX_BUFFER];
    int k, ns = (int*)fd;
    
    while( (k = read(ns, buffer, sizeof(buffer))) != 0)
    {
        printf("%s\n", buffer);
    }
    
    close(ns);
    pthread_detach(pthread_self());
}
