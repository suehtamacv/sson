#ifndef Heuristics_H
#define Heuristics_H

#include <assert.h>
#include "Def.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

/**
 * @brief The Heuristics class
 * Implements some heuristics related to Dijkstra's routing algorithm link costs.
 */
class Heuristics {
  public:
    /**
     * @brief Heuristics::calculateCostLink calculates the link cost. Depends of how many possibilites there are to allocate the requisition in the link.
     * @param Disp is a vector indicating the availability of the link's slots.
     * @param L is the length of the requisition.
     * @return the inverse of the number of allocation possibilities plus one.
     */
    static long double calculateCostLink(bool *Disp, int L);

    /**
     * @brief Heuristics::calculateCostRouting calculates the cost of the entire routing. depends of how many nodes there are, and how many possibilities there are to allocate the requisition in the link.
     * @param hops is the number of nodes in the route.
     * @param Disp is a vector indicating the availability of the link's slots.
     * @param L is the length of the requisition.
     * @return a ponderation between the number of hops and the number of allocation possibilities.
     */
    static long double calculateCostRouting(int hops, bool *Disp, int L);

    /**
     * @brief Heuristics::calcNumFormAloc calculates how many possibilities there are to allocate the requisition in the link.
     * @param L is the length of the requisition.
     * @param Disp is a vector indicating the availability of the link's slots.
     * @return how many possibilities there are to allocate the requisition in the link.
     */
    static int calc_NumAllocationPossibilities(int L, bool *Disp);
};

#endif
