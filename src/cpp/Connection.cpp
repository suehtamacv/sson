#include "Connection.h"

Connection::Connection(Route r, int *si, int *sf, TIME tD) : route(r),
    tDesc(tD) {
    Initial_Slots = new int[route.get_NumHops()];

    if (si != NULL)
        for (int i = 0; i < route.get_NumHops(); i++) {
            Initial_Slots[i] = si[i];
        }

    Last_Slots = new int[route.get_NumHops()];

    if (sf != NULL)
        for (int i = 0; i < route.get_NumHops(); i++) {
            Last_Slots[i] = sf[i];
        }
}

Connection::~Connection() {
    delete[] Initial_Slots;
    delete[] Last_Slots;
}

int Connection::getFirstSlot(int i) {
    assert((i < route.get_NumHops()));
    return Initial_Slots[i];
}

int Connection::getLastSlot(int i) {
    assert((i < route.get_NumHops()));
    return Last_Slots[i];
}

Route *Connection::getRoute() {
    return &route;
}

TIME Connection::getTimeDesc() {
    return tDesc;
}

int Connection::getNHops() {
    return route.get_NumHops();
}

void Connection::setFirstSlot(int i, int si) {
    assert((i < route.get_NumHops()));
    assert((si < Def::SL));
    Initial_Slots[i] = si;
}

void Connection::setLastSlot(int i, int sf) {
    assert((i < route.get_NumHops()));
    assert((sf < Def::SL));
    assert((Initial_Slots[i] <= sf));

    Last_Slots[i] = sf;
}
