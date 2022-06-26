# RexxXML

The RexxXML library provides a Rexx interface to data represented using HTML or any XML dialect. The intent is to allow XML data to be processed in a straight-forward manner within a Rexx program, and to allow Rexx to be used from within certain XML-based applications.

### Notice

RexxXML is written by Patrick TJ McPhee (ptjm@interlog.com). It is offered here for better availability. 
Originally from http://home.interlog.com/~ptjm/

( Please read the file [rexxxml.pdf](rexxxml.pdf) for a complete documentation. It serves as an excellent brief introduction into the Rexx language. )

## Introduction

Rexx is a programming language which was designed to be learned and used easily by non-professional programmers. It is meant to make it easy to write programs, at the expense of complicating the language implementation. Its main characteristics are the absence of program structure, isolation from machine limitations, integration with application environments, and a set of built-in functions which is useful for a wide array of data processing applications.

HTML is an SGML application which was designed for on-line presentation of technical reports, and which is firmly entrenched as the primary data representation on the world-wide web. XML is an SGML application profile which was intended to allow greater freedom in marking up web content, and which has gained some currency as a data exchange protocol. It was specifically designed to simplify parsing (as compared to the full generality of SGML) at the expense of complicating data generation. Most document-generation-friendly features of SGML have been dropped, even those which don’t make parsing more complex. In that sense, XML could be considered the anti-Rexx. SGML is an ISO standard language for defining document mark-up.

RexxXML’s XML processing is provided by the Gnome project’s libxml and libxslt, both written by Daniel Veillard. RexxXML does not attempt to provide a full interface to those libraries, and it may provide less flexibility at the expense of greater simplicity. Books and other reference material about those packages can still be helpful in using RexxXML.

This guide is both an introduction to XML processing using RexxXML and a complete reference to the library. The reader is not expected to be familiar with Rexx or XML syntax – I include a brief introduction to both, as well as the XML-related technologies, XPath and XSLT – but additional reference materials are needed to write effective programs.

# Installation

The RexxXML package includes pre-compiled binaries for Win32 platforms and source code which should compile on those platforms and most Unix systems. It does not include libxml, libxslt, or a Rexx interpreter.

The distribution also does not include an installation program. The general installation instructions are to copy the appropriate library file to an appropriate directory. From this package, only rexxxml.dll (or, on Unix, librexxxml.so) is required to use the library, and applications using it can be distributed with only this file. The documentation file, rexxxml.pdf, should also be distributed if end users are expected to write macros using these functions.

## Win32

If you don’t have a Rexx interpreter, I suggest obtaining Regina Rexx. There are several other inter- preters available for win32 platforms. See the Rexx Language Association web site for details. Note that the port of Regina which was included with the Windows NT resource kit is not suitable for use with libraries such as RexxXML.

You must obtain libxml and libxslt from the libxml and libxslt web pages. My recommendation is to get the pre-compiled windows binaries which are available from a link on those pages. RexxXML may or may not work with binaries obtained from other sources. If you want to, for instance, add Rexx support to an application which uses a non-standard build of libxml, you may need to rebuild either RexxXML or the application.

There are two pre-compiled libraries in the distribution file. If you use Regina, the appropriate file is RexxXML/win32/rexxxml.dll. If you use any other interpreter, the appropriate file is RexxXML/rexxtrans rexxxml.dll, and you need to obtain RexxTrans from its web site.

To install the pre-compiled library, copy the appropriate version of rexxxml.dll to either a directory in your program search path or the directory containing the Rexx executable. See section 1.1.4 for information about compiling the library from source code.

## Unix

The distribution does not include a configuration script, but it includes make files which have been known to work using the stock vendor compiler on several Unix systems. If you have one of those systems, link the appropriate make file to the name ‘Makefile’ and build the ‘dist’ target. For instance, on Solaris:

'''
ln Makefile.sun Makefile
make dist
'''

On most platforms, this builds a shared library called librexxutil.so. On HP-UX, the file is called librexxutil.sl, and on AIX, it’s called librexxutil.a. The path to this library can be set in three ways: Most Unix systems allow a shared library search path to be embedded into program files. If you build Regina (or your Rexx-enabled application) such that this path is set to include a directory such as /opt/regina/lib or /usr/local/lib, you can install RexxXML by copying the shared library to this directory (see section 1.1.4 for more information). If this is not possible, you need to either set an environment variable or change the way the system searches for shared libraries.

Unix systems typically use a different path for shared libraries than they do for program files. The name of the environment variable used for the shared library path is not standardised, however most systems use LD_LIBRARY_PATH. Notable exceptions are AIX (LIBPATH) and HP-UX (SHLIB_PATH for 32-bit executables, LD_LIBRARY_PATH for 64-bit executables). To install RexxXML, add an appropriate directory to the shared library path and copy the shared library to that directory.

Finally, some systems provide a utility (often called ldconfig) which can be used either to set the standard search path for shared libraries, or to provide a database of shared libraries. On such a system, RexxXML can be installed by copying the shared library to an appropriate directory and using this utility to add it to the search database. You’ll need to consult your system documentation for more information.

