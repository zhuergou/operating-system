#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>

int max_squares = 0;
char ** dead_end_boards[1000];
int position = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct package{
    char ** table;
    int currentx;
    int currenty;
    int row;
    int column;
    int step;
} variable;

void * knight(void * arg){
	variable * arg_temp;
	arg_temp = (variable *)arg;
    char ** table = arg_temp -> table;
    int currentx = arg_temp -> currentx;
    int currenty = arg_temp -> currenty;
    int row =  arg_temp -> row;
    int column =  arg_temp -> column;
    int step =  arg_temp -> step;
    free(arg);

	int choicex[8] = {1, 2, 2, 1, -1, -2, -2, -1};
	int choicey[8] = {2, 1, -1, -2, -2, -1, 1, 2};

	table[currentx][currenty] = 'k';
	
	int i;
	int count = 0;
	int children;
	int j;
	int m;
	int k;

	for (i = 0; i < 8; i++){
		if ((currentx + choicex[i]) < 0 || (currentx + choicex[i])>= row){
			count++;
			continue;
		}
		if ((currenty + choicey[i]) < 0 || (currenty + choicey[i]) >= column){
			count++;
			continue;
		}
		if (table[currentx + choicex[i]][currenty + choicey[i]] == 'k'){
			count++;
			continue;
		}
	}
	children = 8 - count;
	count = 0;

	if (children == 0){
		printf("THREAD %u: Dead end after move #%d\n", (unsigned int)pthread_self(), step);
		fflush(stdout);

        pthread_mutex_lock(&mutex);
        if (max_squares < step){
            max_squares = step;
        }
        dead_end_boards[position] = calloc(row, sizeof(char *));
        char ** temp_pt = dead_end_boards[position];
        for (i = 0; i < row; i++){
            *temp_pt = calloc(column, sizeof(char));
            temp_pt++;
        }
        temp_pt = dead_end_boards[position];
        for (i = 0; i < row; i++){
            for (j = 0; j < column; j++){
                temp_pt[i][j] = table[i][j];
            }
        }
        position ++;
        pthread_mutex_unlock(&mutex);

		char ** free_help = table;
		for (i = 0; i < row; i++){
			free(*free_help);
			free_help++;
		} 
		free(table);

		return NULL;
	}
	else if(children == 1){
		for (i = 0; i < 8; i++){
			if ((currentx + choicex[i]) < 0 || (currentx + choicex[i])>= row){
				count++;
				continue;
			}
			if ((currenty + choicey[i]) < 0 || (currenty + choicey[i]) >= column){
				count++;
				continue;
			}
			if (table[currentx + choicex[i]][currenty + choicey[i]] == 'k'){
				count++;
				continue;
			}
            variable * s1;
            s1 = (variable *)malloc(sizeof(variable));
            s1 -> table = table;
            s1 -> currentx = currentx + choicex[i];
            s1 -> currenty = currenty + choicey[i];
            s1 -> row = row;
            s1 -> column = column;
            s1 -> step = step + 1;
			knight((void *)s1);
           
			return NULL;
		}
	}



	printf("THREAD %u: %d moves possible after move #%d; creating threads\n", (unsigned int)pthread_self(), children, step);
	fflush(stdout);

    pthread_t tid[children];
    int rc;

	j = 0;
	char ** n;
	char ** table_temp;
    variable * s2;

	for (i = 0; i < 8; i++){
		if ((currentx + choicex[i]) < 0 || (currentx + choicex[i])>= row){
			count++;
			continue;
		}
		if ((currenty + choicey[i]) < 0 || (currenty + choicey[i]) >= column){
			count++;
			continue;
		}
		if (*(*(table + currentx + choicex[i]) + currenty + choicey[i]) == 'k'){
			count++;
			continue;
		}
        
        s2 = (variable *)malloc(sizeof(variable));

		table_temp = calloc(row, sizeof(char *));
		n = table_temp;
		for (m = 0; m < row; m++){
			*n = calloc(column, sizeof(char));
			n++;
		}
		for (m = 0; m < row; m++){
			for (k = 0; k < column; k++){
				table_temp[m][k] = table[m][k];
			} 
		}
        s2 -> table = table_temp;

        s2 -> currentx = currentx + choicex[i];
        s2 -> currenty = currenty + choicey[i];
        s2 -> row = row;
        s2 -> column = column;
        s2 -> step = step + 1;

        rc = pthread_create(&tid[j], NULL, knight, (void *)s2);       
        if (rc != 0){
            perror("ERROR: <create thread failed>");
        }
        
		j = j + 1;
	}	
	children = 8 - count;

	n = table;
	for (i = 0; i < row; i++){
		free(*n);
		n++;
	}
	free(table);

	for (i = 0; i < children; i++){
        rc = pthread_join(tid[i], NULL);
        if (rc != 0){
            fprintf(stderr, "ERROR: <Could not join>");
        }
    }
    return NULL;
}

int main(int argc, char *argv[]){
	
	if (argc < 3){
		fprintf(stderr, "ERROR: Invalid argument(s)\n");
		fprintf(stderr, "USAGE: a.out <m> <n>\n");
		return EXIT_FAILURE;
	}

	int row = atoi(argv[1]);
	int column = atoi(argv[2]);
	int i;
	int j;
	int k;
	

	if (row <= 2 || column <= 2){
		fprintf(stderr, "ERROR: Invalid argument(s)\n");
		fprintf(stderr, "USAGE: a.out <m> <n>\n");
		return EXIT_FAILURE;
	}


	char **table = calloc(row, sizeof(char *));
	char **m = table;
	for (i = 0; i < row; i++){
		*m = calloc(column, sizeof(char));
		m++;
	}

	m = table;
	for (i = 0; i < row; i++){
		for(j = 0; j < column; j++){
			*(*(m + i) + j) = '.';
		}
	}
	
	printf("THREAD %u: Solving the knight's tour problem for a %dx%d board\n", (unsigned int)pthread_self(), row, column);
    variable * s3;
    s3 = (variable *)malloc(sizeof(variable));
    s3 -> table = table;
    s3 -> currentx = 0;
    s3 -> currenty = 0;
    s3 -> row = row;
    s3 -> column = column;
    s3 -> step = 1;
    knight((void *)s3);
    
    
    printf("THREAD %u: Best solution found visits %d squares (out of %d)\n", (unsigned int)pthread_self(), max_squares, row * column);

/*
	m = table;
	for (i = 0; i < row; i++){
		free(*m);
		m++;
	}
	free(table);
*/
	char tmp[40];
	char *pt;
	char ** help;
	for (i = 0; i < position; i++){
		for (j = 0; j < row; j++){
			pt = tmp;
			if (j == 0){
				sprintf(tmp, "THREAD %u: > ", (unsigned int)pthread_self());
			}
			else{
				sprintf(tmp, "THREAD %u:   ", (unsigned int)pthread_self());
			}
			while (*pt != '\0'){
				pt++;
			}
			for (k = 0; k < column; k++){
				sprintf(pt, "%c", *(*(dead_end_boards[i] + j) + k));
				pt++;
			}
			sprintf(pt, "\n");
			printf("%s", tmp);
			fflush(stdout);
		}	
		help = dead_end_boards[i];
		for (j = 0; j < row; j++){
			free(*help);
			help++;
		}
		free(dead_end_boards[i]);
	}
}

