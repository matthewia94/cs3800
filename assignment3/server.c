/******************************************************************************
* @file server.cpp
* @brief The server application for a simple chatroom that uses sockets for
*        communication.
* @author Matt Anderson <mia2n4@mst.edu>
*         Matt Gualdoni <mjg6y5@mst.edu>
* @version 0.1
******************************************************************************/

#include "chatroom.h"
#include <signal.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>

void initClientList( );
bool addClient( int fileDesc );
void removeClient( int fileDesc );
void writeClients( char msg[] );
void *clientHandle( void* sock_addr );

//Create table of clients
int clients[MAX_CLIENTS];

int main(int argc, char** argv)
{
    //Mutex for client table
    pthread_mutex_t clientLock;

    //Create the socket addresses for client and server
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
	struct sockaddr_in client_addr = { AF_INET  };
    
    bool success;

	int client_len = sizeof( client_addr );
	char buffer[512], *host;
	int sock, ns, k, pid;

	//Open the socket
	if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		printf("Server: Socket failed\n");
		exit( 1 );
	}

	//Bind the socket to the server port
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
    
    while(1)
    {
        if( ( ns = accept( sock, (struct sockaddr*)&client_addr, &client_len ) ) 
                   == -1 )
        {
            printf("Server: Accept failed\n");
            exit( 1 );
        }
        
        pthread_mutex_lock(&clientLock);
        success = addClient(ns);
        pthread_mutex_unlock(&clientLock);
        
        if(success)
        {
            printf("Parent accept() was successful, a client has connected.\n");
        }
    }

    //unlink(server_addr.sin_addr);

    return 0;
}

void initClientList( )
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i] = -1;
    }
    
    return;
}

bool addClient( int fileDesc )
{
    bool added = false;
    
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        //find empty spot in the list
        if(clients[i] == -1)
        {
            //add client
            clients[i] = fileDesc;
            added = true;
            break;
        }
    }
    
    if(!added)
    {
        printf("Chatroom is full, connection refused");
    }
    
    return added;
}

void removeClient( int fileDesc )
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(clients[i] = fileDesc)
        {
            clients[i] = -1;
        }
    }
    
    return;
}

void writeClients( char msg[] )
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        //check client is valid
        if(clients[i] > -1)
        {
            write(clients[i], msg, sizeof(msg));
        }
    }
}

void *clientHandle( void* sock_addr )
{
    char buffer[MAX_BUFFER];
    bool init;
    int k, ns;
    
    printf("Parent accept() was successful, a client has connected.\n");
    //Data transfer on connected socket
    while( (k = read(ns, buffer, sizeof(buffer))) != 0)
    {
        printf("SERVER RECIEVED: %s\n", buffer);
    }
    
    writeClients( buffer ); 
    
    //close(ns);
    //close(sock);
}

