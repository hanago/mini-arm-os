#include "shell.h"

#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include "threads.h"
#include "string.h"
#include "usart.h"
#include "asm.h"

#include "romfs.h"

/** Copy from rtenv+ **/
#define MAX_CMDNAME  19
#define MAX_DESCRIBE 20

/* Enumeration for command types. */
enum {
  CMD_FIBONACCI = 0,
  CMD_FIBONACCITHREAD,
  CMD_LS,
  CMD_CD,
  CMD_PWD,
  CMD_VIEW,
  CMD_GREETING,
  CMD_EXIT,
  CMD_HELP,
  CMD_COUNT
} CMD_TYPE;

/* Structure for command handler. */
typedef struct {
  char cmd[MAX_CMDNAME + 1];
  void (*func)(int, char**);
  char describe[MAX_DESCRIBE + 1];
} hcmd_entry;

const hcmd_entry cmd_data[CMD_COUNT] = {
  [CMD_FIBONACCI] = {.cmd = "fibonacci", .func = fibonacci_shell, .describe=" %d"},
  [CMD_FIBONACCITHREAD] = {.cmd = "fibonacciThread", .func = fibonacciThread_shell,  .describe=" %d"},
  [CMD_LS] = {.cmd = "ls", .func = ls_shell, .describe=""},
  [CMD_CD] = {.cmd = "cd", .func = cd_shell, .describe=" %path"},
  [CMD_PWD] = {.cmd = "pwd", .func = pwd_shell, .describe=""},
  [CMD_VIEW] = {.cmd = "view", .func = view_shell, .describe=" %path"},
  [CMD_GREETING] = {.cmd = "greeting", .func = greeting_shell}, 
  [CMD_EXIT] = {.cmd = "exit", .func = exit_shell},
  [CMD_HELP] = {.cmd = "help", .func = help_shell},
};

void simple_shell()
{
  char buffer[100];
  char *argv[5];
  int argc, i;

  if(init_romfs()){
    print_str("[init]ROMFS open\n");
    char* path = "/tmp/min-os";
    if(findnode_romfs(path)){
      print_str("find ");print_str(path);print_str("\n");
      romf* fp;
      fp = fopen_romfs("/init");
      if(fp){
        print_str("open init\n");
        print_c(fread_c_romfs(fp));
        print_str(fread_str_romfs(fp));
        print_str("\n");
        fclose_romfs(fp);
      }
     show_dirlist_romfs(path);
   }
  }

  while(1){
    /* Get user input */
    *(argv[0]) = '\0';
    print_str("$ ");
    get_str(buffer, 99);

    /* Get arguments. */
    argv[0] = cmdtok(buffer);
    argc = 1;
    do{
      argv[argc] = cmdtok(NULL);
      if(!(argv[argc]) || argc > 5)
        break;
      argc++;
    }while(1);

    /* Decode and execute command. */
    if(*(argv[0]) == '\0')
      continue;

    for(i = 0; i < CMD_COUNT; i++)
      if (strcmp(argv[0], cmd_data[i].cmd) == 0){
        cmd_data[i].func(argc, argv);
        break;
      }
    
    if(i == CMD_COUNT){
      print_str(argv[0]);
      print_str(":Command not found\n");
    }
  }
}

/* Privative function: Shell commands */
/* Homework7 */
void fibonacci_shell(int argc, char* argv[]){
  int result = fibonacci(str2int(argv[1]));
  print_str("fibonacci(");
  print_str(argv[1]);
  print_str(") = ");
  print_int(result);
  print_c('\n');
}

void fibonacciThread(void * userdata){
  int result = fibonacci(str2int(userdata));
  print_str("fibonacci(");
  print_str(userdata);
  print_str(") = ");
  print_int(result);
  print_c('\n');
}
void fibonacciThread_shell(int argc, char* argv[]){
  thread_create(fibonacciThread, (void *) argv[1]);
}

/* Homework8 */ //Still not support relative addres.
#define MAX_PATH 50
static char nowPath[MAX_PATH] = "/.";
void ls_shell(int argc, char* argv[]){
  show_dirlist_romfs(nowPath);
}

void pwd_shell(int argc, char* argv[]){
  print_str(nowPath);
  print_c('\n');
}

void cd_shell(int argc, char* argv[]){
  int i;
  if(argc > 2){
    print_str("Invalid commands\n");
    return;
  }

  for(i=0; *(argv[1]+i) && i < 50;i++)
    *(nowPath+i) = *(argv[1]+i);
  *(nowPath+i) = '\0';
}

void view_shell(int argc, char* argv[]){
    if(argc > 2){
      print_str("Invalid commands\n");
      return;
    }

    romf* fp = fopen_romfs(argv[1]);
    if(!fp){
      print_str("Can not open the file\n");
      return;
    }

    char* ptr = NULL;
    do{
      ptr = fread_str_romfs(fp);
      print_str(ptr);
    }while(*ptr);
    print_c('\n');
}

/* Other commands */
void greeting_shell(int argc, char* argv[]){
  print_str("Hello, i'm simple shell!\n");
}

void exit_shell(int argc, char* argv[]){
  print_str("Press \"Ctrl + a\" and then \"x\" to terminate qemu.\n");
}

void help_shell(int argc, char* argv[]){
  int i;
  for(i = 0; i < CMD_COUNT; i++){
    print_str(cmd_data[i].cmd);
    print_str(cmd_data[i].describe);
    print_c('\n');
  }
}

