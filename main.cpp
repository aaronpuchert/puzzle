#include <string>
#include <iostream>
#include "puzzle.h"

int main(int argc, char **argv)
{
	// extract puzzle out of command line
	int nRad = 10;
	if (argc > 2)	// then there is a radix argument
		nRad = atoi(argv[1]);
	Puzzle::Puzz puzz(argv[argc-1], nRad);

	std::cout << "There are " << puzz.DomainSize() << " different letters.\n\n";

	// try all maps
	int nCount=0;
	try {
		Puzzle::MapGen Gen(puzz.DomainSize(), nRad);
		for (int i=0; i<puzz.DomainSize(); ++i)
			std::cout << puzz[i] << ' ';
		std::cout << std::endl;
		do
			if (puzz.Eval(*Gen)) {
				++nCount;
				for (int i=0; i<puzz.DomainSize(); ++i)
					std::cout << Gen[i] << ' ';
				std::cout << std::endl;
			}
		while (Gen.NextMap());
	}
	catch (char *Exc) {
		std::cout << "Error: " << Exc << "\n\n";
	}

	std::cout << nCount << " solutions found.";

	return 0;
}
