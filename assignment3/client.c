#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h> //defines socket
#include <netdb.h>       /* define internet socket */ 
 
#define SERVER_PORT 9999     /* define a server port number */ 

int main( int argc, char* argv[], char* userName )
{
	int sockDesc; //Used to store the descriptor that references the socket
	struct sockaddr_in serverAddr = { AF_INET, htons( SERVER_PORT ) };
	char buf[512];
	struct hostent *hp;

	if( argc != 2 )
	{
		printf( "Usage: %s hostname\n", argv[0] );
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
	printf( "Socket created." );
	
	//Connect a socket
	if( connect( sockDesc, (struct sockAddr*)&serverAddr, sizeof(serverAddr) ) == -1)
	{
		perror( "Client: Connect() failed:" );
		exit(1);
	}
	
	printf( "Connect() successful! Messages can now be sent to server\n" );
	printf( "Input a string:\n" );

	while( gets(&buf) != EOF )
	{
		write( sockDesc, userName, sizeof(userName) );
		write( sockDesc, ": ", 2 );
		write( sockDesc, buf, sizeof(buf) );
		read( sockDesc, buf, sizeof(buf) );
		printf( "SERVER ECHOED: %s\n", buf );
	}
	
	close(sockDesc);
	return(0);
}

	
