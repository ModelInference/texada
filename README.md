# Introduction

Texada is a research tool designed for extracting patterns from text data. Specifically, Texada is designed for mining specifications from program executions. To use Texada you need to provide it with at least the following two inputs:

1. A user-defined linear temporal logic ([LTL](http://en.wikipedia.org/wiki/Linear_temporal_logic)) formula whose atomic propositions are variables. We call this a property type.

2. A text file containing multiple program executions, each of which is composed of a totally ordered sequence of events. We call this a log.

Texada's output is a set of LTL formulae that are instantiations of the input property type. That is, in an instantiation, the variable atomic propositions are replaced with events from the log. The output set of formulae all satisfy the input set of executions in the log. That is, each formula evaluates to true over each of the executions.

Texada is developed in C++. The rest of this page describes the steps to get Texada working from a newly cloned repository.

### Directory Structure

      Texada -- top level directory in which repo is stored, contains makefile
         /texada-src -- contains all source code and generated .o, .d files
            /bin  -- created by makefile, contains .o, .d files
               /src -- contains src objects
                  /checkers
                  /instantiation-tools
                  /main 
                  /parsing
               /tests -- contains test objects
            /src -- source code for main project
               /checkers -- code for checkers, which traverse traces and formulae to check that those formulae (instantiated prop. types) hold on trace
               /instantiation-tools -- tools to create all permutations of events for instantiations, to instantiate a SPOT formula
               /main -- contains main and main mining functions
               /parsing -- code for trace parsing 
            /tests -- main project tests

### Required libraries

Texada depends on a few non standard libraries, [Google Test](https://code.google.com/p/googletest/), [SPOT](http://spot.lip6.fr/wiki/GetSpot), and [Boost](http://www.boost.org/). Note that SPOT itself also depends on Boost. 

#### Google Test

Google test can be downloaded [here](http://code.google.com/p/googletest/). To integrate with Texada, the location of Google Test headers and libraries will need to be inputted to uservars.mk (see "Building the project" below), so unpack and build Google Test in a logical place.

#####If using Eclipse:

Google Test can be used in Eclipse if the C/C++ Unit Testing Support item, which can be installed along with the CDT. To install the CDT and Unit Testing Support, add the website http://download.eclipse.org/tools/cdt/releases/youreclipseversion to the Help->Install New Software... dialogue, with youreclipseversion replaced by your eclipse version (e.g. Kepler). The C/C++ Unit Testing Support can then be installed under CDT Optional Features.

To create a test, create a new Runner using 'Run As -> Run Configurations -> C/C++ Unit Test. Choose 'Google Tests Runner' for 'Tests Runner' in the 'C/C++ Testing' tab. In the 'Main' tab, enter texadatest as the C/C++ Application and click 'Use workspace settings' in the 'Build (if required) before launching' section.

#### SPOT

To install SPOT, navigate to the link above and download the tar.gz file. Extract the file with:

    tar -zxvf spot-version.tar.gz

Note that SPOT requires the installation of the [Boost](http://www.boost.org/) libraries. Its full functionality requires Python 2.0+ headers, but if these are not already installed, they can be omitted for the purposes of Texada. 

Navigate to the extracted SPOT folder and run these commands to install SPOT:

    ./configure (or ./configure --disable-python if Python is not installed)
    make
    make check
    make install

TODO: add installation instruction for other OSs if possible

#### Boost

The boost program_options library is used by Texada. You can install Boost [here](http://www.boost.org/doc/libs/1_55_0/more/getting_started/index.html). 

### Cloning project

An installation of mercurial is required. On *nix machines, sudo apt-get install mercurial should be sufficient. If using Eclipse, get the [MercurialEclipse](http://mercurial.selenic.com/wiki/MercurialEclipse) plug-in.

Create a [Bitbucket account](https://bitbucket.org/) and navigate to the [Texada code base](https://bitbucket.org/bestchai/texada). Clicking on "Clone" will give you the correct terminal command to clone, something like:
hg clone https://yourusername@bitbucket.org/bestchai/texada

From there, you can either run this command directly from terminal whilst in the directory you wish to clone to, or go from Eclipse. 

To clone from Eclipse run File->Import->Mercurial->Clone From Existing Mercurial Repository to import the project to your workspace. This process is detailed in the above link to MercurialEclipse.

### Building the project

In the top-level Texada directory, where the makefile exists, create a file called uservars.mk. In this file, provide the correct values for the following four variables:

SPOT_LIB: the location of the spot library
SPOT_INCL: the location of pot header files 
GTEST\_LIB: the location of gtest and gtest_main libraries
GTEST_INCL: the location of gtest header files

For example, uservars.mk might look like

      # User-Specific Variables
      # Specify path to SPOT library
      SPOT_LIB:=/path/to/libspot.a/
      # Specify path to GTest Libraries
      GTEST_LIB:=/path/to/libgtest.a/
      # Specify path to SPOT headers
      SPOT_INCL:=/path/to/spot/headers/
      # Specify path to GTest headers
      GTEST_INCL:=/path/to/gtest/headers/

with all dummy paths replaced by real paths. SPOT\_LIB will probably point to pathtospot/spot/src/.libs/ if that is where libspot.a is. On a ubuntu machine, SPOT\_INCL will likely point to /usr/local/include/spot. The GTest libraries will be located wherever you build GTest and the GTest headers should be in pathtogtest/gtest/include (GTEST_INCL should point there). 

If building from shell, simply type the *make* command in the top-level directory.

If building from Eclipse, right-click on the Project. Follow Properties->C/C++Build. In Builder Settings, disable "Generate Makefiles automatically" and change Build Location to the Texada project. This should be ${workspace_loc:/Texada}, but can be navigated to via clicking the Workspace... option and selecting the Texada project. To build, press ctrl/cmd-b or right-click on the project and click "Build Project"