# Final assignment
## Team 5800

Our repository contains 8 .c files in total.

	- 1 milestone file
 	- 3 separate improvements
	- 4 "failed" experiments

Our milestone file fulfills the requirements of the milestone assignment.

We have created 3 improvements for our milestone.

	- Experiment 1
	- Experiment 2
	- Experiment 3 (Combination of 1 and 2)

There are two separate improvements, both are an implementation of their own idea. The third experiment is a combination of the two improvements, into one implementation. This is however not the fastest implementation, and therefore our final program is the one called experiment 2. All three experiments yield 100 % reliability.

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
All of the treatment of our hash value happens in func(). Since the most basic way to do reverse hashing is by brute forcing, this was the first implementation that we made. We have created a struct that maked it possible for us to take all of the 49 bits given by the client, and split it into the parts that we need. That is why, the first thing we do is use a buffer to split the 49 bits into start, end, hash and priority.
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

We have noticed that the first time the client requests from the server, it is slower than the second time. If we keep the server running, and then run the client again, the duplicates are found much faster. Since a seed is used, all numbers in the request are considered duplicates the second time, and are then found quickly by the linked list. The issue lies within the fact that in the second run, a duplicate can be found in about 1000 microseconds, whereas the third time takes about 1000000 microseconds.
The reason that the first run is so much slower than the following might be because of errors in our code. We have not been able to determine exactly why this happens, but it would seem that perhaps the linked list "saves" the results only when the request generator finishes.

We believe that this could be improved even further upon, given a bit more time, as there seems to be a bug in our implementation.

### Experiment 2:

Experiment 2 was the first experiment in which we successfully implemented parallelism to handle requests. Instead of using threads, which we had unsuccessfully experimented with earlier on, we instead went with forking.
After accepting the data packet, a new process is created through a call to the 'fork()' function. A new process is then created, which handles reading the data packet, cracking the sha encryption, and writing back to the request generator, before terminating.

With this implementation, a new process would start every time a new request was generated. As such, the program solving the hashes would be pipelined, and we saw a large decrease in average solve time for the milestone test.

*Our inspiration:*

[Forking](https://www.geeksforgeeks.org/fork-system-call/)

[Forking and piping](https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/)

### Experiment 3:
With this experiment, we wanted to combine the two previous improvements of both having parallelism and a solutions storage. 
The major concern for this was however, that since we were using processes, we could no longer share memory between processes, and writing a process' answer to the solutions list would be difficult. 

After some research, we learned about piping in C. This would allow for one-way communication between processes through what is essentially a virtual file, which would be written to by one process, and read by another. 
The pipe is established by the function call 'pipe()', in which an int array of size 2 is passed. One of these would be our read end, and the other would be the write end.
In this experiment, we would have the child process write the result number and the corresponding hash as a struct through the pipe. The parent process would then read said result, and store it in a linked list in the parent process. 

We figured that since there will only be a rather small number of requests, this initial check through the linked list could be run serially, with the sha cracking itself being run in parallel.

This was faster than our sequential implementation, but was slower than our pure process-implementation. This is due to the fact that piping is a rather inefficient way to pass data between processes, which we would do after every request was solved.

*Inspiration:*

[Sharing data between processes](https://stackoverflow.com/questions/49581349/how-to-get-return-value-from-child-process-to-parent)

[Piping](https://www.youtube.com/watch?v=Mqb2dVRe0uo)

All other sites have been mentioned in the previous implentations.

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

*We have used the following, as an inspiration for our implementation.*

[Deleting nodes in linked list](https://www.geeksforgeeks.org/linked-list-set-3-deleting-node/)

Previous sites used for linked lists.
___
We found that a problem with our implementation is that there is no way for the server to know, when requests are no longer coming in from the client. This could be mitigated by just having the server run without end, and instead continuously push items from the request generator into the linked list which would be in shared memory. As mentioned in the comments of threads, the threads would then launch at the beginning of the program, and fetch every time a new request is added. 

Unfortunately, due to lack of time, we haven’t had a chance to try and implement these, and we decided to stick to a more basic process approach.
___
## Runtimes

Every test was run 3 times. Times are averaged.
Experiments run on an Apple computer with intel chip.
2.3 GHz Quad-Core Intel Core i7
16 GB 1600 MHz DDR3

Times are in microseconds
#### Milestone
| #   | num      | 1                | 2                    | 3                       | Avg                   |
|-----|----------|------------------|----------------------|-------------------------|-----------------------|
| 1   | 7143775  |    1,551,569.00  |        1,590,259.00  |           1,564,804.00  |            1,568,877  |
| 2   | 24345043 |    6,049,385.00  |        6,500,363.00  |           6,412,195.00  |            6,320,648  |
| 3   | 24113749 |    10,496,167.0  |      11,292,041.00   |         11,174,566.00   |          10,987,591   |
| 4   | 24345043 |    15,005,545.0  |      16,079,256.00   |         15,914,301.00   |          15,666,367   |
| 5   | 2770368  |    14,855,989.0  |      15,965,040.00   |         15,778,036.00   |          15,533,022   |
| 6   | 28127708 |    20,993,965.0  |      21,655,869.00   |         21,495,801.00   |          21,381,878   |
| 7   | 28127708 |    27,720,908.0  |      27,373,769.00   |         27,092,368.00   |          27,395,682   |
| 8   | 15466514 |    30,518,612.0  |      30,185,383.00   |         29,893,010.00   |          30,199,002   |
| 9   | 7143775  |    31,328,891.0  |      30,470,774.00   |         30,160,349.00   |          30,653,338   |
| 10  | 7909485  |    32,395,767.0  |      32,884,965.00   |         32,553,441.00   |          32,611,391   |
| Avg |          |    19,696,618.0  |      20,049,808.00   |         19,845,106.00   |          19,863,844   |

#### Experiment 1, reset server each time
|     |          | 1                   | 2                   | 3                   | Avg                 |
|-----|----------|---------------------|---------------------|---------------------|---------------------|
| 1   | 7143775  |          1,587,849  |          1,721,409  |          1,632,100  |          1,647,119  |
| 2   | 24345043 |          6,441,447  |          6,769,503  |          6,719,144  |          6,643,365  |
| 3   | 24113749 |        11,238,333   |        11,588,122   |        11,622,634   |        11,483,030   |
| 4   | 24345043 |        10,487,329   |        16,439,920   |        16,512,087   |        14,479,779   |
| 5   | 2770368  |        10,353,987   |        16,353,767   |        16,389,894   |        14,365,883   |
| 6   | 28127708 |        16,046,460   |        21,886,831   |        22,275,696   |        20,069,662   |
| 7   | 28127708 |        15,295,461   |        27,418,146   |        28,044,520   |        23,586,042   |
| 8   | 15466514 |        18,122,936   |        30,128,831   |        30,944,710   |        26,398,826   |
| 9   | 7143775  |        19,190,717   |        30,467,789   |        31,293,357   |        26,983,954   |
| 10  | 7909485  |        18,440,096   |        32,900,470   |        33,731,244   |        28,357,270   |
| Avg |          |        13,364,606   |        20,244,429   |        20,588,453   |        18,065,829   |

#### Experiment 1, same server, new client request
|     |          | 1              | 2    | 3    | Avg             |
|-----|----------|----------------|------|------|-----------------|
| 1   | 7143775  |     1,600,301  | 1046 | 887  |        534,078  |
| 2   | 24345043 |     6,567,282  | 1143 | 1249 |     2,189,891   |
| 3   | 24113749 |    11,404,003  | 1322 | 1312 |     3,802,212   |
| 4   | 24345043 |    16,252,353  | 1091 | 1185 |     5,418,210   |
| 5   | 2770368  |    16,137,653  | 1272 | 1238 |     5,380,054   |
| 6   | 28127708 |    22,006,428  | 1059 | 1460 |     7,336,316   |
| 7   | 28127708 |    27,717,802  | 1506 | 1171 |     9,240,160   |
| 8   | 15466514 |    30,622,548  | 1617 | 1631 |   10,208,599    |
| 9   | 7143775  |    30,987,145  | 1627 | 1281 |   10,330,018    |
| 10  | 7909485  |    33,428,246  | 1395 | 1302 |   11,143,648    |
| Avg |          |    20,329,104  | 1422 | 1396 |     6,777,307   |

#### Experiment 2
|     |          | 1              | 2              | 3              | Avg                |
|-----|----------|----------------|----------------|----------------|--------------------|
| 1   | 7143775  |     1,565,852  |     1,579,071  |     1,607,016  |        1,583,980   |
| 2   | 24345043 |       709,000  |       703,878  |       737,836  |           716,905  |
| 3   | 24113749 |     7,025,832  |     7,208,940  |     7,013,039  |        7,082,604   |
| 4   | 24345043 |     3,730,382  |     3,345,125  |     7,872,151  |        4,982,553   |
| 5   | 2770368  |     8,752,979  |     8,534,757  |     3,623,825  |        6,970,520   |
| 6   | 28127708 |     3,718,976  |     3,619,199  |     3,510,353  |        3,616,176   |
| 7   | 28127708 |     8,836,501  |     8,563,188  |     8,328,334  |        8,576,008   |
| 8   | 15466514 |     6,161,031  |     6,252,400  |     5,915,263  |        6,109,565   |
| 9   | 7143775  |     9,120,935  |     9,014,407  |     8,704,877  |        8,946,740   |
| 10  | 7909485  |     9,621,162  |     8,903,987  |     9,091,287  |        9,205,479   |
| Avg |          |     6,626,848  |     6,493,389  |     6,341,702  |        6,487,313   |

#### Experiment 3, reset server every time
|     |          | 1               | 2               | 3               | Avg                 |
|-----|----------|-----------------|-----------------|-----------------|---------------------|
| 1   | 7143775  |      1,635,481  |      1,732,218  |      1,662,727  |          1,676,809  |
| 2   | 24345043 |      6,551,626  |      6,848,763  |      6,517,740  |          6,639,376  |
| 3   | 24113749 |     11,374,244  |     11,823,454  |     11,289,700  |        11,495,799   |
| 4   | 24345043 |     10,625,231  |     11,073,679  |     10,540,037  |        10,746,316   |
| 5   | 2770368  |     10,513,714  |     11,005,164  |     10,437,663  |        10,652,180   |
| 6   | 28127708 |     16,332,918  |     17,236,748  |     16,209,240  |        16,592,969   |
| 7   | 28127708 |     15,583,399  |     16,487,241  |     15,459,705  |        15,843,448   |
| 8   | 15466514 |     18,463,627  |     19,397,796  |     18,381,468  |        18,747,630   |
| 9   | 7143775  |     18,877,092  |     19,809,693  |     18,680,329  |        19,122,371   |
| 10  | 7909485  |     19,628,690  |     20,564,306  |     19,431,103  |        19,874,700   |
| Avg |          |     13,613,764  |     14,282,782  |     13,512,745  |        13,803,097   |

#### Experiment 4, same server, new client request
|     |          | 1            | 2            | 3        | Avg            |
|-----|----------|--------------|--------------|----------|----------------|
| 1   | 7143775  |   1,560,988  |       1,264  |   1,401  |       521,218  |
| 2   | 24345043 |   6,373,631  |       1,651  |   1,748  |    2,125,677   |
| 3   | 24113749 |  11,156,481  |       1,630  |   1,588  |    3,719,900   |
| 4   | 24345043 |  10,408,562  |       1,340  |   1,689  |    3,470,530   |
| 5   | 2770368  |  10,311,715  |       1,389  |   2,241  |    3,438,448   |
| 6   | 28127708 |  16,007,719  |       1,710  |   1,589  |    5,337,006   |
| 7   | 28127708 |  15,257,634  |       1,760  |   2,028  |    5,087,141   |
| 8   | 15466514 |  18,076,583  |       2,060  |   2,156  |    6,026,933   |
| 9   | 7143775  |  18,378,685  |       1,456  |   2,295  |    6,127,479   |
| 10  | 7909485  |  19,130,044  |       1,336  |   2,440  |    6,377,940   |
| Avg |          |  13,303,567  |       1,724  |   2,092  |    4,435,794   |
 

___
## Concluding remarks on experiments
**1. Is multithreading faster than multi-processing?**

In our implementation multi-processing is much faster than multithreading. In our experience, it is primarily due to the sharing of data in multithreading that gives trouble with the result. In multi-processing they are run as separate processes, and can actually be run in parallel, without needing delays.

**2. What sorts of pre-processing of data is feasible?**

We concluded that storing each previous hash attempt is not feasible. The memory requirement scales poorly and quickly grows to a dataset the size of the search interval. Furthermore, the only performance increase would be in not having to run the sha algorithm.

**3. Is it worth the time waste to save previous results?**

In some cases it is worth it. In cases where the repetition is high, there is a lot of time to be saved. As can be seen in our runtimes above, we have saved some time, with just 10 requests from the client.

**4. Is it possible to improve forking, by adding a result list?**

Having a shared memory pool which would include the result list would in theory provide a performance increase as the more duplicates appears in a session, the faster these would be solved, but the piping which is necessary for interprocess communication is rather inefficient, and in our experiments the solution would run slower than just having the process run without checking the results beforehand.
