//g++ demo.cpp -o PMan
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <signal.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <cstring>

using namespace std;


// Flag used throughout PMan
int status = 0;
int bpid = 0;

// Parse command and args
string getCommand(string);
string getArgs(string);
bool hasArg(string);

// Main Functions of PMan
void bg(string);
void bgKill(string);
void bgStart(string);
void bgStop(string);

void exitPMan();

// Common text display
void commandNotFound(string);
void invalidArg(string);

void invalidProcess(int);


int main() {
    while (true) {
        string input;
        cout << "status: " << status << endl;
        cout << "PMan: > ";
        getline(cin, input);

        string cmd = getCommand(input);
        bool arg = hasArg(input);
        string args = getArgs(input);

        // Match the command
        if (cmd == "bgstart") {
            if (arg) bgStart(args);
            else invalidArg(cmd);
        } else if (cmd == "bgstop") {
            if (arg) bgStop(args);
            else invalidArg(cmd);
        } else if (cmd == "bg") {
            if (arg) bg(args);
            else invalidArg(cmd);
        } else if (cmd == "bgkill") {
            if (arg) bgKill(args);
            else invalidArg(cmd);
        } else if (cmd == "exit") {
            if (!arg) exitPMan();
            else invalidArg(cmd);
        } else commandNotFound(cmd);
    }
}

void commandNotFound(string input) { cout << "PMan: > " << input << " command not found" << endl; }

void invalidArg(string input) { cout << "PMan: > invalid argument supplied for " << input << endl; }

void invalidProcess(int pid) {
    cout << "PMan: > The pid is not a valid process id of " << pid << " background process." << endl;
}

string getCommand(string input) {
    string delimiter = " ";
    int pos = input.find(delimiter);
    return input.substr(0, pos);
}

bool hasArg(string input) {
    string delimiter = " ";
    int pos = input.find(delimiter);
    string args = getArgs(input);
    return args != "" && pos != -1;
}

string getArgs(string input) {
    string delimiter = " ";
    int pos = input.find(delimiter);
    return input.erase(0, pos + delimiter.length());
}

int countArg(string input) {
    int arg = 0;
    for (int i = 0; i < input.length(); i++) {
        if (input[i] == ' ') ++arg;
    }
    if (input[input.length() - 1] == ' ') return arg;
    return arg + 1;
}

bool fileExist(const char *filename) {
    std::ifstream ifile(filename);
    return (bool) ifile;
}

void bg(string input) {
    // Using waitpid() to check child already finished before
    int notRunning = waitpid(bpid, NULL, WNOHANG);
    if (notRunning) status = 0;

    // Check status
    if (status != 0) {
        cout << "PMan: > Error: there is a background process already. " << endl;
        return;
    }

    // Check name is null or not
    if (input == "" || input.length() == 0) {
        cout << "PMan: > Error: Name is null. " << endl;
        return;
    }

    // Fork a child process
    bpid = fork();
    if (bpid < 0) exit(EXIT_FAILURE);
    else if (bpid > 0) {
        // Parent Code
        cout << "PMan: > Background process started with pid " << bpid << endl;
        status = 1;
    } else if (bpid == 0) {
        // Child Code

        // Parse command into array
        // Init Array
        int size = countArg(input);
        char *arg[size + 1];

        // Put substring into array
        string delimiter = " ";
        for (int argCount = 0; argCount < size; argCount++) {
            int pos = input.find(delimiter);
            arg[argCount] = strdup(input.substr(0, pos).c_str());
            input.erase(0, pos + delimiter.length());
        }
        arg[size] = NULL;

        // Execute the command
        if (execvp(arg[0], arg) == -1) {
            // If execute failed
            if (fileExist(arg[0])) {
                cout << "execvp() is failed. Do you mean ./" << arg[0] << "?" << endl;
            } else cout << "execvp() is failed." << endl;
        }
    }
}

void bgKill(string pidStr) {
    // Check status
    if (status == 0) {
        cout << "PMan: > No background process to kill." << endl;
        return;
    }

    // Check the pid is the background pid
    int pid = atoi(pidStr.c_str());
    if (pid == 0 || pid != bpid) return invalidProcess(pid);

    // Kill the process
    kill(pid, SIGTERM);
    cout << "PMan: > Background process with pid " << pid << " was killed." << endl;

    // Reset all flag
    status = 0;
    bpid = 0;
}

void bgStart(string pidStr) {
    // Check status
    if (status != 2) {
        cout << "PMan: > No running background process to start." << endl;
        return;
    }

    // Check the pid is the background pid
    int pid = atoi(pidStr.c_str());
    if (pid == 0 || pid != bpid) return invalidProcess(pid);

    // Start the process
    kill(pid, SIGCONT);
    cout << "PMan: > Background process with pid " << pid << " was started." << endl;

    // Set status flag
    status = 1;
}

void bgStop(string pidStr) {
    // Check status
    if (status != 1) {
        cout << "PMan: > No stopped background process to stop." << endl;
        return;
    }

    // Check the pid is the background pid
    int pid = atoi(pidStr.c_str());
    if (pid == 0 || pid != bpid) return invalidProcess(pid);

    // Stop the process
    kill(pid, SIGSTOP);
    cout << "PMan: > Background process with pid " << pid << " was stopped." << endl;

    // Set status flag
    status = 2;
}

void exitPMan() {
    if (status != 0 && 0 != kill(bpid, SIGKILL)) {
        // Kill Failed.
        cout << "PMan: > kill() is failed." << endl;
    }

    // Exit anyway no matter kill was successful.
    exit(EXIT_SUCCESS);
}