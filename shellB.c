
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 40 /* 80 chars per line, per command, should be enough. */

/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also 
modifies the args parameter so that it holds points to the null-terminated strings  which 
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/
//
//Nick Armstrong
//
char history[200];
void setup(char inputBuff[], char *args[],int *background, int hSize)
{
    int length,  /* Num characters in the command line */
        i,       /* Index for inputBuff arrray          */
        j,       /* Where to place the next parameter into args[] */
        start;   /* Beginning of next command parameter */
	char temp[40];
	char filepath[40];
    /* Read what the user enters */
    length = read(STDIN_FILENO, inputBuff, MAXLINE);  

    j = 0;
    start = -1;
    FILE *f1; 
    if (length == 0){
	f1 = fopen("history.txt", "w+");
	if(f1!=NULL){
		for(j=0; j<hSize; j++){
			for(i = 0; i <40 ; i++){
				temp[i] = history[i+(40*j)];
					if(temp[i]!='\0'){
						fprintf(f1,"%c",temp[i]);}
			}
			fprintf(f1,"\n");
		}
	}
	fclose(f1);

        exit(0);     
     }
       /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
	exit(-1);           /* Terminate with error code of -1 */
    }
    
    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {
 
        switch (inputBuff[i]){
        case ' ':
        case '\t' :          /* Argument separators */

            if(start != -1){
                args[j] = &inputBuff[start];    /* Set up pointer */
                j++;
            }

            inputBuff[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inputBuff[start];     
                j++;
            }

            inputBuff[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuff[i] = '\0';
            break;
            
        default :             /* Some other character */
            if (start == -1)
                start = i;
	}
 
    }    
    args[j] = NULL; /* Just in case the input line was > 80 */
} 

int main(void)
{
    char inputBuff[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int background;         /* Equals 1 if a command is followed by '&', else 0 */
    pid_t  pid;
    int i =0, hSize = 0, number =0, j=0, position; 	
    char temp[40]; 
    char pointer[40];
// Read old history 
FILE *f1; 
f1 = fopen("history.txt", "r");
if(f1!=NULL){
	
	while(fscanf(f1,"%s",pointer)!=EOF){
		for(i=0; i<40; i++){
			if(pointer[i] == '\0'){
				history[i+(hSize*40)] = '\0';
			}
			else{
					history[i+hSize*40] = pointer[i];
			}
		}
		hSize++;
	}
	fclose(f1);
}
    while (1){            /* Program terminates normally inside setup if appropriate */

	background = 0;
	printf("CSE2431Sh$ ");  /* Shell prompt */
        fflush(0);
        setup(inputBuff, args, &background,hSize);       /* Get next command */
//Adds to history
	if(strcmp(*args,"h") && strcmp(*args, "history")&&strcmp(*args,"rr")&&(args[0][0]!='r')){
		for(i =0; i<40; i++){
			if(hSize==5){
				hSize=0;
				position=1;
			}
			if(inputBuff[i] == '\0'){
				history[i+(hSize*40)] = '\0';
				i=40; 
			}
			else{	
				history[i+(hSize*40)] = inputBuff[i];
			}
		}
		hSize++;
	}
//RR command 
	if(args[0]!=NULL && args[1]==NULL){
		if(!strcmp(*args,"rr")){
			for(i=0; i<40; i++){
				if(history[i+(hSize-1)*40]!='\0'){
					inputBuff[i] = history[i+(hSize-1)*40];
				}
				else{
					inputBuff[i] = '\0';
				}
			}
		}

	} 

//Rnum command
	if(args[0]!=NULL){
		if(args[0][0] == 'r'){
			if(isdigit(args[0][1])){
				number = (args[0][1])-'0';
				if(0>number){
					printf("\n Negative numbers and 0 is not accepted \n");
				}
				if(position!=1){
					if(number>hSize){
						printf("\n cant accsess numbers outside the historys size \n");
					}
				}
				else{
					if(number>5){
						printf("\n cant accsess numbers outside the historys size \n");
					}
				}				
				for(i=0; i<40; i++){
					if(history[i+(number-1)*40]!='\0'){
						inputBuff[i] = history[i+(number-1)*40];
					}
					else{
						inputBuff[i] = '\0';					
					}
				}
			}
		}
	}
// Check if user typed history
	if(args[0] != NULL && hSize>0){
		if(!strcmp(*args,"h") || !strcmp(*args, "history")){
			if(position==1){
				for(i =0; i<5; i++){
					printf("\n%d-",i+1);
					for(j =0; j<40; j++){
						if(history[j+i*40]!='\0'){
							printf("%c" , history[j+i*40]);
						}
					}
				}
			}
			else{
				for(i =0; i<hSize; i++){
						printf("\n%d-",i+1);
						for(j =0; j<40; j++){
							if(history[j+i*40]!='\0'){
								printf("%c" , history[j+i*40]);
							}
						}
					
				}
			}
		printf("\n");
		}
	}
	
	pid =fork();
	if(pid==0){
		execvp(args[0],args);
	}
	else{
		if(background==0){
			waitpid(pid);
		}
		else sleep(1);
	}
    }

}
