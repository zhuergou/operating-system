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

#define BUFFER_SIZE 2000
#define MAX_CLIENTS 100

int server_type[MAX_CLIENTS];
int socket_list[MAX_CLIENTS];
char * name_list[MAX_CLIENTS];
struct sockaddr_in addr_list[MAX_CLIENTS];

int index_g = 0;

typedef struct package{
	int newsock;
	struct sockaddr_in client;
} variable;


void sort_help(char *name_list[], int socket_list[], int server_type[], struct sockaddr_in addr_list[], int index_g);
void * whattodo(void * arg);

int main(int argc, char *argv[]){
	fd_set readfds;
	pthread_t tid[MAX_CLIENTS];
	struct sockaddr_in tmp_client[MAX_CLIENTS];
	int count = 0;

	int sock_1 = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_1 < 0){
		perror("socket() failed");
		return EXIT_FAILURE;
	}

	int sock_2 = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_2 < 0){
		perror("socket() failed");
		return EXIT_FAILURE;
	}


	struct sockaddr_in server_1;
	server_1.sin_family = AF_INET;
	server_1.sin_addr.s_addr = htonl(INADDR_ANY);
	unsigned short port_1 = atoi(argv[1]);
	server_1.sin_port = htons(port_1);
	int len_1 = sizeof(server_1);

	if (bind(sock_1, (struct sockaddr *)&server_1, len_1) < 0){
		perror("bind() failed");
		return EXIT_FAILURE;
	}
	listen(sock_1, 5);


	struct sockaddr_in server_2;
	server_2.sin_family = AF_INET;
	server_2.sin_addr.s_addr = htonl(INADDR_ANY);
	unsigned short port_2 = atoi(argv[2]);
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
	int i;
	int j;
	int m;
	int n;
	int rc;

	struct sockaddr_in client;
	int len_client = sizeof(client);

	while (1){
		struct timeval timeout;
		timeout.tv_sec = 3;
		timeout.tv_usec = 500;

		FD_ZERO(&readfds);
		FD_SET(sock_1, &readfds);
		FD_SET(sock_2, &readfds);
	
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
					if (n < 9 || n > 26){
						sendto(sock_2, "ERROR Invalid userid\n", 21, 0, (struct sockaddr *)&client, sizeof(client));
						continue;
					}
					char namebuffer[30];
					i = 0;
					while(buffer[i + 6] != '\n'){
						namebuffer[i] = buffer[i + 6];
						i++;
					}
					namebuffer[i] =  '\0';

					int flag = 0;
					for (i = 0; i < index_g; i++){
						if(strcmp(name_list[i], namebuffer) == 0){
							if (server_type[i] == 2){
								sendto(sock_2, "ERROR Already connected\n", 24, 0, (struct sockaddr *)&client, sizeof(client));
							}
							else{
								if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
									sendto(sock_2, "ERROR Already connected\n", 24, 0, (struct sockaddr *)&client, sizeof(client));
								}
								else{
									// not sure
									memcpy(&addr_list[i], &client, sizeof(client));
									/*
									addr_list[i].sin_addr.s_addr = client.sin_addr.s_addr;
									addr_list[i].sin_port = client.sin_port;
									*/
									socket_list[i] = sock_2;
									sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
								}
							}
							flag = 1;
							break;
						}
					}
					if (flag == 1){
						continue;
					}

					for (i = 0; i < index_g; i++){
						if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
							if (server_type[i] == 1){
								sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
								char * tmp = malloc((strlen(namebuffer) + 1)*sizeof(char));
								strcpy(tmp, namebuffer);
								char * swap = name_list[i];
								name_list[i] = tmp;
								free(swap);
								socket_list[i] = sock_2;
								flag = 1;
								break;
							}
						}
					}
					if (flag == 1){
						continue;
					}

					sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
					memcpy(&addr_list[index_g], &client, sizeof(client));
					char * tmp2 = malloc((strlen(namebuffer) + 1)*sizeof(char));
					strcpy(tmp2, namebuffer);
					name_list[index_g] = tmp2;
					socket_list[index_g] = sock_2;
					server_type[index_g] = 1;
					index_g++;
				}
				
				if (buffer[0] == 'W' && buffer[1] == 'H'){
					int who_index = 0;
					char who_rst[2000];

					//mutex here
					sort_help(name_list, socket_list, server_type, addr_list, index_g);

					m = sprintf(who_rst, "OK\n");
					who_index += m;
					
					i = 0;
					while(i < index_g){
						m = sprintf(who_rst + who_index, "%s", name_list[i]);
						// not sure
						who_index = who_index + m + 1;
						sprintf(who_rst + who_index - 1, "\n");
						i++;
					}

					sendto(sock_2, who_rst, who_index, 0, (struct sockaddr *)&client, sizeof(client));
				}


				if (buffer[0] == 'L' && buffer[1] == 'O' && buffer[2] == 'G' && buffer[3] == 'O' && buffer[4] == 'U' && buffer[5] == 'T'){
					for (i = 0; i < index_g; i++){
						if (server_type[i] == 1){
							if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
								int tmp_value;
								for (tmp_value = i; tmp_value < index_g - 1; tmp_value++){
									addr_list[tmp_value] = addr_list[tmp_value + 1];
									name_list[tmp_value] = name_list[tmp_value + 1];
									socket_list[tmp_value] = socket_list[tmp_value + 1];
									server_type[tmp_value] = server_type[tmp_value + 1];
								}
								//not sure
								free(name_list[i]);
								index_g--;
								break;
							}
						}
					}

					sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
				
				}

				
				if (buffer[0] == 'S' && buffer[1] == 'E'){
					i = 0;
					j = 0;
					while(buffer[i] != ' '){
						i++;
					}
					i = i + 1;

					char userid_send[50];
					while(buffer[i] != ' '){
						userid_send[j] = buffer[i];
						i++;
						j++;
					}
					userid_send[j] = '\0';

					int flag2 = 0;
					int target = -1;
					for (m = 0; m < index_g; m++){
						if(strcmp(userid_send, name_list[m]) == 0){
							flag2 = 1;
							target = m;
							break;
						}
					}
					if (flag2 == 0){
						sendto(sock_2, "ERROR Unknown userid\n", 21, 0, (struct sockaddr *)&client, sizeof(client));
						continue;
					}


					int msglen = 0;
					i = i + 1;
					while(buffer[i] != ' '){
						msglen = 10*msglen + (buffer[i] - '0');
						i++;
					}
					
					if (msglen < 1 || msglen > 994){
						sendto(sock_2, "ERROR Invalid msglen\n", 21, 0, (struct sockaddr *)&client, sizeof(client));
						continue;
					}

					char *message = malloc(msglen*sizeof(char));
					char *tmp_send = message;
					i = i + 1;
					while(buffer[i] != '\n'){
						*tmp_send = buffer[i];
						tmp_send++;
						i++;
					}

					char *sender_name;
					for(i = 0; i < index_g; i++){
						if(server_type[i] == 1){
							if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
								sender_name = name_list[i];
								break;
							}
						}
					}
					

					int index_send = 0;
					m = 0;
					char rst[2000];
					m = sprintf(rst, "FROM ");
					index_send += m;
					m = sprintf(rst + index_send, "%s", sender_name);
					index_send += m;
					m = sprintf(rst + index_send, " %d ", msglen);
					index_send += m;
					m = sprintf(rst + index_send, "%s", message);
					index_send += m;
					m = sprintf(rst + index_send, "\n");
					index_send += m;

					sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
					sendto(socket_list[target], rst, index_send, 0, (struct sockaddr *)&addr_list[target], sizeof(addr_list[target]));
					free(message);
				}


				if (buffer[0] == 'B' && buffer[1] == 'R'){
					i = 0;
					j = 0;
					while(buffer[i] != ' '){
						i++;
					}
					i = i + 1;
					
					int msglen2 = 0;
					while(buffer[i] != ' '){
						msglen2 = 10*msglen2 + (buffer[i] - '0');
						i++;
					}
					
					if (msglen2 < 1 || msglen2 > 994){
						sendto(sock_2, "ERROR Invalid msglen\n", 21, 0, (struct sockaddr *)&client, sizeof(client));
						continue;
					}

					char *message2 = malloc(msglen2 * sizeof(char));
					char *tmp_send2 = message2;
					i = i + 1;
					while(buffer[i] != '\n'){
						*tmp_send2 = buffer[i];
						tmp_send2++;
						i++;
					}

					char *sender_name2;
					for(i = 0; i < index_g; i++){
						if(server_type[i] == 1){
							if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
								sender_name2 = name_list[i];
								break;
							}
						}
					}

					int index_send2 = 0;
					m = 0;
					char rst2[2000];
					m = sprintf(rst2, "FROM ");
					index_send2 += m;
					m = sprintf(rst2 + index_send2, "%s", sender_name2);
					index_send2 += m;
					m = sprintf(rst2 + index_send2, " %d ", msglen2);
					index_send2 += m;
					m = sprintf(rst2 + index_send2, "%s", message2);
					index_send2 += m;
					m = sprintf(rst2 + index_send2, "\n");
					index_send2 += m;


					sendto(sock_2, "OK\n", 3, 0, (struct sockaddr *)&client, sizeof(client));
					for(i = 0; i < index_g; i++){
						sendto(socket_list[i], rst2, index_send2, 0, (struct sockaddr *)&addr_list[i], sizeof(addr_list[i]));
					}
					free(message2);
				}

				if (buffer[0] == 'S' && buffer[1] == 'H'){
					sendto(sock_2, "ERROR SHARE not supported over UDP\n", 35, 0, (struct sockaddr *)&client, sizeof(client));
				}
			}
		}

		if (FD_ISSET(sock_1, &readfds)){
			int newsock = accept(sock_1, (struct sockaddr *)&client, (socklen_t *)&len_client);
			memcpy(&tmp_client[count], &client, sizeof(client));
			variable *t;
			t = (variable *)malloc(sizeof(variable));
			t -> newsock = newsock;
			t -> client = tmp_client[count];
			rc = pthread_create(&tid[count], NULL, whattodo, t);
			if(rc != 0){
				fprintf(stderr, "MAIN: Could not create thread(%d)\n", rc);
				return EXIT_FAILURE;
			}
			count++;
		}
	
	}

}

void * whattodo(void * arg){
	variable * arg_temp;
	arg_temp = (variable *)arg;
	int new_sock = arg_temp -> newsock;
	struct sockaddr_in client = arg_temp -> client;

	int m;
	int n;
	int i;
	int j;
	char buffer[BUFFER_SIZE];

	do{
		n = recv(new_sock, buffer, BUFFER_SIZE, 0);
		if (n == -1){
			perror("recv() failed");
			exit(EXIT_FAILURE);
		}
		else if(n == 0){
			for (i = 0; i < index_g; i++){
				if (server_type[i] == 2){
					if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
						int tmp_value;
						for (tmp_value = i; tmp_value < index_g - 1; tmp_value++){
							addr_list[tmp_value] = addr_list[tmp_value + 1];
							name_list[tmp_value] = name_list[tmp_value + 1];
							socket_list[tmp_value] = socket_list[tmp_value + 1];
							server_type[tmp_value] = server_type[tmp_value + 1];
						}
						index_g--;
						free(name_list[i]);
						break;
					}
				}
			}
		}
		else{
			if (buffer[0] == 'L' && buffer[1] == 'O' && buffer[2] == 'G' && buffer[3] == 'I' && buffer[4] == 'N'){
				if (n < 9 || n > 26){
					send(new_sock, "ERROR Invalid userid\n", 21, 0);
					continue;
				}
				char namebuffer[30];
				/*
				m = sprintf(namebuffer, "%s", buffer + 6);
				sprintf(namebuffer + m - 1, "\0");
				*/
				i = 0;
				while(buffer[i + 6] != '\n'){
					namebuffer[i] = buffer[i + 6];
					i++;
				}
				namebuffer[i] =  '\0';

				int flag = 0;
				for (i = 0; i < index_g; i++){
					if(strcmp(name_list[i], namebuffer) == 0){
						if (server_type[i] == 2){
							send(new_sock, "ERROR Already connected\n", 24, 0);
						}
						else{
							// not sure
							memcpy(&addr_list[i], &client, sizeof(client));
							/*
							addr_list[i].sin_addr.s_addr = client.sin_addr.s_addr;
							addr_list[i].sin_port = client.sin_port;
							*/
							socket_list[i] = new_sock;
							server_type[i] = 2;
							send(new_sock, "OK\n", 3, 0);
							
						}
						flag = 1;
						break;
					}
				}
				if (flag == 1){
					continue;
				}

				for (i = 0; i < index_g; i++){
					if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
						if (server_type[i] == 2){
							send(new_sock, "OK\n", 3, 0);
							char * tmp = malloc((strlen(namebuffer) + 1)*sizeof(char));
							strcpy(tmp, namebuffer);
							char * swap = name_list[i];
							name_list[i] = tmp;
							free(swap);
							socket_list[i] = new_sock;
							flag = 1;
							break;
						}
					}
				}
				if (flag == 1){
					continue;
				}

				send(new_sock, "OK\n", 3, 0);
				memcpy(&addr_list[index_g], &client, sizeof(client));
				char * tmp2 = malloc((strlen(namebuffer) + 1)*sizeof(char));
				strcpy(tmp2, namebuffer);
				name_list[index_g] = tmp2;
				socket_list[index_g] = new_sock;
				server_type[index_g] = 2;
				index_g++;
			}
				
			if (buffer[0] == 'W' && buffer[1] == 'H'){
				int who_index = 0;
				char who_rst[2000];

				//mutex here
				sort_help(name_list, socket_list, server_type, addr_list, index_g);

				m = sprintf(who_rst, "OK\n");
				who_index += m;
					
				i = 0;
				while(i < index_g){
					m = sprintf(who_rst + who_index, "%s", name_list[i]);
					// not sure
					who_index = who_index + m + 1;
					sprintf(who_rst + who_index - 1, "\n");
					i++;
				}

				send(new_sock, who_rst, who_index, 0);
			}


			if (buffer[0] == 'L' && buffer[1] == 'O' && buffer[2] == 'G' && buffer[3] == 'O' && buffer[4] == 'U' && buffer[5] == 'T'){
				for (i = 0; i < index_g; i++){
					if (server_type[i] == 2){
						if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
							int tmp_value;
							for (tmp_value = i; tmp_value < index_g - 1; tmp_value++){
								addr_list[tmp_value] = addr_list[tmp_value + 1];
								name_list[tmp_value] = name_list[tmp_value + 1];
								socket_list[tmp_value] = socket_list[tmp_value + 1];
								server_type[tmp_value] = server_type[tmp_value + 1];
							}
							//not sure
							free(name_list[i]);
							index_g--;
							break;
						}
					}
				}

				send(new_sock, "OK\n", 3, 0);
				
			}

				
			if (buffer[0] == 'S' && buffer[1] == 'E'){
				i = 0;
				j = 0;
				while(buffer[i] != ' '){
					i++;
				}
				i = i + 1;

				char userid_send[50];
				while(buffer[i] != ' '){
					userid_send[j] = buffer[i];
					i++;
					j++;
				}
				userid_send[j] = '\0';

				int flag2 = 0;
				int target = -1;
				for (m = 0; m < index_g; m++){
					if(strcmp(userid_send, name_list[m]) == 0){
						flag2 = 1;
						target = m;
						break;
					}
				}
				if (flag2 == 0){
					send(new_sock, "ERROR Unknown userid\n", 21, 0);
					continue;
				}


				int msglen = 0;
				i = i + 1;
				while(buffer[i] != ' '){
					msglen = 10*msglen + (buffer[i] - '0');
					i++;
				}
					
				if (msglen < 1 || msglen > 994){
					send(new_sock, "ERROR Invalid msglen\n", 21, 0);
					continue;
				}

				char *message = malloc(msglen*sizeof(char));
				char *tmp_send = message;
				i = i + 1;
				while(buffer[i] != '\n'){
					*tmp_send = buffer[i];
					tmp_send++;
					i++;
				}

				char *sender_name;
				for(i = 0; i < index_g; i++){
					if(server_type[i] == 2){
						if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
							sender_name = name_list[i];
							break;
						}
					}
				}
					

				int index_send = 0;
				m = 0;
				char rst[2000];
				m = sprintf(rst, "FROM ");
				index_send += m;
				m = sprintf(rst + index_send, "%s", sender_name);
				index_send += m;
				m = sprintf(rst + index_send, " %d ", msglen);
				index_send += m;
				m = sprintf(rst + index_send, "%s", message);
				index_send += m;
				m = sprintf(rst + index_send, "\n");
				index_send += m;

				send(new_sock, "OK\n", 3, 0);
				sendto(socket_list[target], rst, index_send, 0, (struct sockaddr *)&addr_list[target], sizeof(addr_list[target]));
				free(message);
			}


			if (buffer[0] == 'B' && buffer[1] == 'R'){
				i = 0;
				j = 0;
				while(buffer[i] != ' '){
					i++;
				}
				i = i + 1;
					
				int msglen2 = 0;
				while(buffer[i] != ' '){
					msglen2 = 10*msglen2 + (buffer[i] - '0');
					i++;
				}
				
				if (msglen2 < 1 || msglen2 > 994){
					send(new_sock, "ERROR Invalid msglen\n", 21, 0);
					continue;
				}

				char *message2 = malloc(msglen2 * sizeof(char));
				char *tmp_send2 = message2;
				i = i + 1;
				while(buffer[i] != '\n'){
					*tmp_send2 = buffer[i];
					tmp_send2++;
					i++;
				}

				char *sender_name2;
				for(i = 0; i < index_g; i++){
					if(server_type[i] == 2){
						if (addr_list[i].sin_addr.s_addr == client.sin_addr.s_addr && addr_list[i].sin_port == client.sin_port){
							sender_name2 = name_list[i];
							break;
						}
					}
				}

				int index_send2 = 0;
				m = 0;
				char rst2[2000];
				m = sprintf(rst2, "FROM ");
				index_send2 += m;
				m = sprintf(rst2 + index_send2, "%s", sender_name2);
				index_send2 += m;
				m = sprintf(rst2 + index_send2, " %d ", msglen2);
				index_send2 += m;
				m = sprintf(rst2 + index_send2, "%s", message2);
				index_send2 += m;
				m = sprintf(rst2 + index_send2, "\n");
				index_send2 += m;


				send(new_sock, "OK\n", 3, 0);
				for(i = 0; i < index_g; i++){
					sendto(socket_list[i], rst2, index_send2, 0, (struct sockaddr *)&addr_list[i], sizeof(addr_list[i]));
				}
				free(message2);
			}

			if (buffer[0] == 'S' && buffer[1] == 'H'){
				i = 0;
				j = 0;
				while(buffer[i] != ' '){
					i++;
				}
				i = i + 1;

				char userid_send2[50];
				while(buffer[i] != ' '){
					userid_send2[j] = buffer[i];
					i++;
					j++;
				}
				userid_send2[j] = '\0';

				int flag3 = 0;
				int target2 = -1;
				for (m = 0; m < index_g; m++){
					if(strcmp(userid_send2, name_list[m]) == 0){
						flag3 = 1;
						target2 = m;
						break;
					}
				}
				if (flag3 == 0){
					send(new_sock, "ERROR Unknown userid\n", 21, 0);
					continue;
				}

				if (server_type[target2] == 1){
					send(new_sock, "ERROR SHARE not supported because recipient is using UDP\n", 59, 0);
					continue;
				}

				int msglen3 = 0;
				i = i + 1;
				while(buffer[i] != ' '){
					msglen3 = 10 * msglen3 + (buffer[i] - '0');
					i++;
				}

				send(new_sock, "OK\n", 3, 0);
				
				char share_buffer[1024];

				while(1){
					n = recv(new_sock, share_buffer, 1024, 0);
					sendto(socket_list[target2], share_buffer, 1024, 0, (struct sockaddr *)&addr_list[target2], sizeof(addr_list[target2]));
					send(new_sock, "OK\n", 3, 0);
					if (n < 1024){
						break;
					}
				}


			}
		
		}
	
	}
	while(n > 0);

	close(new_sock);
	return NULL;


}


void sort_help(char *name_list[], int socket_list[], int server_type[], struct sockaddr_in addr_list[], int index){
	int i;
	int j;
	char * tmp_name;
	int tmp_socket;
	int tmp_server;
	struct sockaddr_in tmp_addr;
	for (i = 0; i < index; i++){
		for (j = i + 1; j < index; j++){
			if (strcmp(name_list[i], name_list[j]) > 0){
				tmp_name = name_list[i];
				name_list[i] = name_list[j];
				name_list[j] = tmp_name;
				tmp_socket = socket_list[i];
				socket_list[i] = socket_list[j];
				socket_list[j] = tmp_socket;
				tmp_server = server_type[i];
				server_type[i] = server_type[j];
				server_type[j] = tmp_server;
				tmp_addr = addr_list[i];
				addr_list[i] = addr_list[j];
				addr_list[j] = tmp_addr;
			}
		}
	}
}
