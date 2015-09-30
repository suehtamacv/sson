#ifndef OSNRTREE_H
#define	OSNRTREE_H

#include "Route.h"
#include <vector>
#include <map>

using namespace std;

/**
 * @brief The OSNR_Tree class caches the OSNR in the destination nodes.
 *
 * OSNR in the Destinations's node only depends of the route, not depending of the networks state. It may be cached.
 */
class OSNR_Tree {
  private:
    struct Branch {
        int whoami;
        long double OSNR;
        Branch *Father_Branch;
        map<int, Branch *> Leaves;

        Branch(int i) : whoami(i) {
            Father_Branch = NULL;
            Leaves.clear();
            OSNR = Def::MAX_LONGDOUBLE;
        }
    };

  public:
    OSNR_Tree();

    /**
     * @brief get_OSNR returns the OSNR in the destination node.
     * @param R is the route in which the OSNR is being evaluated.
     * @return the OSNR in the destination node.
     */
    long double get_OSNR(const Route *R);

    /**
     * @brief refresh_OSNR caches the OSNR evaluated in the node R.
     * @param R is the route in which the OSNR will be cached.
     * @param OSNR is the evaluated OSNR.
     */
    void refresh_OSNR(const Route *R, long double OSNR);

    /**
     * @brief reset_Tree cleans the entire route.
     */
    void reset_Tree();
    static bool use_Tree;
  private:
    map<int, Branch *> PrincipalBranch;

    /**
     * @brief check_inFamily verifies whether the Node is already visited in this subtree.
     * @return true if the Node is already in the subtree.
     */
    bool check_inFamily(int Node, Branch *G);

    /**
     * @brief locate_OSNR_in_Tree traverses the tree looking for the OSNR in the OSNR_Tree.
     * @param R is the route in which the OSNR will be evaluated.
     * @return the OSNR of the route, or Def::MAX_LONGDOUBLE if it hasn't been already cached.
     */
    long double locate_OSNR_in_Tree(const Route *R, int Hop, Branch *G);
    void reset_Branch(Branch *G);
};

#endif	/* OSNRTREE_H */

