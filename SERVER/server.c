#include "server.h"

/**
 * @mainpage Project : TCP Socket test
 * @section intro 소개
 *    - 차후 계획한 개인 프로젝트를 위해 미리 제작 
 * @section  CreateInfo 작성 정보
 *    - 작성자 :   정동욱
 *    - 작성일 :   2020/07/10
 * @subsection exec 실행 방법 및 인수 설명
 *    - 실행 방법\n
 *      서버       : SERVER/server
 *      클라이언트 : CLIENT/client
 */

/**
 * @fn int main( int argc, char **argv)
 * @brief server 구동을 위한 main 함수
 * @return int
 * @param argc 매개변수 개수
 * @param argv ip 와 포트 정보
 */
int main( int argc, char **argv){
	//	if( argc != 3){
	//		printf("	| ! need param : ip port\n");
	//		return -1;
	//	}

	server_t *server = server_init();
	if( server == NULL){
		printf("	| ! Server : Fail to initialize\n");
		return -1;
	}

	server_conn( server);

	server_destroy( server);
}



// -----------------------------------------

/**
 * @fn server_t* server_init()
 * @brief server 객체를 생성하고 초기화하는 함수
 * @return 생성된 server 객체
 */
server_t* server_init(){
	server_t *server = ( server_t*)malloc( sizeof( server_t));

	memset( &server->addr, 0, sizeof( struct sockaddr));
	server->addr.sin_family = AF_INET;
	server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server->addr.sin_port = htons(SERVER_PORT);

	if( ( server->fd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("	| ! Server : Fail to open socket\n");
		free( server);
		return NULL;
	}

	int reuse = 1;
	if( setsockopt( server->fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse))){
		printf("	| ! Server : Fail to set the socket's option\n");
		free( server);
		return NULL;
	}

	if( bind( server->fd, ( struct sockaddr*)( &server->addr), sizeof( server->addr)) < 0){
		printf("	| ! Server : Fail to bind socket\n");
		close( server->fd);
		free( server);
		return NULL;
	}

	if( listen( server->fd, MSG_QUEUE_NUM)){
		printf("	| ! Server : listen error\n");
		return;
	}

	printf("	| @ Server : Success to create a object\n");
	printf("	| @ Server : Welcome\n\n");
	return server;
}

/**
 * @fn void server_destroy( server_t *server)
 * @brief server 객체를 삭제하기 위한 함수
 * @return void
 * @param server 삭제하기 위한 server 객체
 */
void server_destroy( server_t *server){
	close( server->fd);
	free( server);
	printf("	| @ Server : Success to destroy the object\n");
	printf("	| @ Server : BYE\n\n");
}

/**
 * @fn void server_conn( server_t *server)
 * @brief client 와 연결되었을 때 데이터를 수신하고 데이터를 처리하기 위한 함수
 * @return void
 * @param server 데이터 처리를 위한 server 객체
 */
void server_conn( server_t *server){
	if( server->fd <= 0){
		printf("	| ! Server : fd error\n");
		return;
	}

	int rv = 0;
	struct sockaddr_in client_addr;
	int client_addr_len = sizeof( client_addr);
	memset( &client_addr, 0, client_addr_len);

	int client_fd = accept( server->fd, ( struct sockaddr*)( &client_addr), ( socklen_t*)( &client_addr_len));
	if( client_fd < 0){
		printf("	| ! Server : accept error\n");
		return;
	}

	int flag = fcntl( client_fd, F_GETFL, 0);
	fcntl( client_fd, F_SETFL, flag | O_NONBLOCK);

	rv = server_process_data( server, client_fd);
	if( rv < 0){
		printf("    | ! Server : process end\n");
		return ;
	}
}

int server_process_data( server_t *server, int fd){
	int read_rv = 0;
	int send_rv = 0;
	printf("        | @ Server : waiting...\n");

	while(1){
		read_rv = server_recv_data( fd);
		if( read_rv < 0){
			close( fd);
			return read_rv;
		}
		else if( read_rv == 0){
			send_rv = server_send_data( fd);
			if( send_rv < 0){
				close( fd);
				return send_rv;
			}
		}
	}

	return 0;
}

int server_recv_data( int fd){
	char read_buf[ BUF_MAX_LEN];
	int read_rv = 0;
	size_t read_buf_size = sizeof( read_buf);
	memset( read_buf, 0x00, read_buf_size);

	read_rv = read( fd, read_buf, read_buf_size);
	if( read_rv < 0){
		if( errno == EAGAIN){
			//printf("        | @ Server : EAGAIN\n");
			return 1;
		}
		else{
			printf("	| ! Server : read error\n");
			printf("	| ! Server : socket close\n");
			return -2;
		}
	}
	else if( read_rv == 0){
		printf("	| ! Server : socket close\n");
		return -1;
	}
	else{
		printf("        | @ Server : < [ %s ]\n", read_buf);

		if( !memcmp( read_buf, "q", 1)){
			printf("	| @ Server : Finish\n");
			return -3;
		}
	}
	return 0;
}

int server_send_data( int fd){
	ssize_t send_bytes;
	char send_buf[ BUF_MAX_LEN];
	size_t send_buf_size = sizeof( send_buf);
	memset( send_buf, 0, send_buf_size);
	snprintf( send_buf, BUF_MAX_LEN, "%s", "OK");

	if( ( send_bytes = write( fd, send_buf, send_buf_size, 0)) <= 0){
		printf("        | ! Server : Fail to send msg\n");
		printf("        | ! Server : > [ %s ]\n\n", send_buf);
		return -1;
	}
	return 0; 
}

