#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>


int knight(char **table, int currentx, int currenty, int row, int column, int step){
	int choicex[8] = {1, 2, 2, 1, -1, -2, -2, -1};
	int choicey[8] = {2, 1, -1, -2, -2, -1, 1, 2};

	table[currentx][currenty] = 'k';
	
	int i;
	int count = 0;
	int children;
	int j;

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
		printf("PID %d: Dead end after move #%d\n", getpid(), step);
		fflush(stdout);

#ifdef DISPLAY_BOARD
		char tmp[40];
		char *pt;
		for (i = 0; i < row; i++){
			pt = tmp;
			sprintf(tmp, "PID %d:   ", getpid());
			while (*pt != '\0'){
				pt++;
			}
			for (j = 0; j < column; j++){
				sprintf(pt, "%c", *(*(table + i) + j));
				pt++;
			}
			sprintf(pt, "\n");
			printf("%s", tmp);
			fflush(stdout);
		}

#endif
		printf("PID %d: Sent %d on pipe to parent\n", getpid(), step);
		fflush(stdout);
		return step;
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
			int t = knight(table, currentx + choicex[i], currenty + choicey[i], row, column, step + 1);
			return t;
		}
	}



	printf("PID %d: %d moves possible after move #%d\n", getpid(), children, step);
	fflush(stdout);

#ifdef DISPLAY_BOARD
	char tmp2[40];
	char *pt2;
	for (i = 0; i < row; i++){
		pt2 = tmp2;
		sprintf(tmp2, "PID %d:   ", getpid());
		while(*pt2 != '\0'){
			pt2++;
		}
		for (j = 0; j < column; j++){
			sprintf(pt2, "%c", *(*(table + i) + j));
			pt2++;
		}
		sprintf(pt2, "\n");
		printf("%s", tmp2);
		fflush(stdout);
	}
#endif

	int **pipe_set = malloc(children * sizeof(int *));
	for (i = 0; i < children; i++){
		pipe_set[i] = malloc(2 * sizeof(int));
		int rc = pipe(pipe_set[i]);
		if (rc == -1){
			perror("pipe() failed");
			return EXIT_FAILURE;
		}
	}


	j = 0;
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

		pid_t pid = fork();

		if (pid < 0){
			perror("ERROR: <fork failed>");
			return EXIT_FAILURE;
		}

		if (pid == 0){
			int t = knight(table, currentx + choicex[i], currenty + choicey[i], row, column, step + 1);
			close(pipe_set[j][0]);
			pipe_set[j][0] = -1;
			write(pipe_set[j][1], &t, 4);
/*			if (number_write == -1){
				perror("ERROR <write failed>");
				return EXIT_FAILURE;
			}
*/			exit(EXIT_SUCCESS);
		}

#ifdef NO_PARALLEL
		wait(NULL);
#endif


		j = j + 1;
	}


	children = 8 - count;
	int maxValue = 0;

	for (i = 0; i < children; i++){
#ifndef NO_PARALLEL
		int status;
		wait(&status);

		if (WIFSIGNALED(status)){
			perror("ERROR: <child terminated>");
			exit(EXIT_FAILURE);
		}
		else if(WIFEXITED(status)){
			close(pipe_set[i][1]);
			pipe_set[i][1] = -1;
			int rc = 0;
			read(pipe_set[i][0], &rc, 4);
/*			if (number_read == -1){
				perror("ERROR <read failed>");
				return EXIT_FAILURE;
			}
*/			maxValue = (maxValue > rc)? maxValue: rc;
			printf("PID %d: Received %d from child\n", getpid(), rc);
			fflush(stdout);
		}
#endif
	
#ifdef NO_PARALLEL
		
		close(pipe_set[i][1]);
		pipe_set[i][1] = -1;
		int rc = 0;
		read(pipe_set[i][0], &rc, 4);
		maxValue = (maxValue > rc)? maxValue: rc;
		printf("PID %d: Received %d from child\n", getpid(), rc);
		fflush(stdout);
#endif
	}

	for (i = 0; i < children; i++){
		free(pipe_set[i]);
	}
	free(pipe_set);
	
	if (step == 1){
		printf("PID %d: Best solution found visits %d squares (out of %d)\n", getpid(), maxValue, row*column);
		fflush(stdout);
	}
	else{
		printf("PID %d: All child processes terminated; sent %d on pipe to parent\n", getpid(), maxValue);
		fflush(stdout);
	}
	return maxValue;
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

#ifdef DISPLAY_BOARD
	int j;
	m = table;
	for (i = 0; i < row; i++){
		for(j = 0; j < column; j++){
			*(*(m + i) + j) = '.';
		}
	}
#endif


	
	printf("PID %d: Solving the knight's tour problem for a %dx%d board\n", getpid(), row, column);
	knight(table, 0, 0, row, column, 1);

	m = table;
	for (i = 0; i < row; i++){
		free(*m);
		m++;
	}
	free(table);

}

