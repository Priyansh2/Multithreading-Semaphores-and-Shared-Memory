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


int no_of_token;
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

int cmd_tokenise(char *token_str, char **cmd_str)
{
  int y = 0;

  char *token = strtok(token_str," /t/n");
  while(token!=NULL)
  {
    cmd_str[y] = token;
    printf("%s\n",cmd_str[y]);
    y++;
    token = strtok(NULL," /t/n");
  }

return y;

}
int tokenise(char* cmdline, char **tokens)
{
  int x = 0;
  
  char * token = strtok_r(cmdline,";",&ptr1);

  while(token!=NULL)
  {
    tokens[x] = token;
    x++;
//    no_of_token = 1;
    token = strtok_r(NULL,";",&ptr1);
  }


  return x;
}

int other_cmds(char **tokens)
{

  pid_t pid, wpid,cmd_pid;
  int status;

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
  }
  else // Parent process
  {

    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));


  }
  return 1;

}

char *space_delim(char *str)
{
  char *end;
long long int l;
  while(isspace(*str))
    str++;

  if(*str == 0) 
  {
    return str;
  }
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) 
    end--;

  *(end+1) = 0;
return str;
}


int main()
{
  char Hostname[10000];
  char *username;
  char home[10000];
  char cwd[10000];
  int i,j,k;
  char *cmd_line = (char*)malloc(10000*sizeof(char));  
  int flag=0;
  int err_no;
  char **tokens = (char**)malloc(10000*sizeof(char*));//contains tokens 
  char **tokens1 = (char**)malloc(10000*sizeof(char*));//contains tokens 
  char dummy[10000];
  char *str = (char*)malloc(10000*sizeof(char));

  //char **cmd_str = (char**)malloc(10000*sizeof(char*));//contains each token as delim(" /t/n") separated
 int p,q,r;

  username=getenv("USER");
  gethostname(Hostname,10000);

  getcwd(home,10000);

  strcpy(cwd,home);
  modify_cwd(cwd,home);  

  while(1)
  {
    print(username,Hostname,cwd);    

for(p=0;p<10000;p++)
  tokens[p] = '\0';


    fgets(cmd_line,10000,stdin);

i = tokenise(cmd_line,tokens);

    for(k=0;k<i;k++)
    { 

      str = space_delim(tokens[k]);
    
      strcpy(tokens[k],str);
    
    }

    for(k=0;k<i;k++)
    {
   //    char* temp_str = strdup(tokens[k]);

  char **cmd_str = (char**)malloc(10000*sizeof(char*));//contains each token as delim(" /t/n") separated
    
for(p=0;p<10000;p++)
cmd_str[p] = '\0';  


q = cmd_tokenise(strdup(tokens[k]),cmd_str);

  if(strncmp(tokens[k],"cd\0",2)==0)
      {
       
//        cmd_tokenise(tokens[k],cmd_str);          
            
        if(cmd_str[1] == NULL || strcmp(cmd_str[1], "~\0") == 0 || strcmp(cmd_str[1], "~/\0") == 0) 
        {                
          chdir(home);
          strcpy(cwd,home);
          modify_cwd(cwd,home);
        }
        else if(chdir(cmd_str[1]) == 0) 
        {

          getcwd(cwd,10000);
          modify_cwd(cwd,home);

        }
        else {
          perror("Error");
        }

      }

      if(strncmp(tokens[k],"exit\0",4)==0)
      {
        flag=1;
        break;

      }


      else if(strncmp(tokens[k],"pwd\0",3)==0)
      {

        getcwd(dummy,10000);

        printf("%s\n",dummy);
      }

      else if(strncmp(tokens[k],"echo\0",4)==0)
      {
  //     cmd_tokenise(tokens[k],cmd_str);          
        printf("%s\n",cmd_str[1]);
      }

      else
      {
    //   cmd_tokenise(tokens[k],cmd_str);          
        err_no= other_cmds(cmd_str);

        if(err_no==0)
          exit(-1);

      }

      free(cmd_str);
    }

    if(flag==1 || err_no==0)
      break;


  }

  return 0;
}
