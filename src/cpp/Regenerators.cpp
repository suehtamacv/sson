#include "Regenerators.h"
#include "Def.h"
#include "AuxiliarMain.h"
#include "RWA.h"
#include "Event.h"

long double eval_OSNR(Route *route, Def *Config, AuxiliarMain *Aux);
Event *insert_Connection(Route *route, int si, int NslotsUsed, long double Time,
                         AuxiliarMain *Aux);
long double simulate_Network(Def *Config, AuxiliarMain *Aux);
int eval_SlotsNeeded(long double BitRate, ModulationScheme Esquema);
void try_ToConnect(const Route *route, const int NslotsReq, int &NslotsUsed,
                   int &si, Def *Config);

int *Regenerators::SQP_LNMax;
long double Regenerators::BR = 100E9;
long double Regenerators::RA_Both_DecisionRatio;
AuxiliarMain *Regenerators::MainAux;

void Regenerators::RP_NDF(int NumberTotalRegenerators,
                          int RegeneratorsPerNode) {
    assert((NumberTotalRegenerators % RegeneratorsPerNode == 0));
    int NumTranslucentNodes = NumberTotalRegenerators / RegeneratorsPerNode;
    int ChosenNode;
    assert((NumTranslucentNodes <= Def::NumNodes));

    vector<int> NodeDegree;

    for (unsigned i = 0; i < Def::Node_Degree.size(); i++) {
        NodeDegree.push_back(Def::Node_Degree.at(i));
    }

    for (int i = 0; i < NumTranslucentNodes ; i++) {
        int MaiorGrau = NodeDegree.at(0);

        for (unsigned j = 0; j < NodeDegree.size(); j++)
            if (MaiorGrau < NodeDegree.at(j)) {
                MaiorGrau = NodeDegree.at(j);
            }

        vector<int> MaximumNodes;

        for (unsigned j = 0; j < NodeDegree.size(); j++)
            if ((NodeDegree.at(j) == MaiorGrau) &&
                    (MainAux->Network.at(j).get_NodeType() == Node::TransparentNode)) {
                MaximumNodes.push_back(j);
            }

        ChosenNode = MaximumNodes.at(floor(General::uniform(0,
                                           MaximumNodes.size())));
        MainAux->Network.at(ChosenNode).set_NodeType(Node::TranslucentNode);
        MainAux->Network.at(ChosenNode).set_NumberRegenerators(RegeneratorsPerNode);

        NodeDegree.at(ChosenNode) = -1;

        for (int j = 0; j < Def::NumNodes; j++) {
            if (AuxiliarMain::Topology[ChosenNode * Def::NumNodes + j] == 1) {
                NodeDegree.at(j) -= 1;
            }
        }
    }
}

void Regenerators::RP_CNF(int NumberTotalRegenerators,
                          int RegeneratorsPerNode) {
    assert((NumberTotalRegenerators % RegeneratorsPerNode == 0));
    int NumTranslucentNodes = NumberTotalRegenerators / RegeneratorsPerNode;
    assert((NumTranslucentNodes <= Def::NumNodes));

    vector <int> NodeCentrality(Def::NumNodes);

    for (unsigned i = 0; i < NodeCentrality.size(); i++) {
        NodeCentrality.at(i) = 0;
    }

    RWA::Dijkstra(MainAux, MainAux->Config);

    for (int orN = 0; orN < Def::NumNodes; orN++) {
        for (int deN = 0; deN < Def::NumNodes; deN++) {
            if (orN == deN) {
                continue;
            }

            for (int No = 0;
                    No <= MainAux->All_Routes[orN * Def::NumNodes + deN].at(0)->get_NumHops();
                    No++) {
                NodeCentrality.at(MainAux->All_Routes[orN * Def::NumNodes + deN].at(
                                      0)->get_Node(No))++;
            }
        }
    }

    for (int i = 0; i < NumTranslucentNodes; i++) {
        int GreatestDegree = NodeCentrality.at(0);

        for (unsigned j = 0; j < NodeCentrality.size(); j++)
            if ((GreatestDegree < NodeCentrality.at(j)) &&
                    (MainAux->Network.at(j).get_NodeType() == Node::TransparentNode)) {
                GreatestDegree = NodeCentrality.at(j);
            }

        vector<int> NosMaximos;

        for (unsigned j = 0; j < NodeCentrality.size(); j++)
            if (NodeCentrality.at(j) == GreatestDegree) {
                NosMaximos.push_back(j);
            }

        int NoEscolhido = NosMaximos.at(floor(General::uniform(0,
                                              NosMaximos.size())));
        MainAux->Network.at(NoEscolhido).set_NodeType(Node::TranslucentNode);
        MainAux->Network.at(NoEscolhido).set_NumberRegenerators(RegeneratorsPerNode);
        NodeCentrality.at(NoEscolhido) = -1;
    }
}

void Regenerators::RP_TLP(int NumberTotalRegenerators,
                          int RegeneratorsPerNode) {
    assert((NumberTotalRegenerators % RegeneratorsPerNode == 0));
    int NumTranslucentNodes = NumberTotalRegenerators / RegeneratorsPerNode;
    assert((NumTranslucentNodes <= Def::NumNodes));

    AuxiliarMain::FlagRP_TLP = true;
    Def *Config = new Def();
    MainAux->laNet = MainAux->LaNet_Min;
    cout << "Running RP-TLP Algorithm... " << flush;
    simulate_Network(Config, MainAux);

    for (int i = 0; i < NumTranslucentNodes; i++) {
        int GreatestDegree = MainAux->RP_TLP_NodeUsage[0];

        for (int j = 0; j < Def::NumNodes; j++)
            if (GreatestDegree < MainAux->RP_TLP_NodeUsage[j]) {
                GreatestDegree = MainAux->RP_TLP_NodeUsage[j];
            }

        vector<int> MaximumNodes;

        for (int j = 0; j < Def::NumNodes; j++)
            if ((MainAux->RP_TLP_NodeUsage[j] == GreatestDegree) &&
                    (MainAux->Network.at(j).get_NodeType() == Node::TransparentNode)) {
                MaximumNodes.push_back(j);
            }

        int chosenNode = MaximumNodes.at(floor(General::uniform(0,
                                               MaximumNodes.size())));
        MainAux->Network.at(chosenNode).set_NodeType(Node::TranslucentNode);
        MainAux->Network.at(chosenNode).set_NumberRegenerators(RegeneratorsPerNode);
        MainAux->RP_TLP_NodeUsage[chosenNode] = -1;
    }

    cout << " Done." << endl;

    AuxiliarMain::FlagRP_TLP = false;
    delete Config;
}

void Regenerators::RP_SQP(int NumberTotalRegenerators,
                          int RegeneratorsPerNode) {
    assert((NumberTotalRegenerators % RegeneratorsPerNode == 0));
    int NumTranslucentNodes = NumberTotalRegenerators / RegeneratorsPerNode;
    assert((NumTranslucentNodes <= Def::NumNodes));

    cout << "Running RP-SQP Algorithm... " << flush;
    SQP_evalLNMax();
    AuxiliarMain::FlagRP_SQP = true;

    //Setting up offline simulation.
    Def *Config = new Def();
    MainAux->laNet = MainAux->LaNet_Min;
    AuxiliarMain::choose_NetworkType = TransparentNetwork;
    simulate_Network(Config, MainAux);
    AuxiliarMain::choose_NetworkType = TranslucentNetwork;

    for (int i = 0; i < NumTranslucentNodes; i++) {
        int GreatestDegree = MainAux->RP_SQP_NodeUsage[0];

        for (int j = 0; j < Def::NumNodes; j++)
            if (GreatestDegree < MainAux->RP_SQP_NodeUsage[j]) {
                GreatestDegree = MainAux->RP_SQP_NodeUsage[j];
            }

        vector<int> MaximumNodes;

        for (int j = 0; j < Def::NumNodes; j++)
            if ((MainAux->RP_SQP_NodeUsage[j] == GreatestDegree) &&
                    (MainAux->Network.at(j).get_NodeType() == Node::TransparentNode)) {
                MaximumNodes.push_back(j);
            }

        int NoEscolhido = MaximumNodes.at(floor(General::uniform(0,
                                                MaximumNodes.size())));
        MainAux->Network.at(NoEscolhido).set_NodeType(Node::TranslucentNode);
        MainAux->Network.at(NoEscolhido).set_NumberRegenerators(RegeneratorsPerNode);
        MainAux->RP_SQP_NodeUsage[NoEscolhido] = -1;
    }

    cout << " Done." << endl;

    AuxiliarMain::FlagRP_SQP = false;
}

void Regenerators::RP_MU(int NumberTotalRegenerators, int RegeneratorsPerNode) {
    assert((NumberTotalRegenerators % RegeneratorsPerNode == 0));
    int NumTranslucidNodes = NumberTotalRegenerators / RegeneratorsPerNode;
    assert((NumTranslucidNodes <= Def::NumNodes));

    AuxiliarMain::FlagRP_MU = true;
    Def *Config = new Def();
    MainAux->laNet = MainAux->LaNet_Min;

    cout << "Running RP-MU Algorithm... " << flush;

    for (int i = 0; i < Def::NumNodes; i++) {
        MainAux->Network.at(i).set_NodeType(Node::TranslucentNode);
    }

    simulate_Network(Config, MainAux);

    for (int i = 0; i < Def::NumNodes; i++) {
        MainAux->Network.at(i).set_NodeType(Node::TransparentNode);
    }

    for (int i = 0; i < NumTranslucidNodes; i++) {
        int GreatestDegree = MainAux->RP_MU_NodeUsage[0];

        for (int j = 0; j < Def::NumNodes; j++)
            if (GreatestDegree < MainAux->RP_MU_NodeUsage[j]) {
                GreatestDegree = MainAux->RP_MU_NodeUsage[j];
            }

        vector<int> MaximumNodes;

        for (int j = 0; j < Def::NumNodes; j++)
            if ((MainAux->RP_MU_NodeUsage[j] == GreatestDegree) &&
                    (MainAux->Network.at(j).get_NodeType() == Node::TransparentNode)) {
                MaximumNodes.push_back(j);
            }

        int NoEscolhido = MaximumNodes.at(floor(General::uniform(0,
                                                MaximumNodes.size())));
        MainAux->Network.at(NoEscolhido).set_NodeType(Node::TranslucentNode);
        MainAux->Network.at(NoEscolhido).set_NumberRegenerators(RegeneratorsPerNode);
        MainAux->RP_MU_NodeUsage[NoEscolhido] = -1;
    }

    cout << " Done." << endl;

    delete Config;
    AuxiliarMain::FlagRP_MU = false;
}

void Regenerators::RP_MSU(int NumberTotalRegenerators) {
    AuxiliarMain::FlagRP_MSU = true;
    Def *Config = new Def();
    MainAux->laNet = MainAux->LaNet_Min;

    cout << "Running RP-MSU Algorithm... " << flush;

    for (int i = 0; i < Def::NumNodes; i++) {
        MainAux->Network.at(i).set_NodeType(Node::TranslucentNode);
    }

    simulate_Network(Config, MainAux);

    long double Sum_SimUsedReg = 0;

    for (int i = 0; i < Def::NumNodes; i++) {
        MainAux->Network.at(i).set_NodeType(Node::TransparentNode);
        Sum_SimUsedReg += MainAux->RP_MSU_NodeUsage[i];
    }

    if (Sum_SimUsedReg != 0) {
        for (int i = 0; i < Def::NumNodes; i++) {
            int NumReg = round(1.0 * NumberTotalRegenerators * MainAux->RP_MSU_NodeUsage[i]
                               / Sum_SimUsedReg);

            if (NumReg != 0) {
                MainAux->Network.at(i).set_NodeType(Node::TranslucentNode);
                MainAux->Network.at(i).set_NumberRegenerators(NumReg);
            } else {
                MainAux->Network.at(i).set_NodeType(Node::TransparentNode);
                MainAux->Network.at(i).set_NumberRegenerators(0);
            }
        }
    }

    cout << " Done." << endl;

    delete Config;
    AuxiliarMain::FlagRP_MSU = false;
}

bool Regenerators::RA_FLR(Route *route, int nBitRate, Def *Config,
                          AuxiliarMain *Aux) {
    assert((nBitRate < Def::get_numPossibleBitRates()));

    Event *evt;

    long double BitRate = Def::Possible_BitRates.at(nBitRate);

    int NeededRegenerators = ceil(BitRate / BR);
    int NoS, NoX;
    int r = 0;
    int s, x;
    int L_or, L_de;
    int NslotsUsed;
    int NslotsReq;
    int SI = -1; //InitialSlot

    vector<int> TransparentSections;
    int numTransparentSections = 0;
    TransparentSections.push_back(route->get_Node(0));

    for (s = 0; s <= route->get_NumHops(); s++) {
        NoS = route->get_Node(s);

        for (x = s + 1; x <= route->get_NumHops(); x++) {
            NoX = route->get_Node(x);    //find the xth route's node

            if ((Aux->Network.at(NoX).get_NumberAvailableRegenerators() >=
                    NeededRegenerators) || (x == route->get_NumHops())) {
                Route rotaQuebrada = route->break_Route(NoS, NoX);

                if (existsSpectrumAndQuality(&rotaQuebrada, BitRate, Config, Aux)) {
                    if (x == route->get_NumHops()) {   //destination
                        TransparentSections.push_back(NoX);

                        //Insert connection
                        long double Time = General::exponential(AuxiliarMain::mu);
                        evt = insert_Connection(route, 0, 1, Time, Aux);

                        int hop = 0;

                        for (int i = 0; i <= numTransparentSections; i++) {
                            SI = -1;
                            NslotsUsed = 0;
                            Route Section = route->break_Route(TransparentSections.at(i),
                                                               TransparentSections.at(i + 1));
                            NslotsReq = eval_SlotsNeeded(BitRate, BestScheme(&Section, BitRate, Config,
                                                         Aux));
                            try_ToConnect(&Section, NslotsReq, NslotsUsed, SI, Config);

                            assert((SI != -1));
                            assert((NslotsReq == NslotsUsed));

                            for (int c = 0; c < Section.get_NumHops(); c++) {
                                evt->conn->setFirstSlot(hop, SI);
                                evt->conn->setLastSlot(hop, SI + NslotsReq - 1);
                                L_or = Section.get_Node(c);
                                L_de = Section.get_Node(c + 1);

                                for (int s = evt->conn->getFirstSlot(hop);
                                        s <= evt->conn->getLastSlot(hop); s++) {
                                    assert((Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or *
                                                               Def::NumNodes + L_de] == false));
                                    Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes +
                                                       L_de] =
                                                           true;
                                    //Slots marked as used
                                }

                                hop++;
                            }

                            if (i != 0) {
                                int NumReg = Aux->Network.at(TransparentSections.at(i)).request_Regenerators(
                                                 BitRate);
                                assert((NumReg != 0));

                                if (AuxiliarMain::FlagRP_MU) {
                                    Aux->RP_MU_NodeUsage[TransparentSections.at(i)] += NumReg;
                                }

                                if (AuxiliarMain::FlagRP_MSU) {
                                    if (Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] < Aux->Network.at(
                                                TransparentSections.at(i)).get_NumberUsedRegenerators()) {
                                        Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] = Aux->Network.at(
                                                    TransparentSections.at(i)).get_NumberUsedRegenerators();
                                    }
                                }

                                evt->Used_Regenerators[ TransparentSections.at(i) ] = NumReg;
                                evt->Used_Regenerators_Number += NumReg;
                            }
                        }

                        return true; //Request Accepted
                    } else {
                        r = x; //Refreshes Regeneration Point
                    }
                } else {
                    if (r != s) {
                        numTransparentSections += 1;
                        TransparentSections.push_back(route->get_Node(r));
                        s = r; //Refresh Source
                        NoS = route->get_Node(s);
                        x = r; //Refresh Test Point
                        NoX = route->get_Node(x);
                    } else {
                        return false; //Blocked Request
                    }
                }
            }
        }
    }

    return false;
}

bool Regenerators::RA_FNS(Route *route, int nBitRate, Def *Config,
                          AuxiliarMain *Aux) {
    assert((nBitRate < Def::get_numPossibleBitRates()));

    Event *evt;
    long double BitRate = Def::Possible_BitRates.at(nBitRate);

    ModulationScheme Schemes[] = {_64QAM, _16QAM, _4QAM}; //Use the Modulation Schemes in this order.

    int NeededRegenerators = ceil(BitRate / BR);
    int NoS, NoX;
    int r = 0;
    int s, x;
    int L_or, L_de;
    int NslotsUsed;
    int NslotsReq;
    int SI = -1; //Initial Slot
    int m = 0; //First Scheme

    vector<int> TransparentSections;
    int numTransparentSections = 0;
    TransparentSections.push_back(route->get_Node(0));

    for (s = 0; s <= route->get_NumHops(); s++) {
        NoS = route->get_Node(s);

        for (x = s + 1; x <= route->get_NumHops(); x++) {
            NoX = route->get_Node(x);    //find the xth route's node

            if ((Aux->Network.at(NoX).get_NumberAvailableRegenerators() >=
                    NeededRegenerators) || (x == route->get_NumHops())) {
                Route rotaQuebrada = route->break_Route(NoS, NoX);

                if (existsSpectrumAndQuality(&rotaQuebrada, BitRate, Config, Aux,
                                             Schemes[m])) {   //there's QoT
                    if (x == route->get_NumHops()) {   //Destination
                        TransparentSections.push_back(NoX);

                        //Insert Connection
                        long double Time = General::exponential(AuxiliarMain::mu);
                        evt = insert_Connection(route, 0, 1, Time, Aux);

                        int hop = 0;

                        for (int i = 0; i <= numTransparentSections; i++) {
                            SI = -1;
                            Route Section = route->break_Route(TransparentSections.at(i),
                                                               TransparentSections.at(i + 1));
                            NslotsReq = eval_SlotsNeeded(BitRate, BestScheme(&Section, BitRate, Config,
                                                         Aux));
                            try_ToConnect(&Section, NslotsReq, NslotsUsed, SI, Config);

                            assert((SI != -1));
                            assert((NslotsReq == NslotsUsed));

                            for (int c = 0; c < Section.get_NumHops(); c++) {
                                evt->conn->setFirstSlot(hop, SI);
                                evt->conn->setLastSlot(hop, SI + NslotsReq - 1);
                                L_or = Section.get_Node(c);
                                L_de = Section.get_Node(c + 1);

                                for (int s = evt->conn->getFirstSlot(hop);
                                        s <= evt->conn->getLastSlot(hop); s++) {
                                    assert((Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or *
                                                               Def::NumNodes + L_de] == false));
                                    Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes +
                                                       L_de] =
                                                           true;
                                    //Slots marked as used
                                }

                                hop++;
                            }

                            if (i != 0) {
                                int NumReg = Aux->Network.at(TransparentSections.at(
                                                                 i)).request_Regenerators(
                                                 BitRate);
                                assert((NumReg != 0));

                                if (AuxiliarMain::FlagRP_MU) {
                                    Aux->RP_MU_NodeUsage[TransparentSections.at(i)] += NumReg;
                                }

                                if (AuxiliarMain::FlagRP_MSU) {
                                    if (Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] < Aux->Network.at(
                                                TransparentSections.at(i)).get_NumberUsedRegenerators()) {
                                        Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] = Aux->Network.at(
                                                    TransparentSections.at(i)).get_NumberUsedRegenerators();
                                    }
                                }

                                evt->Used_Regenerators[ TransparentSections.at(i) ] = NumReg;
                                evt->Used_Regenerators_Number += NumReg;
                            }
                        }

                        return true;
                    } else {
                        if (m != 0) {
                            r = x;
                            s = x;
                            NoS = route->get_Node(s);
                            m = 0;
                            TransparentSections.push_back(route->get_Node(r));
                            numTransparentSections += 1;
                        } else {
                            r = x; //Refresh Regeneration Point
                        }
                    }
                } else {
                    if (r != s) {
                        numTransparentSections += 1;
                        TransparentSections.push_back(route->get_Node(r));
                        s = r; //Refresh Source
                        NoS = route->get_Node(s);
                        x = r; //Refresh Test Point
                        NoX = route->get_Node(x);
                    } else {
                        x -= 1;
                        NoX = route->get_Node(x);
                        m += 1;

                        if (m == num_ModulationSchemes) {
                            return false;  //Blocked.
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Regenerators::RA_Both(Route *route, int nBitRate, Def *Config,
                           AuxiliarMain *Aux) {
    assert((nBitRate < Def::get_numPossibleBitRates()));

    Event *evt;
    long double BitRate = Def::Possible_BitRates.at(nBitRate);

    ModulationScheme Schemes[] = {_64QAM, _16QAM, _4QAM}; //Use the Modulation Schemes in this order.

    int NeededRegenerators = ceil(BitRate / BR);
    int NoS, NoX;
    int r = 0;
    int s, x;
    int L_or, L_de;
    int NslotsUsed;
    int NslotsReq;
    int SI = -1; //Initial Slot
    int m = 0; //First Scheme

    vector<int> TransparentSections;
    int numTransparentSections = 0;
    TransparentSections.push_back(route->get_Node(0));

    for (s = 0; s <= route->get_NumHops(); s++) {
        NoS = route->get_Node(s);

        for (x = s + 1; x <= route->get_NumHops(); x++) {
            NoX = route->get_Node(x);    //find the xth route's node

            if ((Aux->Network.at(NoX).get_NumberAvailableRegenerators() >=
                    NeededRegenerators) || (x == route->get_NumHops())) {
                Route rotaQuebrada = route->break_Route(NoS, NoX);

                if (1.0 * Aux->Network.at(NoX).get_NumberAvailableRegenerators() /
                        AuxiliarMain::RegNumber_PerNode > RA_Both_DecisionRatio) { //use FNS

                    if (existsSpectrumAndQuality(&rotaQuebrada, BitRate, Config, Aux,
                                                 Schemes[m])) {   //there's QoT
                        if (x == route->get_NumHops()) {   //Destination
                            TransparentSections.push_back(NoX);

                            //Insert Connection
                            long double Time = General::exponential(AuxiliarMain::mu);
                            evt = insert_Connection(route, 0, 1, Time, Aux);

                            int hop = 0;

                            for (int i = 0; i <= numTransparentSections; i++) {
                                SI = -1;
                                Route Section = route->break_Route(TransparentSections.at(i),
                                                                   TransparentSections.at(i + 1));
                                NslotsReq = eval_SlotsNeeded(BitRate, BestScheme(&Section, BitRate, Config,
                                                             Aux));
                                try_ToConnect(&Section, NslotsReq, NslotsUsed, SI, Config);

                                assert((SI != -1));
                                assert((NslotsReq == NslotsUsed));

                                for (int c = 0; c < Section.get_NumHops(); c++) {
                                    evt->conn->setFirstSlot(hop, SI);
                                    evt->conn->setLastSlot(hop, SI + NslotsReq - 1);
                                    L_or = Section.get_Node(c);
                                    L_de = Section.get_Node(c + 1);

                                    for (int s = evt->conn->getFirstSlot(hop);
                                            s <= evt->conn->getLastSlot(hop); s++) {
                                        assert((Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or *
                                                                   Def::NumNodes + L_de] == false));
                                        Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes +
                                                           L_de] =
                                                               true;
                                        //Slots marked as used
                                    }

                                    hop++;
                                }

                                if (i != 0) {
                                    int NumReg = Aux->Network.at(TransparentSections.at(
                                                                     i)).request_Regenerators(
                                                     BitRate);
                                    assert((NumReg != 0));
                                    evt->Used_Regenerators[ TransparentSections.at(i) ] = NumReg;
                                    evt->Used_Regenerators_Number += NumReg;

                                    if (AuxiliarMain::FlagRP_MU) {
                                        Aux->RP_MU_NodeUsage[TransparentSections.at(i)] += NumReg;
                                    }

                                    if (AuxiliarMain::FlagRP_MSU) {
                                        if (Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] < Aux->Network.at(
                                                    TransparentSections.at(i)).get_NumberUsedRegenerators()) {
                                            Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] = Aux->Network.at(
                                                        TransparentSections.at(i)).get_NumberUsedRegenerators();
                                        }
                                    }
                                }
                            }

                            return true;
                        } else {
                            if (m != 0) {
                                r = x;
                                s = x;
                                NoS = route->get_Node(s);
                                m = 0;
                                TransparentSections.push_back(route->get_Node(r));
                                numTransparentSections += 1;
                            } else {
                                r = x; //Refresh Regeneration Point
                            }
                        }
                    } else {
                        if (r != s) {
                            numTransparentSections += 1;
                            TransparentSections.push_back(route->get_Node(r));
                            s = r; //Refresh Source
                            NoS = route->get_Node(s);
                            x = r; //Refresh Test Point
                            NoX = route->get_Node(x);
                        } else {
                            x -= 1;
                            NoX = route->get_Node(x);
                            m += 1;

                            if (m == num_ModulationSchemes) {
                                return false;  //Blocked.
                            }
                        }
                    }
                } else {
                    if (existsSpectrumAndQuality(&rotaQuebrada, BitRate, Config, Aux)) {
                        if (x == route->get_NumHops()) {   //destination
                            TransparentSections.push_back(NoX);

                            //Insert connection
                            long double Time = General::exponential(AuxiliarMain::mu);
                            evt = insert_Connection(route, 0, 1, Time, Aux);

                            int hop = 0;

                            for (int i = 0; i <= numTransparentSections; i++) {
                                SI = -1;
                                NslotsUsed = 0;
                                Route Section = route->break_Route(TransparentSections.at(i),
                                                                   TransparentSections.at(i + 1));
                                NslotsReq = eval_SlotsNeeded(BitRate, BestScheme(&Section, BitRate, Config,
                                                             Aux));
                                try_ToConnect(&Section, NslotsReq, NslotsUsed, SI, Config);

                                assert((SI != -1));
                                assert((NslotsReq == NslotsUsed));

                                for (int c = 0; c < Section.get_NumHops(); c++) {
                                    evt->conn->setFirstSlot(hop, SI);
                                    evt->conn->setLastSlot(hop, SI + NslotsReq - 1);
                                    L_or = Section.get_Node(c);
                                    L_de = Section.get_Node(c + 1);

                                    for (int s = evt->conn->getFirstSlot(hop);
                                            s <= evt->conn->getLastSlot(hop); s++) {
                                        assert((Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or *
                                                                   Def::NumNodes + L_de] == false));
                                        Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes +
                                                           L_de] =
                                                               true;
                                        //Slots marked as used
                                    }

                                    hop++;
                                }

                                if (i != 0) {
                                    int NumReg = Aux->Network.at(TransparentSections.at(i)).request_Regenerators(
                                                     BitRate);
                                    assert((NumReg != 0));
                                    evt->Used_Regenerators[ TransparentSections.at(i) ] = NumReg;
                                    evt->Used_Regenerators_Number += NumReg;

                                    if (AuxiliarMain::FlagRP_MU) {
                                        Aux->RP_MU_NodeUsage[TransparentSections.at(i)] += NumReg;
                                    }

                                    if (AuxiliarMain::FlagRP_MSU) {
                                        if (Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] < Aux->Network.at(
                                                    TransparentSections.at(i)).get_NumberUsedRegenerators()) {
                                            Aux->RP_MSU_NodeUsage[TransparentSections.at(i)] = Aux->Network.at(
                                                        TransparentSections.at(i)).get_NumberUsedRegenerators();
                                        }
                                    }
                                }
                            }

                            return true; //Request Accepted
                        } else {
                            r = x; //Refreshes Regeneration Point
                        }
                    } else {
                        if (r != s) {
                            numTransparentSections += 1;
                            TransparentSections.push_back(route->get_Node(r));
                            s = r; //Refresh Source
                            NoS = route->get_Node(s);
                            x = r; //Refresh Test Point
                            NoX = route->get_Node(x);
                        } else {
                            return false; //Blocked Request
                        }
                    }
                }
            }
        }
    }

    return false;

}

bool Regenerators::existsSpectrumAndQuality(Route *route, long double BitRate,
        Def *Config, AuxiliarMain *Aux, ModulationScheme Scheme) {
    int NslotsUsed = 0;
    int NslotsReq;
    int SI = -1; //Initial Slot

    long double OSNR;

    NslotsReq = eval_SlotsNeeded(BitRate, Scheme);
    try_ToConnect(route, NslotsReq, NslotsUsed, SI, Config);
    OSNR = eval_OSNR(route, Config, Aux);

    if ((NslotsUsed != 0) &&
            (OSNR >= Def::get_OSNRThreshold(Scheme, BitRate))) {
        return true;
    }

    return false;
}

bool Regenerators::existsSpectrumAndQuality(Route *route, long double BitRate,
        Def  *Config, AuxiliarMain *Aux) {
    ModulationScheme Schemes[num_ModulationSchemes] = {_64QAM, _16QAM, _4QAM};

    for (int i = 0; i < num_ModulationSchemes; i++) {
        if (Regenerators::existsSpectrumAndQuality(route, BitRate, Config, Aux,
                Schemes[i])) {
            return true;
        }
    }

    return false;
}

ModulationScheme Regenerators::BestScheme(Route *route,
        long double BitRate, Def *Config, AuxiliarMain *Aux) {
    ModulationScheme Schemes[num_ModulationSchemes] = {_64QAM, _16QAM, _4QAM};
    int i;

    for (i = 0; i < num_ModulationSchemes; i++) {
        if (Regenerators::existsSpectrumAndQuality(route, BitRate, Config, Aux,
                Schemes[i])) {
            return Schemes[i];
        }
    }

    assert((0 != 0));
    return (ModulationScheme) - 1;
}

void Regenerators::refresh_Regenerators(AuxiliarMain *Aux) {
    for (int i = 0; i < Def::NumNodes; i++) {
        Aux->Network.at(i).set_NodeType(Node::TransparentNode);
        Aux->Network.at(i).set_NumberRegenerators(0);
    }
}

void Regenerators::SQP_evalLNMax() {
    SQP_LNMax = new int[Def::get_numPossibleBitRates() * num_ModulationSchemes];

    for (int i = 0; i < Def::get_numPossibleBitRates() * num_ModulationSchemes;
            i++) {
        SQP_LNMax[i] = 0;
    }

    Routing_Algorithm RoutingAlg_Backup = AuxiliarMain::choose_RoutingAlgorithm;
    AuxiliarMain::choose_RoutingAlgorithm = Routing_SP;
    RWA::Dijkstra(MainAux, MainAux->Config);

    ModulationScheme Schemes[] = {_4QAM, _16QAM, _64QAM};

    for (int Scheme = 0; Scheme  < num_ModulationSchemes; Scheme++) {
        for (int numBitRate = 0; numBitRate < Def::get_numPossibleBitRates();
                numBitRate++) {
            long double OSNRth = Def::get_OSNRThreshold(Schemes[Scheme],
                                 Def::Possible_BitRates.at(numBitRate));

            //Visit all routes, looking for the route with the greatest number of hops and that is under the OSNRth constraint
            for (int Route = 0; Route < Def::NumNodes * Def::NumNodes; Route++) {
                if (MainAux->All_Routes[Route].empty()) {
                    continue;
                }

                long double OSNR = eval_OSNR(MainAux->All_Routes[Route].at(0), MainAux->Config,
                                             MainAux);

                if (OSNR >= OSNRth &&
                        MainAux->All_Routes[Route].at(0)->get_NumHops() > SQP_LNMax[Scheme *
                                num_ModulationSchemes + numBitRate]) {
                    SQP_LNMax[Scheme * num_ModulationSchemes + numBitRate] =
                        MainAux->All_Routes[Route].at(
                            0)->get_NumHops();
                }
            }
        }
    }

    AuxiliarMain::choose_RoutingAlgorithm = RoutingAlg_Backup;
}

int Regenerators::SQP_getLNMax(ModulationScheme Scheme, int nBitRate) {
    assert((nBitRate < Def::get_numPossibleBitRates()));
    int nScheme;

    switch (Scheme) {
        case _4QAM:
            nScheme = 0;
            break;

        case _16QAM:
            nScheme = 1;
            break;

        case _64QAM:
            nScheme = 2;
            break;
    }

    return SQP_LNMax[nScheme * num_ModulationSchemes + nBitRate];
}

void Regenerators::SQP_evaluateRoute(Route *route, int nBitRate,
                                     AuxiliarMain *Aux) {
    assert((AuxiliarMain::FlagRP_SQP));

    int LNMax = 0;
    ModulationScheme Schemes[] = {_64QAM, _16QAM, _4QAM};

    for (int i = 0; i < num_ModulationSchemes; i++) {
        LNMax = SQP_getLNMax(Schemes[i], nBitRate);

        if (LNMax != 0) {
            break;
        }
    }

    if (LNMax != 0) {
        int *Ij = new int[Def::NumNodes];

        for (int i = 0; i < Def::NumNodes; i++) {
            Ij[i] = 0;
        }

        for (int i = LNMax; i <= route->get_NumHops(); i += LNMax) {
            Ij[ route->get_Node(i) ] = 1;

            if (i != 0) {
                Ij[ route->get_Node(i - 1) ] = 1;
            }

            if (i != route->get_NumHops()) {
                Ij [ route->get_Node(i + 1) ] = 1;
            }
        }

        for (int i = 0; i < Def::NumNodes; i++) {
            Aux->RP_SQP_NodeUsage[i] += Ij[i];
        }

        delete[] Ij;
    }
}

void Regenerators::set_BR(long double Num) {
    BR = Num;
}

long double Regenerators::get_BR() {
    return BR;
}
