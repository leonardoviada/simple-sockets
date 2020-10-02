#include "sys/socket.h"
#include "arpa/inet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_MSG 1024

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

void errore(char* desc, int exitCode);

int main(int argc, char**argv, int verbose) {

  if(argc < 4) {
    printf("USAGE: %s IP PORT MSG\n", *argv);
    return(-1);
  }
  
  
  char* ip = *(argv+1);
  printf("[%d] ip:%s\n", getpid(), ip);
  
  int port = atoi(*(argv+2));
  printf("[%d] porta:%d\n", getpid(), port);
  
  char *msg = *(argv+3);
  printf("[%d] messaggio:%s\n---\n", getpid(), msg);
  
  printf("[%d] creo socket\n", getpid());
  int socket_id = socket(AF_INET, SOCK_DGRAM, 0);

  if(socket_id < 0)
    errore("socket", -3);
  printf("[%d] socket ok, rc=%d\n", getpid(), socket_id);

  struct sockaddr_in server;

  printf("[%d] inizio struttura indirizzo\n", getpid());
  server.sin_family = AF_INET;
  inet_aton(ip, &server.sin_addr);
  server.sin_port = htons(port);

  for(int i = 0; i<8; i++)
    server.sin_zero[i] = 0;
  printf("[%d] indirizzo ok\n", getpid());

  printf("[%d] invio messaggio: `%s`\n", getpid(), msg);
  int rc = sendto(socket_id, msg, strlen(msg)+1, 0, (struct sockaddr*) &server, (socklen_t) sizeof server);
  if(rc != strlen(msg)+1)
    errore("sendto", -2);
  printf("[%s:%d] `%s`\n", ip, port, msg);

  printf("[%d] compilo mittente\n", getpid());
  struct sockaddr_in mitt;
  int addr_len = sizeof(struct sockaddr);
  char buffer[MAX_MSG + 1];
  printf("[%d] mittente compilato\n", getpid());

  printf("[%d] attendo messaggio...\n", getpid());
  rc = recvfrom(socket_id, buffer, MAX_MSG, 0, (struct sockaddr*) &mitt, (socklen_t*) &addr_len);
  buffer[rc] = '\0';

  char* mitt_ip = inet_ntoa(mitt.sin_addr);
  int mitt_port = ntohs(mitt.sin_port);

  printf(ANSI_COLOR_CYAN "[%s:%d] %s \n" ANSI_COLOR_RESET, mitt_ip, mitt_port, buffer);  

  printf("[%d] chiudo socket\n", getpid());
  close(socket_id);

  return 0;
}

void errore(char* desc, int exitCode) {
  printf(ANSI_COLOR_RED "\nErrore su: %s\n Uscita con codice %d\n" ANSI_COLOR_RESET, desc, exitCode);
  exit(exitCode);
}

