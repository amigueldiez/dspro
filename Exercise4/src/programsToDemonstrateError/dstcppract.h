/**
 *************************************************************************************
 * All rights reserved (C) 2018-2019 by José María Foces Morán and
 * José María Foces Vivancos
 *
 * dstcppract.h
 * (In Linux, use the -lpthread gcc compiler flag -not necessary in OS-X)
 *
 * Stream-socket based server for basic TCP protocol analysis
 *
 * V. 1.5
 *
 *************************************************************************************
 */

#ifndef DSTCPPRACT_H
#define DSTCPPRACT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* DSTCPPRACT_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <memory.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#define REQSHUTDOWN "Shutdown server"
#define REQDATE "Send the date"
#define RESPDATE "Server's date follows: "
#define RESPSHUTDOWN "After receiving a Shutdown request, only the "\
"client-exclusive worker thread is being finished. "\
"Server thread will remain running.\n"

#define REQMULT2 "Multiply integer by 2"
#define RESPMULT2 "Integer multiplied by 2"

#define RESPOTHER "Unknown request. This server responds to DATE and SHUTDOWN requests, only\n"
#define RESPERROR "Socket receive error on delegate Socket"


#define TRUE 1
#define FALSE 0
#define DEFAULTPORT 60000

//For listen() socket function:
#define BACKLOG 10

#define MAXREQSIZE 1024
#define MAXRESP 1024

void * clientServerProtocol(int *ds);
