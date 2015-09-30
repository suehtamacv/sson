#include "Heuristics.h"
#include "limits.h"

long double Heuristics::calculateCostLink(bool *Disp, int L) {
    int numFormsL = Heuristics::calc_NumAllocationPossibilities(L, Disp);
    return (long double) 1.0 / (numFormsL + 1.0);
}

long double Heuristics::calculateCostRouting(int hops, bool *Disp, int L) {
    long double alpha = 0.9999; //de onde saiu?
    int numFormsL = Heuristics::calc_NumAllocationPossibilities(L, Disp);

    if (numFormsL == 0) {
        return Def::MAX_LONGDOUBLE / 2.0;  //infinito
    }

    return (long double) alpha * hops + (1.0 - alpha) * ((long double) 1.0 /
            (numFormsL + 1.0));
}

int Heuristics::calc_NumAllocationPossibilities(int L, bool *Disp) {
    int NumFormas = 0, SlotInicial, Slot;

    for (SlotInicial = 0; SlotInicial <= Def::SL - L; SlotInicial++) {
        for (Slot = SlotInicial; Slot < SlotInicial + L;
                Slot++)   //Checa se os slots [SlotInicial : SlotInicial+L] estao disponiveis
            if (Disp[Slot] == false) {   //Slot ocupado
                break;
            }

        if (Slot == SlotInicial + L) {   // Os slots estao disponiveis
            NumFormas++;
        }
    }

    return NumFormas;
}
