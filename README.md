# addlength

## What is this?
A set of programs for loading CSV with newline character in the string field into the Oracle DB table.

## What is the value of using this?
Even with a large amount of data, conversion work can be processed with one command.

## Tell me how to use.

### Do it before you start using this.
1. Prepare a Windows machine.
1. Install Microsoft Visual Studio 2013 or later.
1. git clone https://github.com/plumsix/addlength.git
1. cd /d path/to/houjin_bangou
1. nmake EXE="addlength.exe" OBJS="addlength.obj" CPPFLAGS="/nologo /EHsc /Zi /O2"

### Usage
.addlength <src_file> <dst_file> 
