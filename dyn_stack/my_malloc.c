/*
Note: malloc_heap_start > malloc_heap_end, since stack grows towards lower addresses
Adding an offset to a memory address will make it go down (shrinking), and subtracting will make it go up (growing).

This implementation of malloc saves heap space by minimizing the segment header (only stores segment size and next pointer), at the cost of runtime (no previous pointer).
*/

#include "my_malloc.h"


typedef unsigned char uchar;

static uchar* malloc_heap_start;						//Start of the dynamic memory heap
static uchar* malloc_heap_end;							//Absolute end of the memory segment for the heap; cannot allocate further than this

static uchar* malloc_break;								//Also referred as "brk", the current end for the allocated heap	
static Heap_Seg *freelist_head;							//Head of the first heap free list entry



/************************************************************************/
/*								HELPERS			 						*/
/************************************************************************/

#define MAX_HEAP_SIZE	(size_t)(malloc_heap_start - malloc_heap_end)

#define segment_end(p_entry)	((uchar*)p_entry + sizeof(Heap_Seg) + p_entry->size)

/*Implement this function properly if you want to prevent the heap from smashing into the stack.
This function is called by grow_malloc_break(), and it passes a new malloc break location (end of heap) for testing.
If this new break location would smash into the stack (or violates it in any ways), return 0. 
Return 1 if no issues will arise*/
static int check_stack_integrity(void* new_break)
{
	return 1;
}


/*Used by free() and realloc(), makes sure p is a valid pointer on the heap first*/
static int pointer_is_valid(void* p)
{
	Heap_Seg *p_entry = p - sizeof(Heap_Seg);
	
	if(p == NULL)
		return 0;
	
	//Make sure p is within the heap's bound
	if((uchar*)p < malloc_break || (uchar*)p < malloc_heap_end || (uchar*)p > malloc_heap_start)
	{
		fprintf(stderr,"p is not within the current heap range!\n");
		return 0;
	}
	
	//Make sure p's allocation entry fields appears "sane"
	if(p_entry->size >= MAX_HEAP_SIZE || p_entry->next != NULL)
	{
		fprintf(stderr,"p does not seem to be a valid allocation entry!\n");
		fprintf(stderr,"P: %p, Size: %zu, Next: %p\n", p, p_entry->size, p_entry->next);
		return 0;
	}
	
	return 1;
}


static inline void write_seg_header(void* seg_header, size_t len, Heap_Seg* next)
{
	Heap_Seg* new_entry =  (Heap_Seg*)seg_header;
	new_entry->size = len;
	new_entry->next = next;
}

static void* grow_malloc_break(size_t amount)			//Similar to sbrk() in unix
{
	uchar* new_break = malloc_break - amount;
	
	if(new_break < malloc_heap_end || new_break > malloc_heap_start || !check_stack_integrity(new_break))
	{
		fprintf(stderr,"Cannot continue. Allocation will exceed heap or smash into stack.\n");
		return NULL;
	}	
	malloc_break = new_break;
	
	return malloc_break;
}

void* get_malloc_break()			//Similar to brk() in unix
{
	return malloc_break;
}



/************************************************************************/
/*							INITIALIZATION		 						*/
/************************************************************************/

int init_malloc(uchar* start, uchar* end)
{
	if(start < end)
	{
		fprintf(stderr,"Heap starting address must be greater than end address!\n");
		return 0;
	}
	
	malloc_heap_start 	= start;
	malloc_heap_end 	= end;
	malloc_break 		= malloc_heap_start;	
	freelist_head 		= NULL;
	
	#ifdef DEBUG
	printf("Heap Start: %p, Heap End: %p\n\n", malloc_heap_start, malloc_heap_end);
	#endif
	return 1;
}


Malloc_Param save_malloc_param(void)
{
	Malloc_Param p;
	
	p.malloc_heap_start 	= malloc_heap_start;
	p.malloc_heap_end 		= malloc_heap_end;
	p.malloc_break 			= malloc_break;
	p.freelist_head 		= freelist_head;
	
	return p;
}


void load_malloc_param(Malloc_Param p)
{
	malloc_heap_start 	= p.malloc_heap_start;
	malloc_heap_end 	= p.malloc_heap_end;
	malloc_break 		= p.malloc_break;
	freelist_head 		= p.freelist_head;
}











/************************************************************************/
/*								MALLOC		  							*/
/************************************************************************/

void* my_malloc(size_t len)
{
	
	Heap_Seg *current_piece = NULL, *previous_piece = NULL;
	Heap_Seg *exact_piece = NULL, *exact_piece_prev = NULL;
	Heap_Seg *next_smallest_piece = NULL;
	
	uchar* retaddr = NULL;

	/*If stack has not yet been initialized, skip to step 3*/

	/************************************************/
	/*		Attempt 1: Find an exact piece		    */
	/************************************************/
	
	
	/*Find the first Freelist large enough to fit the length requested*/
	for(current_piece = freelist_head; current_piece; current_piece = current_piece->next, previous_piece = current_piece)
	{
		//Found an exact piece
		if(current_piece->size == len)
		{
			exact_piece = current_piece;
			exact_piece_prev = previous_piece;
			break;
		}
		
		//Record the smallest piece of memory available (of at least length "len"), if exact piece is not yet found
		if(current_piece->size > len + sizeof(Heap_Seg) && (!next_smallest_piece || current_piece->size < next_smallest_piece->size))
			next_smallest_piece = current_piece;
	}

	
	//Grant the exact piece if available
	if(exact_piece)
	{
		retaddr = (uchar*)exact_piece + sizeof(Heap_Seg);
			
		//Disconnect the current piece from the fl chain
		//If the piece used have no previous free piece, that means this is also freelist_head
		if(exact_piece_prev)
			exact_piece_prev->next = exact_piece->next;
		else
			freelist_head = exact_piece->next;	
		exact_piece->next = NULL;
		
		#ifdef DEBUG
		printf("malloc: Using an exact piece of size %zu at %p\n", len, exact_piece);
		#endif

		return retaddr;
	}
	
	
	/************************************************/
	/*		Attempt 2: Split an larger piece	   	*/
	/************************************************/

	if(next_smallest_piece)
	{	
		#ifdef DEBUG
		printf("malloc: Planning to split a piece of size %zu at %p\n", next_smallest_piece->size, next_smallest_piece);
		#endif
		
		//Shrink the size of the original segment to accomodate the requested lengths and a new seg header
		next_smallest_piece->size -= len + sizeof(Heap_Seg);
		
		//Calculate the expected return address for the granted memory
		retaddr = (uchar*)next_smallest_piece + sizeof(Heap_Seg);			//The actual start of the original segment
		retaddr += next_smallest_piece->size + sizeof(Heap_Seg);			//The actual start of the splitted piece to be returned
		
		//Write a new allocation entry for the new splitted segment to be returned. 
		//The shrunken free piece is still at its original location (RIGHT side of the splitted/allocated piece)
		write_seg_header(retaddr - sizeof(Heap_Seg), len, NULL);
		
		#ifdef DEBUG
		printf("malloc: Piece 1: size %zu at %p\n", len, retaddr - sizeof(Heap_Seg));
		printf("malloc: Piece 2 (free): size %zu at %p\n", next_smallest_piece->size, next_smallest_piece);
		#endif

		return retaddr;
	}
	
	
	/************************************************/
	/*		Attempt 3: Allocate more heap space 	*/
	/************************************************/
	
	//Calculate the return address for the request (in advance, assuming additional heap will be granted)
	
	if(malloc_break == malloc_heap_start)				//Heap is currently unallocated
		retaddr = malloc_heap_start - len;				
	else												//No existing free pieces available on the heap
		retaddr = malloc_break - len;

	//Allocate additional heap space needed for the requested length and a new header
	if(grow_malloc_break(len + sizeof(Heap_Seg)))
	{
		#ifdef DEBUG
		printf("malloc: Using a new piece of size %zu at %p; Malloc break at %p\n", len, retaddr - sizeof(Heap_Seg), malloc_break);
		#endif
		
		write_seg_header(retaddr - sizeof(Heap_Seg), len, NULL);
		return retaddr;
	}
	
	//If we are unable to grow the break, there's nothing more we can do :(
	return NULL;
}










/************************************************************************/
/*								CALLOC		  							*/
/************************************************************************/

void* my_calloc(size_t nitems, size_t size)
{
	size_t total_len = nitems * size;
	void *retaddr = my_malloc(total_len); 
	
	if(!retaddr)
		return NULL;
	memset(retaddr, 0, total_len);
	
	return retaddr;
}










/************************************************************************/
/*								FREE		  							*/
/************************************************************************/

void my_free(void *p)
{
	Heap_Seg *p_entry = p - sizeof(Heap_Seg);
	Heap_Seg *p_entry_prev = NULL;
	
	Heap_Seg *current_piece = NULL;
	Heap_Seg *closest_left = NULL, *closest_left_prev = NULL;
	Heap_Seg *closest_right = NULL;
	
	
	if(!pointer_is_valid(p))
		return;
	
	#ifdef DEBUG
	printf("free: Freeing %p of size %zu\n", p_entry, p_entry->size);
	#endif
	
	/************************************************/
	/*		Step 1: Freeing the requested piece 	*/
	/************************************************/
	
	//Iterate through the current freelist and locate the closest neighbouring free segments to p
	for(current_piece = freelist_head; current_piece; current_piece = current_piece->next)
	{
		if(current_piece > p_entry)
		{
			closest_left_prev = closest_left;
			closest_left = current_piece;
		}
		else if(current_piece < p_entry)
		{
			closest_right = current_piece;
			break;
		}
		else
		{
			//There should never be an occurance where p_entry is in the freelist. This might be a double free attempt
			fprintf(stderr, "Double free detected! Free Piece %p, size %zu, next %p\n", current_piece, current_piece->size, current_piece->next);
			return;
		}
		
	}
	
	//Update the freelist chain with the new free piece in it
	if(!closest_left)
	{
		p_entry->next = freelist_head;
		freelist_head = p_entry;
	}
	else
	{
		p_entry->next = closest_left->next;
		closest_left->next = p_entry;
		p_entry_prev = closest_left;
	}
	
	#ifdef DEBUG
	if(closest_left)
		printf("free: Found adjacent LEFT piece at %p, size %zu, next %p\n", closest_left, closest_left->size, closest_left->next);
	if(closest_right)
		printf("free: Found adjacent RIGHT piece at %p, size %zu, next %p\n", closest_right, closest_right->size, closest_right->next);
	#endif
	
	
	
	/************************************************/
	/*	Step 2: Merge with adjacent left piece	 	*/
	/************************************************/
	
	if(closest_left && segment_end(p_entry) == (uchar*)closest_left)
	{	
		//Update new header, and erase the old one
		p_entry->size += closest_left->size + sizeof(Heap_Seg);
		write_seg_header(closest_left, 0, NULL);
		
		//Update freelist
		if(closest_left_prev)
		{
			closest_left_prev->next = p_entry;
			p_entry_prev = closest_left_prev;
		}
		else
			freelist_head = p_entry;
		
		#ifdef DEBUG
		printf("free: Merged with adjacent left piece. New size %zu at %p\n", p_entry->size, p_entry);
		#endif
	}
	
	
	/************************************************/
	/*	Step 3: Merge with adjacent right piece	 	*/
	/************************************************/
	
	if(closest_right && segment_end(closest_right) == (uchar*)p_entry)
	{
		//Update new header, and erase the old one
		closest_right->size += p_entry->size + sizeof(Heap_Seg);
		write_seg_header(p_entry, 0, NULL);
		p_entry = closest_right;
		
		//Update freelist
		if(p_entry_prev)
			p_entry_prev->next = closest_right;
		else
			freelist_head = p_entry;
		
		#ifdef DEBUG
		printf("free: Merged with adjacent right piece. New size %zu at %p\n", p_entry->size, p_entry);
		#endif
	}
	
	
	/************************************************/
	/*			Step 4: Reduce Malloc Break		 	*/
	/************************************************/
	
	if((uchar*)p_entry == malloc_break && !p_entry->next)
	{
		//Reduce the break to where the tail piece ends, and erase the old header
		malloc_break = segment_end(p_entry);
		write_seg_header(p_entry, 0, NULL);
		
		//Update freelist
		if(p_entry_prev)
			p_entry_prev->next = NULL;
		else
			freelist_head = NULL;
		
		#ifdef DEBUG
		printf("free: Eliminated new free piece by reducing malloc break to %p\n", malloc_break);
		#endif
	}
}










/************************************************************************/
/*								REALLOC		  							*/
/************************************************************************/

void* my_realloc(void *p, size_t len)
{
	Heap_Seg *p_entry = p - sizeof(Heap_Seg);
	Heap_Seg *old_entry = p_entry;
	
	int size_diff;
	uchar* new_break;
	uchar* retaddr = NULL;
	
	Heap_Seg *current_piece = NULL;
	Heap_Seg *closest_right = NULL;
	
	
	if(!pointer_is_valid(p))
		return NULL;
	
	
	/****************************************/
	/*				Shrinking				*/
	/****************************************/
	
	size_diff = len - p_entry->size;
	
	#ifdef DEBUG
	printf("realloc: Resizing %p, current size %zu. Size difference: %d\n", p_entry, p_entry->size, size_diff);
	#endif
	
	if(size_diff == 0)
		return p;
	
	else if(size_diff < 0)
	{
		size_diff = p_entry->size - len;		//Make size_diff positive
		
		//Don't shrink if the size difference isn't big enough to insert a new segment header
		if(size_diff <= sizeof(Heap_Seg))
		{
			#ifdef DEBUG
			printf("realloc: size difference too insignificant. The piece will not be shrunk.\n");
			#endif
			return p;
		}
		
		//Write a new segment entry above the requested length
		retaddr = (uchar*)p + size_diff;
		write_seg_header(retaddr - sizeof(Heap_Seg), len, NULL);
		
		#ifdef DEBUG
		printf("realloc: New shrunk piece of size %zu at %p\n", len, retaddr - sizeof(Heap_Seg));
		printf("realloc: Freeing the space above the shrunk piece (size %zu at %p)\n", old_entry->size, old_entry);
		#endif
		
		//Rewrite the old segment header and mark it as free
		old_entry->size = size_diff - sizeof(Heap_Seg);
		my_free(p);	
		
		return retaddr;
	}
	
	
	/****************************************/
	/*			Growing In-place			*/
	/****************************************/

	//Iterate the freelist and find the closest free piece on the right to p
	for(current_piece = freelist_head; current_piece; current_piece = current_piece->next)
	{
		if(current_piece < p_entry)
		{
			closest_right = current_piece;
			break;
		}
	}

	
	/*Scenario: The expanding piece has a free chunk directly above itself, and is big enough to merge with*/
	if(closest_right && segment_end(closest_right) == (uchar*)p_entry)
	{	
		//Merging with top piece if it fits exactly
		if(closest_right->size + sizeof(Heap_Seg) == size_diff )
		{
			retaddr = (uchar*)closest_right + sizeof(Heap_Seg);
			
			#ifdef DEBUG
			printf("realloc: Merging with top piece yields exact size. New piece at %p, size %zu\n", closest_right, closest_right->size);
			#endif
		}
		
		//Merging with top piece yields excess free spaces
		else if(closest_right->size >= size_diff)
		{
			retaddr = (uchar*)p - size_diff;
			
			#ifdef DEBUG
			printf("realloc: Planning to split top piece of size %zu at %p for merging\n", closest_right->size, closest_right);
			#endif
			
			//Update the free piece's original entry and reduce its free size
			if(closest_right->size > size_diff)
				closest_right->size -= size_diff;
			
			#ifdef DEBUG
			printf("realloc: Piece 1: size %zu at %p\n", len, retaddr - sizeof(Heap_Seg));
			printf("realloc: Piece 2 (free): size %zu at %p\n", closest_right->size, closest_right);
			#endif
		}
	}
	
	/*Scenario: The expanding piece is at the malloc break*/
	else if((uchar*)p_entry == malloc_break)
	{
		//If the requested is at the malloc break, simply expand it to fulfil the requested length
		retaddr = (uchar*)p - size_diff;
		
		if(!grow_malloc_break(len + sizeof(Heap_Seg)))
			return NULL;
		
		p_entry = (Heap_Seg*)malloc_break;
		
		#ifdef DEBUG
		printf("realloc: Expanding malloc break to %p for growth\n", malloc_break);
		#endif
	}
	
	
	if(retaddr)
	{
		//Write a new segment entry above the requested length
		write_seg_header(p_entry, len, NULL);	
			
		//Wipe the old seg entry, as it's now part of the allocated memory
		old_entry->size = 0;
		old_entry->next = NULL;
			
		return retaddr;	
	}

	
	/****************************************/
	/*		New Allocation for growth		*/
	/****************************************/
	
	//Because it's not possible to expand the current piece in-place, we must use malloc to create a new larger piece
	
	#ifdef DEBUG
	printf("realloc: Cannot grow in-place. Allocating a new piece using malloc...\n");
	#endif
	
	retaddr = my_malloc(len);
	if(!retaddr) 
		return NULL;
	memcpy(retaddr, p, p_entry->size);
	my_free(p);

	return retaddr;
}


#undef MAX_HEAP_SIZE
#undef segment_end
