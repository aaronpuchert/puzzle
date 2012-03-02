#include <windows.h>
#include <strstream>
#include <fstream>
#include "puzzle.h"

#define CYCLECOUNT

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // extract puzzle out of command line
    char chPuzzle[128];
    int nRad = 10;
    std::istrstream Cmd(lpCmdLine);
    Cmd >> chPuzzle >> nRad;
    Puzzle::Puzz puzz(chPuzzle, nRad);

#ifdef CYCLECOUNT
    __int64 begin, end;
    __asm {
        rdtsc
        mov dword ptr begin, eax
        mov dword ptr [begin+4], edx
    }
#endif

    std::ofstream hOutp;
    hOutp.open("res.txt");
    hOutp << "Lösungen von \"" << chPuzzle << "\" bezüglich Basis " << nRad;
    hOutp << ".\nEs treten " << puzz.DomainSize() << " verschiedene Buchstaben auf.\n\n";

    // try all maps
    int nCount=0;
    try {
        Puzzle::MapGen Gen(puzz.DomainSize(), nRad);
        do
            if (puzz.Eval(*Gen)) {
                ++nCount;
                for (int i=0; i<puzz.DomainSize(); ++i)
                    hOutp << puzz[i] << " = " << Gen[i] << std::endl;
                hOutp << "\n";
            }
        while (Gen.NextMap());
    }
    catch (char *Exc) {
        hOutp << "FEHLER: " << Exc << "\n\n";
    }

#ifdef CYCLECOUNT
    __asm {
        rdtsc
        mov dword ptr end, eax
        mov dword ptr [end+4], edx
    }
    hOutp << nCount << " Lösungen gefunden, " << (end-begin) << " Taktzyklen gebraucht.";
#else
    hOutp << nCount << " Lösungen gefunden.";
#endif

    hOutp.close();
}