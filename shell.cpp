/****************
LE2: Basic Shell
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
#include <sys/wait.h> // wait
#include "Tokenizer.h"
#include <iostream>
#include <algorithm>

using namespace std;

int main () {
    // TODO: insert le1 main here
    // lists all the files in the root directory in the long format
    //char* cmd1[] = {(char*) "ls", (char*) "-al", (char*) "/", nullptr};
    // translates all input from lowercase to uppercase
    //char* cmd2[] = {(char*) "tr", (char*) "a-z", (char*) "A-Z", nullptr};

    // TODO: add functionality

    //save original stdin and stdout
    int savestdin = dup(0);
    int savestdout = dup(1);

    string input;
    string temp;
    //create an infinite while loop
    while (true) {
        cout << "Provide commands: ";
        getline(cin, input);

        //if input is exit(case invariant) break out of loop
        temp = input;
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        if (temp == "exit") {
            break;
        }

        //create an instance of tokenizer
        //tokenizer parse the input
        Tokenizer token(input);

        //foreach commands in token.commands
        for (int i = 0; i < (int)(token.commands.size()); i++) {
            // Create pipe
            int pipefd[2];
            pipe(pipefd);

            int pid = fork();
             // Create child to run first command
            if (pid == 0) {
                if (i < (int)(token.commands.size() - 1)) {
                    // In child, redirect output to write end of pipe
                    dup2(pipefd[1], 1);
                }
                // Close the read end of the pipe on the child side.
                close(pipefd[0]);
                // In child, execute the command commands.args
                //we need to convert the given string vector into a char * menaing we need to cstring each and every one of them 
                Command current = *token.commands.at(i);
                //current.args.push_back(nullptr);
                char** argument = new char* [current.args.size() + 1];
                for (int a = 0; a < (int)current.args.size(); a++) {
                    argument[a] = (char*)(current.args.at(a).c_str());
                }
                argument[current.args.size()] = nullptr;
                execvp(argument[0], argument);
            }
            else {
                //redirect the shell(PARENT)'s input to the read end of the pipe
                dup2(pipefd[0], 0);

                //close the write end of the pipe
                close(pipefd[1]);

                //wait until the last command finishes
                if (i == (int)(token.commands.size()-1)) {
                    waitpid(pid, nullptr, 0);
                }
            }
        }

        // Reset the input and output file descriptors of the parent.
        dup2(savestdin, 0);
        dup2(savestdout, 1);
    }
}