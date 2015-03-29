// Nalin Gabriel Prabindh
// nalin.gaby@gmail.com, 2015


#ifndef _CONNECTOR_UTILS_H
#define _CONNECTOR_UTILS_H

#define __DEBUG

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "sys/time.h"
#else
#include "stdafx.h"
#include <winsock2.h>
#include <sys/timeb.h>
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <signal.h>

#include "http_parser.h"

#ifndef WIN32
typedef unsigned int SOCKET;
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)
typedef unsigned int UINT32;
typedef char _TCHAR;
#define closesocket close
#define _tmain main
#endif

#ifdef __DEBUG
#define D_PRINTF printf
#else
#define D_PRINTF
#endif

unsigned int localhost_init();
unsigned int localhost_uninit();
unsigned int localhost_create_listener(SOCKET *pSocket, struct sockaddr_in* pAddress);
UINT32 localhost_accept(SOCKET *pSocket, struct sockaddr_in* pAddress, SOCKET *pNewSocket);
unsigned int localhost_recv(
				  SOCKET *pSocket,
				  char* outBuff, 
				  int outBufLen,
				  int *actualRecvd
				  );
unsigned int localhost_send(
				  SOCKET *pSocket,
				  char* outBuff, 
				  int outBufLen
				  );
#endif
