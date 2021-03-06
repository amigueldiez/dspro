
/**
*******************************************************************
* All rights reserved (C) 2018-2019 by Jose Maria Foces Moran and
* José María Foces Vivancos
*
* TCP-Client.c
*
* Stream-socket based client for basic TCP protocol analysis
* Sends a request for the server to compute 2N where N is a
* 32-bit integer. The request encodes the operation with string
* "Multiply integer by 2". N follows the latter string.
*******************************************************************
*/

#include "dstcppract.h"


int sendRequest(int sock, char *message, int nbytes){

  ssize_t n = write(sock, message, nbytes);

  return n;

}

int receiveResponse(int sock, char *message, int nbytes){

  ssize_t n = read(sock, message, nbytes);

  return n;

}

void runCS_Protocol(int sock){

  static char unsigned request[MAXREQSIZE];

  strncpy( request, REQMULT2, strlen(REQMULT2) );

  //The 32-bit integer to be sent
  const uint32_t n = 0x12345678;
  //Translate n to Network Byte Order:
  uint32_t n_NBO = htonl( n );
  //Append n_NBO to the string representing the request (REQMULT2) for computing 2*N
  memcpy(request + strlen(REQMULT2), (char *) &n_NBO, sizeof(uint32_t));

  printf("Sending a request of type '%s' with integer 0x%x concatenated in Net Byte Order\n", REQMULT2, n);
  fflush(stdout);

  //Send the request alongside N_NBO appended to it
  sendRequest(sock, request, strlen(REQMULT2) + sizeof(uint32_t));

  unsigned char *response = malloc(MAXRESP);

  int length = receiveResponse(sock, response, MAXRESP);


  if(strncmp(response, RESPMULT2, strlen(RESPMULT2) ) != 0 ){
    fprintf(stdout, "Unexpected response.\n");
    close(sock);
    exit(-1);
  }

  printf("Length of response = %d\n", length);
  printf("Bytes in response:\n");
  fflush(stdout);

  printf("Received response is [equal] to: '%s'\n", RESPMULT2);
  printf("Received word-32 still in Network byte order:\n");

  unsigned char *p = response + strlen(RESPMULT2);
  unsigned v0 = (unsigned int ) *p;
  unsigned v1 = (unsigned int ) *(p+1);
  unsigned v2 = (unsigned int ) *(p+2);
  unsigned v3 = (unsigned int ) *(p+3);

  printf("[0] = 0x%x; [1] = 0x%x; [2] = 0x%x; [3] = 0x%x\n", v0, v1, v2, v3);

  printf("Received result = 0x%x\n", ntohl( *((unsigned int *) p)));
}

int connectToServer(int sock, char *ipAddress, short int port){

  //Socket address for server
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(ipAddress);

  /*
  * Assumming the server's welcome socket is in the LISTEN state, the
  * following call will causes the local TCP/IP stack to initiate the
  * 3-way handshake by sending a SYN packet, wait for the ACK-SYN and
  * send the final ACK. From this moment on, the connection is in the
  * ESTABLISHED state.
  */
  int r = connect(sock, (struct sockaddr *) &server, sizeof (server));

  printf("Return value of connect() = %d\n", r);
  fflush(stdout);

  return r;

}

int createSocket(){

  //Create client socket of the STREAM TYPE (TCP)
  //Last parameter is always 0 for these sockets
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  //Create a loacl socket-address for the client itself
  struct sockaddr_in client;
  client.sin_family = AF_INET;
  client.sin_port = INADDR_ANY;
  client.sin_addr.s_addr = INADDR_ANY;

  /* Bind a local address to the new socket
  * In the present stream socket case, this is binding is not
  * necessary since the upcoming to connect() will implicitly
  * accomplish the binding of a local address to the Socket
  */

  bind(sock, (struct sockaddr *) &client, sizeof (client));

  return sock;

}

void client(char *ip, int port){

  int sock = createSocket();

  int r;

  if ((r = connectToServer(sock, ip, port)) != 0){
    fprintf(stderr, "Connection to server failed\n");
    exit(r);
  };

  runCS_Protocol(sock);

  close(sock);

}

int main(int argc, char** argv){

  if (argc == 3){

    client(argv[1], atoi(argv[2]));

  }else{
    printf("$ client  <server ip address> <TCP port number>");
    printf("\nExiting.\n");
  }

}
