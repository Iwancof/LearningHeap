#include<stdio.h>
#include<stdlib.h>

static void* main_arena;

int main() {
	size_t malloc_size = 0x20;


	void* tcache[7];
	for(int i = 0;i < 7;i++) {
		tcache[i] = malloc(malloc_size);
	}
	void* small[5];
	for(int i = 0;i < 5;i++) {
		small[i] = malloc(malloc_size);
	}
	
	for(int i = 0;i < 7;i++) {
		free(tcache[i]); // consume tcache
		printf("Add to tcache %p at %d\n", tcache[i], i);
	}
	for(int i = 0;i < 5;i++) {
		free(small[i]);
		printf("Add to smallbin %p at %d\n", small[i], i);
	}

	for(int i = 0;i < 7;i++) {
		tcache[i] = malloc(malloc_size);
		printf("Get from tcache %p at %d\n", tcache[i], i);
	}

	for(int i = 0;i < 5;i++) {
		small[i] = malloc(malloc_size);
		printf("Get testing %p at %d\n", small[i], i);
	}

}

