#define DEBUG
#include <server.h>

int main(){
  Server* server = createServer("127.0.0.1",8000);
  start_server(server);
  free(server);
  return 0;
}