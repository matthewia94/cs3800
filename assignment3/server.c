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
void writeClients( int ns, char msg[] );
void *clientHandle( void* sock_addr );
void signalHandler( int sig );

//Create table of clients
int clients[MAX_CLIENTS];
int ns;
//Mutex for client table
pthread_mutex_t clientLock;
//Mutex for writing to clients
pthread_mutex_t writeLock;

int main(int argc, char** argv)
{   
    //thread id
    pthread_t tid;

    //Create the socket addresses for client and server
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
	struct sockaddr_in client_addr = { AF_INET  };
    
    bool success;

	int client_len = sizeof( client_addr );
	char buffer[MAX_BUFFER], *host;
	int sock, k, pid;
    
    initClientList();

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
    
    signal(SIGINT, signalHandler);
    
    while(1)
    {
        if( ( ns = accept( sock, (struct sockaddr*)&client_addr, &client_len ) ) 
                   == -1 )
        {
            printf("Server: Accept failed\n");
            exit( 1 );
        }
        else
        {
            pthread_mutex_lock(&clientLock);
            success = addClient(ns);
            pthread_mutex_unlock(&clientLock);
        
            if(success)
            {
                pthread_create(&tid, NULL, &clientHandle, (void*)ns);
            }
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

void writeClients( int ns, char msg[] )
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        //check client is valid
        if(clients[i] > -1 && clients[i] != ns)
        {
            write(clients[i], msg, MAX_BUFFER);
        }
    }
}

void *clientHandle( void* sock_addr )
{
    char buffer[MAX_BUFFER];
    char username[MAX_BUFFER];
    char msg[MAX_BUFFER];
    bool init = true;
    int k, ns = (int*)sock_addr;
    
    //Data transfer on connected socket
    while( (k = read(ns, buffer, sizeof(buffer))) != 0)
    {
        if(init)
        {
            strcpy(username, buffer);
            strcat(buffer, " has connected.");
            pthread_mutex_lock(&writeLock);
            printf("%s\n", buffer);
            writeClients(ns, buffer);
            pthread_mutex_unlock(&writeLock);
            init = false;
        }
        else if(strcmp(buffer, "/exit") == 0 ||
                strcmp(buffer, "/quit") == 0 ||
                strcmp(buffer, "/part") == 0)
        {
            pthread_mutex_lock(&clientLock);
            removeClient(ns);
            pthread_mutex_unlock(&clientLock);
            strcat(username, " has left.");
            pthread_mutex_lock(&writeLock);
            printf("%s\n", username);
            writeClients( ns, username);
            pthread_mutex_unlock(&writeLock);
            close(ns);
            break;
        }
        else
        {
            strcpy(msg, username);
            strcat(msg, ": ");
            strcat(msg, buffer);
            pthread_mutex_lock(&writeLock);
            writeClients( ns, msg );
            printf("%s\n", msg);
            pthread_mutex_unlock(&writeLock);
        }
    }
    
    pthread_detach(pthread_self());
}

void signalHandler( int sig )
{
    char msg[] = "Server will shut down in 10 seconds.";
   
    //Print shutdown message
    pthread_mutex_lock(&writeLock);
    printf("%s\n", msg);
    writeClients(-1, msg);
    pthread_mutex_unlock(&writeLock);
    
    //wait for 10 seconds
    sleep(10);
    
    strcpy(msg, "Have a nice day!");
    
    //Signal shutdown
    pthread_mutex_lock(&writeLock);
    printf("%s\n", msg);
    writeClients(-1, msg);
    pthread_mutex_unlock(&writeLock);
   
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(clients[i] != -1)
        {
            close(clients[i]);
        }
    }
    
    close(ns);
    
    exit(0); 
}


