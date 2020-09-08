#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


#define BUF_MAX_LEN 1024
#define MSG_QUEUE_NUM 10
#define SERVER_PORT 5060


/// @struct server_t
/// @brief client 의 요청에 따른 응답을 처리하기 위한 구조체
typedef struct server_s server_t;
struct server_s{
	/// server tcp socket file descriptor
	int fd;
	/// server socket address
	struct sockaddr_in addr;
};


server_t* server_init();
void server_destroy( server_t **server);
void server_conn( server_t *server);

int server_process_data( server_t *server, int fd);
int server_recv_data( int fd);
int server_send_data( int fd);

#endif

