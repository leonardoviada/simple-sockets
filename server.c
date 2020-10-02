#include "sys/socket.h"
#include "arpa/inet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_MSG 1024
#define DHCP_ADDR "0.0.0.0"
#define LOC_ADDR "127.0.0.1"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

void errore(char* desc, int exitCode);
int isLocal(const char**argv, int verbose);

int main(int argc, char const *argv[])
{

  if(argc < 3) {
    printf("USAGE: %s PORT MSG MODE (dhcp if not specified, -l for loopback)\n", *argv);
    return(-1);
  }

  int port = atoi(*(argv+1));
  printf("[%d] porta:%d\n", getpid(), port);

  char* msg = *(argv+2);
  printf("[%d] messaggio:`%s`\n---\n", getpid(), msg);


  printf("[%d] inizio socket\n", getpid());
  int socket_id = socket(AF_INET, SOCK_DGRAM , 0);

  if(socket_id < 0)
    errore("socket", -3);
  printf("[%d] socket ok, rc=%d\n", getpid(), socket_id);

  struct sockaddr_in self;
  self.sin_family = AF_INET;
  inet_aton(isLocal(argv, 1) == 1 ? LOC_ADDR : DHCP_ADDR, &self.sin_addr);
  self.sin_port = htons(port);

  for(int i = 0; i <8; i++) self.sin_zero[i] = 0;
  
  printf("[%d] inizio binding\n", getpid());
  int rc = bind(socket_id, (struct sockaddr*) &self, (socklen_t) sizeof (struct sockaddr));
  if(rc < 0) errore("bind", -2);
  printf("[%d] binding ok\n", getpid());

  /* while(1) { */
  printf("[%d] inizio struttura mittente\n", getpid());
  struct sockaddr_in mitt;
  int addr_len = sizeof (struct sockaddr);
  char buffer[MAX_MSG + 1];
  printf("[%d] struttura mittente ok\n", getpid());

  printf("[%s:%d] attendo messaggio... \n", isLocal(argv, NULL) == 1 ? LOC_ADDR : DHCP_ADDR, port);
  rc = recvfrom(socket_id, buffer, MAX_MSG, 0, (struct sockaddr*) &mitt, (socklen_t*) &addr_len);
  buffer[rc] = '\0';
  printf("[%d] messaggio ricevuto\n", getpid());

  char* mitt_ip = inet_ntoa(mitt.sin_addr);
  int mitt_port = ntohs(mitt.sin_port);
  printf("[%d] dati mittente registrati\n", getpid());

  printf(ANSI_COLOR_CYAN "[%s:%d] %s \n" ANSI_COLOR_RESET, mitt_ip, mitt_port, buffer);  

  printf("[%d] invio risposta: `%s`\n", getpid(), msg);
  rc = sendto(socket_id, msg, strlen(msg)+1, 0, (struct sockaddr*) &mitt, (socklen_t) sizeof mitt);
  if(rc != strlen(msg)+1)
    errore("sendto", -2);
  printf("[%s:%d] %s\n", mitt_ip, port, msg);
 /* } */

  printf("[%d] chiudo socket\n", getpid());
  close(socket_id);

  return 0;
}

void errore(char* desc, int exitCode) {
  printf(ANSI_COLOR_RED "\n [%d] errore su: %s\n Uscita con codice %d\n" ANSI_COLOR_RESET, getpid(), desc, exitCode);
  exit(exitCode);
}

/* Descrizione */
int isLocal(const char**argv, int verbose) {
  /* printf("FLAG _> %s  n", *(argv+3)); */
  if(*(argv+3) && strcmp(*(argv+3), "-l") == 0) {
    if(verbose)
      printf("[%d] LOOPBACK ok \n", getpid());
    return 1;
  }
  if(verbose)
    printf("[%d] DHCP ok \n", getpid());
  return -1;
}