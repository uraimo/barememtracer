# BareMemTracer -- a C/C++ bare bones memory tracer

A simple memory allocation tracer written in C (new/delete tracing available in a C++ environment) 
that can be used to track down memory leaks. Due to its simplicity, it should work in nearly every
environment (highly constricted embedded environment too) and should be quite easy to modify/extend 
if needed.

If you need a real dynamic analysis&debugging tool, you should consider using Valgrind instead if your 
platform is supported.


## Usage

Just add this include statement (after your system/c header includes):

	#include "memtracer.h" 

And invoke the following function to print out the allocation status wherever you want:

	dumpAlloc();

See test.c and test.cpp for an example of usage.


## Notes

The tracing...() calls are not thread-safe, if your application has multiple threads allocating memory        
please consider introducing a lock at the beginning of tracingInit().

The constant ALLOC_TABLE_SIZE in memtracer.h can be used to increase the size of the allocation descriptors
hashmap thus reducing its load and increasing the performarnce of add/remove operations.

When used with C++ some modification could be needed to the "new" declarations if your environment doesn't 
support namespaces. 


## License

BSD license, see COPYING
