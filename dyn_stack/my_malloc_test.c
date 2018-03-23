#include "my_malloc.h"

void* malloc_dbg(size_t len)
{
	void *retval = my_malloc(len);
	//check_alignment();
	return retval;
}

void* calloc_dbg(size_t nitems, size_t size)
{
	void *retval = my_calloc(nitems, size);
	//check_alignment();
	return retval;
}

void free_dbg(void *p)
{
	my_free(p);
	//check_alignment();
}

void* realloc_dbg(void *ptr, size_t len)
{
	void *retval = my_realloc(ptr, len);
	//check_alignment();
	return retval;
}



/***************/



void test_stack()
{
	int i;
	int arrtest[3] = {1, 2, 3};
	
	for(i=0; i<3; i++)
		printf("arrtest[%d] = %p\n", i, &arrtest[i]);
}



void test_malloc()
{
	char memory[4096];
	char *str[10];
	
	
	printf("Putting \"aaabbbcccdddeee\" onto the stack...\n");
	sprintf(&memory[0], "aaabbbcccdddeee");
	printf("%s\n\n", &memory[0]);
	
	printf("Putting \"111222333\" onto the stack...\n");
	sprintf(&memory[15], "111222333");
	printf("%s\n\n", &memory[15]);
	
	init_malloc(&memory[4095], &memory[1024]);
	
	printf("Allocating \"1234567890abcdef\" (17) onto the heap...\n");
	str[0] = malloc_dbg(17);
	sprintf(str[0], "1234567890abcdef");
	printf("%s\n\n", str[0]);
	
	printf("Allocating \"qwertyui\" (9) onto the heap...\n");
	str[1] = malloc_dbg(9);
	sprintf(str[1], "qwertyui");
	printf("%s\n\n", str[1]);
	
	printf("Allocating \"hello world!\" (13) onto the heap...\n");
	str[2] = malloc_dbg(13);
	sprintf(str[2], "hello world!");
	printf("%s\n\n", str[2]);
	
	printf("Allocating \"The quick brown fox jumps over the lazy dog\" (256) onto the heap...\n");
	str[3] = malloc_dbg(256);
	sprintf(str[3], "The quick brown fox jumps over the lazy dog");
	printf("%s\n\n", str[3]);
	
	printf("Allocating \"Sometimes I dream about Cheese\" (137) onto the heap...\n");
	str[4] = malloc_dbg(137);
	sprintf(str[4], "Sometimes I dream about Cheese");
	printf("%s\n\n", str[4]);
	
	printf("Allocating \"GET OFF MY PLANE\" (32) onto the heap...\n");
	str[5] = malloc_dbg(32);
	sprintf(str[5], "GET OFF MY PLANE");
	printf("%s\n\n", str[5]);
	
	printf("Allocating \"i wish p=np\" (12) onto the heap...\n");
	str[6] = malloc_dbg(12);
	sprintf(str[6], "i wish p=np");
	printf("%s\n\n", str[6]);
	
	
	

	printf("\n***Testing originals***\n");
	printf("%s\n", &memory[0]);
	printf("%s\n", &memory[15]);
	printf("%s\n", str[0]);
	printf("%s\n", str[1]);
	printf("%s\n", str[2]);
	printf("%s\n", str[3]);
	printf("%s\n", str[4]);
	printf("%s\n", str[5]);
	printf("%s\n", str[6]);
}



void test_calloc()
{
	char memory[4096];
	char *str[10];
	
	
	printf("Putting \"aaabbbcccdddeee\" onto the stack...\n");
	sprintf(&memory[0], "aaabbbcccdddeee");
	printf("%s\n\n", &memory[0]);
	
	printf("Putting \"111222333\" onto the stack...\n");
	sprintf(&memory[15], "111222333");
	printf("%s\n\n", &memory[15]);
	
	init_malloc(&memory[4095], &memory[1024]);
	
	printf("Allocating \"1234567890abcdef\" (17) onto the heap...\n");
	str[0] = calloc_dbg(17, sizeof(char));
	sprintf(str[0], "1234567890abcdef");
	printf("%s\n\n", str[0]);
	
	printf("Allocating \"qwertyui\" (9) onto the heap...\n");
	str[1] = calloc_dbg(9, sizeof(char));
	sprintf(str[1], "qwertyui");
	printf("%s\n\n", str[1]);
	
	printf("Allocating \"hello world!\" (13) onto the heap...\n");
	str[2] = calloc_dbg(13, sizeof(char));
	sprintf(str[2], "hello world!");
	printf("%s\n\n", str[2]);
	
	printf("Allocating \"The quick brown fox jumps over the lazy dog\" (256) onto the heap...\n");
	str[3] = calloc_dbg(256, sizeof(char));
	sprintf(str[3], "The quick brown fox jumps over the lazy dog");
	printf("%s\n\n", str[3]);
	
	printf("Allocating \"Sometimes I dream about Cheese\" (137) onto the heap...\n");
	str[4] = calloc_dbg(137, sizeof(char));
	sprintf(str[4], "Sometimes I dream about Cheese");
	printf("%s\n\n", str[4]);
	
	printf("Allocating \"GET OFF MY PLANE\" (32) onto the heap...\n");
	str[5] = calloc_dbg(32, sizeof(char));
	sprintf(str[5], "GET OFF MY PLANE");
	printf("%s\n\n", str[5]);
	
	printf("Allocating \"i wish p=np\" (12) onto the heap...\n");
	str[6] = calloc_dbg(12, sizeof(char));
	sprintf(str[6], "i wish p=np");
	printf("%s\n\n", str[6]);
	
	
	

	printf("\n***Testing originals***\n");
	printf("%s\n", &memory[0]);
	printf("%s\n", &memory[15]);
	printf("%s\n", str[0]);
	printf("%s\n", str[1]);
	printf("%s\n", str[2]);
	printf("%s\n", str[3]);
	printf("%s\n", str[4]);
	printf("%s\n", str[5]);
	printf("%s\n", str[6]);
}



void test_free()
{
	char memory[4096];
	char *str[10];
	
	sprintf(&memory[0], "aaabbbcccdddeee");
	sprintf(&memory[15], "111222333");
	printf("%s\n", &memory[0]);
	printf("%s\n", &memory[15]);
	
	
	init_malloc(&memory[4095], &memory[1024]);
	
	
	printf("Allocating \"1234567890abcdef\" (17) onto the heap...\n");
	str[0] = malloc_dbg(17);
	sprintf(str[0], "1234567890abcdef");
	printf("%s\n\n", str[0]);
	
	printf("Allocating \"qwertyui\" (9) onto the heap...\n");
	str[1] = malloc_dbg(9);
	sprintf(str[1], "qwertyui");
	printf("%s\n\n", str[1]);
	
	printf("Allocating \"hello world!\" (13) onto the heap...\n");
	str[2] = malloc_dbg(13);
	sprintf(str[2], "hello world!");
	printf("%s\n\n", str[2]);
	
	printf("Allocating \"The quick brown fox jumps over the lazy dog\" (256) onto the heap...\n");
	str[3] = malloc_dbg(64);
	sprintf(str[3], "The quick brown fox jumps over the lazy dog");
	printf("%s\n\n", str[3]);
	
	printf("Allocating \"Sometimes I dream about Cheese\" (137) onto the heap...\n");
	str[4] = malloc_dbg(65);
	sprintf(str[4], "Sometimes I dream about Cheese");
	printf("%s\n\n", str[4]);
	
	printf("Allocating \"GET OFF MY PLANE\" (32) onto the heap...\n");
	str[5] = malloc_dbg(66);
	sprintf(str[5], "GET OFF MY PLANE");
	printf("%s\n\n", str[5]);
	
	printf("Allocating \"i wish p=np\" (12) onto the heap...\n");
	str[6] = malloc_dbg(12);
	sprintf(str[6], "i wish p=np");
	printf("%s\n\n", str[6]);
	
	printf("Allocating \"THIS IS A REALLY BIG PIECE AT THE END\" (403) onto the heap...\n");
	str[7] = malloc_dbg(403);
	sprintf(str[7], "THIS IS A REALLY BIG PIECE AT THE END");
	printf("%s\n\n", str[7]);
	
	printf("Allocating \"i wish p=np\" (13) onto the heap...\n");
	str[8] = malloc_dbg(13);
	sprintf(str[8], "i wish p=np");
	printf("%s\n\n", str[8]);
	

	
	printf("\n**************FREEING******************\n");
	
	printf("Freeing 3\n");
	free_dbg(str[3]);
	printf("\n");
	
	printf("Freeing 5\n");
	free_dbg(str[5]);
	printf("\n");
	
	//testing merge
	printf("Freeing 4\n");
	free_dbg(str[4]);
	printf("\n");
	
	
	//testing break reduction
	printf("Freeing 8\n");
	free_dbg(str[8]);
	printf("\n");
	
	printf("Freeing 7\n");
	free_dbg(str[7]);
	printf("\n");
	
	//testing exact piece
	printf("Freeing 1 (9)\n");
	free_dbg(str[1]);
	printf("\n");
	
	printf("Allocating \"iuytrewq\" (9) onto the heap...\n");
	str[1] = malloc_dbg(9);
	sprintf(str[1], "iuytrewq");
	printf("%s\n\n", str[1]);
	
	//another allocation
	printf("Allocating \"6969696969696969696969696969\" (69) onto the heap...\n");
	str[3] = malloc_dbg(69);
	sprintf(str[3], "6969696969696969696969696969");
	printf("%s\n\n", str[3]);
	
	
	printf("\n***Testing originals***\n");
	printf("%s\n", &memory[0]);
	printf("%s\n", &memory[15]);
	printf("%s\n", str[0]);
	printf("%s\n", str[2]);
	printf("%s\n", str[6]);
	printf("%s\n", str[1]);
	printf("%s\n", str[3]);
}



void test_realloc()
{
	char memory[4096];
	char *str[10];
	
	sprintf(&memory[0], "aaabbbcccdddeee");
	sprintf(&memory[15], "111222333");
	printf("%s\n", &memory[0]);
	printf("%s\n", &memory[15]);
	
	
	init_malloc(&memory[4095], &memory[1024]);

	
	//Testing shrink
	printf("******\n");
	
	printf("Allocating \"1234567890abcdef1234567890abcdef\" (48) onto the heap...\n");
	str[0] = malloc_dbg(48);
	sprintf(str[0], "1234567890abcdef1234567890abcdef");
	printf("%s\n\n", str[0]);
	
	printf("Shrinking from (48) to (20) as \"fedcba0987654321\"...\n");
	str[0] = realloc_dbg(str[0], 20);
	sprintf(str[0], "fedcba0987654321");
	printf("%s\n\n", str[0]);
	
	
	//Testing growth at malloc break
	printf("******\n");
	
	printf("Allocating \"qwertyuiqwertyuiqwertyui\" (25) onto the heap...\n");
	str[1] = malloc_dbg(25);
	sprintf(str[1], "qwertyuiqwertyuiqwertyui");
	printf("%s\n\n", str[1]);
	
	printf("Expanding from (25) to (40) as \"qwertyuiopasdfghjklqwertyuiopasdfghjkl\"...\n");
	str[1] = realloc_dbg(str[1], 40);
	sprintf(str[1], "qwertyuiopasdfghjklqwertyuiopasdfghjkl");
	printf("%s\n\n", str[1]);

	
	//Testing growth, when exact amount of free space is right above the current piece
	printf("******\n");
	
	printf("Allocating \"hello world!\" (13) onto the heap...\n");
	str[2] = malloc_dbg(13);
	sprintf(str[2], "hello world!");
	printf("%s\n\n", str[2]);
	
	printf("Allocating \"HELLO WORLD!\" (14) onto the heap...\n");
	str[3] = malloc_dbg(14);
	sprintf(str[3], "HELLO WORLD!");
	printf("%s\n\n", str[3]);
	
	printf("Allocating \"hElLo WoRlD!\" (15) onto the heap...\n");
	str[4] = malloc_dbg(15);
	sprintf(str[4], "hElLo WoRlD!");
	printf("%s\n\n", str[4]);
	
	printf("Freeing 3 (14)...\n");
	free_dbg(str[3]);
	printf("\n");
	
	printf("Expanding from (25) to (%zu) as \"helloworld~HELLOWORLD~hElLo\"...\n", 13+14+sizeof(Heap_Seg));
	str[2] = realloc_dbg(str[2], 13+14+sizeof(Heap_Seg));
	sprintf(str[2], "helloworld~HELLOWORLD~hElLo");
	printf("%s\n\n", str[2]);
	
	
	
	//Testing growth, but new allocation piece is needed
	printf("******\n");
	
	printf("Expanding from (%zu) to (48) as \"The quick brown fox jumps over the lazy dog\"...\n", 13+14+sizeof(Heap_Seg));
	str[0] = realloc_dbg(str[0], 48);
	sprintf(str[0], "The quick brown fox jumps over the lazy dog");
	printf("%s\n\n", str[0]);

	
	
	printf("\n***Testing originals***\n");
	printf("%s\n", &memory[0]);
	printf("%s\n", &memory[15]);
	printf("%s\n", str[0]);
	printf("%s\n", str[1]);
	printf("%s\n", str[2]);
	printf("%s\n", str[4]);

}


int main()
{
	
	//test_malloc();
	//test_calloc();
	//test_free();
	test_realloc();
	
}
