#ifndef CONNECTION_H
#define CONNECTION_H

#include <assert.h>
#include "Def.h"
#include "Route.h"

/**
 * @brief The Connection class
 * is constituded by a route, a duration, and a pair initial-final slot to every node in the route.
 */
class Connection {
  public:
    /**
     * @param r is the route where the connection is established.
     * @param si is a vector of NumHops integers, each integer corresponding to the initial slot of the connection, in that link.
     * @param sf is a vector of NumHops integers, each integer corresponding to the final slot of the connection, in that link.
     * @param tD is the duration of the connection.
     */
    Connection(Route r, int *si, int *sf, TIME tD);

    ~Connection();

    /**
     * @param i is the i-th link in the connection.
     * @return the first slot of the i-th link of the connection.
     */
    int getFirstSlot(int i);

    /**
     * @param i is the i-th link in the connection.
     * @return the last slot of the i-th link of the connection.
     */
    int getLastSlot(int i);

    /**
     * @param i is the i-th link in the connection.
     * @param si is the initial slot in the i-th link in the connection.
     */
    void setFirstSlot(int i, int si);

    /**
     * @param i is the i-th link in the connection.
     * @param sf is the final slot in the i-th link in the connection.
     */
    void setLastSlot(int i, int sf);

    /**
     * @return a pointer to the route object over which the connection was established.
     */
    Route *getRoute();

    /**
     * @return the duration of the connection.
     */
    TIME getTimeDesc();

    /**
     * @return the number of hops, or links, in the connection.
     */
    int getNHops();

  private:
    /**
     * @brief Initial_Slots is a vector with the initial slot of the connection in each link of the route.
     */
    int *Initial_Slots;

    /**
     * @brief Last_Slots is a vector with the final slot of the connection in each link of the route.
     */
    int *Last_Slots;
    Route route; //Route
    TIME tDesc;
};

#endif /* CONNECTION_H */
