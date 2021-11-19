# Final assignment
## Team 5800

Our repository contains 8 .c files in total.

	- 1 milestone file
 	- 3 experinment files
	- 4 "failed" experiments

### Milestone:

Our milestone file contains the basic program described in the milestone assignment. We have used this as the base for all of our experiments.
___
The first part of our milestone is, to set up the server. The main part is, to set up the TCP socket. In our main function, we start by creating a socket. Next we assign an IP address and a port. Once this is done, we bind the socket to the IP address. Now the socket is set up.
The next part is to start listening for the server. Once the client is found, it should be accepted. Since our client will give us multiple requests, our accept and listen are both in an endless loop. This makes it possible for the server to recieve all requests from the client, no matter how many. Lastly, once the client has been accepted, we call func() is called.
___
All of the treatment of our hash value happens in func(). Since the most basic way to do reverse hashing is by brute forcing, this was the first implementation that we made. We have created a struct, that maked it possible for us, to take all of the 49 bits given by the client, and split it into the parts that we need. That is why, the first thing we do is use a buffer to split the 49 bits into start, end, hash and priority.
We create two variables, one is the result, and one is the counter we will use for our comparison.
For the brute force, we have created a for-loop, that continues from the lowest value of the given interval, to the highest. First we take the number from the counter, and put it through the SHA256 algorithm. This will give us the hash string for that number. Next, we compare each character in the new hash string, to the character from the same index in the request hash string.
If at any point the two characters are not the same, the loop breaks, and the program will continue with the next number.
Once we find a hash that is equal to the one in the request, we set this as the result. We then convert the result to big endian, and write it to the client.

### Experiment 1:

The thought for our first experiment comes from the fact, that there is a chance of repetition in the requests from the client. The idea is, that if we create an array where we put in all the results and their hashes, we can start by looking through this, and then find the repeated results even faster.
We started by making a struct, that contains both the number and the hash of a result. In the begging, we worked with an array of these structs, but after working woth it for a while, we realised that this would take up both unnecessary time and memory. Because of this we changed our strategy, so we use a linked list instead. This way, we have a dynamic list, that we can fill up as the results are found.
___
The difference in this implementation from the milestone is, firstly, that we have moved the comparison to a function on its own. This function is called compareHashes(), and it only returns true, if the guess and the request are equal. We have moved it, because it is used more than once, for each request.
In func() we start by checking through the entire linked list, to see if the request is within the previous results. If this is the case, we change the result, and we also make the resultLock true. The resultLock ensures, that if we have already found the result, we don't need to go through another compa

### Experiment 2:

Experiment 2 was the first experiment in which we successfully implemented parallelism to handle requests. Instead of using threads, which we had unsuccessfully experimented with earlier on, we instead went with forking.
After accepting the data packet, a new process is created through a call to the 'fork()' function. A new process is then created, which handles reading the data packet, cracking the sha encryption, and writing back to the request generator, before terminating.

With this implementation, a new process would start every time a new request was generated. As such, the program solving the hashes would be pipelined, and we saw a large decrease in average solve time for the milestone test.

### Experiment 3:
With this experiment, we wanted to combine the two previous improvements of both having parallelism and a solutions storage. 
The major concern for this was however, that since we were using processes, we could no longer share memory between processes, and writing a process' answer to the solutions list would be difficult. 

After some researched, we learned about piping in C. This would allow for one-way communication between processes through what is essentially a virtual file, which would be written to by one process, and read by another. 
The pipe is established by the function call 'pipe()', in which an int array of size 2 is passed. One of these would be our read end, and the other would be the write end.
In this experiment, we would have the child process write the result number and the corresponding hash as a struct through the pipe. The parent process would then read said result, and store it in a linked list in the parent process. 

We figured that since there will only be a rather small number of requests, this initial check through the linked list could be run serially, with the sha cracking itself being run in parallel.

### Old hash lookup:

### Threading:

### Threading 2:

### Priorities:
