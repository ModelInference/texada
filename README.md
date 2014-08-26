## Table of contents
* [Overview](#markdown-header-overview)
* [Installation guide](#markdown-header-installation-guide)
* [Usage guide](#markdown-header-usage-guide)
* [Command line usage screen](#markdown-header-command-line-usage-screen)

# Overview
-----------------------

Texada is a research tool designed for extracting patterns from text data. Specifically, Texada mines temporal relations from totally-ordered sequences of events. To use Texada you need to provide it with at least the following two inputs:

1. A user-defined linear temporal logic ([LTL](http://en.wikipedia.org/wiki/Linear_temporal_logic)) formula whose atomic propositions are variables. We call this a property type.

2. A text file containing multiple traces, each of which is composed of a totally ordered sequence of events. We call this a log.

Texada's output is a set of LTL formulae that are instantiations of the input property type. That is, in an instantiation, the variable atomic propositions are replaced with events from the log. Each formula in the output satisfies all of the input traces in the input log. That is, each formula evaluates to true over each of the traces.

As an example, consider the LTL property type that encodes the "x always followed by y" temporal relation between "x" and "y": G(x -> XF(y)). This property states that whenever an event of type "x" appears in a trace, it must be followed in the same trace by an event of type "y". Given an input log with two traces: "a,b,a,b,c" and "c,a,b,b", then Texada will output the property instantiations G(a -> XF(b)). That is, in the input two traces, "a is always followed by b".


# Installation guide
-----------------------

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
-----------------------

Texada is a command line tool. The following assumes that you were
able to install Texada by following the installation instructions and
are able to execute `texadatest` and `texada` binaries.

## Basic options

The basic command line to invoke Texada looks like this:

    ./texada -m -f 'G(x -> X(F(y))' --log-file path/to/some/logfile.txt

This is the simplest invocation of Texada and it consists of four parts:

A call to run the Texada program,

    ./texada ...

An argument specifying the trace representation type,

    ... -m ...

(-m specifies that the program will internally represent traces as maps. In place of -m, we can input -l or -p, which would tell the program to represent traces as vectors or prefix-trees, respectively. Note that these choices do not effect the output of the program, only its efficiency.)

An argument specifying the property type to mine,

    ... -f 'G(x -> X(F(y))' ...

And lastly, an argument specifying the log file to mine.

    ... --log-file path/to/some/logfile.txt


In order to see these arguments in context, we look at a very basic example.


## Example 1

Suppose we have the following log in the form of a text file located at ~/texada/traces/basic_log.txt.

    a
    c
    d
    --
    b
    d
    c
    d
    --

By default, Texada interprets '--' as the termination of a trace; so the above log would be interpreted as a set of two traces {"a,c,d","b,d,c,d"}. All other lines are interpreted as events, and equivalent lines (i.e. lines whose strings match) are interpreted as instances of the same event.

Now, suppose that we want to find out whether there are events x and y, such that whenever x occurs, y occurs right afterwards. In LTL this can be expressed as 'G(x -> XF(y))'. To determine such events we run Texada as follows:

    ./texada -m -f 'G(x -> X(F(y))' --log-file ~/texada/traces/basic_log.txt

This produces the following output:

    'G(c -> X(F(d))'

which says that c is always followed by d. And looking at our log, we see that indeed this is true.


## Additional options

A more advanced invocation of Texada, using additional options, looks like this (note that this example covers only a subset of all possible options; see "Command line usage screen" below for a full listing):

    ./texada -m -f 'G(x -> X(F(y))' --log-file path/to/some/logfile.txt -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)' -s 'end' -i

Here, in addition to the 4 required arguments seen previously, we see the following additional options:

A set of regular expressions specifying the structure of log lines (see "Regular expression arguments" below for more information),

    ... -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)'

(by the above regular expressions, two log lines 'SUCCESS: open door' and 'FAIL: open door' would be considered instances of the same event 'open door')

A regular expression specifying the lines which are to mark the termination of a trace,

    ... -s 'end'

And lastly, a flag specifying that all lines which fail to match the provided regular expressions are to be ignored.

    ... -i


As before, we look at a simple example to see these arguments in context.


## Example 2


Suppose we have the following log in the form of a text file as before located at ~/texada/traces/structured_log.txt.

    // This is a comment
    SUCCESS: a
    SUCCESS: c
    // This is another commment
    FAIL: d
    end
    SUCCESS: b
    FAIL: d
    SUCCESS: c
    SUCCESS: d
    end

Note that in Texada's default setting, this log would be interpreted as one single, non-terminating trace, with lines like "// This is a comment" interpreted as events.

But suppose we would like Texada to interpret 'end' as marking the termination of a trace and ignore lines beginning with '//'. Futhermore, we would like the program to ignore the "SUCCESS: " and "FAIL: " parts of the log lines and determine a line's event type purely on the final symbols (e.g. we would like to interpret "SUCCESS: d" and "FAIL: d" as instances of the same event "d"). In order to do this, we would append the following options to our command:

    ... -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)' -s 'end' -i

Here, the input

    ... -r 'SUCCESS: (?<ETYPE>.*)' ...

says, in effect, that if a log line matches the regular expression 'SUCCESS: (?<ETYPE>.\*)', its event type is the string captured by the named group (?<ETYPE>.\*).

In full, a command to find events x and y which satisfy the relationship 'x is always followed by y' (or in LTL: 'G(x -> XF(y))') would look like this:

    ./texada -m -f 'G(x -> X(F(y))' --log-file ~/texada/traces/structured_log.txt -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)' -s 'end' -i

This would produce the following output:

    'G(c -> X(F(d))'


## Regular expression arguments


Texada relies on user-supplied regular expressions ([Perl regular expressions](http://www.boost.org/doc/libs/1_55_0/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html)) to parse structured log lines. By default, Texada will treat every single line as an event, equivalent lines (i.e. lines whose strings match) as instances of the same event type, and "--" as partitions between traces. However, in order to parse logs which contain any kind of structure, regular expressions must be provided to determine the event type of a log line.

There are two command line options which require regular expression arguments: 

First, the option -s, which specifies the log lines to be interpreted as partitions between traces, takes along with it a single regular expression, and all lines matching said regular expression will be treated as partitions between traces. 

Second, the option -r takes along with it a list of regular expressions, each specifying the structure of some log line. Unlike option -s, the regex arguments of -r require a capturing group having the name <ETYPE> to be specified. When the program runs, it will go through each line and try to match it with one of these regular expressions; once a match is found, the string captured by the named group <ETYPE> will become the event type of the line.

If a line fails to match any of the provided regular expressions, the program will be unable to go on and abort. In order to ignore these unmatched log lines, -i option can be invoked.


# Command line usage screen
-----------------------

~~~~
 Usage: texada [opt1] ... [optN] log-file
 
 Required options: -f, log-file, one of [-l, -m, -p]
 
 Options:
   -h [ --help ]                print help message
   -f [ --property-type ] arg   property type to mine
   --log-file arg               log file
   -m [ --map-trace ]           use a map trace representation
   -l [ --linear-trace ]        use a linear trace representation
   -p [ --prefix-tree-trace ]   use a prefix tree trace representation
   --pregen-instants            pre-generate all property type instantiations 
                                [default: false, using on-fly-instantiation]
   --allow-same-bindings        allow different formula variables to be bound to
                                the same events [default: false]
   -c [ --config-file ] arg     file containing command line options. Command 
                                line options will override file options.
   -e [ --event ] arg           formula variable name to be interpreted as a 
                                constant event.
   -r [ --regex ] arg           regular expression to parse event types from log
                                [default (?<ETYPE>.*)]
   -s [ --separator_regex ] arg regular expression matching execution separator 
                                lines in the log [default: --]
   -i [ --ignore_nm_lines ]     ignore non-matching lines [default: false]
~~~~