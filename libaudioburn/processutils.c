/*
  libaudioburn - an audio cd burning library
  Copyright (C) 2003  Neill Miller
  This file is part of libaudioburn.

  libaudioburn is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  libaudioburn is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libaudioburn; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.
*/
#include "libaudioburn.h"


int fork_execute(char *path, char *const argv[],
                 int *infd, int *outfd, int *errfd,
                 int flags, pid_t *child_pid)
{
    int ret = 1;
    int fd1[2] = {0,0};
    int fd2[2] = {0,0};
    pid_t ret_pid = (pid_t)0;
    pid_t new_pid = (pid_t)0;

    if (path && argv)
    {
        if (outfd && errfd)
        {
            /* return bad args */
            return 2;
        }

        if (infd && (pipe(fd1) == -1))
        {
            fprintf(stderr,"fork_execute: failed to create pipe1\n");
            return ret;
        }

        if (pipe(fd2) == -1)
        {
            if (infd)
            {
                close(fd1[0]);
                close(fd1[1]);
            }
            fprintf(stderr,"fork_execute: failed to create pipe2\n");
            return ret;
        }

        new_pid = fork();

        if (new_pid != -1)
        {
            if (new_pid != 0)
            {
                /*
                  store the child's pid in the outgoing
                  arg (if available)
                */
                if (child_pid)
                {
                    *child_pid = new_pid;
                }

                /*
                  close parent descriptors and copy out
                  new child descriptors if necessary
                */
                close(fd1[0]);
                close(fd2[1]);

                if (infd)
                {
                    *infd = fd1[1];
                }
                else
                {
                    close(fd1[1]);
                }

                if (outfd)
                {
                    *outfd = fd2[0];
                }
                else if (errfd)
                {
                    *errfd = fd2[0];
                }
                else
                {
                    close(fd2[0]);
                }

                if (flags & CHILD_WAIT)
                {
                    ret_pid = waitpid(new_pid,&ret,0);

                    if (ret_pid != -1)
                    {
                        ret = 0;
                    }
                    else
                    {
                        fprintf(stderr,"fork_execute: waitpid error -- "
                                "returned %d\n",(int)ret_pid);
                    }
                }
                else
                {
                    ret = 0;
                }
            }
            else
            {
                close(fd1[1]);
                close(fd2[0]);

                /* setup routing for child's stdin descriptor */
                if (fd1[0] != STDIN_FILENO)
                {
                    if (dup2(fd1[0],STDIN_FILENO) != STDIN_FILENO)
                    {
                        close(fd2[1]);
                        fprintf(stderr,"Child failed to "
                                "route stdin (errno %d).\n",errno);
                    }
                    close(fd1[0]);
                }

                /* setup routing for child's stdout descriptor */
                if (outfd && (fd2[1] != STDOUT_FILENO))
                {
                    if (dup2(fd2[1],STDOUT_FILENO) != STDOUT_FILENO)
                    {
                        close(fd1[0]);
                        fprintf(stderr,"Child failed to "
                                "route stdout (errno %d).\n",errno);
                    }
                    close(fd2[1]);
                }
                /* setup routing for child's stderr descriptor */
                else if (errfd && (fd2[1] != STDERR_FILENO))
                {
                    if (dup2(fd2[1],STDERR_FILENO) != STDERR_FILENO)
                    {
                        close(fd1[0]);
                        fprintf(stderr,"Child failed to "
                                "route stderr (errno %d).\n",errno);
                    }
                    close(fd2[1]);
                }

                /* launch the specified child process */
                ret = execv(path,argv);
                fprintf(stderr,"fork_execute: execv failed "
                        "(errno %d) on %s\n",errno,path);
            }
        }
        else
        {
            fprintf(stderr,"fork_execute: fork call failed\n");
        }
    }
    return ret;
}

int fork_execute_with_io_callback(char *path, char *const argv[],
                                  fn_callback_t fnptr, int type)
{
    int ret = 0;
    int infd = 0;
    int outfd = 0;
    int errfd = 0;
    int *fd = (int *)0;
    char buf[MAX_BUF_LEN];
    ssize_t nread = 0;
    pid_t child_pid = 0;
    pid_t child2_pid = 0;
    int num_children_pending = 0;

    if (path && argv &&
        ((type == CHILD_IO_ERR) || (type == CHILD_IO_OUT)))
    {
        /* set up descriptor to be either errfd or outfd */
        fd = ((type == CHILD_IO_ERR) ? &errfd : &outfd);

        ret = fork_execute(path,argv,&infd,
                           ((type == CHILD_IO_OUT) ? fd : NULL),
                           ((type == CHILD_IO_ERR) ? fd : NULL),
                           0,&child_pid);

        if (ret == 0)
        {
            /*
              fork again here to wait for the exec'd child
              and to also be able to perform the read/callback
              mechanism.  original parent process will wait
              for both children to exit before exiting
            */
            child2_pid = fork();
            if (child2_pid)
            {
                num_children_pending = 2;

                while(num_children_pending)
                {
                    wait(NULL);
                    --num_children_pending;
                }
                exit(0);
            }

            /* set descriptor to non-blocking */
            fcntl(*fd,F_SETFL,fcntl(*fd,F_GETFD) | O_NONBLOCK);

            while(1)
            {
                /* monitor child's descriptor */
                memset(buf,0,sizeof(MAX_BUF_LEN));
                nread = read(*fd,buf,MAX_BUF_LEN-1);

                if ((nread > 0) && fnptr &&
                    (child_still_running(child_pid)))
                {
                    (*fnptr)(child_pid,buf,(int)nread);
                }
                else
                {
                    if (nread == -1)
                    {
                        if ((errno == EAGAIN) || (errno == EINTR))
                        {
                            usleep(100000);
                            continue;
                        }
                        fprintf(stderr,"fork_execute_with_io_callback: "
                                "read on descriptor failed (errno %d)\n",
                                errno);
                    }

                    /*
                      always call the callback with NULL args
                      when we're finished (this notifies the
                      callback/caller that we're finished)
                    */
                    if (fnptr)
                    {
                        (*fnptr)(child_pid,NULL,0);
                    }
                    break;
                }
            }
            close(infd);
            close(*fd);
        }
    }
    return ret;
}

int which(char *program_name, char *path, int pathlen)
{
    int ret = 1;
    int len = 0;
    int infd = 0;
    int outfd = 0;
    ssize_t nread = 0;
    char buf[MAX_BUF_LEN] = {0};
    char *args[3] = {(char *)0,(char *)0,(char *)0};
    char *ptr = (char *)0;
    pid_t child_pid = 0;

    if (program_name && path)
    {
        args[0] = WHICH_PATH;
        args[1] = program_name;
        args[2] = NULL;

        if (fork_execute(WHICH_PATH,args,&infd,
                         &outfd,NULL,
                         CHILD_WAIT,&child_pid) == 0)
        {
            nread = read(outfd,buf,MAX_BUF_LEN);
            if (nread > 0)
            {
                if (strstr(buf,"which: no") == 0)
                {
                    len = MIN(nread,pathlen);
                    memcpy(path,buf,len);

                    /* strip any carriage returns (if any) */
                    ptr = strstr(path,"\n");
                    if (ptr)
                    {
                        *ptr = '\0';
                    }
                    else
                    {
                        path[len-1] = '\0';
                    }
                    ret = 0;
                }
            }
            close(infd);
            close(outfd);
        }
    }
    return ret;
}

int child_still_running(pid_t child_pid)
{
    int ret = 1;

    if (kill(child_pid,0) == -1)
    {
        ret = 0;
    }
    return ret;
}

int terminate_child_process(pid_t child_pid)
{
    return ((kill(child_pid,SIGTERM) == -1) ? 1 : 0);
}
