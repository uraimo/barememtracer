# BareMemTracer -- a C/C++ bare bones memory tracer

A simple memory allocation tracer written in C (new/delete tracing available in a C++ environment) 
that can be used to track down memory leaks. Due to its simplicity, it should work in nearly every
environment (highly constricted embedded environment too) and should be quite easy to modify/extend 
if needed.
If your platform is supported, you should consider using Valgrind instead.


## Usage

Just add this include statement (after your system/c header includes):

	#include "memtracer.h" 

And invoke the following function to print out the allocation status wherever you want:

	dumpAlloc();

See test.c and test.cpp for an example of usage.


## Notes

The tracing*() calls are not thread-safe, if your application has multiple threads allocating memory        
please consider introducing a lock at the beginning of all the tracing*() functions and dumpAlloc().

When used with C++ some modification could be needed to the new declarations if your environment don't 
support namespaces. 


## License

See COPYING
