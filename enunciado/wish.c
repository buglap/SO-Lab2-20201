#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<ctype.h>

int BSIZE= 1024;
char errorMessage[20] = "ocurrió un error\n";
int batch=0;
int pathChanged=0;
char *path;
int CLOSED=0;
int isEmpty=0;
char parallelPath[1024][1024];
int nParallelPaths=0;

void printError();
void printWish();
int runProcess(char *command[]);
int tokenize(char *buffer);


int main(int argc, char* argv[]){
        FILE *file = NULL;
        path=(char*) malloc(BSIZE);
        char buffer[BSIZE];
        
        //Modo promp wish>
        if(argc==1){ 
		//Guardar en archivo estandar
                file=stdin; 
                printWish();
        }
        //Modo batch, argumento como archivo a leer 
        else if(argc==2){ 
                char *batchFile= strdup(argv[1]);
                file = fopen(batchFile, "r");
                if (file == NULL) printError();
                batch=1;
	}
        else{
                printError();
        }
	
        while(fgets(buffer, BSIZE, file)){ 
                if(strstr(buffer,"&")!=NULL){
                        int i=0;
                        char *command[sizeof(buffer)];
                        command[0]= strtok(buffer,"\n\t&");                            
                        while(command[i]!=NULL){
                                i++;
                                command[i]=strtok(NULL,"\n\t&");
                        }
                        command[i+1]=NULL;
                        int pid[i];
                        for(int j=0;j<i;j++){
				//pid[j] = runProcess(command[j]);
                                pid[j]= tokenize(command[j]);
                        for(int n=0;n<i;j++){
                                int status=0;
                                waitpid(pid[n],&status,0);                        
                                if (status == 1){
                                        printError();    
                                }
			}
                      }
                }
                else{
                      tokenize(buffer);
                }
                if(argc == 1) printWish();
	}
}

int tokenize(char *buffer){
	int rc;
	if(buffer[0] != '\0' && buffer[0] != '\n') {
		char *command[sizeof(buffer)];
		command[0] = strtok(buffer, " \t\n");
		int p=0;
		while(command[p]!=NULL){
			p++;
			command[p]=strtok(NULL, " \n\t");
		}
		command[p+1]=NULL;
		if(strcmp(command[0],"cd") == 0){
			 if(p==2){
				 if(chdir(command[1])!=0){
					printError();
				 }
			 }else{
				 printError();
			 }
		}
		else if(strcmp(command[0],"path") == 0){
			if(p==2){
				path=strdup(command[1]);
				 if(path[strlen(path)-1]!='/'){
					 strcat(path,"/");
				 }
			}else if(p!=0){
				for(int i=1;i<p;i++){
					char *temp=strdup(command[i]);
					if(temp[strlen(temp)-1]!='/')strcat(temp,"/");
					strcpy(parallelPath[i-1],temp);
					nParallelPaths++;
				}

			}
			if(p==1) isEmpty = 1;
		}
		else if(strcmp(command[0],"exit") == 0) {
			if(p==1) exit(0);
			else printError();
		}
		else{
			if(isEmpty == 1) printError();
			else{
			       	rc = runProcess(command);
			}
		}	
	}
	return rc;	
}

int runProcess(char *command[]){
	int rc=fork();
	if(rc<0){
		printError();
		exit(1);
	}
	else if(rc==0){
		 path=strdup("/usr/bin/");
		 path=strcat(path,command[0]);
		if(execv(path,command) == -1){
			printError();
			exit(0);
		}
	}
	return rc;
}
        
void printError(){
	write(STDERR_FILENO, errorMessage, strlen(errorMessage));
	exit(1);
}
void printWish(){
	write(STDOUT_FILENO, "wish> ", strlen("wish> "));
}
