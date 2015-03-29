// Nalin Gabriel Prabindh
// nalin.gaby@gmail.com, 2015

#include "connector-utils.h"

SOCKET localSocket=-1;
SOCKET newSocket=-1;
struct sockaddr_in localAddr;
char recvBuff[1000]; //TODO
char sendBuff[30000]; //TODO
int portNum = 26488;//80;
int count = 0;

void adam_server_signal_handler(int reason)
{
	if(localSocket>0) closesocket(localSocket);
	if(newSocket>0) closesocket(newSocket);
	localhost_uninit();
	exit(-1);
}


static int hh_on_info(http_parser* p) {
  printf("In http parser Info\n");
  return 0;
}


static int hh_on_data_field(http_parser* p, const char *at, size_t length) {
  return 0;
}

static int hh_on_data_value(http_parser* p, const char *at, size_t length) {
  return 0;
}
static char gResponseBuff[0x1000];
static int hh_on_url(http_parser* p, const char *url, size_t length) {
  char tempchar[100] = {0};
  if(length > 99) length = 99;
  memcpy(tempchar, url, length);
  if(!strcmp(tempchar, "/capabilities"))
  {
	strcpy(gResponseBuff, "{\"cpus\":1,\"wheels\":2,\"cables\":2,\"batteries\":4,\"motors\":2,\"leds\":2,\"ir-sensors\":1,\"color-sensors\":0}");
  }
  else if(!strcmp(tempchar, "/ports"))
  {
	//TODO - retrieve port names dynamically
	strcpy(gResponseBuff, "{\"porta\":\"motor1\",\"portb\":\"motor2\",\"portc\":\"empty\",\"portd\":\"empty\",\"port1\":\"touch-sensor\",\"port2\":\"ir-sensor\",\"port3\":\"empty\",\"port4\":\"empty\"}");
  }
  else
  {
	strcpy(gResponseBuff, "Failed to understand command!");
  }
  D_PRINTF("In http parser url %s with length of %d\n", tempchar, (int)length);
  return 0;
}

static int hh_on_data(http_parser* p, const char *at, size_t length) { return 0;}

static http_parser_settings hh_settings = {
  .on_message_begin = hh_on_info,
  .on_headers_complete = hh_on_info,
  .on_message_complete = hh_on_info,
  .on_header_field = hh_on_data_field,
  .on_header_value = hh_on_data_value,
  .on_url = hh_on_url,
  .on_status = hh_on_data,
  .on_body = hh_on_data
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
* A P P L I C A T I O N     L O O P
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int _tmain(int argc, _TCHAR* argv[])
{
	int err;

	localhost_init();

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = htons ( portNum );

	err = localhost_create_listener(&localSocket, &localAddr);
	if(err) goto uninit;
	signal(SIGINT, adam_server_signal_handler);

	/* Respond to GET or PUT */
	while(1)
	{
		D_PRINTF("Listening at port %d\n", portNum);
		err = localhost_accept(&localSocket, &localAddr, &newSocket);
		D_PRINTF("Received connection request @ %d\n", portNum);
		if(err == 2) break;


		http_parser parser;
		http_parser_init(&parser, HTTP_REQUEST);
		//parser.data = (void*)newSocket;

		while(1)
		{
			err = localhost_recv(&newSocket, recvBuff, sizeof(recvBuff), &count);
			D_PRINTF("Received HTTP request = %s\n", recvBuff);
			D_PRINTF("count = %d, err = %d\n", count, err);
			if((count > 0) && (err == 0))
			{
				int nparsed = http_parser_execute(&parser, &hh_settings, recvBuff, count);

				if(strstr(recvBuff, "capabilities"))
				{
					strcpy(sendBuff, "HTTP/1.1 200 OK\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Server: Adam Server\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Access-Control-Allow-Origin: *\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Access-Control-Allow-Methods: GET\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Type: text/html; charset=UTF-8\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Length: ");
					sprintf(sendBuff+strlen(sendBuff), "%d", (int)strlen(gResponseBuff));
					strcat(sendBuff, "\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, gResponseBuff);
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					D_PRINTF("Response = %s\n", sendBuff);
				}
				else if(strstr(recvBuff, "ports"))
				{
					strcpy(sendBuff, "HTTP/1.1 200 OK\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Server: Adam Server\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Access-Control-Allow-Origin: *\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Access-Control-Allow-Methods: GET\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Type: text/html; charset=UTF-8\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Length: ");
					sprintf(sendBuff+strlen(sendBuff), "%d", (int)strlen(gResponseBuff));
					strcat(sendBuff, "\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, gResponseBuff);
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					D_PRINTF("Response = %s\n", sendBuff);
				}
				else if(strstr(recvBuff, "adam_rotate?mot=0&dir=front&angle=90"))
				{
					strcpy(sendBuff, "HTTP/1.1 200 OK\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Server: Adam Server\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Type: text/html; charset=UTF-8\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Length: ");
					sprintf(sendBuff+strlen(sendBuff), "%d", (int)strlen("adam rotated"));
					strcat(sendBuff, "\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "adam rotated");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					D_PRINTF("Response = %s\n", sendBuff);
				}
				else
				{
					strcpy(sendBuff, "HTTP/1.1 404 Not Found\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Server: Adam Server\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Type: text/html; charset=UTF-8\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "Content-Length: 0\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					strcpy(sendBuff, "\r\n");
					localhost_send(&newSocket, sendBuff, strlen(sendBuff));
					D_PRINTF("Response = %s\n", sendBuff);
				}
			}
		}
	}//while

uninit:
	if(localSocket>0) closesocket(localSocket);
	if(newSocket>0) closesocket(newSocket);
	localhost_uninit();
	return 0;
}


