#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memtracer.h"


class testClass {
public:
int a;
long b;
};


int main(){
	testClass* a= new testClass();
	delete (a);
	a= new testClass();
	delete (a);
	a= new testClass();
	delete (a);
	char*  str=new char[10];
	delete (str);
	str=new char[10];
	delete (str);
	str=new char[10];
	delete (str);
	a= new testClass();
	delete (a);
	a= new testClass();
	delete (a);
	testClass* arr= new testClass[100];
	dumpAlloc();
	return(0);
}
