#include "OSNR_Tree.h"

bool OSNR_Tree::use_Tree = true;

OSNR_Tree::OSNR_Tree() {
    PrincipalBranch.clear();

    for (int i = 0; i < Def::NumNodes; i++) {
        PrincipalBranch[i] = new Branch(i);
    }
}

bool OSNR_Tree::check_inFamily(int Node, Branch *G) {
    assert((Node < Def::NumNodes));

    while (G != NULL) {
        if (G->whoami == Node) {
            return true;
        } else {
            G = G->Father_Branch;
        }
    }

    return false;
}

long double OSNR_Tree::locate_OSNR_in_Tree(const Route *R, int Hop,
        Branch *G) {
    assert((Hop <= R->get_NumHops()));

    if (Hop == R->get_NumHops()) {   //Destination
        assert(G->whoami == R->get_DestinationNode());
        return G->OSNR;
    } else {
        int NextNode = R->get_Node(Hop + 1);

        if (!G->Leaves.count(NextNode)) {      //Leaf doesn't exist.
            G->Leaves[NextNode] = new Branch(NextNode);
        }

        return locate_OSNR_in_Tree(R, Hop + 1, G->Leaves[NextNode]);
    }
}

long double OSNR_Tree::get_OSNR(const Route *R) {
    return locate_OSNR_in_Tree(R, 0, PrincipalBranch[R->get_SourceNode()]);
}

void OSNR_Tree::refresh_OSNR(const Route *R, long double OSNR) {
    Branch *G = PrincipalBranch[R->get_SourceNode()];
    int Hop = 0, NextNode;

    while (G->whoami != R->get_DestinationNode()) {
        NextNode = R->get_Node(++Hop);
        G = G->Leaves[NextNode];
    }

    assert((Hop == R->get_NumHops()));
    G->OSNR = OSNR;
}

void OSNR_Tree::reset_Tree() {
    for (int i = 0; i < Def::NumNodes; i++) {
        reset_Branch(PrincipalBranch[i]);
    }
}

void OSNR_Tree::reset_Branch(Branch *G) {
    G->OSNR = Def::MAX_LONGDOUBLE;

    if (!G->Leaves.empty()) {
        for (map<int, Branch *>::iterator iter = G->Leaves.begin();
                iter != G->Leaves.end(); ++iter) {
            reset_Branch(iter->second);
        }
    }
}
