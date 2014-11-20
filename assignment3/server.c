/******************************************************************************
* @file server.cpp
* @brief The server application for a simple chatroom that uses sockets for
*        communication.
* @author Matt Anderson <mia2n4@mst.edu>
*         Matt Gualdoni <mjg6y5@mst.edu>
* @version 0.1
******************************************************************************/

//#include <pthread>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVER_PORT 9999

int main(int argc, char** argv)
{
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
	struct sockaddr_in client_addr = { AF_INET  };
	int client_len = sizeof( client_addr );
	char buffer[512], *host;
	
	int sock, ns, k, pid;

	//Stream socket
	if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		printf("Server: Socket failed\n");
		exit( 1 );
	}

	//Bind the sockt to the server port
	if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
	{
		printf("Server: Bind failed\n");
		exit( 1 );
	}

    //Listen for clients
    if( listen( sock, 1 ) == -1 )
    {
        printf("Server: Listen failed\n");
        exit( 1 );
    }

    printf("Listening for clients...\n");
    
    if( (pid=fork()) == 0 )
    {
        if( ( ns = accept( sock, (struct sockaddr*)&client_addr, &client_len ) ) 
                   == -1 )
        {
            printf("Server: Accept failed\n");
            exit( 1 );
        }
        
        printf("Parent accept() was successful, a client has connected.\n");
        //Data transfer on connected socket
        while( (k = read(ns, buffer, sizeof(buffer))) != 0)
        {
            printf("SERVER(Child) RECIEVED: %s\n", buffer);
            write(ns, buffer, k);
        }
        close(ns);
        close(sock);
    }
    else
    {
        if( (ns = accept( sock, (struct sockaddr*)&client_addr, &client_len ) ) == -1 )
        {
            printf("Server: Accept failed\n");
            exit( 1 );   
        }

        printf("Parent accept() successful.. client has connected.\n");

        //Data transfer on ns
        while( (k = read(ns, buffer, sizeof(buffer))) != 0)
        {
            printf("SERVER(Parent) RECIEVED: %s\n", buffer);
            write(ns, buffer, k);
        }
        
        close(ns);
        close(sock);
    }

    unlink(server_addr.sin_addr);

    return 0;
}
