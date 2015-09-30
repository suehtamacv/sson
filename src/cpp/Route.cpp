#include "Route.h"
#include "AuxiliarMain.h"

Route::Route(std::vector<Link *> &links) {
    for (unsigned i = 0; i < links.size(); i++) {
        Path.push_back(links.at(i)->get_SourceNode()->whoami);
    }

    Path.push_back(links.back()->get_DestinationNode()->whoami);

    Links = links;
}

int Route::get_DestinationNode() const {
    assert((Path.size() > 0));
    return Path.back();
}

int Route::get_NumHops() const {
    assert((Path.size() > 0));
    return Path.size() - 1;
}

int Route::get_Node(unsigned int p) const {
    assert((Path.size() > 0));
    return Path.at(p);
}

int Route::get_SourceNode() const {
    assert((Path.size() > 0));
    return Path.front();
}

void Route::print_Route() const {
    for (int i = 0; i < (int) Path.size(); i++) {
        std::cout << Path.at(i) << "--";
    }
}

Route Route::break_Route(int OrN, int DeN) {
    int LocOrN = -1, LocDeN = -1;

    for (unsigned i = 0; i < Path.size(); i++) {
        if (OrN == Path.at(i)) {
            LocOrN = i;
        }

        if (DeN == Path.at(i)) {
            LocDeN = i;
        }
    }

    assert((LocOrN != -1));
    assert((LocDeN != -1));
    assert((LocOrN < LocDeN));

    vector <Link *> BrokenRoute_Links;

    for (int i = LocOrN; i < LocDeN; i++) {
        BrokenRoute_Links.push_back(Links.at(i));
    }

    assert((BrokenRoute_Links.back()->get_DestinationNode()->whoami == DeN));

    return Route(BrokenRoute_Links);
}

std::vector<Link *> *Route::get_Nodes() {
    return &Links;
}

long double Route::get_Length() {
    long double Length = 0;

    for (unsigned i = 0; i < Links.size(); i++) {
        Length += Links.at(i)->get_Length();
    }

    return Length;
}
