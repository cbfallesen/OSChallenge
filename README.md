# Final assignment
## Team 5800

Our repository contains 8 .c files in total.

	- 1 milestone file
 	- 3 experinment files
	- 4 failed experiments

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

### Experiment 2:

### Experiment 3:

### Old hash lookup:

### Threading:

### Threading 2:

### Priorities:
