# ADAK - Distributed Keying Distribution

## Build Instructions
1) `mkdir build`
2) `cd build`
3) `cmake .. -DBUILD_TESTING=0`
    - **Note**: The `-DBUILD_TESTING=0` flag disables building unit tests. Building the unittests adds a decent bit of
    time to a clean compile and is not currently working.
4) `make -j4 -DBUILD_TESTING=0`
5) The build will be completed at this point, executable is `bin/`

NOTE: a script is included in the scripts folder in this repository but is not guaranteed to work on every system. It is an example script that is used with the development environment. 

## Configuration for simulation
The project is configurable via the Config class (`include/config.hpp`, `src/config.cpp`) via JSON. The simulation will
look in the same directory as the executable for `config.json`.

There are 3 connection modes. To change the connection mode, alter connModeStr in the config file. The three options for the connection mode are: `"full"`, `"partial"`, and `"single"`.
* Full connection mode: creates a full connected graph out of the nodes.
* Partial connection mode: randomly connected graph (dependent on Config.visiblePeer's percentage connected).
* Single connection mode: randomly connected MST (one connection only per node determined on creation).

### Example `config.json`
```json
{
    "numNodes": 10,
    "connModeStr": "full",
    "csvOutputPath": "out.csv",
    "creationRate": 1.0,
    "networkScale": 0.3
}
```
## Configuration for visual analytics
The visualizations created by the project require Python as well as the `pandas` and `plotly` modules to be installed. This assumes that PIP has been installed to manage Python packages.
1) `pip install pandas`
2) `pip install plotly==4.5.4`

Alternatively, if pip does not work well on your machine, here are alternate instructions. 
1) `sudo-apt install python3-pandas`

For more information on [plotyl](https://plot.ly/python/getting-started/).
For more information on [pandas](https://pandas.pydata.org/pandas-docs/stable/getting_started/install.html).

## Configuration for User Interface 
(also requires the configuration for visual analytics for the visual analytics to work properly through the UI)
The User Interface created requires Python as well as the tkinter interface and sshpass. To install tkinter and sshpass on linux use: 
1) `sudo apt-get install python3-tk`
2) `sudo apt install sshpass`

## User Interface Start 
navigate to the client directory in DistKeySim and run GUI.py with python3 using the parameters: 
1) server ip 
2) username
3) password
4) FULL DistKeySim folder location on the server (including the DistKeySim folder)

## User Interface Parameters
Different parameters in the simulation are controlled by inputs from the UI. Below is a description of each of those inputs as well as how they will be used. Several of these inputs are used as parameters into statistical distributions from which we sample. We describe the distributions and their parameters.
 
 * **Visible Peers \(connected % \)** : *This input field only influences the simulation when the connection mode is set to `"partial"` in the config file.* When a new node is added, this is the \% chance that the new node is connected to another node. For example, if you desired there to be a 75\% chance that a new node is connected to another node, then you would enter 75.
 * **Lambda 3 \(time between creating objects\)** : Expected time for nodes between creating objects; how expected much time passes for nodes between creating one object and creating the next object \(&lambda;<sub>3</sub>\). Upon creation of each node, a time in seconds is randomly generated by sampling from a distribution that is geom\(&lambda;<sub>3</sub>\/TIMESTEP\). This time represents the amount of time this node takes between creating objects.
 * **Chunkiness \(\# of keys to shift\)** : The number of keys that is sent when another node requests more keyspace. For example, if a value of 10 is entered, then every time a node requests more keys, 10 keys are sent to that node.  
 * **Heartbeat Period** : Distance in time in seconds between two consecutive hearbeats. A heartbeat is a message that is broadcasted to all connected nodes to indicate that a particular node is online and connected. A heartbeat is different from a time step in that a time step is a unit of time passage in the simulation. In this simulation a real world clock is used with each time step representing a second of "real-time" passing.

In the simulation, nodes are randomly selected to disconnect and reconnect. The disconnect/reconnect times are randomly generated from a geometric distribution, and the disconnect/reconnect times are measured in time steps (each time step represents a second of "real time" passing; currently the time step is controlled by the TIMESTEP variable which equals 1). Nodes that are selected to disconnect/reconnect are handed a message that describes when to disconnect and then subsequently reconnect.

   + **Lambda 1 (time offline)** : Expected time for nodes to go from online to offline in (real world) seconds \(&lambda; <sub>1</sub>\). For each selected node, a time in seconds is randomly generated by sampling from a distribution that is geom\(&lambda;<sub>1</sub>\/TIMESTEP\). This value represents in how many seconds from the current time the node will disconnect. For example, if a value of 10.5 is randomly generated from this distribution, then 10.5 seconds from the current time, the node will go offline.
   + **Lambda 2 (time online)** : Expected time for nodes to go from offline back to online in (real world) seconds \(&lambda;<sub>2</sub>\). For each selected node, a time in seconds is randomly generated by sampling from a distribution that is geom(&lambda;<sub>2</sub>\/TIMESTEP). This value represents the total amount of time the node will remain disconnected. For example, if a value of 10.5 is randomly generated from this distribution, then 10.5 seconds from the disconnect time, the node will come back online.
 
 ## Visualizations
 There were several different visualizations with which we experiemented. These are run by two different python scripts (DataVis.py, DataVis2.py) in the project. Below, by file, are listed descriptions of the different visualizations. Both run on the provided data and can take up to several minutes to display.
 
 ### DataVis.py
 Running this script will result in three pages opening in the default browser. The three analytic graphs that are displayed are described below. Note: for large datasets, this script takes an ***extremely long*** amount of time to run, and while it offers some insight, the density of nodes can render it largely indeciperable.
 
 It's important to know what you're looking at when analyzing the analytics that are produced. For each graph, hovering over a point displays a box with exact data about that node.
 1. **Node consumption and keyspace size over time**: This graph displays how the consumption and keyspace size vary for each time step over the course of the simulation. Each node is a different color. The size of each point varies with the size of the keyspace; the larger the point, the larger the keyspace of that node at that time step. The y-value of the point (how high it is relative to the y-axis) corresponds to the *consumption* of the node at that time step. The greater the y-value, the more objects created; the smaller the y-value, the fewer the objects created.
    + Information displayed by hovering => In the box: The time step, consumption, total keys in the keyspace; Next to the box (in colored text): ID of the node.
    
    |Good performance indicators | Poor performance indicators |
    |----------------------------|-----------------------------|
    | **A large point high up on the y-axis** | **A large point low down on the y-axis** |
    | This indicates that the node is creating a lot of objects, but it also has lots of keys | This indicates that the node is creating a lot of object, but it does not have keys and might be at risk of running out. |
    | **A small point lown on the y-axis** | **A small point highup on the y-axis** |
    | This indicates that the node is not creating many object, and it does not have many keys, so it is not at risk of running out | This means that the node is creating many objects, but it does not have many keys, so it is at risk of running out. |
    
    + When looking at the graph, there is healthy pattern for node behavior that should be ocurring. A node with high consumption should retain its size, or should get gradually smaller and then return to its previous size or larger. This indicates that as it continues to allocate keys to the objects that it is creating, it is given more keys. A node with low consumption should retain its size without running out if it is small; if it is large relative to its consumption, then it should get smaller as it gives keys to other nodes that have higher consumption rate. At no point should any of the points completely disappear (run out of keys).
    
 2. **Node consumption and sharing over time**: This graph displays how the consumption and sharing vary for each time step over the course of the simulation. Each node is a different color. The size of each point varies with the amount of keys that were shared by that node at that time step; the larger the point, the more keys the node gave to other nodes. The y-value of the point (how high it is relative to the y-axis) corresponds to the *consumption* of the node at that time step. The greater the y-value, the more objects created; the smaller the y-value, the fewer the objects created.
    + Information displayed by hovering => In the box: The time step, consumption, total number of keys shared/given to other nodes; Next to the box (in colored text): ID of the node.
    
    |Good performance indicators | Poor performance indicators |
    |----------------------------|-----------------------------|
    | **A small(er) point high up on the y-axis** | **A large(r) point low down on the y-axis** |
    | This indicates that the node is creating a lot of objects, and it is not sharing (as) many keys, which is what we expect from an active node. | This indicates that the node is creating a lot of objects, but is still sharing many keys; thus, such a node might be at risk of running out. |
    | **A large(r) point lown on the y-axis** | **A large(r) point high up on the y-axis** |
    | This indicates that the node is not creating many objects, and it is sharing more keys with other nodes that ask for more. | This means that the node is creating many objects, and is sharing more keys with other nodes that ask more; thus, such a node might be at risk of running out of keys. |
    + When looking at the graph, there is healthy pattern for node behavior that should be ocurring. A node with high consumption should retain its size, or get smaller if it had a relatively large size. This indicates that such a more active node does not superfluously/excessively give away more keys even though its own demand/need is large. A node with low consumption should retain its size or grow larger; if it is large relative to its consumption, then it should stay the same size or get larger as it gives keys to other nodes that have higher consumption rate. At no point should any of the points completely disappear (run out of keys).
 3. **Local keyspace remaining over time**: This graph displays how the total local keyspace varies for node for each time step over the course of the simulation. Each node is a different color. The main thing to look out for is nodes that reach or near 0. The frequency with which nodes reach this point is a good indicator of how well the algorithm shares keys between nodes. If a large number of nodes frequently grounds, then it might indicate that nodes should share a greater number of keys with each other each time more keys are requested.
 
### DataVis2.py
This script produces two graphs for each node, a histogram and a KDE density plot. 
1. **Histogram** : The x-axis is the number of keys/object; the y-axis represents the frequency that the node had that many object/keys. The axis used is NOT a standardize axis. That is, each histogram has different axis scales/values to make viewing the png as easy as possible. There are three kinds of information that are displayed on the histogram:
    a. Trace 0 (Blue) is the total number of keys this node has.
    b. Trace 1 (Red) is the total consumption for this node; that is, it is the total number of objects that this node created.
    c. Trace 2 (Green) is the total number of keys that this node shared to other nodes.

2. **Density plot**: In many ways, the density plot appears like a smoothed over histogram. However, the KDE density plot is an statistical approximation of the distribution of the plotted entities. The x-axis is the number of objects, and the y-axis is the density. Because it is a statistical approximation of the distribution of data entered, it does allow for the distribution to have negative values; in this context, that doesn't mean anything (because we can't have negative keys), so all negative portions of the plotted distribution are ignored, and the x-axis only displays the portion of the distribution that is greater than or equal to zero. There are three kinds of information displayed on the density plot:
    a. The blue density plot is the total number of keys.
    b. The red density plot is the total consumption for this node.
    c. The green density plot is the total number of keys shared by this node.
    
### A note about the visualizations
All visualizations are based off of the data output by the logging system to the statslLog.csv file. Given that certain parts of the algorithm have not been fully implemented/tested, care should be given when analyzing these data analytics. Use lots and lots of salt when viewing.... Also:
**NOTE: FOR THE HISTOGRAM, EACH NODE APPEARS IN ITS OWN BROWSER WINDOW. THAT MEANS THAT IF THERE ARE 100 NODES IN THE SIMULATION, 100 BROWSER WINDOWS WILL OPEN. BEWARE!!** For the density plots, a window will open, and plots must be viewed one by one. Once you're done viewing the current plot, exit out of a window, and the next plot will appear. There is no way to go back to a previous plot. Given these incoveniences and limitations, we wanted to output these plots as PNGs and display them side-by-side for each node on an HTML page. As a consequence of our desire, we spent the next 4.5 hours trying to download/install/use the necessary dependencies to use the image_write() and savefig() functions. We did our best to install orca using multiple methods (including npm and conda), but alas, we were not able. This would be a good project for someone to work on in the future.

## Branches
We kept master as our stable branch. We worked solely on develop and then merged things that are fully functional to
master. It is not 100% functionality, release candidate level, but they are stable and will compile/run without trouble.

# Class Diagram Link
https://drive.google.com/file/d/1Czub9HZ2K6M838bzI8IzAxKO0jYjQAwj/view?usp=sharing

## Alernative Algorithms

In further development of this project, it may be worthwhile to examine some alternative strategies that can be implemented by extending the ``ADAKStrategy`` class in a new class. Due to time considerations, we did not implement any alternatives to the algorithm given in the specification (aside from the Control strategy). However, we did propose an optimal alternative algorithm. The general idea is described as follows:

In this algorithm the node will look at all of it's peers. It will calculate the average keyspace size that each node should have if we are to reach "equilibrium" (where equilibrium is defined as all the nodes having very close provisioning rates -- which is object creation rate multiplied by long term allocation rate Al -- which is object creation rate over keyspace in a node, otherwise defined as how fast a node is consuming the keyspace it has been given). If the keyspace of the current node is smaller than or equal to that average, the node does nothing. However, if the node's keyspace is larger, it will share its keyspace surplus with it's peers.

The keyspace sharing mentioned above will take place in the following way. First, look through all of the Nodes' peers. For each peer with a keyspace size less than the average, add up the deficit. Second, create an int array of all the peers who were under the average. Third, for each peer under the average, set the respective spot in the array with the quantity (Node surplus * peer deficit/sum of deficit). For each index in that array use a binary expansion to determine how big of blocks to send.

Each node will use this algorithm and it's repeated running will result in the whole network ending in a balanced state. This algorithm will avoid oscillations as well because a node will give away more keyspace than its surplus.

# MEMES
<img src="/memes/Compiler_Errors.jpg" height="400" />
<img src="/memes/Prayer.jpg" height="400" />
<img src="/memes/google_programmer.jpg" height="400" />
<img src="/memes/geek_vs_programmers.jpg" height="400" />
<img src="/memes/memeForLightSys.png" height="400" />
<img src="/memes/meme1.jpg" height="400" />
<img src="/memes/meme2.jpg" height="400" />
<img src="/memes/meme3.png" height="400" />
<img src="/memes/meme4.png" height="400" />                                        
<img src="/memes/meme5.png" height="400" />
<img src="/memes/meme6.jpg" height="400" />
