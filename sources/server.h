#include <stdio.h>
#include <Winsock2.h>
#include <Winerror.h>
#include <errno.h>

#ifdef DEBUG
  #define DEBUG_LOG(...) { printf(__VA_ARGS__); }
#else
  #define DEBUG_LOG(...) {}
#endif


void server_test(){  
  printf("test");
}

int start_server(const char* addr, unsigned int port){
  WORD version_requiested = MAKEWORD(1,1);
  WSADATA wsa_data;
  int err = WSAStartup(version_requiested, &wsa_data);
  if(err != 0){
    DEBUG_LOG("ERR: failed to start winsock, err number: %d\n", err);
    return 1;
  }
  DEBUG_LOG("startup succesful\n");
  
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(server_socket == INVALID_SOCKET){
    DEBUG_LOG("ERR: Socket creation failed, err number: %d\n", WSAGetLastError());
    return 1;
  }
  DEBUG_LOG("socket created succesfully\n");
  
  int reuse = 1;
  if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) == SOCKET_ERROR){
    DEBUG_LOG("ERR: socketoption failed, err number: %d\n", WSAGetLastError());
    return 1;
  }
  
  struct sockaddr_in service = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = inet_addr(addr),
    .sin_port = htons(port),
  };
  
  err = bind(server_socket, (SOCKADDR *) &service, sizeof(service));
  if(err != 0){
    DEBUG_LOG("ERR: binding failed, err code: %d\n", WSAGetLastError());
    return 1;
  }
  DEBUG_LOG("bind succesful\n");
  
  int backlog = 100;
  if(listen(server_socket, backlog) == SOCKET_ERROR){
    DEBUG_LOG("ERR: failed to establish listenning, err code: %d\n", WSAGetLastError());
    return 1;
  }
  DEBUG_LOG("Started listening\n");
  
  struct sockaddr client_addr;
  int addr_size = sizeof(struct sockaddr);
  int client_socket = accept(server_socket, &client_addr, &addr_size);
  if(client_socket == INVALID_SOCKET){
    DEBUG_LOG("ERR: failed connection, err code: %d\n", WSAGetLastError());
    return 1;
  }
  DEBUG_LOG("Client connected\n");
  int bytes_sent = send(client_socket, "HTTP/1.1 200 OK\r\n\r\n", 19,0);
  if(bytes_sent == SOCKET_ERROR){
    DEBUG_LOG("ERR: response failed, err code: %d \n", WSAGetLastError());
  }
  DEBUG_LOG("response sent succesfully\n");
  
  //closesocket(server_socket);
  //WSACleanup();
  return 0;
}