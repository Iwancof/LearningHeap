#include<stdio.h>
#include<stdlib.h>

void* tcaches[7];

void allocate_tcache();
void free_tcache();

int main() {
	allocate_tcache();

	void* ptr1 = malloc(0x30);
	void* ptr2 = malloc(0x30);
	printf("ptr1: %p, ptr2: %p\n", ptr1, ptr2);

	free_tcache();

	free(ptr1);
	free(ptr2);
	free(ptr1);

	allocate_tcache();

	void* ptr3 = malloc(0x30); // get ptr1. ptr2, ptr1 moves to tcache
	void* ptr4 = malloc(0x30); // get ptr2
	void* ptr5 = malloc(0x30); // get ptr1

	printf("ptr3: %p, ptr4: %p, ptr5: %p\n", ptr3, ptr4, ptr5);
}


void allocate_tcache() {
	for(int i = 0;i < 7;i++) {
		tcaches[i] = malloc(0x30);
	}
}
void free_tcache() {
	for(int i = 0;i < 7;i++) {
		free(tcaches[i]);
	}
}


