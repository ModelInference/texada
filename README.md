[![Screenshot](https://github.com/ModelInference/texada/raw/master/images/play-demo-video.jpg)](https://youtu.be/sNeGZG1LN9U)

A demo showing how to use Texada to validate properties of a solution to the [dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem).


Texada is also described in two papers published at ASE 2015:

 * Main paper: [General LTL Specification Mining](http://ieeexplore.ieee.org/document/7371998/)
 ```
C. Lemieux, D. Park and I. Beschastnikh, General LTL Specification Mining.
2015 30th IEEE/ACM International Conference on Automated Software Engineering (ASE),
Lincoln, NE, 2015, pp. 81-92, doi: 10.1109/ASE.2015.71.
 ```
 Bibtex:
 ```
 @InProceedings{Lemieux2015,
  author    = {C. {Lemieux} and D. {Park} and I. {Beschastnikh}},
  title     = {{General LTL Specification Mining}}, 
  booktitle = {30th IEEE/ACM International Conference on Automated Software Engineering (ASE)}, 
  year      = {2015},
  pages     = {81-92},
}
 ```
 * Tool paper: [Investigating Program Behavior Using the Texada LTL Specifications Miner](http://ieeexplore.ieee.org/document/7372082/)


## Table of contents
* [Overview](#overview)
* [Installation guide](#installation-guide)
* [Web-interface installation guide](#web-interface-installation-guide)
* [Usage guide](#usage-guide)
* [Command line usage screen](#command-line-usage-screen)


# Overview

Texada is a research tool designed for extracting patterns from text data. Specifically, Texada mines temporal relations from totally-ordered sequences of events. To use Texada you need to provide it with at least the following two inputs:

1. A user-defined linear temporal logic ([LTL](http://en.wikipedia.org/wiki/Linear_temporal_logic)) formula whose atomic propositions are variables. We call this a property type.

2. A text file containing multiple traces, each of which is composed of a totally ordered sequence of events. We call this a log.

Texada's output is a set of LTL formulae that are instantiations of the input property type. That is, in an instantiation, the variable atomic propositions are replaced with events from the log. Each formula in the output satisfies all of the input traces in the input log. That is, each formula evaluates to true over each of the traces.

As an example, consider the LTL property type that encodes the "x always followed by y" temporal relation between "x" and "y": G(x -> XF(y)). This property states that whenever an event of type "x" appears in a trace, it must be followed in the same trace by an event of type "y". Given an input log with two traces: "a,b,a,b,c" and "c,a,b,b", then Texada will output the property instantiations G(a -> XF(b)). That is, in the input two traces, "a is always followed by b".

Two version of Texada are currently supported:
* Texada with SPOT 1.2.4-1.2.6 support, which is in the `master` branch of this repository
* Texada with SPOT 2.X support, which is in the `spot-2.x` branch of this repository. Thanks to [Christopher Banks](https://github.com/cjbanks) for this contribution. This version has two key differences:
   - Code in the `texada-src/src/formula` directory is licensed under GPL-3.0.
   - Formulas are output in [SPIN](http://spinroot.com/spin/Man/ltl.html) format; `[](a ->X<>(b))` rather than `G(a ->XF(b))`.

# Installation guide

Texada is developed in C++. This section describes the steps to get
Texada working from a freshly cloned repository. Texada has been
tested on Linux and OSX. 

If you are installing on Ubuntu, we have
compiled all the commands you need to run to get started on [this wiki page](https://github.com/ModelInference/texada/wiki/Automating-Texada-installation-on-Ubuntu).

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

Texada depends on a few non-standard libraries, [Google Test](https://code.google.com/p/googletest/), [SPOT](http://spot.lip6.fr/wiki/GetSpot), [Boost](http://www.boost.org/), and optionally [Z3](https://github.com/Z3Prover/z3). 

#### Google Test

Google Test can be downloaded [here](http://code.google.com/p/googletest/). Follow the instructions on the website to build it. We use gtest version 1.7.0. 

To integrate with Texada, the location of Google Test headers and libraries will need to be inputted to uservars.mk (see "Building the project" below), so make note of where Google Test is being extracted and built.

##### If using Eclipse:

Google Test can be used in Eclipse if the C/C++ Unit Testing Support item has been installed, which can be done along with the CDT installation. To install the CDT and Unit Testing Support, add the website http://download.eclipse.org/tools/cdt/releases/youreclipseversion to the Help->Install New Software... dialogue, with youreclipseversion replaced by your eclipse version (e.g. Kepler). The C/C++ Unit Testing Support can then be installed under CDT Optional Features.

To create a test, create a new Runner using 'Run As -> Run Configurations -> C/C++ Unit Test. Choose 'Google Tests Runner' for 'Tests Runner' in the 'C/C++ Testing' tab. In the 'Main' tab, enter texadatest as the C/C++ Application and click 'Use workspace settings' in the 'Build (if required) before launching' section.

#### Boost

Boost can be downloaded [here](http://www.boost.org/doc/libs/1_56_0/more/getting_started/unix-variants.html) (for *nix machines) or [here](http://www.boost.org/doc/libs/1_56_0/more/getting_started/windows.html) (for Windows machines). Follow the instructions on the website to install it. Note that Texada depends on the non-header libraries, ProgramOptions and regex, which needs to be built separately (see "5. Prepare to Use a Boost Library Binary" on the Boost website). We have used Texada with BOOST versions 1.59.0-1.77.0.

To integrate with Texada, the location of Boost headers will need to be inputted to uservars.mk (see "Building the project" below), so make note of where it is being extracted.

Texada uses boost_program_options and not boost_program_options-mt. An older installation of Boost may only have the -mt version.

#### SPOT

SPOT can be downloaded [here](http://spot.lip6.fr/wiki/GetSpot). Extract the file with:

    tar -zxvf spot-version.tar.gz

Note that:
- the `master` branch is compatible with SPOT 1.2.4-1.2.6, which is also dependent on BOOST. 
- the `spot-2.x` branch is compatible with SPOT versions 2.X. 

To integrate with Texada, the location of SPOT headers and library will need to be inputted to uservars.mk (see "Building the project" below), so make note of where it is being extracted. 

To install SPOT, navigate to the extracted SPOT folder and run these commands:

    ./configure (or ./configure --disable-python if Python is not installed)
    make
    make check
    make install

make install should place library files in a logical place for your OS. 

### Building the project

To run tests and the runtime comparison option from command line, you must define the environment variable TEXADA_HOME to the location of the top level directory in which the makefile, texada-src/ and traces/ is found. The location should begin with a forward slash and end without one, as in */path/to/texada/home*. 

In the top-level Texada directory, where the makefile exists, there is a file called uservars.mk.example. Rename it to uservars.mk.  In this file, provide the correct values for the following five variables:

    SPOT_LIB: the location of the spot library
    SPOT_INCL: the location of spot header files 
    GTEST_LIB: the location of gtest and gtest_main libraries
    GTEST_INCL: the location of google test header files
    BOOST_INCL: the location of Boost header files

SPOT\_LIB will probably point to /pathtospot/spot/src/.libs if that is where libspot.a is. On a ubuntu machine, SPOT\_INCL will likely point to /usr/local/include/spot. The GTest libraries will be located wherever you built GTest and the GTest headers should be in /pathtogtest/gtest/include (GTEST_INCL should point there). BOOST_INCL may point to /usr/include/boost on a Ubunutu Machine; otherwise the Boost header files can be found in /pathtoboost/boost_1_56_5/boost.

If building from shell, simply type the *make* command in the top-level directory. To make Texada with invariant semantics, type *make smt* instead. (Requires Z3)

##### If using Eclipse:

There is a file included in the repo called .cproject.example. Remove the .example; if this overrides a generated eclipse file, let it. You will need to fill in the location of SPOT_LIB, SPOT_INCL, GTEST_LIB and GTEST_INCL as above, either by directly editing the .cproject.example file and entering these locations in the fields currently marked by $SPOT_LIB, $SPOT_INCL, $GTEST_LIB and $GTEST_INCL, or by right-clicking the project, following 'Properties -> C/C++ Build' and entering the locations in the 'Build Variables' tab. For the second option, make sure you have already replaced the original .cproject with the one provided; the variables should already be in the Build Variables section.

Check that in 'Properties -> C/C++Build -> Builder Settings', 'Generate Makefiles automatically' is disabled. and Build Location is the Texada project. This should be ${workspace_loc:/Texada}, but can be navigated to via clicking the Workspace... option and selecting the Texada project. 

To build, press ctrl/cmd-b or right-click on the project and click "Build Project"



# Web-interface Installation guide

Texada comes with a stand-alone web-server that allows you to interact with the command-line tool through a web browser.

To install the web-server you'll need:

* Install the texada binary following instructions above
* NodeJS and related JavaScript dependencies for the web-client
* Go lang for the web-server
 
   Follow the [detailed web-interface installation guide](https://github.com/ModelInference/texada/tree/master/www).


# Usage guide

Texada is a command line tool. The following assumes that you were
able to install Texada by following the installation instructions and
are able to execute `texadatest` and `texada` binaries.

## Basic options

The basic command line to run Texada looks like this:

    ./texada -m -f 'G(x -> X(F(y)))' --log-file logfile.txt

This line consists of four parts:

1. A call to run the Texada binary:

    ./texada ...

2. An argument specifying the trace representation type:

    ... -m ...

    Here ```-m``` specifies that Texada will internally represent each trace as a map. In place of ```-m```, you can also use ```-l``` (represent traces as vectors) or ```-p``` (represent traces as prefix-trees). Note that these choices do not impact the output, only efficiency.)

3. An argument specifying the property type to mine:

    ... -f 'G(x -> X(F(y)))' ...
   
    Use the standard LTL ascii notation for formulas.

4. An argument specifying the text file to mine:

    ... --log-file logfile.txt


To understand these arguments better, we look at a example.


## Example 1

Suppose we have a file ```basic_log.txt`` that contains:

    a
    c
    d
    --
    b
    d
    c
    d

By default, Texada interprets ```--``` as a trace separator; so the above log would be interpreted as a set of two traces ```a,c,d``` and ```b,d,c,d```. All other lines are interpreted as events, and equivalent lines (i.e., lines whose strings are identical) are interpreted as instances of the same event.

Now, suppose that we want to find out whether there are events x and y, such instances of x is always followed by an instance of y. In LTL this can be expressed as G(x -> XF(y)). To determine such pairs of events we run Texada as follows:

    ./texada -m -f 'G(x -> X(F(y)))' --log-file basic_log.txt --no-vacuous-findings

This produces the following output:

    G(c -> X(F(d)))

which says that the event ```c``` is always followed by event ```d```. You can confirm that this is the case by looking at the two traces above.


## Additional options

A more advanced invocation of Texada, using additional options, looks like this (note that this example covers a subset of possible options; see [Command line usage screen](#command-line-usage-screen) below for a full listing):

    ./texada -m -f 'G(x -> X(F(y)))' --log-file logfile.txt -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)' --trace-separator 'end' -i

Here, in addition to the 4 required arguments described previously, there are the following additional options:

* A set of regular expressions specifying the structure of log lines (see "Regular expression arguments" below for more information),

    ... -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)'

    According to this regular expressions, two log lines ```SUCCESS: open door``` and ```FAIL: open door``` would be considered instances of the same event: open door.

* A regular expression specifying a trace terminator:

    ... --trace-separator 'end'

* A flag specifying that all lines which fail to match the provided regular expressions are to be ignored:

    ... -i


As before, we look at a simple example to see these arguments in context.


## Example 2

Suppose we have a file ```structured_log.txt``` with the following contents:

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

Note that with Texada's default parsing setting, this log would be interpreted as one single, non-terminating trace, with lines like ```// This is a comment``` interpreted as events.

Let's say that we want Texada to: (1) interpret 'end' as marking the termination of a trace, (2) ignore lines beginning with '//', (3) ignore the "SUCCESS: " and "FAIL: " parts of the log lines, and (4) determine a line's event type purely on the final symbols (e.g., we want to interpret "SUCCESS: d" and "FAIL: d" as instances of the event "d"). To do this, we would append the following options to our command:

    ... -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)' --trace-separator 'end' -i

Here, the input

    ... -r 'SUCCESS: (?<ETYPE>.*)' ...

says, in effect, that if a log line matches the regular expression 'SUCCESS: (?<ETYPE>.\*)', its event type is the string captured by the named group (?<ETYPE>.\*).

In full, a command to find events x and y which satisfy the relationship 'x is always followed by y' (or in LTL: G(x -> XF(y))) would look like this:

    ./texada -m -f 'G(x -> X(F(y)))' --log-file structured_log.txt -r 'SUCCESS: (?<ETYPE>.*)' 'FAIL: (?<ETYPE>.*)' --trace-separator 'end' -i

This would produce the following output:

    'G(c -> X(F(d)))'


## Example 3

Suppose we have a file ```stat_filter_log.txt``` with the following contents:

    a
    a
    b
    --
    c
    c
    b

Suppose that we were to look for instances of the property type 'G(x -> Xx -> XXy),' read: whenever x occurs twice in a row, y occurs after. With Texada's default configuration (referred to as the vanilla configuration), the following results would be returned:

    G(a -> Xa -> XXb)
    G(c -> Xc -> XXb)
    G(b -> Xb -> XXa)
    G(b -> Xb -> XXc)

However, a few of these results, namely 'G(b -> Xb -> XXa)' and 'G(b -> Xb -> XXc)', are only vacuously true. If we wanted to filter out such results we could specify the following option:

    --no-vacuous-findings

The '--no-vacuous-findings' option in essence specifies Texada to filter out all results with support less than 1. This option can be generalized to filter out results with support less than x, for any positive integer x by using the '--sup-threshold' option as follows:

    --sup-threshold 3

In a similar way, we can relax the confidence threshold of results. By default, the confidence threshold is set to 1, meaning that instantiations must be fully satisfied over the log to be considered valid. We can change this threshold to return instantiations which may not be satisfied one hundred percent of the time, but which is satisfied "most of the time," using the following option:

    --conf-threshold .95

It should be noted that by default, statistical thresholds are enforced over individual traces. This means that if an instantiation fails to satisfy a threshold at any single trace in the log, it will be filtered out. We can however configure Texada to enforce thresholds over the trace-set as a whole (i.e. globally), by specifying the '--use-global-thresholds' flag as follows:

    --use-global-thresholds

This will ensure that an instantiation which is vacuously true in five of six traces in the given log, but which has a support of 3 in the sixth trace will pass through a support threshold of 3.

Lastly, specifying the --print-stats flag will make Texada print out the statistics of each result.


## Example 4

So far, we have seen how Texada is able to process logs of atomic propositional events. By this, we mean that at a given "point of time," there is a single proposition which holds true (i.e. the proposition specifying the event type). However, in certain situations we want to be able process logs in which multiple propositions are associated with each point in time.

Consider for example that we were to trace the values of a program's variables as it executed over time, producing the following log (which we will call  ```mult_prop_log.txt```):

    a = 5
    b = true
    c = "cat"
    ..
    a = 5
    b = false
    c = "dog"
    ..
    a = 100
    b = true
    c = "cat"
    ..
    a = 100
    b = false
    c = "cat"
    --

Here we use the format accepted by Texada, wherein lines of '..' separate individual events, and the collection of lines between a pair of these event separators make up the set of propositions holding at that point in time.

Now, given the above multi-propositional log, we can use Texada to find valid property instantiations in the same way as for single-propositional logs, by appending the '--parse-mult-prop' flag.

For example, the command:

    ./texada -l -f 'G(x -> X(y))' --log-file basic_log.txt --parse-mult-prop

would produce the following results:

    G('b = true' -> X('c = "cat"'))

## Regular expression arguments

Texada relies on user-supplied regular expressions ([Perl regular expressions](http://www.boost.org/doc/libs/1_55_0/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html)) to parse structured log lines. By default, Texada will treat every line as an event and equivalent lines (i.e., lines whose strings match) as instances of the same event type, and "--" as partitions between traces. However, to parse more complex logs regular expressions must be provided to determine the event type of a log line.

There are two command line options which require regular expression arguments: 

* --trace-separator : specifies the log lines to be interpreted as partitions between traces, takes a single regular expression argument, and all lines matching this expression are treated as partitions between traces.

* --regex or -r : takes a list of regular expressions as argument, each specifying the structure of some log line. Unlike --trace-separator, the regex arguments of -r require a capturing group with the name <ETYPE>. Texada will walk through the log lines and try to match each line with one of these regular expressions; once a match is found, the string captured by the named group <ETYPE> will become the event type of the line.

* --ignore-nm-lines or -i : If a line fails to match any of the provided regular expressions, the program will abort. To ignore these unmatched log lines use the -i option.


# Command line usage screen

~~~~
Usage: texada [opt1] ... [optN] log-file

Required options: -f, log-file (positional option), one of [-l, -m, -p]

Allowed options:
  -h [ --help ]               produce help message
  -f [ --property-type ] arg  property type to mine
  --log-file arg              log file to mine on
  -m [ --map-trace ]          mine on a trace in the form of a map
  -l [ --linear-trace ]       mine on a linear trace
  -p [ --prefix-tree-trace ]  mine on traces in prefix tree form
  --pregen-instants           pre-generate property type instantiations. By default, Texada instantiates them on-the-fly. 
  --allow-same-bindings       allow different formula variables to be bound to the same events. By default, Texada does not 
                              check instantiations of this type.
  -c [ --config-file ] arg    specify file containing command line options. Any options entered directly to command line will 
                              override file options.
  -e [ --event ] arg          specify a variable in the formula to be interpreted as a constant event.
  -r [ --regex ] arg          regular expression to parse event types from log [default (?<ETYPE>.*)]
  --trace-separator arg       regular expression matching trace separator lines in the log [default: --]
  --parse-mult-prop           process given log as multi-propositional. Lines before and after event-separators
                              [by default: ..] are treated as propositions making up a single event.
  -i [ --ignore-nm-lines ]    ignore non-matching lines [default: false]
  --no-vacuous-findings       filter out findings which are only vacuously true (i.e. sup-threshold = 1)
  --sup-threshold arg         only find instances above the given support threshold [default: 0]
  --sup-pot-threshold arg     only find instances above the given support potential threshold [default: 0]
  --conf-threshold arg        only find instances above the given confidence threshold. Must be between 0 and 1 [default: 1]
  --print-stats               print the support and confidence of each finding
  --use-global-thresholds     make all inputed thresholds global [default: false]
  --output-json               write output in json format [default: false]
  --out-file arg              write output to out file at specified location
~~~~
