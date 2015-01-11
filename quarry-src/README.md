## Table of contents
* [Overview](#markdown-header-overview)
* [Usage guide](#markdown-header-usage-guide)
* [Command line usage screen](#markdown-header-command-line-usage-screen)


# Overview
-----------------------

Quarry is a research tool designed for generating data-temporal traces of programs. Specifically, Quarry combines invariants and event sequences outputted by the tool [Daikon](http://plse.cs.washington.edu/daikon/download/doc/daikon.html) to create [Texada](https://bitbucket.org/bestchai/texada/)-processible logs.

Quarry is designed to provide a way of pipelining Daikon output into Texada. By combining information in the invariants and dtrace files outputed by Daikon, Quarry generates a data-temporal log, which can be inputed into Texada as a set of multi-propositional traces. Specifically, a data-temporal log is a collection of traces, each of which describes the state of an object occuring in the execution of the given program as it develops along its lifetime.


# Usage guide
-----------------------

## Basic options

The basic command to run Quarry looks like this:

    python quarry.py <dfile> <ifile> <ofile>

The three basic arguments are as follows:

1. <dfile>: Path to a [Daikon dtrace file](http://plse.cs.washington.edu/daikon/download/doc/developer.html#Example-data-trace-file). A daikon dtrace file is a sequence of a program's method entries and method exits, which provide a sample of the program's execution.

2. <ifile>: Path to a [Daikon invariants file](http://plse.cs.washington.edu/daikon/download/doc/daikon.html#Daikon-output). A daikon invariants file provides a mapping of particular program points (i.e. method entries/exits) and program intervals (i.e. lifetime of an object) to their invariants--properties regarding the state of the program which always hold at those points or along those intervals.

3. <ofile>: Path to an output file.

## Example 1

Suppose that, using Daikon, we have generated a dtrace file called ``TODO.dtrace``:

    TODO

and an invariants file called ``TODO.txt``:

    TODO

In order to generate a basic data-temporal log from these files, we pass them into Quarry like so:

    python quarry.py TODO.dtrace TODO.txt data-temp.txt

Quarry will then partition the program trace found in ``TODO.txt`` into a collection object traces, one for each object occuring in the execution of the program. It will then map onto each program point its corresponding set of invariants, writing the resulting data-temporal log into ``data-temp.txt``.

The contents of ``data-temp.txt`` will now look like this:

    TODO

## Include object/class invariants

By default, Quarry does not include object and class invariants to its final output. In order to enable the inclusion of object and class invariants, we use the --incl_obj and --incl_cls options, respectively, like so:

    python quarry.py TODO.dtrace TODO.txt data-temp.txt --incl_obj --incl_obj

Assuming that ``TODO.dtrace`` and ``TODO.txt`` are defined as above, the data-temporal log generated into ``data-temp.txt`` will now look like this:

    TODO

## Specify a particular class to extract

In certain cases, we may only be interested in the behavior of a particular class. In order to generate traces for objects of a particular class and filter out all others, use the ``-e <class_name>`` option, where <class_name> is the full name of the class to extract. In order to distinguish between similarly named classes inside different namespaces, we need to specify the full package path in which it is defined. For example:

    basepkg::subpkg_1::...::subpkg_n::ClassName

Continuing from the examples above, suppose that we want to extract only the traces of objects of the ``TODO`` class. We would use the -e option with the full name of the class, ``TODO``, like so:

    python quarry.py TODO.dtrace TODO.txt data-temp.txt -e `TODO'

Again assuming that ``TODO.dtrace`` and ``TODO.txt`` are defined as in the above examples, the data-temporal log generated into ``data-temp.txt`` will now look like this:

    TODO


# Command line usage screen
-----------------------

~~~~
 usage: quarry.py [-h] [--incl_obj] [--incl_cls] [-e EXTRACT] dfile ifile ofile

 positional arguments:
   dfile                 path to the Daikon-generated dtrace file.
   ifile                 path to the Daikon-generated invariants file.
   ofile                 path to the output file.

 optional arguments:
   -h, --help            show this help message and exit
   --incl_obj            include object invariances
   --incl_cls            include class invariances
   -e EXTRACT, --extract EXTRACT
                         specify a specific class to extract [by default:
                         extract all]
~~~~