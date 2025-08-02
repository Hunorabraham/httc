#include <stdio.h>
#include <Winsock2.h>
#include <Winerror.h>

#ifdef DEBUG
  #define DEBUG_LOG(...) { printf(__VA_ARGS__); }
#else
  #define DEBUG_LOG(...) {}
#endif

typedef struct Server{
  int socket;
  struct sockaddr_in addres;
} Server;

Server* createServer(const char* addr, unsigned int port){
  Server* server = malloc(sizeof(Server));
  server->addres.sin_family = AF_INET;
  server->addres.sin_addr.s_addr = inet_addr(addr);
  server->addres.sin_port = htons(port);
  return server;
}

int initServer(Server* server){
  WORD version_requiested = MAKEWORD(1,1);
  WSADATA wsa_data;
  int err = WSAStartup(version_requiested, &wsa_data);
  if(err != 0){
    DEBUG_LOG("ERR: failed to start winsock, err number: %d\n", err);
    return err;
  }
  DEBUG_LOG("startup succesful\n");
  
  server->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(server->socket == INVALID_SOCKET){
    err = WSAGetLastError();
    DEBUG_LOG("ERR: Socket creation failed, err number: %d\n", err);
    return err;
  }
  DEBUG_LOG("socket created succesfully\n");
  
  int reuse = 1;
  if(setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) == SOCKET_ERROR){
    err = WSAGetLastError();
    DEBUG_LOG("ERR: socketoption failed, err number: %d\n", err);
    return err;
  }
  
  err = bind(server->socket, (SOCKADDR *) &server->addres, sizeof(server->addres));
  if(err != 0){
    DEBUG_LOG("ERR: binding failed, err code: %d\n", err);
    return err;
  }
  DEBUG_LOG("bind succesful\n");
  
  int backlog = 100;
  if(listen(server->socket, backlog) == SOCKET_ERROR){
    err = WSAGetLastError();
    DEBUG_LOG("ERR: failed to establish listenning, err code: %d\n", err);
    return err;
  }
  DEBUG_LOG("Started listening\n");
  
  struct sockaddr client_addr;
  int addr_size = sizeof(struct sockaddr);
  int client_socket = accept(server->socket, &client_addr, &addr_size);
  if(client_socket == INVALID_SOCKET){
    err = WSAGetLastError();
    DEBUG_LOG("ERR: failed connection, err code: %d\n", err);
    return err;
  }
  DEBUG_LOG("Client connected\n");
  int bytes_sent = send(client_socket, "HTTP/1.1 200 OK\r\n\r\n", 19,0);
  if(bytes_sent == SOCKET_ERROR){
    DEBUG_LOG("ERR: response failed, err code: %d \n", err);
  }
  DEBUG_LOG("response sent succesfully\n");
  
  //closesocket(server->socket);
  //WSACleanup();
  return 0;
}



void deleteServer(Server* server){
  closesocket(server->socket);
  WSACleanup();
  free(server);
}
