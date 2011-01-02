#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memtracer.h"

int main(){
	int* a= malloc(sizeof(int));
	a= malloc(sizeof(int));
	a= malloc(sizeof(int));
	free(a);
	a= malloc(sizeof(int));
	char*  str=calloc(10,sizeof(char*));
	str=calloc(50,sizeof(char*));
	str=calloc(150,sizeof(char*));
	str=calloc(50,sizeof(char*));
	free(str);
	a= malloc(sizeof(int));
	a= malloc(sizeof(int));
	free(a);
	dumpAlloc();
	return(0);
}
