#ifndef Route_H
#define Route_H

#include <assert.h>
#include <iostream>
#include <vector>
#include "Link.h"

/**
 * @brief The Route class implements a route between two nodes in the network.
 */
class Route {
  public:
    /**
     * @brief Route is the Route's constructor.
     * @param path is a vector of pointers to Links.
     */
    Route(std::vector<Link *> &path);
    /**
     * @return the Destination Node's id.
     */
    int get_DestinationNode() const;
    /**
     * @return the number of links in the route.
     */
    int get_NumHops() const;
    /**
     * @return the id of the i-th node of the route.
     * @param i is the i-th node.
     */
    int get_Node(unsigned int i) const;
    /**
     * @return the Origin Node's id.
     */
    int get_SourceNode() const;
    /**
     * @brief print_Route prints the ids of all nodes in the route.
     */
    void print_Route() const;
    /**
     * @return the total length of the route.
     */
    long double get_Length();
    /**
     * @brief break_Route breaks a route in a subroute.
     * @param OrN is the first node of the subroute.
     * @param DeN is the last node of the subroute.
     * @return is a route that also is a subset of the original route.
     */
    Route break_Route(int OrN, int DeN);
    /**
     * @return the vector of nodes of the route.
     */
    std::vector<Link *> *get_Nodes();
  private:
    std::vector<int> Path;
    std::vector<Link *> Links;
};

#endif
