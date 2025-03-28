[Specs]
Input:
The program accepts three inputs from the user.
n - maximum number of concurrent instances
t - number of tank players in the queue
h - number of healer players in the queue
d - number of DPS players in the queue
t1 - minimum time before an instance is finished
t2 - maximum time before an instance is finished 


Output:
The output of the program should show the following:
Current status of all available instances
If there is a party in the instance, the status should say "active"
If the instance is empty, the status should say "empty"
At the end of the execution there should be a summary of how many parties an instance have served and total time served.


[Additional Notes]
- Given that I have done this exact project already in CSOPESY (with a partner), I decided to base my code off of that
implementation of ours. The python implementation, directory named "csopesy_mp_dungeon_queueing" is that exact code
that we submitted in CSOPESY. 

Just in case, the repo URL is: https://github.com/miguelflores-0906/csopesy_mp_dungeon_queueing.git

- The code is a bit messy, because I might not be able to clean up all the useless comments and 
obsolete code (by obsolete, I meant implemented at first, but became unused)



[Compilation Steps]
Go into where run.sh or run.bat is located
Do "chmod -x run.sh" if you are using Bash; else ignore this step
Do "./run.sh" if using Bash, "run.bat" if on CMD

~ If you want to compile manually, just go into the LFG folder, then run the rest of the commands in the shell file