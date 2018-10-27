# addlength

## What is this?
A program that converts a CSV file with newlines in the string into a compatible format with SQL*Loader.

## What is value of using this?
Even a large amount of data can be processed conversion just one command.

## Tell me how to use.

### Do it before you start using this.
1. Prepare a Windows machine.
1. Install Microsoft Visual Studio 2013 or later.
1. git clone https://github.com/plumsix/addlength.git
1. cd /d path/to/addlength
1. nmake EXE="addlength.exe" OBJS="addlength.obj" CPPFLAGS="/nologo /EHsc /Zi /O2"

### Usage
addlength <src_file> <dst_file>
