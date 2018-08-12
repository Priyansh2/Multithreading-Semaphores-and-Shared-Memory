#include <ctype.h>
#include<errno.h>
#include<fcntl.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

char *ptr1,*ptr2;

void print(char *username , char* hostname , char *cwd)
{
  printf("<%s@%s:%s> ",username,hostname,cwd);

}

void modify_cwd(char* cwd, char *home_dir) 
{
  int i, j;
  for(i = 0; cwd[i]==home_dir[i] && cwd[i]!='\0' && home_dir[i] != '\0'; i++);
  if(home_dir[i] == '\0') 
  {
    cwd[0] = '~';
    for(j = 1; cwd[i]!='\0'; j++) 
    {
      cwd[j] = cwd[i++];
    }
    cwd[j] = '\0';
  }
}

void other_cmds(char **tokens)
{
  pid_t  pid,wpid;
  int  status;
  //int i;
  pid = fork();
  if (pid < 0)  //error forking
    perror("Error");	

  else if (pid == 0) //child process
  {    

    if (execvp(*tokens, tokens) < 0) // executing the command  
      perror("Error");	
  }

  else 
  {    // for the parent:    
    do {
      
        wpid = waitpid(pid, &status, WUNTRACED);

    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  }
}
void parsing_other_cmds(char **other_cmd_str,char *token,char delim[])
{

  while(token!=NULL)
  {
    *other_cmd_str++ = token;
    token = strtok_r(NULL,delim,&ptr2);
  }
  *other_cmd_str = '\0';


}

int main()
{
  char hostname[10000];
  char *username;
  char home[10000];
  char cwd[10000];
  int i,j,k;

  char *cmd_line = (char*)malloc(10000*sizeof(char));  
  int flag;
  char *token;//arg
  char *cmd;
  int err_no;
  char *echo_content;
  char dummy[10000];
  char *str = (char*)malloc(10000*sizeof(char));
  char **other_cmd_str = (char**)malloc(10000*sizeof(char*));
  int brk_flag;
  char delim1[]=";";
  char delim2[]=" \t\n";
  char delim3[]="\0";
  brk_flag=0;
  username=getenv("USER");
  gethostname(hostname,10000);

  getcwd(home,10000);

  strcpy(cwd,home);
  modify_cwd(cwd,home); 

  while(1)
  {
    if(brk_flag==1)
    {
      break;
      continue;
    }


    print(username,hostname,cwd);    

    fgets(cmd_line,10000,stdin);

    long long int l = strlen(cmd_line);
    cmd_line[l-1] = '\0';

    cmd = strtok_r(cmd_line,delim1,&ptr1);

    while( cmd !=NULL )
    {
      if(brk_flag==1)
      { 
        break;
        continue;
      }
      token =strtok_r(cmd,delim2,&ptr2);



      if(token==NULL)
        break;
      else if(strcmp(token,"exit\0")==0)
      {

        brk_flag=1;
        break;
        continue;
      }


      else if( strcmp(token,"cd\0") == 0)
      {
        token = strtok_r(NULL,delim2,&ptr2);
        if(token== NULL || strcmp(token, "~\0") == 0 || strcmp(token, "~/\0") == 0) 
        {                
          chdir(home);
          strcpy(cwd,home);
          modify_cwd(cwd,home);
        }
        else if(chdir(token) == 0) 
        {

          getcwd(cwd,10000);
          modify_cwd(cwd,home);

        }
        else {
          perror("Error executing cd commmand");
        }

      }

      else if( strcmp(token,"pwd\0") ==0)
      {
        getcwd(dummy,10000);

        printf("%s\n",dummy);
      }


      else if( strcmp(token,"echo\0") ==0)
      {

        token = strtok_r(NULL,delim3,&ptr2);
        strcpy(str,token);
        flag=0,i=0;

        while(flag!=1)
        {
          if(str[i]!=' ')
            flag = 1;
          i++;
          if(flag==1)
            break;

        }
        echo_content=&str[i-1];
        printf("%s\n",echo_content);	
      }
      else 
      {
        parsing_other_cmds(other_cmd_str,token,delim2);
        if(strcmp(other_cmd_str[0], "exit\0")==0)
        { 
          brk_flag=1;
          break;

        }
        else{
          int bg = 0;
         // token  =strtok_r(NULL,delim2,&ptr2);
          //printf("%s\n",token);
        //  printf("%s\n",strtok_r(NULL,delim2,&ptr2));
         // if(strcmp(token,"&\0")==0 || strcmp(strtok_r(NULL,delim2,&ptr2),"&\0")==0)
           // bg=1;

          other_cmds(other_cmd_str);
        }
      }

      cmd = strtok_r(NULL,delim1,&ptr1);
    }

    if(brk_flag==1)
      break;

  }
  return 0;
}
