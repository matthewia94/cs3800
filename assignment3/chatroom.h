/******************************************************************************
* @file chatroom.h
* @brief A shared header file between the client and the server for the a
*        chatroom application
* @author Matt Anderson <mia2n4@mst.edu>
*         Matt Gualdoni <mjg6y5@mst.edu>
* @version 1.0
******************************************************************************/
#ifndef CHATROOM_H
#define CHATROOM_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h> //defines socket
#include <netdb.h>      /* define internet socket */
#include <signal.h>
#include <pthread.h>

//define bools
typedef int bool;
enum { false, true };

#define SERVER_PORT 9998
#define MAX_BUFFER 512
#define MAX_CLIENTS 12

#endif
