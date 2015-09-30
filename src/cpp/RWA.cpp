#include "RWA.h"
#include "AuxiliarMain.h"

long double eval_OSNR(Route *, Def *, AuxiliarMain *);
int eval_SlotsNeeded(long double BitRate, ModulationScheme Scheme);
#define PI acos(-1)

bool RWA::check_SlotAvailability(const Route *route, const int s,
                                 Def *Config) {
    assert((s >= 0 && s < Def::SL));
    int L_or, L_de;

    for (int c = 0; c < route->get_NumHops(); c++) {
        L_or = route->get_Node(c);
        L_de = route->get_Node(c + 1);

        if (Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes
                               +
                               L_de] == true) {
            return false;
        }
    }

    return true; //Slot s is available in route
}

void RWA::Dijkstra(AuxiliarMain *Aux, Def *Config, int orN, int deN,
                   int nBitRate, ModulationScheme Scheme) {
    if (orN == -1 && deN == -1) {
        assert((AuxiliarMain::choose_RoutingAlgorithm == Routing_MH ||
                AuxiliarMain::choose_RoutingAlgorithm == Routing_SP));

        for (orN = 0; orN < Def::NumNodes; orN++) {
            for (deN = 0; deN < Def::NumNodes; deN++) {
                if (orN == deN) {
                    continue;
                }

                Dijkstra(Aux, Config, orN, deN);
            }
        }
    } else {
        int VA, i, j, k = 0, path, h, hops;
        long double min;
        vector<Link *> r;
        long double *VertexCost = new long double[Def::NumNodes];
        int *Precedent = new int[Def::NumNodes];
        int *ReversePath = new int[Def::NumNodes];
        bool *Status = new bool[Def::NumNodes];

        //Search for every SourceDestination pair the shortest path
        for (i = 0; i < Def::NumNodes; i++) {
            if (i != orN) {
                VertexCost[i] = Def::MAX_DOUBLE;
            } else {
                VertexCost[i] = 0.0;
            }

            Precedent[i] = -1;
            Status[i] = 0;
        }

        VA = Def::NumNodes;

        while (VA > 0) {
            min = Def::MAX_DOUBLE;

            for (i = 0; i < Def::NumNodes; i++)
                if ((Status[i] == 0) && (VertexCost[i] < min)) {
                    min = VertexCost[i];
                    k = i;
                }

            Status[k] = 1;
            VA = VA - 1;

            for (j = 0; j < Def::NumNodes; j++) {
                if ((Status[j] == 0) && (AuxiliarMain::Topology[k * Def::NumNodes + j] != 0)) {
                    long double TempWeight = get_LinksWeight(k, j, Config, nBitRate, Scheme, Aux);

                    if (VertexCost[k] + TempWeight < VertexCost[j]) {
                        VertexCost[j] = VertexCost[k] + TempWeight;
                        Precedent[j] = k;
                    }
                }
            }
        }

        path = orN * Def::NumNodes + deN;

        while (!Aux->All_Routes[path].empty()) {
            delete Aux->All_Routes[path].back();
            Aux->All_Routes[path].pop_back();
        }

        vector<Route *> ().swap(Aux->All_Routes[path]);

        if (deN != orN) {
            ReversePath[0] = deN;
            hops = 0;
            j = deN;

            while (j != orN) {
                hops = hops + 1;
                ReversePath[hops] = Precedent[j];
                j = Precedent[j];
            }

            r.clear();

            for (h = 0; h < hops; h++) {
                r.push_back(&Aux->LinkSet[ReversePath[hops - h]].at(ReversePath[hops - h - 1]));
            }

            Aux->All_Routes[path].push_back(new Route(r));
        }

        delete []VertexCost;
        delete []Precedent;
        delete []Status;
        delete []ReversePath;
    }
}

void RWA::FirstFit(const Route *route, const int NslotsReq, int &NslotsUsed,
                   int &si, Def *Config) {
    si = -1;
    NslotsUsed = 0;
    int sum;

    for (int s = 0; s <= Def::SL - NslotsReq; s++) {
        sum = 0;

        for (int se = s; se < s + NslotsReq; se++) {
            if (check_SlotAvailability(route, se, Config)) {
                sum++;
            } else {
                break;
            }
        }

        if (sum == NslotsReq) { //allocated
            si = s;
            NslotsUsed = NslotsReq;
            break;
        }
    }
}

void RWA::MostUsed(const Route *route, const int NslotsReq, int &NslotsUsed,
                   int &si, Def *Config) {
    int *vetSlotsUsed = new int[Def::SL];
    bool *vetDisp = new bool[Def::SL];

    for (int s = 0; s < Def::SL; s++) {
        vetDisp[s] = RWA::check_SlotAvailability(route, s, Config);
    }

    int sum;

    for (int s = 0; s < Def::SL; s++) {
        if (vetDisp[s] == true) {
            vetSlotsUsed[s] = sumOccupation(s, Config);
        } else {
            vetSlotsUsed[s] = -1;
        }
    }

    bool fit;
    double maxSum = -1; //Smax*N*N+1;
    NslotsUsed = 0;
    si = -1;

    for (int s = 0; s <= Def::SL - NslotsReq; s++) {
        fit = true;
        sum = 0;

        for (int b = s; b < s + NslotsReq; b++)
            if (vetDisp[b] == true) {
                assert((vetSlotsUsed[b] >= 0));
                sum += vetSlotsUsed[b];
            } else {
                fit = false;
                break;
            }

        if (fit == true &&
                sum > maxSum) {
            maxSum = sum;
            NslotsUsed = NslotsReq;
            si = s;
        }
    }

    delete []vetSlotsUsed;
    delete []vetDisp;
}

void RWA::Random(const Route *route, const int NslotsReq, int &NslotsUsed,
                 int &si, Def *Config) {
    int sum = 0, AssignmentSum = 0, allocateSlot;
    bool *vetDisp = new bool[Def::SL];
    int *vetAloc = new int[Def::SL];

    for (int s = 0; s < Def::SL; s++) {
        vetAloc[s] = 0;
    }

    for (int s = 0; s < Def::SL; s++) {
        vetDisp[s] = check_SlotAvailability(route, s, Config);
    }

    bool fit;
    NslotsUsed = 0;
    si = -1;

    for (int s = 0; s <= Def::SL - NslotsReq; s++) {
        fit = true;
        sum = 0;

        for (int b = s; b < s + NslotsReq; b++)
            if (vetDisp[b] == true) {
                sum++;
            } else {
                fit = false;
                break;
            }

        if (fit == true && sum == NslotsReq) {
            AssignmentSum++;
            vetAloc[s] = 1;
        }
    }

    if (AssignmentSum > 0) {
        allocateSlot = rand() % AssignmentSum;
        int s = 0;

        while ((s < Def::SL) && (allocateSlot >= 0)) {
            if (vetAloc[s] == 1) {
                allocateSlot--;
            }

            s++;
        }

        NslotsUsed = NslotsReq;
        si = s - 1;
    } else {
        NslotsUsed = 0;
        si = -1;
    }

    delete []vetDisp;
    delete []vetAloc;
}

int RWA::sumOccupation(int s, Def *Config) {
    int soma = 0;

    for (int origem = 0; origem < Def::NumNodes; origem++)
        for (int destino = 0; destino < Def::NumNodes; destino++)
            if ((AuxiliarMain::Topology[origem * Def::NumNodes + destino] > 0.0) &&
                    (Config->Topology_S[s * Def::NumNodes * Def::NumNodes + origem * Def::NumNodes +
                                        destino] == true)) {
                soma++;
            }

    return soma;
}

void RWA::OSNRR(AuxiliarMain *Aux) {
    /*Implements OSNR-R*/
    int orN, deN;
    long double *BestOSNR = new long double[Def::NumNodes * Def::NumNodes];
    static vector<Node *> Visited;

    for (orN = 0; orN < Def::NumNodes; orN++) {
        for (deN = 0; deN < Def::NumNodes; deN++) {
            if (orN == deN) {
                continue;
            }

            cout << endl << "[orN=" << orN << "  deN=" << deN << "]  route = ";
            BestOSNR[Def::NumNodes * orN + deN] = 0;
            search_Route(&Aux->Network.at(orN), &Aux->Network.at(orN),
                         &Aux->Network.at(deN),
                         &Visited, BestOSNR, AuxiliarMain::Config, Aux);

            if (Aux->All_Routes[Def::NumNodes * orN + deN].at(0)->get_NumHops() != 0) {
                cout << Aux->All_Routes[Def::NumNodes * orN + deN].at(0)->get_NumHops() <<
                     " hops: ";

                for (int h = 0;
                        h <= Aux->All_Routes[Def::NumNodes * orN + deN].at(0)->get_NumHops();
                        h++) {
                    cout << Aux->All_Routes[Def::NumNodes * orN + deN].at(0)->get_Node(h) << "-";
                }
            }
        }
    }

    cout << endl;
}

void RWA::search_Route(Node *orN, Node *Current, Node *deN,
                       std::vector<Node *> *Visited, long double *BestOSNR, Def *Config,
                       AuxiliarMain *Aux) {
    Visited->push_back(Current);

    if (Current->whoami == deN->whoami) {   //Found route
        vector <Link *> Route_Links;

        for (unsigned i = 1; i < Visited->size(); i++) {
            Route_Links.push_back(&Aux->LinkSet[Visited->at(i - 1)->whoami].at(Visited->at(
                                      i)->whoami));
        }

        Route *R = new Route(Route_Links);
        long double OSNRRota = eval_OSNR(R, Config, Aux);
        int path = orN->whoami * Def::NumNodes + deN->whoami;

        if (OSNRRota > BestOSNR[path]) {   //route has better OSNR
            BestOSNR[path] = OSNRRota;

            while (!Aux->All_Routes[path].empty()) {
                delete Aux->All_Routes[path].back();
                Aux->All_Routes[path].pop_back();
            }

            vector<Route *> ().swap(Aux->All_Routes[path]);
            Aux->All_Routes[path].push_back(new Route(Route_Links));
        }

        delete R;
    }

    for (int i = 0; i < Def::NumNodes; i++) {
        if ((AuxiliarMain::Topology[(Current->whoami) * Def::NumNodes + i] == 1) &&
                !(verify_Inclusion(&Aux->Network.at(i), Visited))) {
            search_Route(orN, &Aux->Network.at(i), deN, Visited, BestOSNR, Config,
                         Aux);
        }
    }

    Visited->pop_back(); //Removes node from list
}

bool RWA::verify_Inclusion(Node *No, std::vector<Node *> *Visitados) {
    for (unsigned i = 0; i < Visitados->size(); i++) {
        if (No->whoami == Visitados->at(i)->whoami) {
            return true;
        }
    }

    return false;
}

long double RWA::eval_Noise(Link Enl, Def *Config, AuxiliarMain *Aux) {
    vector <Link *> Links;
    Links.push_back(&Enl);
    Route R(Links);    //Creates dummy route

    long double OSNR = eval_OSNR(&R, Config, Aux);
    long double Noise = Config->get_Pin() / General::dBtoLin(OSNR);
    return Noise;
}

long double RWA::get_LinksWeight(int orN, int deN, Def *Config,
                                 int nBitRate, ModulationScheme Scheme, AuxiliarMain *) {

    assert((AuxiliarMain::Topology[orN * Def::NumNodes + deN] == 1));
    int Length = -1;
    bool DispLink[Def::SL];

    if (AuxiliarMain::choose_RoutingAlgorithm  == Routing_LOR_AlPos ||
            AuxiliarMain::choose_RoutingAlgorithm == Routing_LOR_A) {
        for (int s = 0; s < Def::SL; s++) {
            DispLink[s] = !Config->Topology_S[s * Def::NumNodes * Def::NumNodes + orN *
                                              Def::NumNodes + deN];
        }
    }

    if (nBitRate != -1) {
        Length = eval_SlotsNeeded(Def::Possible_BitRates.at(nBitRate), Scheme);
    }

    switch (AuxiliarMain::choose_RoutingAlgorithm) {
        case Routing_MH:
            return 1.0;

        case Routing_SP:
            return AuxiliarMain::LinkSet[orN].at(deN).get_Length();

        case Routing_LOR_AlPos:
            return AuxiliarMain::LinkSet[orN].at(deN).get_Length() / Link::get_LongestLink()
                   +
                   Heuristics::calculateCostLink(DispLink, Length) + 1;

        case Routing_LOR_A:
            return AuxiliarMain::LinkSet[orN].at(deN).get_Length() / Link::get_LongestLink()
                   -
                   Heuristics::calc_NumAllocationPossibilities(1, DispLink) / Def::SL + 1;

        case _Routing_Null:
            assert((0 != 0));
            break;

        default:
            assert((0 != 0));
    }

    return -1;
}
