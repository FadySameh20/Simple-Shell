# Simple-Shell
A simple Unix shell program that processes a user commands.

Procedures:
1. Your command shell should take the user command and its parameter(s), i.e., “ls” and “–l” in this example "ls -l", and convert them into C strings. (Recall that a C string terminates with a null string, i.e., \0.)
2. The command shell should create a child process via fork().
3. The child process passes the C strings, the command and parameter(s) to execvp() .
4. The parent process, i.e., the command shell, should wait, via wait() , for the child process to finish.
5. The command shell gets the next command and repeats the above steps. The command shell terminates itself when the user types exit.

- In case a user wants to execute the command in background (i.e. as a background process), he/she writes & at the end of the command. For example, a user command can be: "Shell >> firefox &"
- You should keep a log file for your shell program such that whenever a child process terminates, the shell program appends the line “Child process was terminated” to the log file. To do this, you have to write a signal handler that appends the line to the log file when the SIGCHLD signal is received.
