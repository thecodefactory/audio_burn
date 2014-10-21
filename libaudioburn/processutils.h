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
#ifndef __PROCESSUTILS_H
#define __PROCESSUTILS_H

/*
  forks and executes the binary program specified by 'path'
  given the command line arguments stored in the null
  terminated argument list 'argv' (which MUST end with a
  NULL pointer).

  The first arg in argv should also be the path specified
  by 'path'.  The semantics are the same as is used in the
  'execv' system call.

  If 'infd', 'outfd', or 'errfd' are non-NULL, the spawned
  child process' stdin, stdout, or stderr are re-routed
  to them.  'infd' MUST be non-NULL.

  if 'flags' is CHILD_WAIT, this call will not return
  until the child process has terminated.  if this option
  is not used, it is up to the caller to wait on the child
  process for proper process cleanup after termination.
  (i.e. call wait or waitpid)

  'child_pid' will receive the process ID of the spawned
  child if it is non-NULL;

  returns 0 on success; returns 1 on error; returns 2 if the
  arguments are invalid
*/
int fork_execute(char *path, char *const argv[],
                 int *infd, int *outfd, int *errfd,
                 int flags, pid_t *child_pid);

/*
  similar to fork_execute except this method takes a callback
  function pointer and calls it each time the child's stdout
  or stderr descriptors have data.  Only stdout OR stderr can
  be monitored at a time - not both.  Specify one of the other
  using the 'type' argument which can be either CHILD_IO_OUT
  (for stdout) or CHILD_IO_ERR (for stderr).

  The data is read and passed to the callback function in a
  block of up to MAX_BUF_LEN bytes.

  the callback function must be of the following format:

  void function_call(pid_t child_pid, char *buf, int buflen);
  (see declaration notes in constants.h)
*/
int fork_execute_with_io_callback(char *path,
                                  char *const argv[],
                                  fn_callback_t fnptr,
                                  int type);

/*
  outputs the path (up to 'pathlen' bytes) to the program
  specified by 'program_name' in the supplied buffer 'path'
  and returns 0 on success;

  returns 1 and does not modify the supplied buffer 'path'
  on error.
*/
int which(char *program_name, char *path, int pathlen);

/*
  returns 1 if the child process is still executing;
  returns 0 if the child process has terminated
*/
int child_still_running(pid_t child_pid);

/*
  attempts to kill the specified child process.  should
  only be used on process IDs returned through
  fork_execute or fork_execute_with_io_callback.

  returns 0 on success; returns 1 otherwise
*/
int terminate_child_process(pid_t child_pid);

#endif /* __PROCESSUTILS_H */
