#include<errno.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

void print(char *username , char* Hostname , char *cwd)
{
  printf("<%s@%s:%s> ",username,Hostname,cwd);

}

void modify_cwd(char* cwd, char *base_dir) 
{
        int i, j;
        for(i = 0; cwd[i]==base_dir[i] && cwd[i]!='\0' && base_dir[i] != '\0'; i++);
        if(base_dir[i] == '\0') {
                cwd[0] = '~';
                for(j = 1; cwd[i]!='\0'; j++) {
                        cwd[j] = cwd[i++];
                }
                cwd[j] = '\0';
        }
}

int quit()
{
  return 1;
}
int tokenise_cmd_line(char* cmdline, char** cmd_line_token) 
{
        int x = 0;
        char* token = strtok(cmdline, ";");
        while(token!=NULL) 
        {
                cmd_line_token[x++] = token;
                token = strtok(NULL, ";");
        }
        return x;//no of cmd_line tokens
}//--> for ";" separated commands

int  tokenise_cmd_token(char* cmd, char** cmd_token, int index) 
{
        //int y = 0;
        char* token = strtok(cmd, " /t/n");
        while(token!=NULL) 
        {
                cmd_token[index] = token;
index++;
                token = strtok(NULL, " /t/n");
        }
        return index;//no of tokens in total
}//--> for " \t\n" separated commands


int main()
{
  char Hostname[10000];
  char *username;
  char buff[10000],home[10000];
  char cwd[10000];
int i,j,k;
int index;
char *cmd_line = malloc(10000*sizeof(char));  
int flag=0;
  char *arg,*arg1;
  char **cmd_line_token = malloc(10000 * sizeof(char*));//contains each token delim(";") separated
  char **cmd_token = malloc(10000*sizeof(char*));//contains each token as delim(" /t/n") separated
  //char backup[10000];
  
  char **tokens = malloc(10000*sizeof(char*));//contains only tokens as "commands" 
  char dummy[10000];
 username=getenv("USER");

    gethostname(Hostname,10000);

     getcwd(home,10000);
       //strcpy(backup,home); 
        strcpy(cwd,home);
        modify_cwd(cwd,home);  

  while(1)
  {

print(username,Hostname,cwd);    
//char cmdline[10000];

fgets(cmd_line,10000,stdin);
i = tokenise_cmd_line(cmd_line,cmd_line_token);//no of command line tokens
//j=0;//setting of index of cmd_line_token[] to be 0;
index = 0;
for(j=0;j<i;j++)
{
   // k = j;

   k =  tokenise_cmd_token(cmd_line_token[j],cmd_token,index);
            
index = k;


}
//int x = 0;
cmd_token[index] = NULL;
cmd_line_token[i] = NULL;
int x=0;
char** l;
int len=-1;
while(l!=NULL)
{
    l = len+1+cmd_line_token;
  tokens[x] = *l ;
x++;     
len = strlen(*l);

}

//int y=0;
int y;
for(y=0;y<index;y++)
{
  printf("%s %lld",cmd_token[y],strlen(cmd_token[y]));
  printf("\n");
}
for(y=0;y<x;y++)
{
  printf("%s %lld",tokens[y],strlen(tokens[y]));
  printf("\n");
}
int ptr=-1;
for(y=0;y<x;y++)
{

if(strcmp(tokens[y],"cd\0")==0)
      {
           ptr+=2;
 if(cmd_token[ptr] == NULL || strcmp(cmd_token[ptr], "~\0") == 0 || strcmp(cmd_token[ptr], "~/\0") == 0) 
        {                
          chdir(home);
                strcpy(cwd,home);
                modify_cwd(cwd,home);
        }
        else if(chdir(cmd_token[ptr]) == 0) 
        {
          
          getcwd(cwd,10000);
                modify_cwd(cwd,home);
                
        }
        else {
                perror("Error executing cd command");
        }

continue;
      }
      
if(strcmp(tokens[y],"exit\0")==0)
      {
      
        printf("yoyo\n");
     flag =  quit();

      if(flag==1)
        break;

      }
      else if(strcmp(tokens[y],"echo\0")==0)
      {
        /*char *arg = strtok(0,"\t\r\n");
        l = strlen(arg);
        arg[l] = '\0';
        printf("%s\n", arg);*/
continue;
      }
      else if(strcmp(tokens[y],"pwd\0")==0)
      {
        
        getcwd(dummy,10000);
        
        printf("%s\n",dummy);
continue;
      }
      else
      {
        
        pid_t pid, wpid,cmd_pid;
        int status;

        pid = fork();
      
        if (pid == 0) // Child process
        {

          if (execvp(tokens[y],cmd_token) == -1) 
          {
            perror("Error");
          }
          
        } 
        else if (pid < 0) // Error forking
        {

          perror("Error");
        }
        else // Parent process
        {

          do {
            wpid = waitpid(pid, &status, WUNTRACED);
          } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }

      
      }
    }
if(flag==1)
break;  

}
  return 0;
}
