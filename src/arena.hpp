#ifndef PUZZLE_ARENA
#define PUZZLE_ARENA

#include <cstddef>

namespace puzzle {

class Arena {
	static constexpr size_t blockSize = 256;
	static constexpr size_t dataSize = blockSize - sizeof(void *);

	struct Block {
		Block(Block *next) : next(next) {}

		Block *next;
		char data[dataSize];
	};

public:
	static constexpr size_t alignment = sizeof(void*);

	Arena() = default;
	Arena(const Arena &) = delete;
	Arena &operator=(const Arena &) = delete;
	~Arena();

	void* allocate(size_t size);

private:
	Block *head = nullptr;
	char *free = nullptr;
};

} // namespace puzzle

inline void* operator new(size_t size, puzzle::Arena &arena)
{
	return arena.allocate(size);
}

inline void operator delete(void*, puzzle::Arena&) {}

#endif
