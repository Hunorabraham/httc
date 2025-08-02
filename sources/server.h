#include <stdio.h>
#include <Winsock2.h>
#include <Winerror.h>

#ifdef DEBUG
  #define DEBUG_LOG(...) { printf(__VA_ARGS__); }
#else
  #define DEBUG_LOG(...) {}
#endif

#define DEFAULT_SIZE 256
#define GROWTH_SIZE 256

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
  
  
  //int bytes_sent = send(client_socket, "HTTP/1.1 200 OK\r\n\r\n", 19,0);
  //if(bytes_sent == SOCKET_ERROR){
  //  err = WSAGetLastError();
  //  DEBUG_LOG("ERR: response failed, err code: %d \n", err);
  //  return err;
  //}
  //DEBUG_LOG("response sent succesfully\n");
  
  //closesocket(server->socket);
  //WSACleanup();
  return 0;
}

int handleNextRequest(Server* server, int (*handler_func)(char*, int, int)){
  int err;
  struct sockaddr client_addr;
  int addr_size = sizeof(struct sockaddr);
  int client_socket = accept(server->socket, &client_addr, &addr_size);
  if(client_socket == INVALID_SOCKET){
    err = WSAGetLastError();
    DEBUG_LOG("ERR: failed connection, err code: %d\n", err);
    return err;
  }
  DEBUG_LOG("Client connected\n");
  char* receive_buf = malloc(sizeof(char) * DEFAULT_SIZE);
  int buf_len = DEFAULT_SIZE;
  int bytes_received = 0;  
  while(1){
    bytes_received = recv(client_socket, receive_buf, buf_len, MSG_PEEK);
    if(bytes_received == SOCKET_ERROR){
      err = WSAGetLastError();
      DEBUG_LOG("ERR: failed to read incoming request, err code: %d\n", err);
      return err;
    }
    if(bytes_received < buf_len) break;
    
    DEBUG_LOG("buffer too small, increasing size: %d -> %d\n", buf_len, buf_len + GROWTH_SIZE);
    buf_len += GROWTH_SIZE;
    realloc(receive_buf, buf_len);
    continue;
  }
  if(bytes_received == 0){
    //gracefully closed will be treated like force closed; for now
    return WSAECONNRESET;
  }
  //data succesfully read
  realloc(receive_buf, bytes_received+1);
  receive_buf[bytes_received] = '\0';
  DEBUG_LOG("Calling handler callback with: %d bytes of data\n", bytes_received);
  return handler_func(receive_buf, bytes_received, client_socket);
}

void deleteServer(Server* server){
  closesocket(server->socket);
  WSACleanup();
  free(server);
}
