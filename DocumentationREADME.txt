README

Overall structure of the code:

Simulation:
The overall class is simulation. This class is the largeest abstraction to run everyother part of the simulation.
If you want to test, print, pass parameters here is where you will want to change things.
This also outputs everything to a CSV. This can be change relatively easily to give you the data that you really need.

Main:
All this does is run simulation. This is needed to be done here for the makefile to work with catch2 and tests.

//TODO: COME BACK WHEN ITS SOLID
Network:
All this function does is pass the messages between node.
This keeps the peer to peer functionality that we need yet still has an abstraction from node.

In our Network file we do a heartbeat from each node which goes to each node iteratively.
This gives node data iteratively to each node.
If the node data shows that it needs keyspace then when another node is able to give keyspace it does that.
Also once the node gets keyspace (it know this from and ACK) then it stops sending a "distress" keyspace message and
continues with the heartbeat.

There also is a function in here that allows for a fully connected graph (all-to-all) or partial connected
(all are connected somehow to each other but each node does not connect to all nodes)
This is one tunable parameter that is available through json.

It may be nice to have each node have its own thread but you can implement that if you have time and want a more acurate network.


UUID:
This gives the UUID of the nodes as specified in PDF.
Also it is given in as string of hex data (ex 0229ab4ad067907ad9bb9f1d026f023b).
This way it is in hex for protobuf yet easy to compare and read

EventGen:
An abstract class for random.
This will be used in simulation to iteratively go through all the messages from the nodes.

Random:
Picks random nodes to either use a key or connect a node.
This can be changed easily to make it whatever you would like.
Probably want to add nodes that disconnect or something like that.


Node:
This is Node that has all that the node should have as stated in PDF.
It has a vector of keyspaces to hold all keyspaces and subkeyspaces
And it has nodedata for the last day that will be used for the allocation of nodes.
It also has a list of peers, history of the last day. Plus other functions (I trust you can read functions)

In the function giveKeyspaceToNode. There is a nice formula of (((end-start)/2^B) / 2 ) * B.
This can be changed as this is chunkiness!
All we did with the formula was split keyspace in half.

Nodedata:
This returns all the data statistics for the Node according to the PDF. Use this to give different allocations of keys.

PS there is an "ERROR" that prints sometimes (this usually comes from shortTerm allocation.
This is not actually an error as the program handles it correctly for the nodes and nothing will crash.
This was more of if you see this issue you should probably distribute keys better as it it creating keys faster than it
is getting keys.


Channel:
These are the edges of the nodes each has an auto-incrementing ID and has a to and from as said in PDF.

Keyspace:
This has the variables that start, end, suffix as in the PDF.
Here we can find minKey available and other helpful functions.


Message:
This uses protbuf to create a message object.

Lots of things here protobuf probably was overkill and may cause many issues for you but its working.
Key things:
    1. We would recommend using the virtual machine to remote compile (highly recommend will save you time!!).
    2. If you for some reason don't want to do that you may run into a lot of issues (like us)
        a. if using linux you probably will be good as long as gcc is up to date
        b. if using mac you will probably have to find where it is stored and change the make file accordingly
        c. if using windows (good luck) you need a new compiler mingw-w64


JSON:
You can give imports of JSON if needed and can easily change any parameters as you like in the config file


Other things:
If you search many places in the code we tried to point out where stuff that actually matters for changing the algorithm
to make it more efficient. I would suggest using the word Baylor as we tried to make it easy to find stuff in the code.