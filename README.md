# ADAK - Distributed Keying Distribution

## Build Instructions
1) `mkdir build`
2) `cd build`
3) `cmake .. -DBUILD_TESTING=0`
    - **Note**: The `-DBUILD_TESTING=0` flag disables building unit tests. Building the unittests adds a decent bit of
    time to a clean compile and is not currently working.
4) `make -j4 -DBUILD_TESTING=0`
5) The build will be completed at this point, executable is `bin/`

## Configuration for simulation
The project is configurable via the Config class (`include/config.hpp`, `src/config.cpp`) via JSON. The simulation will
look in the same directory as the executable for `config.json`

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

For more information on [plotyl](https://plot.ly/python/getting-started/).
For more information on [pandas](https://pandas.pydata.org/pandas-docs/stable/getting_started/install.html).

## User Interface
Different parameters in the simulation are controlled by inputs from the UI. Below is a description of each of those inputs as well as how they will be used. Several of these inputs are used as parameters into statistical distributions from which we sample. We describe the distributions and their parameters.
 
 * \(\lambda\)_1
 * {\lambda}_2
 * {\lambda}_3
 *
 
 ## Visualizations
 It's important to know what you're looking at when analyzing the analytics that are produced. 
 1.
 2.
 3.

## Branches
We kept master as our stable branch. We worked solely on develop and then merged things that are fully functional to
master. It is not 100% functionality, release candidate level, but they are stable and will compile/run without trouble.

# Class Diagram Link
https://drive.google.com/file/d/1Czub9HZ2K6M838bzI8IzAxKO0jYjQAwj/view?usp=sharing

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
