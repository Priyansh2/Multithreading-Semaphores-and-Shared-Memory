#include<errno.h>
#include<unistd.h>
#include <pwd.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/wait.h>

extern int errno ;
char rootdir[100];
char curdir[100];

void print_initials()
{	
	char hostname[100];
	struct passwd *p = getpwuid(getuid());

	gethostname(hostname,100);
	getcwd(curdir,100);
	printf( "<%s@%s:", p->pw_name, hostname);
	int j=0;
	if( strlen(curdir) >= strlen(rootdir)){

		printf( "~" );
		j=strlen( rootdir );
		char *dir = &curdir[j];
		printf( "%s>", dir);
	}
	else
		printf("%s>",curdir);
}

void gotoprev()
{
	int l=strlen(curdir);
	for(int i=l-1;i>=1;i--)
	{
		if(curdir[i]=='/')
		{
			if(i>1)
				curdir[i]='\0';
			break;
		}
		else
			curdir[i]='\0';
	}
	chdir(curdir);
}
void  execute(char **tokens)
{
	pid_t  pid;
	int  status;
	int i;

	if ((pid = fork()) < 0)            
		fprintf(stderr,"Shell:%s\n",strerror(errno));

	else if (pid == 0) {    

		if (execvp(*tokens, tokens) < 0) // executing the command  
			fprintf(stderr,"Shell:%s\n",strerror(errno));
	}

	else {                                  // for the parent:    
		while (wait(&status) != pid)       // wait for completion
			;
	}
}
void parse(char **tokens,char *ptr2,char *arg){

	while(arg!=NULL){
		*tokens++ = arg;
		arg = strtok_r(NULL," \t",&ptr2);
	}
	*tokens = '\0';
}

void main()
{
	getcwd(rootdir,100);
	char line[10000];
	char *cmd,*ptr1,*ptr2;

	while(1){
		print_initials();
		fgets(line,10000,stdin);
		//if( *line == '\n')
		//	continue;
		int l = strlen(line);
		line[l-1] = '\0';
		cmd = strtok_r(line, ";",&ptr1);
		while( cmd !=NULL )
		{
			char *arg =strtok_r(cmd," \t",&ptr2);

			if(arg==NULL)
				break;

			else if( strcmp(arg,"cd") == 0)
			{
				arg = strtok_r(NULL," \t",&ptr2);
				if( arg==NULL || strcmp(arg,".")==0 || strcmp(arg,"~")==0)
					chdir(rootdir);

				else if( strcmp(arg,"..")==0 )
					gotoprev();

				else if(chdir(arg)==-1)
					fprintf(stderr,"Shell:%s\n",strerror(errno));

				getcwd(curdir,100);

			}

			else if( strcmp(arg,"pwd") ==0)
				printf("%s\n",curdir);

			else if( strcmp(arg,"echo") ==0)
			{
				char meme[10000];
				arg = strtok_r(NULL,"\0",&ptr2);
				strcpy(meme,arg);
				int flag=0,i=0;

				while(flag==0){
					if(meme[i] != ' ')
						flag = 1;
					i++;
				}
				char *mes;
				mes=&meme[i-1];
				printf("%s\n",mes);	
			}

			else 
			{
				char **tokens = malloc(10000*sizeof(char*));
				parse(tokens,ptr2,arg);
				if(strcmp(tokens[0], "exit")==0)
					exit(0);
				execute(tokens);
        free(tokens);
			}

			cmd = strtok_r(NULL,";",&ptr1);
		}
	}
}
