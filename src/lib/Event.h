#ifndef EVENT_H
#define EVENT_H

#include "Def.h"
#include "Connection.h"

/**
 * @brief The EventType enum is used to choose the type of event. Can be a Call Requisition (Req) or a Call Descend (Desc).
 */
enum EventType {
    UNKNOWN, Req, Desc
};

/**
 * @brief The Event struct has a ocurring time, a type of event, a connection, a modulation scheme, a pointer to the next event and a vector indicating the number of regenerators used in each node.
 */
struct Event {
    /**
     * @brief time is the instant when the Event ocurred.
     */
    TIME time;

    /**
     * @brief type indicates whether the Event is a Call Requisition or a Requisition Descending.
     */
    EventType type;

    /**
     * @brief nextEvent is a pointer to the next Event, sorted with their times.
     */
    Event *nextEvent;

    /**
     * @brief conn is the Connection related to this Event.
     */
    Connection *conn;

    /**
     * @brief Scheme is the Modulation Scheme used by the Event, but only if it's a transparent network. It is compliance.
     */
    ModulationScheme Scheme;

    /**
     * @brief Used_Regenerators is a vector indicating how many regenerators are being used by this Event in each node of the Topology.
     */
    int *Used_Regenerators;

    /**
     * @brief Used_Regenerators_Number is the number of used regenerators by this Event.
     */
    int Used_Regenerators_Number = 0;

    Event() {
        Used_Regenerators = new int[Def::NumNodes];

        for (int i = 0; i < Def::NumNodes; i++) {
            Used_Regenerators[i] = 0;
        }
    }

    ~Event() {
        delete[] Used_Regenerators;
    }
};

#endif /* EVENT_H */
