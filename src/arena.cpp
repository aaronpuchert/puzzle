#include "arena.hpp"
#include <cassert>
#include <new>

namespace puzzle {

Arena::~Arena()
{
	Block *curr = head;
	while (curr) {
		Block *next = curr->next;
		delete curr;
		curr = next;
	}
}

void* Arena::allocate(size_t size)
{
	assert(size <= dataSize);

	// Align.
	size = (size + (alignment - 1)) & ~(alignment - 1);

	if (head) {
		// Try to fit into existing block.
		size_t space = (head->data + dataSize) - free;
		if (size <= space) {
			void *ret = free;
			free += size;
			return ret;
		}
	}

	// Allocate new block.
	head = new Block(head);
	free = head->data;

	// Allocate from new block.
	void* result = free;
	free += size;
	return result;
}

} // namespace puzzle
