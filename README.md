# Final assignment
## Team 5800

Our repository contains 8 .c files in total.

	- 1 milestone file
 	- 3 separate improvements
	- 4 "failed" experiments

Our milestone file fulfills the requirements of the milestone assignment.

We have created 3 improvements for our milestone.
Experiment 1
Experiment 2
Experiment 3 (Combination of 1 and 2)
There are two separate improvements, both are an implementation of their own idea. The third experiment is a combination of the two improvements, into one implementation. This is however not the fastest implementation, and therefore our final program is the one called experiment 2.

Some specific experiments we have done:
Is multithreading faster than multi-processing?
What sorts of pre-processing of data is feasible?
Is it worth the time waste to save previous results?
Is it possible to improve forking, by adding a result list?

Our four “failed” experiments are implementations that are either not working completely, or are not faster than the three first mentioned. These files have been a crucial part of our journey to find our fastest implementation, as they helped us find out what works and what doesn’t, as well as what is worth it in execution time.

### Milestone:

Our milestone file contains the basic program described in the milestone assignment. We have used this as the base for all of our experiments.
___
The first part of our milestone is to set up the server. The main part is to set up the TCP socket. In our main function, we start by creating a socket. Next we assign an IP address and a port. Once this is done, we bind the socket to the IP address. Now the socket is set up.
The next part is to start listening for the server. Once the client is found, it should be accepted. Since our client will give us multiple requests, our accept and listen are both in an endless loop. This makes it possible for the server to recieve all requests from the client, no matter how many. Lastly, once the client has been accepted, we call func() is called.
___
All of the treatment of our hash value happens in func(). Since the most basic way to do reverse hashing is by brute forcing, this was the first implementation that we made. We have created a struct, that maked it possible for us, to take all of the 49 bits given by the client, and split it into the parts that we need. That is why, the first thing we do is use a buffer to split the 49 bits into start, end, hash and priority.
We create two variables, one is the result, and one is the counter we will use for our comparison.
For the brute force, we have created a for-loop that continues from the lowest value of the given interval, to the highest. First we take the number from the counter, and put it through the SHA256 algorithm. This will give us the hash string for that number. Next, we compare each character in the new hash string, to the character from the same index in the request hash string.
If at any point the two characters are not the same, the loop breaks, and the program will continue with the next number.
Once we find a hash that is equal to the one in the request, we set this as the result. We then convert the result to big endian, and write it to the client.

### Experiment 1:

The thought for our first experiment comes from the fact that there is a chance of repetition in the requests from the client. The idea is, that if we create an array where we put in all the results and their hashes, we can start by looking through this, and then find the repeated results even faster.
Some considerations we had was the fact that looking through a list would also take time for each iteration, which would be wasteful if it is not a repeated value. Our assessment was that the time saved for a repeat would be worth it. The only case where it is definitely not worth it, is if there are no repeats.
We started by making a struct that contains both the number and the hash of a result. In the beginning, we worked with an array of these structs, but after working worth it for a while, we realised that this would take up both unnecessary time and memory. Because of this we changed our strategy, so we use a linked list instead. This way, we have a dynamic list, that we can fill up as the results are found.
___
The difference in this implementation from the milestone is, firstly, that we have moved the comparison to a function on its own. This function is called compareHashes(), and it only returns true, if the guess and the request are equal. We have moved it, because it is used more than once, for each request.

In func() we start by checking through the entire linked list of results, to see if the request is within the previous results. If this is the case, we change the result, and we also make the resultLock true. The resultLock ensures that if we have already found the result, we don't need to go through another comparison.
If the request is not a previous result the process is the same as in the milestone. There is however a significant addition after the result is found. Here the result and it’s hash needs to be added to the result list.

We use a push function to add the result. The function takes the node to link to and a result struct as it’s arguments. Within the function we add the data struct to a new node, and return the new node as the current node. We also link the previous node and the new node.
___
With this implementation the overall time saved is not much, but there are cases with repeats, where there is time saved, compared to the milestone. The main thought with this improvement is also, to be able to link it with other improvements.

### Experiment 2:

Experiment 2 was the first experiment in which we successfully implemented parallelism to handle requests. Instead of using threads, which we had unsuccessfully experimented with earlier on, we instead went with forking.
After accepting the data packet, a new process is created through a call to the 'fork()' function. A new process is then created, which handles reading the data packet, cracking the sha encryption, and writing back to the request generator, before terminating.

With this implementation, a new process would start every time a new request was generated. As such, the program solving the hashes would be pipelined, and we saw a large decrease in average solve time for the milestone test.

### Experiment 3:
With this experiment, we wanted to combine the two previous improvements of both having parallelism and a solutions storage. 
The major concern for this was however, that since we were using processes, we could no longer share memory between processes, and writing a process' answer to the solutions list would be difficult. 

After some research, we learned about piping in C. This would allow for one-way communication between processes through what is essentially a virtual file, which would be written to by one process, and read by another. 
The pipe is established by the function call 'pipe()', in which an int array of size 2 is passed. One of these would be our read end, and the other would be the write end.
In this experiment, we would have the child process write the result number and the corresponding hash as a struct through the pipe. The parent process would then read said result, and store it in a linked list in the parent process. 

We figured that since there will only be a rather small number of requests, this initial check through the linked list could be run serially, with the sha cracking itself being run in parallel.

This was faster than our sequential implementation, but was slower than our pure process-implementation. This is due to the fact that piping is a rather inefficient way to pass data between processes, which we would do after every request was solved.

### Old hash lookup:
For this experiment we wanted to eliminate the need to use the SHA256 algorithm for every search for a result. The thought was to save all of the hashes together with their numeric value in a table. Our idea was to save the values as we searched for them, so after a couple of requests we would have most of the values gathered in the array.
___
The implementation once again takes its starting point in the milestone implementation. The only difference is that we have added an array calles hashTable, that contains hash values for all indexes. The length of the array is the same as the length between start and end in the request.
Every time the SHA256 algorithm is run, we add the hash to the hashTable. If the hash already has been found, this is used as the guess instead.
___
This implementation didn’t save us any mentionable time, when we had a low difficulty. Once the difficulty became higher, it was no longer possible to have arrays that long, and the program failed. In theory we should be able to save some time, seeing as we could skip a lot of SHA256 over a large scale. It is not, however, the case in the implementation that we are able to test. Seeing as we are not able to test on the scale where the theory should work, we assume that our implementation of the experiment doesn’t work.

### Threading:
This was our first experiment with multithreading. In this, we made use of the 'pthread.h' library. With this, we would start 5 threads every time a new request was read. The interval between start and end would then be split up into 5 different chunks, which each would be passed into the threads upon creation. The threads would then run their part, and the first thread to find the correct answer would then return their result, and block the other threads at the same time. For this, we experimented both with mutex's to lock, as well as manual bool locks, and ended up on the ladder, as we would figured we could lock and then immediately return the value to the request generator, and then terminate all other threads, as they would be created again upon the next request.

We had difficulties with deadlocks in this iteration, as we had to read from the same input packet, as well as write a possible answer. As such, we frequently experienced wrong outputs, or that no result could be found at all. 
Furthermore, we had misunderstood how packets should be read. At first, we had interpreted that only one request was allowed to be processed at a time. This was the basis for the idea of splitting every interval into chunks for the threads to process. As such, bottlenecks were frequent as resources had to be shared more often. Furthermore, the creation of new threads on every new request wasted resources, and most implementations would run slower than the sequential basic program.

### Threading 2:
This was an attempt to rectify some of the downsides of the first thread implementation. The basis of the experiment was the same, but instead of creating new threads on every new request, we instead started exploring having the threads created before any packet was read. The threads themselves would then read any accepted packets and would then process these. 
At this point, we still returned the values to the parent thread, which would write the answers back to the request generator. As such, we still experienced most of the same issues as the first implementation, as it would deadlock when writing the result, but also because all threads would compete to be the first to get the request from the parent thread.
An actually effective implementation that we never got to experiment with would be to have one thread grab all the requests and load them into shared memory. 
The threads would then work as the above, where they will be initialised in the beginning of the program, and run forever, continuously fetching the requests from the shared memory as they are loaded, and write to the request generator on their own.

This function of grabbing the requests separate is something we will also explore with the priorities experiment.

### Priorities:
Part of the request from the client is a priority. The idea is, that the higher the number, the more important it is. Because of this, we wanted to implement a system, where the most important requests were treated first.
Our idea was that we would collect all of the requests in a linked list, before treating them in any way. Once they were collected we would search the list for the first request with the highest priority.
Once it was found we would treat it the same as in the other implementations, and compare the hashes. In our current implementation, we have only worked with the milestone implementation. 
Lastly, the treated requests should be removed from the linked list, and once it was empty, the program should end.
**We have used the following, as an inspiration for our implementation.**
[Deleting nodes in linked list](https://www.geeksforgeeks.org/linked-list-set-3-deleting-node/)
Previous sites used for linked lists.
___
We found that a problem with our implementation is that there is no way for the server to know, when requests are no longer coming in from the client. This could be mitigated by just having the server run without end, and instead continuously push items from the request generator into 

Unfortunately, due to lack of time, we haven’t had a chance to try and implement these.
___
## Runtimes

####Milestone
Every test was run 3 times. Times are averaged.
Experiments run on an Apple computer with intel chip.
2.3 GHz Quad-Core Intel Core i7
16 GB 1600 MHz DDR3

| #   | Are        | Milestone | Experiment 1 |
| ----|:----------:| ---------:|  -----------:|
| 1   | 7143775	 |1.568.877       |
| 2   | 24345043   |6.320.648  |
| 3   | 24113749   |10.987.591 |
| 4   | 24345043	 |15.666.367 |
| 5   | 2770368  	 |15.533.022 |
| 6   | 28127708   |21.381.878 |
| 7   | 28127708	 |27.395.682 |
| 8   | 15466514   |30.199.002 |
| 9   | 7143775    |30.653.338 |
| 10  | 7909485    |32.611.391 |
| Avg | 7909485    |19.863.844 |

___
## Concluding remarks on experiments
**Is multithreading faster than multi-processing?**
In our implementation multi-processing is much faster than multithreading. In our experience, it is primarily due to the sharing of data in multithreading that gives trouble with the result. In multi-processing they are run as separate processes, and can actually be run in parallel, without needing delays.
**What sorts of pre-processing of data is feasible?**
**Is it worth the time waste to save previous results?**
In some cases it is worth it. In cases where the repetition is high, there is a lot of time to be saved. As can be seen in our runtimes above, we have saved some time, with just 10 requests from the client.
**Is it possible to improve forking, by adding a result list?**
