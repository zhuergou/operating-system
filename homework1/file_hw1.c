#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#define MAXPATH 50

struct wordList{
	char *word;
	int count;
};


int main(int argc, char *argv[]){
	struct stat buf;
	int rc;
	int len;
	char *pt;
	FILE *fp = NULL;
	char buffer[80];
	struct wordList *dictionary;
	struct wordList *p;
    int i;
	int low;
	int flag = 0;
	int total;
	int unique;
	int index;
	char c;
	struct dirent * file;

	if (argc == 1){
		perror("ERROR: <this file should be empty>");
		return EXIT_FAILURE;
	}

	//open directory
	DIR * dir = opendir(argv[1]);
	if (dir == NULL){
		perror("ERROR: <opendir failed>");
		return EXIT_FAILURE;
	}

		
	len = 32;
	// allocate initial memory
	if(NULL == (dictionary = (struct wordList *)calloc(len, sizeof(struct wordList)))){
		perror("ERROR: <calloc failed>");
		return EXIT_FAILURE;
	}
	printf("Allocated initial parallel arrays of size %d.\n", len);
		
	total = 0;
	unique = 0;

	while ((file = readdir(dir)) != NULL){
		
		// get the file path 
		char fullpath[MAXPATH];
        sprintf(fullpath, "%s/%s", argv[1], file->d_name);
        pt = strstr(fullpath, "//");
        if (pt != NULL){
			sprintf(fullpath,"%s%s", argv[1], file->d_name);
        }

	    rc = lstat(fullpath, &buf);
       
		if (rc == -1){
			perror("ERROR: <lstat failed>");
			return EXIT_FAILURE;
		}
	
		
		// check file type
		if (S_ISREG(buf.st_mode)){
			

			fp = fopen(fullpath, "r");
			if (NULL == fp){
				perror("ERROR: <can't open file>");
				return EXIT_FAILURE;
			}
			


			while((c = fgetc(fp)) != EOF){

				//get the word
				low = 0;
				if (!isalnum(c)){
					continue;
				}
				else{
					buffer[low++] = c;
					while(isalnum(c = fgetc(fp))){
						buffer[low++] = c;
					}
				}

				buffer[low] = '\0';

				if(strlen(buffer) == 1){
					continue;
				}
				
				// count total number
				total = total + 1;

				// check whether the word in dictionary
				p = dictionary;
				i = 0;
				while(i < len){
					if ((*p).word == NULL){
						break;
					}
					if (strcmp(buffer, (*p).word) == 0){
						(*p).count = (*p).count + 1;
						flag = 1;
						break;
					}
					i = i + 1;
					p = p + 1;
				}
				
				if(i == len){
					if(NULL == (dictionary =  (struct wordList *)realloc(dictionary, (32 + len)* sizeof(struct wordList)))){
						perror("ERROR <realloc failed>");
						return EXIT_FAILURE;
					}
					memset(dictionary + len, 0, 32* sizeof(struct wordList));
					len = len + 32;
					printf("Re-allocated parallel arrays to be size %d.\n", len);
				}

				if (flag == 0){

					// put new word into dictionary
					p = dictionary;
					while((*p).word != NULL){
						p = p + 1;
					}

					if(NULL == ((*p).word = (char *)calloc(strlen(buffer) + 1, sizeof(char)))){
						perror("ERROR <malloc failed>");
						return EXIT_FAILURE;
					}
					strcpy((*p).word, buffer);
					(*p).count = 1;
					unique = unique + 1;
				}

				flag = 0;

				if (c == EOF){
					break;
				}
			}
			fclose(fp);

		}
	}


	printf("All done (successfully read %d words; %d unique words).\n", total, unique);
			
	// print out format
	if (argc == 3 && (total >= 2* atoi(argv[2]))){
		index = atoi(argv[2]);
		printf("First %d words (and corresponding counts) are:\n", index);
		p = dictionary;
		for (i = 0; i < index; i++){
			printf("%s -- %d\n", (*p).word, (*p).count);
			p = p + 1;
		}

		printf("Last %d words (and corresponding counts) are:\n", index);
		p = dictionary;
		for (i = 0; i < unique - index; i++){
			p = p + 1;
		}
		for (i = unique - index; i < unique; i++){	
			printf("%s -- %d\n", (*p).word, (*p).count);
			p = p + 1;
		}

	}
	else{
		printf("All words (and corresponding counts) are:\n");
		p = dictionary;
		for (i = 0; i < unique; i++){
			printf("%s -- %d\n", (*p).word, (*p).count);
			p = p + 1;
		}

	}

	p = dictionary;
	for(i = 0; i < unique; i++){
		free((*p).word);
		(*p).word = NULL;
		p = p + 1;
	}

	free(dictionary);
	dictionary = NULL;
	p = NULL;
	closedir(dir);
	return EXIT_SUCCESS;	

}
