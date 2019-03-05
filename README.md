# CS3103 Process Manager Assignment
## Introduction
This repository is created for `City University HK` course `CS3103 - Operating Systems` programming assignment 1 <br />
All credit goes to: Mark Lai

## Assignment Requirement
Aim to create a text base linux app which manage a single background application <br />
If the user types: `bg foo`, PMan will start the program foo in the background <br />
and print out the process id (pid) of foo. Also give the prompt to accept more commands. <br />
<br />
PMan only allows one program running in the background. <br />
If the background process foo is still running, PMan will not accept any further bg command. <br />
In other words, if there is a background process is still running, when the user types `bg test2` <br />
PMan should print There is a background process still running. <br />


## Getting Started
1. Clone the repository

2. Compile the cpp file
```
$ g++ main.cpp -o PMan
```
3. Start the project
```
$ ./PMan
```
