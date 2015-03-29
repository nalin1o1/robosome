// Nalin Gabriel Prabindh
// nalin.gaby@gmail.com, 2015


#include "connector-utils.h"
#include <errno.h>

struct timeval startTime, endTime;
unsigned int diffTime2;

unsigned int localhost_init()
{
#ifdef WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
		return 1;
#endif
	return 0;
}

unsigned int localhost_uninit()
{
#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}

//create a listener
unsigned int localhost_create_listener(SOCKET *pSocket, struct sockaddr_in* pAddress)
{
	int err;
	//try to bind
	*pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(*pSocket == INVALID_SOCKET)
		return 2;
	 err = bind(*pSocket,(struct sockaddr*)pAddress,sizeof(struct sockaddr_in));
	if(err == SOCKET_ERROR)
	{
#if defined __DEBUG && defined WIN32
		int wserr = WSAGetLastError(); //10054-connection reset
#elif defined __DEBUG
		printf("Error in localhost_create_listener bind = %s\n", strerror(errno));
#endif
		return 1;
	}
	 err = listen(*pSocket, 1);
	if(err == SOCKET_ERROR)
	{
#if defined __DEBUG && defined WIN32
		int wserr = WSAGetLastError(); //10054-connection reset
#elif defined __DEBUG
		printf("Error in localhost_create_listener listen = %s\n", strerror(errno));
#endif
		return 1;
	}
	//setsockopt ?
	return 0;
}

UINT32 localhost_accept(SOCKET *pSocket, struct sockaddr_in* pAddress, SOCKET *pNewSocket)
{
	int err;
#ifdef WIN32
	int len;
#else
	socklen_t len;
#endif
	len = sizeof(struct sockaddr_in);
	*pNewSocket = accept(*pSocket, (struct sockaddr*) pAddress, &len);
	if(*pNewSocket == INVALID_SOCKET)
	{
#if defined __DEBUG && defined WIN32
		int wserr = WSAGetLastError(); //10054-connection reset
#elif defined __DEBUG
		printf("Error in accept = %s\n", strerror(errno));
#endif
		return 2;
	}
	return 0;
}

//only receive
unsigned int localhost_recv(
				  SOCKET *pSocket,
				  char* outBuff, 
				  int outBufLen,
				  int *actualRecvd
				  )
{
	memset(outBuff, 0, outBufLen);
	*actualRecvd = recv(*pSocket,outBuff,outBufLen,0);
	if(*actualRecvd == SOCKET_ERROR)
	{
#if defined __DEBUG && defined WIN32
		int wserr = WSAGetLastError(); //10054-connection reset, or 53 - connection aborted
#endif 
		return 1;	
	}
	return 0;
}

//only send
unsigned int localhost_send(
				  SOCKET *pSocket,
				  char* outBuff, 
				  int outBufLen
				  )
{
	int err;
	err = send(*pSocket,outBuff,outBufLen,0);
	if(err == SOCKET_ERROR)
	{
#if defined __DEBUG && defined WIN32
		int wserr = WSAGetLastError(); //10054-connection reset, or 53 - connection aborted
#endif 
		return 1;	
	}
	return 0;
}



