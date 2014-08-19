## Table of contents
* [Overview](#markdown-header-overview)
* [Installation guide](#markdown-header-installation-guide)
* [Usage guide](#markdown-header-usage-guide)
* [Command line help screen](#markdown-command-line-usage-screen)

# Overview

Texada is a research tool designed for extracting patterns from text data. Specifically, Texada mines temporal relations from totally-ordered sequences of events. To use Texada you need to provide it with at least the following two inputs:

1. A user-defined linear temporal logic ([LTL](http://en.wikipedia.org/wiki/Linear_temporal_logic)) formula whose atomic propositions are variables. We call this a property type.

2. A text file containing multiple traces, each of which is composed of a totally ordered sequence of events. We call this a log.

Texada's output is a set of LTL formulae that are instantiations of the input property type. That is, in an instantiation, the variable atomic propositions are replaced with events from the log. Each formula in the output satisfies all of the input traces in the input log. That is, each formula evaluates to true over each of the traces.

As an example, consider the LTL property type that encodes the "x always followed by y" temporal relation between "x" and "y": G(x -> XF(y)). This property states that whenever an event of type "x" appears in a trace, it must be followed in the same trace by an event of type "y". Given an input log with two traces: "a,b,a,b,c" and "c,a,b,b", then Texada will output the property instantiations G(a -> XF(b)). That is, in the input two traces, "a is always followed by b".


# Installation guide

Texada is developed in C++. This section describes the steps to get
Texada working from a freshly cloned repository. Texada has been
tested on Linux and OSX.

### Directory Structure

* Texada -- top level directory in which repo is stored, contains makefile
    * /texada-src -- contains all source code and generated .o, .d files
        * /bin  -- created by makefile, contains .o, .d files
        * /src -- source code for main project
            * /checkers -- code for checkers, which traverse traces and formulae to check that those formulae (instantiated prop. types) hold on trace
            * /instantiation-tools -- tools to create all permutations of events for instantiations, to instantiate a SPOT formula
            * /main -- contains main and main mining functions
            * /parsing -- code for trace parsing 
        * /tests -- main project tests


### Required libraries

Texada depends on a few non-standard libraries, [Google Test](https://code.google.com/p/googletest/), [SPOT](http://spot.lip6.fr/wiki/GetSpot), and [Boost](http://www.boost.org/). Note that SPOT itself also depends on Boost. 

#### Google Test

Google Test can be downloaded [here](http://code.google.com/p/googletest/). Follow the instructions on the website to build it. 

To integrate with Texada, the location of Google Test headers and libraries will need to be inputted to uservars.mk (see "Building the project" below), so make note of where Google Test is being extracted and built.

##### If using Eclipse:

Google Test can be used in Eclipse if the C/C++ Unit Testing Support item has been installed, which can be done along with the CDT installation. To install the CDT and Unit Testing Support, add the website http://download.eclipse.org/tools/cdt/releases/youreclipseversion to the Help->Install New Software... dialogue, with youreclipseversion replaced by your eclipse version (e.g. Kepler). The C/C++ Unit Testing Support can then be installed under CDT Optional Features.

To create a test, create a new Runner using 'Run As -> Run Configurations -> C/C++ Unit Test. Choose 'Google Tests Runner' for 'Tests Runner' in the 'C/C++ Testing' tab. In the 'Main' tab, enter texadatest as the C/C++ Application and click 'Use workspace settings' in the 'Build (if required) before launching' section.

#### Boost

Boost can be downloaded [here](http://www.boost.org/doc/libs/1_56_0/more/getting_started/unix-variants.html) (for *nix machines) or [here](http://www.boost.org/doc/libs/1_56_0/more/getting_started/windows.html) (for Windows machines). Follow the instructions on the website to install it. Note that Texada depends on the non-header library, ProgramOptions, which needs to be built separately (see "5. Prepare to Use a Boost Library Binary" on the Boost website).

To integrate with Texada, the location of Boost headers will need to be inputted to uservars.mk (see "Building the project" below), so make note of where it is being extracted.

Texada uses boost_program_options and not boost_program_options-mt. An older installation of Boost may only have the -mt version. 

#### SPOT

SPOT can be downloaded [here](http://spot.lip6.fr/wiki/GetSpot). Extract the file with:

    tar -zxvf spot-version.tar.gz

To integrate with Texada, the location of SPOT headers and library will need to be inputted to uservars.mk (see "Building the project" below), so make note of where it is being extracted. 

Note that SPOT requires the installation of the [Boost](http://www.boost.org/) libraries. Its full functionality requires Python 2.0+ headers, but if these are not already installed, they can be omitted for the purposes of Texada. 

To install SPOT, navigate to the extracted SPOT folder and run these commands:

    ./configure (or ./configure --disable-python if Python is not installed)
    make
    make check
    make install

make install should place library files in a logical place for your OS. 

### Cloning project

An installation of mercurial is required. On *nix machines, sudo apt-get install mercurial should be sufficient. If using Eclipse, get the [MercurialEclipse](http://mercurial.selenic.com/wiki/MercurialEclipse) plug-in.

Create a [Bitbucket account](https://bitbucket.org/) and navigate to the [Texada code base](https://bitbucket.org/bestchai/texada). Clicking on "Clone" will give you the correct terminal command to clone, something like:
hg clone https://yourusername@bitbucket.org/bestchai/texada

From there, you can either run this command directly from terminal whilst in the directory you wish to clone to, or go from Eclipse. 

To clone from Eclipse run File->Import->Mercurial->Clone From Existing Mercurial Repository to import the project to your workspace. This process is detailed in the above link to MercurialEclipse.

### Building the project

To run tests and the runtime comparision option from command line, you must define the environment variable TEXADA_HOME to the location of the top level directory in which the makefile, texada-src/ and traces/ is found. The location should begin with a forward slash and end without one, as in */path/to/texada/home*. 

In the top-level Texada directory, where the makefile exists, there is a file called uservars.mk.example. Rename it to uservars.mk.  In this file, provide the correct values for the following five variables:

    SPOT_LIB: the location of the spot library
    SPOT_INCL: the location of spot header files 
    GTEST_LIB: the location of gtest and gtest_main libraries
    GTEST_INCL: the location of google test header files
    BOOST_INCL: the location of Boost header files

SPOT\_LIB will probably point to /pathtospot/spot/src/.libs if that is where libspot.a is. On a ubuntu machine, SPOT\_INCL will likely point to /usr/local/include/spot. The GTest libraries will be located wherever you built GTest and the GTest headers should be in /pathtogtest/gtest/include (GTEST_INCL should point there). BOOST_INCL may point to /usr/include/boost on a Ubunutu Machine; otherwise the Boost header files can be found in /pathtoboost/boost_1_56_5/boost.

If building from shell, simply type the *make* command in the top-level directory.

##### If using Eclipse:

There is a file included in the repo called .cproject.example. Remove the .example; if this overrides a generated eclipse file, let it. You will need to fill in the location of SPOT_LIB, SPOT_INCL, GTEST_LIB and GTEST_INCL as above, either by directly editing the .cproject.example file and entering these locations in the fields currently marked by $SPOT_LIB, $SPOT_INCL, $GTEST_LIB and $GTEST_INCL, or by right-clicking the project, following 'Properties -> C/C++ Build' and entering the locations in the 'Build Variables' tab. For the second option, make sure you have already replaced the original .cproject with the one provided; the variables should already be in the Build Variables section.

Check that in 'Properties -> C/C++Build -> Builder Settings', 'Generate Makefiles automatically' is disabled. and Build Location is the Texada project. This should be ${workspace_loc:/Texada}, but can be navigated to via clicking the Workspace... option and selecting the Texada project. 

To build, press ctrl/cmd-b or right-click on the project and click "Build Project"


# Usage guide

Texada is a command line tool. The following assumes that you were
able to install Texada by following the installation instructions and
are able to execute `texadatest` and `texada` binaries.

## Example usage

Here is an example input/output:

Input: `./texada -m -c ./traces/perracotta-type-traces/alternating/args.txt ./traces/perracotta-type-traces/alternating/trace.txt`

Where args.txt contains:
`-f '(!y W x) & G((x -> X(!x U y))&(y -> X(!y W x)))'`

Output: `(!b W a) & G((a -> X(!a U b)) & (b -> X(!b W a)))`

*TODO: expand*


# Command line usage help screen

> Usage: texada [opt1] ... [optN] log-file
> 
> Required options: -f, log-file, one of [-l, -m, -p]
> 
> Options:
>   -h [ --help ]                print help message
>   -f [ --property-type ] arg   property type to mine
>   --log-file arg               log file
>   -m [ --map-trace ]           use a map trace representation
>   -l [ --linear-trace ]        use a linear trace representation
>   -p [ --prefix-tree-trace ]   use a prefix tree trace representation
>   --pregen-instants            pre-generate all property type instantiations 
>                                [default: false, using on-fly-instantiation]
>   --allow-same-bindings        allow different formula variables to be bound to
>                                the same events [default: false]
>   -c [ --config-file ] arg     file containing command line options. Command 
>                                line options will override file options.
>   -e [ --event ] arg           formula variable name to be interpreted as a 
>                                constant event.
>   -r [ --regex ] arg           regular expression to parse event types from log
>                                [default (?<ETYPE>.*)]
>   -s [ --separator_regex ] arg regular expression matching execution separator 
>                                lines in the log [default: --]
>   -i [ --ignore_nm_lines ]     ignore non-matching lines [default: false]
