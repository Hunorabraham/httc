#define DEBUG
#include <server.h>


int myHandler(char* str, int len, int client){
  printf("received %d bytes:\n%s\n", len, str);
  free(str);
  return 0;
}

int main(){
  Server* server = createServer("127.0.0.1",8000);
  initServer(server);
  while(1){
    int err;
    if(err = handleNextRequest(server, &myHandler)){
      printf("Error: %d", err);
    }
  }
  deleteServer(server);
  return 0;
}