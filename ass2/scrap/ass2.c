#include<errno.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define delim " /t/n"
#define delim1 ";"

int c[1][1];
void print(char *username , char* Hostname , char *cwd)
{
  printf("<%s@%s:%s> ",username,Hostname,cwd);

}

void modify_cwd(char* cwd, char *base_dir) 
{
  int i, j;
  for(i = 0; cwd[i]==base_dir[i] && cwd[i]!='\0' && base_dir[i] != '\0'; i++);
  if(base_dir[i] == '\0') 
  {
    cwd[0] = '~';
    for(j = 1; cwd[i]!='\0'; j++) 
    {
      cwd[j] = cwd[i++];
    }
    cwd[j] = '\0';
  }
}

int quit()
{
  return 1;
}
int tokenise(char* cmdline, char**tokens)
{
  int x = 0;
    int y;
  char * token = strtok(cmdline," \t\n");

  while(token!=NULL)
  {
    tokens[x] = token;
    x++;
    token = strtok(NULL," \t\n");
  }


  tokens[x] = NULL;



  return x;
}

int other_cmds(char **tokens)
{

  pid_t pid, wpid,cmd_pid;
  int status;
int flag;

  pid = fork();



  if (pid == 0) // Child process
  {

    if (execvp(tokens[0],tokens) == -1) 
    {
      perror("Error");
    }
    return 0;
                     
  } 
  else if (pid < 0) // Error forking
  {

    perror("Error");
    //    return 0;
  }
  else // Parent process
  {
                 for(i=0;i<
    do {  
      if(flag==0)
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));


  }
  return 1;



}

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
  // char **cmd_line_token = malloc(10000 * sizeof(char*));//contains each token delim(";") separated
  //  char **cmd_token = malloc(10000*sizeof(char*));//contains each token as delim(" /t/n") separated
  //char backup[10000];

  char **tokens = malloc(10000*sizeof(char*));//contains tokens 
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

    i = tokenise(cmd_line,tokens);

//for(k=0;k<i;k++)
//{
  //printf("%s\n",tokens[k]);
//}


    if(strcmp(tokens[0],"cd\0")==0)
    {
      //  ptr+=2;
      if(tokens[1] == NULL || strcmp(tokens[1], "~\0") == 0 || strcmp(tokens[1], "~/\0") == 0) 
      {                
        chdir(home);
        strcpy(cwd,home);
        modify_cwd(cwd,home);
      }
      else if(chdir(tokens[1]) == 0) 
      {

        getcwd(cwd,10000);
        modify_cwd(cwd,home);

      }
      else {
        perror("Error executing cd command");
      }

      continue;
    }

    if(strcmp(tokens[0],"exit\0")==0)
    {
      break;

      flag =  quit();
      break;
      if(flag==1)
        break;

    }


    else if(strcmp(tokens[0],"pwd\0")==0 && i==1 )
    {

      getcwd(dummy,10000);

      printf("%s\n",dummy);
      continue;
    }
    else
    {

      j = other_cmds(tokens);

      if(j==0)
        exit(-1);
    }

  }
  return 0;
}
