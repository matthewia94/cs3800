#include "chatroom.h" 

int main( int argc, char* argv[])
{
	int sockDesc; //Used to store the descriptor that references the socket
	struct sockaddr_in serverAddr = { AF_INET, htons( SERVER_PORT ) };
	char buf[MAX_BUFFER];
	char msg[MAX_BUFFER];
	struct hostent *hp;

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
		strcpy(msg, argv[2]);
		strcat(msg, ": ");
		strcat(msg, buf);
		write( sockDesc, msg, sizeof(msg) );
		printf("WRITE COMPLETE\n");
		read( sockDesc, msg, sizeof(msg) );
		printf( "SERVER ECHOED: %s\n", buf );
	}
	
	close(sockDesc);
	return(0);
}

	
