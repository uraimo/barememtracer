#define MEM_TRACER_C 1

#include "memtracer.h"

#ifdef __cplusplus
// C section, everything except new/delete overload
extern "C" {
#endif

//--------------------------------------------------------------------------------------------
// Allocation descriptors HashMap related functions

static size_t genHash(size_t max, void* ptr){
	size_t key= (size_t)ptr;
	return (key % max);
}

static int nextNotPow2(int num){
	int nextnotpow2=num;
	while ( (nextnotpow2!=0) && ((nextnotpow2 & (nextnotpow2-1)) == 0) ) {
		nextnotpow2++;
	}
	return nextnotpow2;
}

/**
   Returns an empty hash of the chosen size
 */
allochashmap* hashmap_Init(int size){
	int asize=nextNotPow2(size);
	allochashmap* map=(allochashmap*) malloc(sizeof(allochashmap));
	assert((NULL!=map) &&  MSG_ERR_HM_MALLOC);
	map->size=asize;
	map->table=(allocnode**)calloc(asize,sizeof(allocnode));
	if (NULL==map->table) {
		free(map);
		map=NULL;
	}
	assert((NULL!=map) &&  MSG_ERR_HM_MALLOC);
	return(map);
}

/**
   Free the hash with ALL its content, leaked pointers will be freed.
 */
void hashmap_Destroy(allochashmap* map){
	size_t i=0;
	for(i=0; i<map->size; ++i) {
		while(NULL!= map->table[i]) {
			allocnode* next=map->table[i]->next;
			free(map->table[i]->info.file);
			//External pointer cleanup
			free(map->table[i]->info.ptr);
			free(map->table[i]);
			map->table[i]=next;
		}
	}
	free(map->table);
	map->table=NULL;
	free(map);
	map=NULL;
}

/**
   Remove the allocation descriptor of the given pointer, returns 0: found, -1:not found
 */
int hashmap_Delete(allochashmap* map, void* ptr){
	int idx=genHash(map->size, ptr);
	allocnode* cur= map->table[idx];
	allocnode* prev= map->table[idx];

	while(NULL != cur) {
		if (cur->info.ptr == ptr) {
			allocnode* found=cur;
			if (prev==cur) {
				map->table[idx]=cur->next;
			}else{
				prev->next=cur->next;
			}
			free(found->info.file);
			free(found);
			return(0);
		}else{
			prev=cur;
			cur=cur->next;
		}
	}
	return(-1);
}

/**
   Print out the content of the hashmap using the given function as allocated block address - block size and stats on the load
 */
void hashmap_Print(allochashmap* map, int outp(const char *format, ...)){
	int load=0;
	int totalelem=0;
	int maxloadpernode=0;
	size_t i=0;
	outp("--- Current allocations status ---\n\n");
	for(i=0; i<map->size; ++i) {
		allocnode* nextn=map->table[i];
		if (NULL!=nextn) load++;
		int localload=0;
		while(NULL!= nextn) {
			totalelem++;
			localload++;
			outp("Addr:0x%lX - Size: %ld allocated in %s:%d\n",(size_t)nextn->info.ptr,nextn->info.size,nextn->info.file,nextn->info.line);
			nextn=nextn->next;
		}
		if (localload>maxloadpernode) maxloadpernode=localload;
	}
	outp("\n(Allocation descriptors hashmap load: %d/%ld MaxLocalLoad:%d Total elements:%d)\n",load,map->size,maxloadpernode,totalelem);
}

/**
   Insert a new allocation descriptor, returns hash key (pointer descriptor idx)
 */
int hashmap_Insert(allochashmap* map, allocdescr* data){
	int idx=genHash(map->size, data->ptr);
	allocnode* newnode= (allocnode*) calloc(1,sizeof(allocnode));
	assert((NULL!=newnode) &&  MSG_ERR_HM_MALLOC);
	newnode->info=*data;
	newnode->next=map->table[idx];
	map->table[idx]=newnode;
	return(idx);
}

/**
   Build an allocation descriptor
 */
allocdescr* buildAllocdescr(void* ptr, int size,const char* filename, int line){
	allocdescr* newn=(allocdescr*)malloc(sizeof(allocdescr));
	newn->ptr=ptr;
	newn->size=size;
	newn->file=(char*) malloc(50);
	strcpy(newn->file,filename);
	newn->line=line;
	return newn;
}

/**
   Allocation descriptors HashMap unit test, half of the elements must remain.
 */
/*
int main(){
   int* a;
   size_t mapSize=1733;
   allochashmap* myMap=hashmap_Init(mapSize);
   int i=0;
   for(i=0;i<500;++i){
       a=malloc(sizeof(int));
       hashmap_Insert(myMap,buildAllocdescr(a,sizeof(int),__FILE__,__LINE__));
       if (i % 2 == 0){
           hashmap_Delete(myMap,a);
           free(a);
       }
   }
   hashmap_Print(myMap);
   hashmap_Destroy(myMap);
   return 0;
}
*/
//--------------------------------------------------------------------------------------------

static allochashmap* allocMap=NULL;

void tracingInit(){
	if (NULL == allocMap) {
		allocMap=hashmap_Init(ALLOC_TABLE_SIZE);
	}
}

void* tracingMalloc(int size, const char* name, int line){
	tracingInit();
	void* ptr=malloc(size);
	hashmap_Insert(allocMap,buildAllocdescr(ptr,size,name,line));
	return ptr;
}

void* tracingRealloc(void* ptr, int size, const char* name, int line){
	tracingInit();
	hashmap_Delete(allocMap,ptr);
	void* ptrnew=realloc(ptr,size);
	hashmap_Insert(allocMap,buildAllocdescr(ptrnew,size,name,line));
	return ptr;
}

void* tracingCalloc(int num, int size, const char* name, int line){
	tracingInit();
	void* ptr=calloc(num,size);
	hashmap_Insert(allocMap,buildAllocdescr(ptr,num*size,name,line));
	return ptr;
}

void tracingFree(void* ptr){
	tracingInit();
	hashmap_Delete(allocMap,ptr);
	free(ptr);
}

void dumpAlloc(){
    tracingInit();
	hashmap_Print(allocMap,&printf);
	hashmap_Destroy(allocMap);
}

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
void * operator new(size_t size) throw(std::bad_alloc){
	return(tracingMalloc(size,"-N/A-",0));
}

void * operator new[] (size_t size) throw(std::bad_alloc){
	return(tracingMalloc(size,"-N/A-",0));
}

void * operator new(size_t size,const char* name,int line){
	return(tracingMalloc(size,name,line));
}

void * operator new[] (size_t size,const char* name,int line){
	return(tracingMalloc(size,name,line));
}

void operator delete(void* ptr) throw(){
	tracingFree(ptr);
}

void operator delete[](void* ptr) throw(){
	tracingFree(ptr);
}
#endif //ifdef __cplusplus

