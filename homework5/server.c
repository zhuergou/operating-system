#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<pthread.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

int socket_udp[MAX_CLIENTS];
char * udpnamelist[MAX_CLIENTS];
sockaddr_in udplist[MAX_CLIENTS];
int index_udp = 0;

int socket_tcp[MAX_CLIENTS];
char * tcpnamelist[MAX_CLIENTS];
sockaddr_in tcplist[MAX_CLIENTS];
int index_tcp = 0;

int main(){
	fd_set readfds;
	pthread_t tid[MAX_CLIENTS];

	int sock_1 = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_1 < 0){
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	int sock_2 = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_2 < 0){
		perror("socket() failed");
		return EXIT_FAILURE;
	}


	struct sockaddr_in server_1;
	server_1.sin_family = AF_INET;
	server_1.sin_addr.s_addr = htonl(INADDR_ANY);
	unsigned short port_1 = argv[1];
	server_1.sin_port = htons(port_1);
	int len_1 = sizeof(server_1);

	if (bind(sock_1, (struct sockaddr *)&server_1, len_1) < 0){
		perror("bind()");
		exit(EXIT_FAILURE);
	}
	listen(sock_1, 5);


	struct sockaddr_in server_2;
	server_2.sin_family = AF_INET;
	server_2.sin_addr.s_addr = htonl(INADDR_ANY);
	unsigned short port_2 = argv[2];
	server_2.sin_port = htons(port_2);
	int len_2 = sizeof(server_2);

	if (bind(sock_2, (struct sockaddr *)&server_2, len_2) < 0){
		perror("bind() failed");
		return EXIT_FAILURE;
	}

	if (getsockname(sock_2,  (struct sockaddr *)&server_2, (socklen_t *)&len_2) < 0){
		perror("getsockname() failed");
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_SIZE];
	int count = 0;
	int i;
	int n;
	int rc;
	int m;
	int j;
	struct sockaddr_in client;
	int len_client = sizeof(client);

	while (1){
		struct timeval = timeout;
		timeout.tv_sec = 3;
		timeout.tv_usec = 500;

		FD_ZERO(&readfds);
		FD_SET(sock_1, &readfds);
		FD_SET(sock_2, &readfds);
		
		/*
		for(i = 0; i < client_sockte_index; i++){
			FD_SET(client_sockets[i], &readfds);
		}
		*/
	
		int ready = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

		if (ready == 0){
			printf("No activity\n");
			continue;
		}
	
		if (FD_ISSET(sock_2, &readfds)){
			n = recvfrom(sock_2, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, (socklen_t *)&len_client);
			if (n < 0){
				perror("recvfrom() failed");
			}
			else{
				if (buffer[0] == 'L' && buffer[1] == 'O' && buffer[2] == 'G' && buffer[3] == 'I' && buffer[4] == 'N'){
					if (n < 10 || n > 27){
						sendto(sock_2, "ERROR <Invalid userid>\n", 23, 0, (struct sockaddr *)&client, sizeof(client));
						continue;
					}
					char namebuffer[30];
					m = sprintf(namebuffer, buffer + 7);
					sprintf(namebuffer + m - 1, '\0');

					int flag = 0;
					for (i = 0; i < index_tcp; i++){
						if(strcmp(tcpnamelist[i], namebuffer) == 0){
							sendto(sock_2, "ERROR <Already connected>\n", 25, 0, (struct sockaddr *)&client, sizeof(client));
							flag = 1;
							break;
						}
					}
					if (flag == 1){
						continue;
					}
					for (i = 0; i < index_udp; i++){
						if (udplist[i].sin_addr.s_addr == client.sin_addr.s_addr && updlist[i].sin_port == client.sin_port){
							if(strcmp(udpnamelist[i], namebuffer) == 0){
								sendto(sock_2, "ERROR <Already connected>\n", 25, 0, (struct sockaddr *)&client, sizeof(client));
							}
							else{
								sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
								char * tmp = malloc((strlen(namebuffer) + 1)*sizeof(char));
								strcpy(tmp, namebuffer);
								char * swap = udpnamelist[i];
								udpnamelist[i] = tmp;
								free(swap);
								socket_udp[i] = sock_2;
							}
							flag = 1;
							break;
						}
					}
					if (flag == 1){
						continue;
					}
					for (i = 0; i < index_udp; i++){
						if (strcmp(udpnamelist[i], namebuffer) == 0){
							udplist[i] = client;
							socket_udp[i] = sock_2;
							flag = 1;
							sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
							break;
						}
					}
					if (flag == 1){
						continue;
					}

					sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
					udplist[index_udp] = client;
					char * tmp2 = malloc((strlen(namebuffer) + 1)*sizeof(char));
					strcpy(tmp2, namebuffer);
					udpnamelist[index_udp] = tmp2;
					socket_udp[index_udp] = sock_2;
					index_udp++;
				}
				
				if (buffer[0] == 'W' && buffer[1] == 'H'){
					int who_index = 0;
					char who_rst[2000];

					//mutex here
					sort_help(udpnamelist, index_udp);
					sort_help(tcpnamelist, index_tcp);

					m = sprintf(who_rst, "OK\n");
					who_index += m;
					
					i = 0;
					j = 0;
					while(i < index_udp && j < index_tcp){
						if (strcmp(udpnamelist[i], tcpnamelist[j]) < 0){
							m = sprintf(who_rst + who_index, udpnamelist[i]);
							who_index = who_index + m;
							sprintf(who_rst + who_index - 1, '\n');
							i++;
						}else{
							m = sprintf(who_rst + who_index, udpnamelist[j]);
							who_index = who_index + m;
							sprintf(who_rst + who_index - 1, '\n');
							j++;
						}
					
					}
					while(i < index_udp){
						m = sprintf(who_rst + who_index, udpnamelist[i]);
						who_index = who_index + m;
						sprintf(who_rst + who_index - 1, '\n');
						i++;
					}
					while(j < index_tcp){
						m = sprintf(who_rst + who_index, udpnamelist[j]);
						who_index = who_index + m;
						sprintf(who_rst + who_index - 1, '\n');
						j++;
					}

					sendto(sock_2, who_rst, who_index, 0, (struct sockaddr *)&client, sizeof(client));
				
				}

				if (buffer[0] == 'L' && buffer[1] == 'O' && buffer[2] == 'G' && buffer[3] == 'O' && buffer[4] == 'U' && buffer[5] == 'T'){
					for (i = 0; i < index_udp; i++){
						if (udplist[i].sin_addr.s_addr == client.sin_addr.s_addr && updlist[i].sin_port == client.sin_port){
							int tmp_value;
							for (tmp_value = i; tmp_value < index_udp - 1; tmp_value++){
								udplist[tmp_value] = udplist[tmp_value + 1];
								udpnamelist[tmp_value] = udpnamelist[tmp_value + 1];
								socket_udp[tmp_value] = socket_udp[tmp_value + 1];
							}
							index_udp--;
							break;
						}
					}

					sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
				
				}

				
				if (buffer[0] == 'S' && buffer[1] == 'E'){
					i = 0;
					j = 0;
					while(buffer[i] != '<'){
						i++;
					}
					i = i + 1;
					char userid_2[50];
					while(buffer[i] != '>'){
						userid_2[j] = buffer[i];
						i++;
						j++;
					}
					userid_2[j] = '\0';
					int flag2 = 0;
					int target = -1;
					for (m = 0; m < index_udp; m++){
						if(strcmp(userid_2, udpnamelist[m]) == 0){
							flag2 = 1;
							target = m;
							break;
						}
					}
					for (m = 0; m < index_tcp; m++){
						if(strcmp(userid_2, tcpnamelist[m]) == 0){
							flag2 = 1;
							target = m;
							break;
						}
					}
					if (flag2 == 0){
						sendto(sock_2, "ERROR <Unknown userid>\n", 23, 0, (struct sockaddr *)&client, sizeof(client));
						continue;
					}


					int msglen = 0;
					i = i + 3;
					while(buffer[i] != '>'){
						msglen = 10*msglen + (buffer[i] - '0');
						i++;
					}
					
					if (msglen < 1 || msglen >994){
						sendto(sock_2, "Invalid msglen\n", 15, 0, (struct sockaddr *)&client, sizeof(client));
						continue;
					}

					char *message = malloc(msglen*sizeof(char));
					char *tmp2 = message;
					i = i + 3;
					while(buffer[i] != '>'){
						*tmp2 = buffer[i];
						i++;
					}


					int send_index = 0;
					m = 0;
					char rst[2000];
					m = sprintf(rst, "FROM <");
					send_index += m;
					m = sprintf(send_index, )
					



				
				
				}
			
			}
		}

		if (FD_ISSET(sock_1, &readfds)){
			int newsock = accept(sock_1, (struct sockaddr *)&client, (socklen_t *)&len_client);
			/*
			client_sockets[client_socket_index++] = newsock;
			*/
			rc = pthread_create(tid[count], NULL, whattodo, &newsock);
			count ++;
		
		}
	
	}

}


void * whattodo(void * arg){
	int newsock = *(int *)arg;
	int n;
	char buffer[BUFFER_SIZE];
	char * usrname;
	int i;
	int m;
	do{
		n = recv(newsock, buffer, BUFFER_SIZE, 0);
		if (n == -1){
			perror("recv() failed");
			return EXIT_FAILURE;
		}
		else if(n == 0){
		
		}
		else{
			if(buffer[0] == 'L' and buffer[1] == 'O'){
				if (n < 10 || n > 27){
					m = send(newsock, "ERROR <Invalid userid>\n", 23, 0);
				}
				else{
				
				
				
				}
				for(i = 0; i < MAX_CLIENTS; i++)
			
			}
		
		
		
		}
	
	
	
	
	}


}


void sort_help(char *udpnamelist[], int index_udp){
	int i;
	int j;
	char * tmp;
	for (i = 0; i < index_udp; i++){
		for (j = i + 1; j < index_udp; j++){
			if (strcmp(udpnamelist[i], udpnamelist[j]) > 0){
				tmp = udpnamelist[i];
				udpnamelist[i] = udpnamelist[j];
				udpnamelist[j] = tmp;
			}
		}
	}
}
