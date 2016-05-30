#include "sh_lib.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

// Tries the dup2 system call and returns error if failed.
void TryDup2(int old_fd, int new_fd) {
  int result = dup2(old_fd, new_fd);
  if (result < 0) {
    fprintf(stderr, "failed to dup file descriptors: %s.\n",
            strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void SetupRedirection(struct redircmd* cmd) {
  int redirection_fd = 0;
  if (cmd->type == '>') {
    redirection_fd = open(cmd->file, cmd->mode, S_IRWXU);
  } else {
    redirection_fd = open(cmd->file, cmd->mode);
  }
  if (redirection_fd < 0) {
    fprintf(stderr, "failed to open file for redirection: %s.\n",
            strerror(errno));
    exit(EXIT_FAILURE);
  }
  TryDup2(redirection_fd, cmd->fd);
}

// Tries to fork a new process and return the created pid. If the new process
// is not created successfully, prints error to stderr and returns -1.
int TryFork() {
  int pid = fork();
  if (pid == -1) {
    perror("fork() error");
  }
  return pid;
}

void PipeImpl(struct pipecmd* pcmd) {
  int p[2];
  int result = pipe(p);
  if (result == -1) {
    fprintf(stderr, "pipe system call did not complete successfully: %s.\n",
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Child 0 executes pcmd->left.
  if (TryFork() == 0) {
    close(p[0]);
    TryDup2(p[1], STDOUT_FILENO);
    close(p[1]);
    RunCmd(pcmd->left);
  }

  // Child 1 executes pcmd->right.
  if (TryFork() == 0) {
    close(p[1]);
    TryDup2(p[0], STDOUT_FILENO);
    close(p[0]);
    RunCmd(pcmd->right);
  }

  close(p[0]);
  close(p[1]);

  int r;
  wait(&r);
  wait(&r);
  exit(EXIT_SUCCESS);
}

void RunCmd(struct cmd *cmd) {
  int p[2], r;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit(0);
  
  switch(cmd->type){
  default:
    fprintf(stderr, "unknown RunCmd\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(0);
    // Real logic starts from here.
    if (execv(SearchPath(ecmd->argv[0]), ecmd->argv) < 0) {
      fprintf(stderr, "execv returned with error: %s.\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    break;

  case '>':
  case '<':
    rcmd = (struct redircmd*)cmd;
    fprintf(stderr, "redir not implemented\n");
    SetupRedirection(rcmd);
    RunCmd(rcmd->cmd);
    break;

  case '|':
    pcmd = (struct pipecmd*)cmd;
    PipeImpl(pcmd);
    break;
  }    
  exit(0);
}
