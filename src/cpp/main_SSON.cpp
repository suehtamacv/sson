/**
  * @file main_SSON.cpp Is where the principal functions are.
  **/

#include <assert.h>
#include <climits>
#include <fstream>
#include <iostream>
#include <limits>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "ASE_Noise.cpp"
#include "AuxiliarMain.h"
#include "Connection.h"
#include "Def.h"
#include "General.h"
#include "Heuristics.h"
#include "Link.h"
#include "OSNR_Tree.h"
#include "Metrics.cpp"
#include "Node.h"
#include "Regenerators.h"
#include "Route.h"
#include "RWA.h"
#include "MainWindow.h"
#include "SSON_Thread.h"
#include <thread>
#include <QApplication>
#include <QDesktopWidget>

using namespace std;

//Prototypes
void account_Blocking(int NslotsReq, int NslotsUsed, int nTaxa, Def *Config);
bool check_FitSi(const bool *vetDisp, int s, int NslotsReq);
void clear_Memory(Def *Config, AuxiliarMain *Aux);
void create_Structures();
void define_NextEventOfConnection(Event *evt);
void eval_NodeDegrees(Def *Config);
int eval_SlotsNeeded(long double BitRate, ModulationScheme Scheme);
void exec_Interface();
void find_Multiplier(Def *Config, AuxiliarMain *Aux);
void implement_TranslucentNetwork();
Event *insert_Connection(Route *route, int si, int NslotsUsed,
                         long double Tempo,
                         AuxiliarMain *Aux);
void load();
void load_Coefficients();
void refresh_Noise(Def *Config);
void remove_Conn(Event *, Def *Config, AuxiliarMain *Aux);
int request_BitRate();
void request_Conn(Event *, Def *Config, AuxiliarMain *MainAux);
void request_SourceDestinationPair(int &orN, int &deN);
void schedule_Event(Event *, AuxiliarMain *Aux);
void set_RequestEvent(Event *, TIME);
void Sim(AuxiliarMain *Aux);
void SimAmpDist(Def *Config, AuxiliarMain *Aux);
void SimOSNR(AuxiliarMain *Aux);
void SimPbReq(AuxiliarMain *Aux);
void SimRegeneratorNumber();
void SimSlotsNum(AuxiliarMain *Aux);
void SimWhiskers();
long double simulate_Network(Def *Config, AuxiliarMain *MainAux);
void Simulate(Def *Config, AuxiliarMain *Aux);
void try_ToConnect(const Route *route, const int NslotsReq, int &NslotsUsed,
                   int &si, Def *Config);

/**
 * @brief Aux is a standard AuxiliarMain object using through several functions.
 */
AuxiliarMain *Aux;

/**
 * @brief main implements the Threads, and calls a textual or graphic interface depending on the parameters passed.
 */
int main(int argc, char *argv[]) {
    bool Run = true;
    AuxiliarMain::setFiles();
    AuxiliarMain::TheTextualThread = new SSON_Thread(SSON_Thread::Textual_Thread);
    AuxiliarMain::TheGraphicThread = new SSON_Thread(SSON_Thread::Graphic_Thread);
    AuxiliarMain::SupremeFlag = true;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-nw") == 0) {
            AuxiliarMain::isGraphic = _NO;
        } else if (strcmp(argv[i], "-w") == 0) {
            AuxiliarMain::isGraphic = _YES;
        } else if (strcmp(argv[i], "-lang=en") == 0) {
            AuxiliarMain::Translation = Translation_English;
        } else if (strcmp(argv[i], "-lang=pt") == 0) {
            AuxiliarMain::Translation = Translation_Portuguese;
        } else if (strcmp(argv[i], "-lang=fr") == 0) {
            AuxiliarMain::Translation = Translation_French;
        } else if (strcmp(argv[i], "-h") == 0) {
            cout << "\t\tSSON - Simulator for Slice Optical Networks" << endl << endl <<
                 "Usage:\tsson [options]" << endl << endl <<
                 "Options:" << endl <<
                 "\t-nw\t\tCommand-line version" << endl  <<
                 "\t-w\t\tGUI version" << endl <<
                 "\t-lang=LANG\ttranslates the SSON, see language options" << endl <<
                 "\t-h\t\tShow this help text" << endl << endl <<
                 "Languages:" << endl <<
                 "\t-lang=en\tEnglish" << endl <<
                 "\t-lang=pt\tPortuguese" << endl <<
                 "\t-lang=fr\tFrench" << endl;
            Run = false;
            break;
        } else {
            cout << "Invalid command " << argv[i] << "\nTry running with -h option.\n";
            Run = false;
            break;
        }
    }

    if (AuxiliarMain::isGraphic == _YES && Run) {
        AuxiliarMain::TheGraphicThread->run(argc, argv);
    } else if (AuxiliarMain::isGraphic == _NO && Run) {
        AuxiliarMain::TheTextualThread->run(argc, argv);
    }

    return 0;
}

/**
 * @brief exec_Interface initiates the simulation cascade.
 *
 * It loads the inserted values, create the needed structures to the simulation, and calls a specific function depending on what simulation will be run. Then it cleans after itself.
 */
void exec_Interface() {
    if (AuxiliarMain::isGraphic == _NO) {
        load();
    } else if (AuxiliarMain::isGraphic == _YES) {
        AuxiliarMain::window->GraphicInterface_LoadValues(Aux);
    }

    AuxiliarMain::Tree = new OSNR_Tree();
    cout << "Starting simulation:" << endl;
    create_Structures();

    if (AuxiliarMain::choose_RoutingAlgorithm == Routing_MH ||
            AuxiliarMain::choose_RoutingAlgorithm == Routing_SP) {
        RWA::Dijkstra(Aux, Aux->Config);
    } else if (AuxiliarMain::choose_RoutingAlgorithm == Routing_OSNRR) {
        RWA::OSNRR(Aux);
    }

    implement_TranslucentNetwork();

    if (AuxiliarMain::choose_Simulation == Sim_PbReq) {
        SimPbReq(Aux);
    } else if (AuxiliarMain::choose_Simulation == Sim_OSNR) {
        SimOSNR(Aux);
    } else if (AuxiliarMain::choose_Simulation == Sim_AmpDist) {
        SimAmpDist(AuxiliarMain::Config, Aux);
    } else if (AuxiliarMain::choose_Simulation == Sim_SlotsNum) {
        SimSlotsNum(Aux);
    } else if (AuxiliarMain::choose_Simulation == Sim_Whiskers) {
        SimWhiskers();
    } else if (AuxiliarMain::choose_Simulation == Sim_RegenNum) {
        SimRegeneratorNumber();
    }

    delete []AuxiliarMain::Topology;
    delete []AuxiliarMain::LinkSet;
    delete AuxiliarMain::Tree;

    cout << "Simulation has ended.";
    cin.get();
    cin.get();
}

/**
 * @brief account_Blocking accounts for a Blocked Requisition.
 * @param NslotsReq is the number of required slots.
 * @param NslotsUsed is the number of used slots. If the Requisition was blocked, should be zero.
 * @param nBitRate is an integer representing the position of the Bitrate in Def::Possible_BitRates.
 * @param Config is the set of parameters over which the simulation happened.
 */
void account_Blocking(int NslotsReq, int NslotsUsed, int nBitRate,
                      Def *Config) {
    if (NslotsUsed <= 0) {   //Connection has been blocked
        Config->num_ReqBloq++;
        Config->num_ReqBloqBitRate[nBitRate]++;
    }

    Config->num_SlotsBloq += (NslotsReq - NslotsUsed);
}

/**
 * @brief check_FitSi checks whether the connection fits in [s:s+NslotsReq].
 * @param vetDisp is a vector indicating whether the slots are or not available.
 * @param s is the initial slot.
 * @param NslotsReq is the number of required slots.
 * @return true iff the connection fits in [s:s+NslotsReq].
 */
bool check_FitSi(const bool *vetDisp, int s, int NslotsReq) {
    assert((s + NslotsReq - 1 < Def::SL));

    for (int b = s; b < s + NslotsReq; b++)
        if (vetDisp[b] == false) {
            return false;
        }

    return true;
}

/**
 * @brief clear_Memory call destructors and reset variables.
 * @param Config is the set of parameters over which the simulation happened.
 * @param Aux is the set of parameters over which the simulation happened.
 */
void clear_Memory(Def *Config, AuxiliarMain *Aux) {
    int s, L_or, L_de;
    Connection *con;
    Event *evtPtr;
    const Route *route;
    Config->num_Req = Config->num_ReqBloq = Config->num_SlotsReq =
            Config->num_SlotsBloq = Config->num_HopsPerRoute = Config->net_Occupancy =
                                        Aux->sim_Time = Config->num_ReqBloqDueToQOT = Config->num_SlotsBloqDueToQOT =
                                                Config->TotalData = 0.0;

    while (Aux->first_Event != NULL) {
        if (Aux->first_Event->conn != NULL) {
            //There's a connection
            con = Aux->first_Event->conn;
            route = con->getRoute();

            for (int c = 0; c < route->get_NumHops(); c++) {
                //release all occupied slots
                L_or = route->get_Node(c);
                L_de = route->get_Node(c + 1);

                for (s = con->getFirstSlot(c); s <= con->getLastSlot(c); s++) {
                    Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes +
                                       L_de] = false;
                }
            }

            delete con;
        }

        evtPtr = Aux->first_Event;
        Aux->first_Event = Aux->first_Event->nextEvent;
        delete evtPtr;
    }

    for (int i = 0; i < Def::get_numPossibleBitRates(); i++) {
        Config->num_ReqBloqBitRate[i] = 0;
        Config->num_ReqBitRate[i] = 0;
        Config->TotalTime_Scheme[i] = 0;
    }

    for (int i = 0; i < num_ModulationSchemes; i++) {
        Config->num_ReqAccepScheme[i] = 0;
        Config->TotalData_Scheme[i] = 0;
    }

    if (AuxiliarMain::choose_NetworkType == TranslucentNetwork)
        for (int i = 0; i < Def::NumNodes; i++) {
            Aux->Network.at(i).set_NumberRegenerators(Aux->Network.at(
                        i).get_NumberRegenerators());
        }

    //Checks whether cleaning was ok
    for (int orN = 0; orN < Def::NumNodes; orN++)
        for (int deN = 0; deN < Def::NumNodes; deN++)
            for (s = 0; s < Def::SL; s++) {
                assert((Config->Topology_S[s * Def::NumNodes * Def::NumNodes + orN *
                                           Def::NumNodes + deN] == false));
            }

    assert((Aux->first_Event == NULL));
}

/**
 * @brief create_Structures reads the Topology file and create the structures related to it.
 */
void create_Structures() {
    AuxiliarMain::Topology = new long double[Def::NumNodes *
            Def::NumNodes]; //adjacency matrix

    int orN, deN;

    for (orN = 0; orN < Def::NumNodes; orN++) {
        for (deN = 0; deN < Def::NumNodes; deN++) {
            switch (AuxiliarMain::choose_Topology) {
                case Top_PacificBell:
                    AuxiliarMain::TopolPBell >> AuxiliarMain::Topology[orN * Def::NumNodes + deN];
                    break;

                case Top_NSFNet:
                    AuxiliarMain::TopolNFSNet >> AuxiliarMain::Topology[orN * Def::NumNodes + deN];
                    break;

                case Top_NFSNetMod:
                    AuxiliarMain::TopolNSFNetMod >> AuxiliarMain::Topology[orN * Def::NumNodes +
                                                 deN];
                    break;

                case Top_PointToPoint8:
                    AuxiliarMain::TopolReduced8 >> AuxiliarMain::Topology[orN * Def::NumNodes +
                                                deN];
                    break;

                case Top_PointToPoint4:
                    AuxiliarMain::TopolReduced4 >> AuxiliarMain::Topology[orN * Def::NumNodes +
                                                deN];
                    break;

                case Top_Top1:
                    AuxiliarMain::TopolTopology1 >> AuxiliarMain::Topology[orN * Def::NumNodes +
                                                 deN];
                    break;

                case Top_Top2:
                    AuxiliarMain::TopolTopology2 >> AuxiliarMain::Topology[orN * Def::NumNodes +
                                                 deN];
                    break;

                case Top_European:
                    AuxiliarMain::TopolEuropean >> AuxiliarMain::Topology[orN * Def::NumNodes +
                                                deN];
                    break;

                case Top_German:
                    AuxiliarMain::TopolGerman >> AuxiliarMain::Topology[orN * Def::NumNodes + deN];
                    break;

                case Top_USBackbone:
                    AuxiliarMain::TopolUSBackbone >> AuxiliarMain::Topology[orN * Def::NumNodes +
                                                  deN];
                    break;

                case _Top_Null:
                    assert((0 != 0));
                    break;
            }

            cout << AuxiliarMain::Topology[orN * Def::NumNodes + deN] << " ";
        }

        cout << endl;
    }

    delete Aux;
    Aux = new AuxiliarMain();

    eval_NodeDegrees(AuxiliarMain::Config);

    for (int i = 0; i < Def::NumNodes; i++) {
        Aux->Network.push_back(Node(i));
    }

    long double multiplier;

    switch (AuxiliarMain::choose_Topology) {
        case Top_PacificBell:
            AuxiliarMain::TopolPBell >> multiplier;
            break;

        case Top_NSFNet:
            AuxiliarMain::TopolNFSNet >> multiplier;
            break;

        case Top_NFSNetMod:
            AuxiliarMain::TopolNSFNetMod >> multiplier;
            break;

        case Top_PointToPoint8:
            AuxiliarMain::TopolReduced8 >> multiplier;
            break;

        case Top_PointToPoint4:
            AuxiliarMain::TopolReduced4 >> multiplier;
            break;

        case Top_Top1:
            AuxiliarMain::TopolTopology1 >> multiplier;
            break;

        case Top_Top2:
            AuxiliarMain::TopolTopology2 >> multiplier;
            break;

        case Top_European:
            AuxiliarMain::TopolEuropean >> multiplier;
            break;

        case Top_German:
            AuxiliarMain::TopolGerman >> multiplier;
            break;

        case Top_USBackbone:
            AuxiliarMain::TopolUSBackbone >> multiplier;
            break;

        case _Top_Null:
            assert((0 != 0));
            break;
    }

    //Create links
    AuxiliarMain::LinkSet = new vector<Link>[Def::NumNodes];

    for (int i = 0; i < Def::NumNodes; i++) {
        for (int j = 0; j < Def::NumNodes; j++) {
            long double aux_length;

            switch (AuxiliarMain::choose_Topology) {
                case Top_PacificBell:
                    AuxiliarMain::TopolPBell >> aux_length;
                    break;

                case Top_NSFNet:
                    AuxiliarMain::TopolNFSNet >> aux_length;
                    break;

                case Top_NFSNetMod:
                    AuxiliarMain::TopolNSFNetMod >> aux_length;
                    break;

                case Top_PointToPoint8:
                    AuxiliarMain::TopolReduced8 >> aux_length;
                    break;

                case Top_PointToPoint4:
                    AuxiliarMain::TopolReduced4 >> aux_length;
                    break;

                case Top_Top1:
                    AuxiliarMain::TopolTopology1 >> aux_length;
                    break;

                case Top_Top2:
                    AuxiliarMain::TopolTopology2 >> aux_length;
                    break;

                case Top_European:
                    AuxiliarMain::TopolEuropean >> aux_length;
                    break;

                case Top_German:
                    AuxiliarMain::TopolGerman >> aux_length;
                    break;

                case Top_USBackbone:
                    AuxiliarMain::TopolUSBackbone >> aux_length;
                    break;

                case _Top_Null:
                    assert((0 != 0));
                    break;
            }

            if (AuxiliarMain::Topology[i * Def::NumNodes + j] == 1) {
                AuxiliarMain::LinkSet[i].push_back(Link(&Aux->Network.at(i),
                                                        &Aux->Network.at(j),
                                                        multiplier * aux_length));
            } else {
                AuxiliarMain::LinkSet[i].push_back(Link(NULL, NULL, Def::MAX_INT));
            }
        }
    }
}

/**
 * @brief define_NextEventOfConnection creates a Descending Event, given a Requisition Event.
 * @param evt is the pointer to the Descending Event.
 */
void define_NextEventOfConnection(Event *evt) {
    Connection *con = evt->conn;
    evt->time = con->getTimeDesc();
    evt->type = Desc;
}

/**
 * @brief eval_SlotsNeeded evaluates how many slots are needed to implement a requisition with a bitrate BitRate under a modulation scheme Scheme.
 * @param BitRate is the Bitrate, as defined in Def::Possible_BitRates.
 * @param Scheme is the Modulation Scheme, as defined in ModulationScheme.
 * @return the number of needed slots.
 */
int eval_SlotsNeeded(long double BitRate, ModulationScheme Scheme) {
    return ceil(BitRate / (AuxiliarMain::choose_PolarizationModes * log2((
                               int)Scheme) * Def::get_BSlot()));
}

/**
 * @brief eval_NodeDegrees evaluates the degree of each node.
 */
void eval_NodeDegrees(Def *Config) {
    int node_temp = 0;
    Config->clear_NodeDegree();

    for (int orN = 0; orN < Def::NumNodes; orN++) {
        for (int deN = 0; deN < Def::NumNodes; deN++) {
            if (AuxiliarMain::Topology[orN * Def::NumNodes + deN] == 1) {
                node_temp++;
            }
        }

        Def::set_NodeDegree(node_temp);
        node_temp = 0;
    }
}
/**
 * @brief find_Multiplier increases linearly the topology, until the Topology Diameter hasn't enough Quality to hold a Connection.
 * @param Config is the set of parameters over which the simulation happens.
 * @param Aux is the set of parameters over which the simulation happens.
 */
void find_Multiplier(Def *Config, AuxiliarMain *Aux) {
    RWA::Dijkstra(Aux, Config);
    Event *evt = new Event;
    set_RequestEvent(evt, 0);
    Config->set_AmpDist(80);    //80km
    Config->set_OSNRin(30);    //30dB

    cout << "Limiar: " << Def::get_OSNRThreshold(evt->Scheme,
            Def::GreatestBitRate) << "dB" << endl;

    int orN, deN;
    long double Max = - Def::MAX_DOUBLE;

    for (int i = 0; i < Def::NumNodes; i++) {
        for (int j = 0; j < Def::NumNodes; j++) {
            if (Max < Aux->All_Routes[i * Def::NumNodes + j].front()->get_Length()) {
                Max = Aux->All_Routes[i * Def::NumNodes + j].front()->get_Length();
                orN = i;
                deN = j;
            }
        }
    }

    for (long double multiplier = 1; multiplier <= 30; multiplier += 1) {

        if (multiplier != 1)
            for (int i = 0;
                    i < Aux->All_Routes[orN * Def::NumNodes + deN].at(0)->get_NumHops();
                    i++) {
                int SourceNode = Aux->All_Routes[orN * Def::NumNodes + deN].at(0)->get_Node(i);
                int DestinNode = Aux->All_Routes[orN * Def::NumNodes + deN].at(0)->get_Node(
                                     i + 1);
                long double oldDist = AuxiliarMain::LinkSet[SourceNode].at(
                                          DestinNode).get_Length();
                long double newDist = oldDist * multiplier / (multiplier - 1);
                AuxiliarMain::LinkSet[SourceNode].at(DestinNode).set_Length(newDist);
                AuxiliarMain::LinkSet[DestinNode].at(SourceNode).set_Length(newDist);
            }

        refresh_Noise(Config);
        set_RequestEvent(evt, 0);
        long double OSNRout;
        OSNRout = eval_OSNR(Aux->All_Routes[orN * Def::NumNodes + deN].at(0),
                            Config, Aux);
        cout << "Multiplier = " << multiplier << ", OSNR = " << OSNRout << "dB" <<
             endl;
    }

    if (AuxiliarMain::window != NULL) {
        AuxiliarMain::window->Simulation_Ended();
    }

    delete evt;
}

/**
 * @brief insert_Connection inserts a connection in the simulation.
 * @param route is the route over which the connection happens.
 * @param si is the initial slot of the connection.
 *
 * If the simulation is translucent, an other function should adjust the initial slot to every hop.
 *
 * @param NslotsUsed is the number of used slots by the connection.
 * @param Time is when the connection happens.
 * @param Aux is the set of parameters over which the simulation is happening.
 * @return a pointer to the Requisition Event.
 */
Event *insert_Connection(Route *route, int si, int NslotsUsed,
                         long double Time, AuxiliarMain *Aux) {
    if (Aux->FlagRP_TLP) {
        for (int i = 0; i <= route->get_NumHops(); i++) {
            Aux->RP_TLP_NodeUsage[ route->get_Node(i) ]++;
        }
    }

    //Creates connection
    int SI[route->get_NumHops()];
    int SF[route->get_NumHops()];

    for (int i = 0; i < route->get_NumHops(); i++) {
        SI[i] = si;
        SF[i] = si + NslotsUsed - 1;
    }

    Connection *new_Connection = new Connection(*route, SI, SF,
            Aux->sim_Time + Time);
    //Schedule new event
    Event *evt = new Event;
    evt->conn = new_Connection;
    define_NextEventOfConnection(evt);
    schedule_Event(evt, Aux);

    return evt;
}

/**
 * @brief implement_TranslucentNetwork implements the Translucent Network, calling the selected Regenerator Placement Algorithm.
 */
void implement_TranslucentNetwork() {
    Regenerators::MainAux = Aux;

    if (AuxiliarMain::RegNumber_Total != 0) {
        switch (AuxiliarMain::choose_RPAlgorithm) {
            case RP_NDF:
                Regenerators::RP_NDF(AuxiliarMain::RegNumber_Total,
                                     AuxiliarMain::RegNumber_PerNode);
                break;

            case RP_CNF:
                Regenerators::RP_CNF(AuxiliarMain::RegNumber_Total,
                                     AuxiliarMain::RegNumber_PerNode);
                break;

            case RP_TLP:
                Regenerators::RP_TLP(AuxiliarMain::RegNumber_Total,
                                     AuxiliarMain::RegNumber_PerNode);
                break;

            case RP_SQP:
                Regenerators::RP_SQP(AuxiliarMain::RegNumber_Total,
                                     AuxiliarMain::RegNumber_PerNode);
                break;

            case RP_MU:
                Regenerators::RP_MU(AuxiliarMain::RegNumber_Total,
                                    AuxiliarMain::RegNumber_PerNode);
                break;

            case RP_MSU:
                Regenerators::RP_MSU(AuxiliarMain::RegNumber_Total);
                break;

            case _RP_Null:
                break;
        }
    }
}

/**
 * @brief load loads the values inserted in the Textual Interface onto the simulation.
 */
void load() {
    int Npoints, aux;
    long double op;

    cout << "Choose Simulation. " << endl << "\tBlocking Probability <" <<
         Sim_PbReq << ">;" << endl << "\tOSNR <" << Sim_OSNR << ">; " << endl <<
         "\tInLine Amplifiers Distance <" << Sim_AmpDist << ">;" << endl <<
         "\tSlots per Link Number <" << Sim_SlotsNum << ">;" << endl << "\tWhiskers <" <<
         Sim_Whiskers << ">;\n\tRegenerator Number <" <<
         Sim_RegenNum << ">." << endl;
    cin >> aux;
    AuxiliarMain::choose_Simulation = (Simulations) aux;

    if (AuxiliarMain::choose_Simulation == Sim_Whiskers) {
        cout << "How many repetitions of simulation?" << endl;
        cin >> aux;
        assert((aux > 0));
        Def::maxSim_Whiskers = aux;
    }

    cout << "Choose Topology." << endl << "\tPacific Bell <" << Top_PacificBell <<
         ">; " << endl << "\tNSFNet <" << Top_NSFNet << ">; " << endl <<
         "\tNSFNet Modified (Reduced) <" << Top_NFSNetMod << ">;" << endl <<
         "\tPoint-To-Point with 4 Nodes <" << Top_PointToPoint4 << ">; " << endl <<
         "\tPoint-To-Pont with 8 Nodes <" << Top_PointToPoint8 << ">; " << endl <<
         "\tTop1 <" << Top_Top1 << ">;" << endl << "\tTop2 <" << Top_Top2 << ">; " <<
         endl
         << "\tEuropean Topology <" << Top_European <<
         ">;" << endl << "\tGerman Topology <" << Top_German << ">;" << endl <<
         "\tUS Backbone Topology <" << Top_USBackbone << ">;" << endl;
    cin >> aux;
    AuxiliarMain::choose_Topology = (Topologies) aux;

    //Acquires number of nodes:
    switch (AuxiliarMain::choose_Topology) {
        case Top_PacificBell:
            AuxiliarMain::TopolPBell >> aux;
            break;

        case Top_NSFNet:
            AuxiliarMain::TopolNFSNet >> aux;
            break;

        case Top_NFSNetMod:
            AuxiliarMain::TopolNSFNetMod >> aux;
            break;

        case Top_PointToPoint4:
            AuxiliarMain::TopolReduced4 >> aux;
            break;

        case Top_PointToPoint8:
            AuxiliarMain::TopolReduced8 >> aux;
            break;

        case Top_Top1:
            AuxiliarMain::TopolTopology1 >> aux;
            break;

        case Top_Top2:
            AuxiliarMain::TopolTopology2 >> aux;
            break;

        case Top_European:
            AuxiliarMain::TopolEuropean >> aux;
            break;

        case Top_German:
            AuxiliarMain::TopolGerman >> aux;
            break;

        case Top_USBackbone:
            AuxiliarMain::TopolUSBackbone >> aux;
            break;

        case _Top_Null:
            assert((0 != 0));
            break;
    }

    Def::set_NumNodes(aux);
    cout << "Number of Nodes in chosen Topology: " << Def::NumNodes << endl;

    AuxiliarMain::consider_PhyImpair = _YES;

    cout << "Enter Slots Bandwidth, in GHz (common values are 1.5625, 3.125, 6.25 or 12.5)"
         << endl;
    cin >> op;
    Def::set_BSlot(op);
    Def::set_BRef(12.5);

    switch (AuxiliarMain::choose_Topology) {
        case Top_PacificBell:
            AuxiliarMain::TopolPBell >> aux;
            break;

        case Top_NSFNet:
            AuxiliarMain::TopolNFSNet >> aux;
            break;

        case Top_NFSNetMod:
            AuxiliarMain::TopolNSFNetMod >> aux;
            break;

        case Top_PointToPoint4:
            AuxiliarMain::TopolReduced4 >> aux;
            break;

        case Top_PointToPoint8:
            AuxiliarMain::TopolReduced8 >> aux;
            break;

        case Top_Top1:
            AuxiliarMain::TopolTopology1 >> aux;
            break;

        case Top_Top2:
            AuxiliarMain::TopolTopology2 >> aux;
            break;

        case Top_European:
            AuxiliarMain::TopolEuropean >> aux;
            break;

        case Top_German:
            AuxiliarMain::TopolGerman >> aux;
            break;

        case Top_USBackbone:
            AuxiliarMain::TopolUSBackbone >> aux;
            break;

        case _Top_Null:
            assert((0 != 0));
            break;
    }

    Def::set_SL(aux);
    cout << "Number of Slots Per Link: " << Def::SL << endl;

    Def::set_SR(Def::SL);

    delete AuxiliarMain::Config;
    AuxiliarMain::Config = new Def();    //redefines Config to reallocate Topology_S
    delete Aux;
    Aux = new AuxiliarMain();

    if (AuxiliarMain::choose_Simulation != Sim_RegenNum) {
        cout << "\t" << TransparentNetwork << " - Transparent\n\t" << TranslucentNetwork
             <<
             " - Translucent\nEnter Network Type: ";
        cin >> aux;
        AuxiliarMain::choose_NetworkType = (NetworkType) aux;

        if (AuxiliarMain::choose_NetworkType == TranslucentNetwork) {

            cout << "\t" << RP_NDF << " - NDF\n\t" << RP_CNF << " - CNF\n\t" << RP_TLP <<
                 " - TLP\n\t" << RP_SQP << " - SQP\n\t" << RP_MU << " - MU\n\t" << RP_MSU <<
                 " - MSU\nEnter the RP Algorithm: ";
            cin >> aux;
            AuxiliarMain::choose_RPAlgorithm = (RP_Algorithm) aux;

            cout << "Enter the total number of regenerators: ";
            cin >> op;
            assert((op >= 0));
            AuxiliarMain::RegNumber_Total = op;

            if (AuxiliarMain::choose_RPAlgorithm != RP_MSU) {
                cout << "Enter the number of regenerators per node: ";
                cin >> op;
                assert((op >= 0));
                assert((op <= AuxiliarMain::RegNumber_Total));
                AuxiliarMain::RegNumber_PerNode = op;
            }

            cout << "\t" << RA_FLR << " - FLR\n\t" << RA_FNS <<
                 " - FNS\n\t" << RA_Both << " - Both\nEnter the RA Algorithm: ";
            cin >> aux;
            AuxiliarMain::choose_RAAlgorithm = (RA_Algorithm) aux;

            if (AuxiliarMain::choose_RAAlgorithm == RA_Both) {
                cout << "Enter the threshold ratio between FNS and FLR: ";
                cin >> op;
                assert((op <= 1.0));
                assert((op >= 0.0));
                Regenerators::RA_Both_DecisionRatio = op;
            }
        }

    } else {
        AuxiliarMain::choose_NetworkType = TranslucentNetwork;
        AuxiliarMain::RegNumber_Total = 0;
        AuxiliarMain::RegNumber_PerNode = 0;

        cout << "\t" << RA_FLR << " - FLR\n\t" << RA_FNS <<
             " - FNS\n\t" << RA_Both << " - Both\nEnter the RA Algorithm: ";
        cin >> aux;
        AuxiliarMain::choose_RAAlgorithm = (RA_Algorithm) aux;

        if (AuxiliarMain::choose_RAAlgorithm == RA_Both) {
            cout << "Enter the threshold ratio between FNS and FLR: ";
            cin >> op;
            assert((op <= 1.0));
            assert((op >= 0.0));
            Regenerators::RA_Both_DecisionRatio = op;
        }
    }

    cout << "\t" << Routing_MH << " - Minimum Hops \n\t" << Routing_SP <<
            " - Shortest Path\n\t" << Routing_LOR_AlPos <<
            " - LOR Allocation Possibilities\n\t"
         << Routing_LOR_A << " - LOR Availability\n\t" << Routing_OSNRR << " - OSNR-R\n";
    cout << "Enter the Routing Algorithm: ";
    cin >> aux;
    AuxiliarMain::choose_RoutingAlgorithm = (Routing_Algorithm) aux;
    cout << "\t" << RD << " - Random \n\t" << FF << " - FirstFit \n\t" << MU <<
         " - Most Used" << endl;
    cout << "Enter the Spectrum Assignment Algorithm: ";
    cin >> aux;
    AuxiliarMain::choose_SpectrumAssignmentAlgorithm =
        (SpectrumAssignment_Algorithm) aux;

    cout << "Enter mu (connections deactivation rate): ";
    cin >> AuxiliarMain::mu;

    if (AuxiliarMain::choose_Simulation == Sim_OSNR ||
            AuxiliarMain::choose_Simulation == Sim_AmpDist) {
        cout << "Enter..." << endl;
        cout << "the minimum OSNR: ";
        cin >> AuxiliarMain::OSNR_Min; // La = connections arrival rate
        cout << "the maximum OSNR: ";
        cin >> AuxiliarMain::OSNR_Max;
        cout << "#Points in the Result: ";
        cin >> Npoints;
        AuxiliarMain::OSNR_Step = (AuxiliarMain::OSNR_Max - AuxiliarMain::OSNR_Min) /
                                  (Npoints - 1);
    }

    if (AuxiliarMain::choose_Simulation == Sim_RegenNum) {
        cout << "Enter the minimum number of Regenerators per Node: ";
        cin >> op;
        assert((op >= 0));
        AuxiliarMain::RegPerNode_Min = op;

        cout << "Enter the maximum number of Regenerators per Node: ";
        cin >> op;
        assert((op >= 0));
        assert((op >= AuxiliarMain::RegPerNode_Min));
        AuxiliarMain::RegPerNode_Max = op;

    }

    if (AuxiliarMain::choose_Simulation == Sim_PbReq) {
        cout << "La = Connections Arrival Rate. Enter..." << endl;
        cout << "the minimum La: ";
        cin >> AuxiliarMain::LaNet_Min;
        cout << "the maximum La: ";
        cin >> AuxiliarMain::LaNet_Max;
        cout << "#Points in the Result: ";
        cin >> Npoints;
        AuxiliarMain::LaNet_Step = (AuxiliarMain::LaNet_Max -
                                    AuxiliarMain::LaNet_Min) / (Npoints - 1);
    }

    if (AuxiliarMain::choose_Simulation == Sim_Whiskers ||
            AuxiliarMain::choose_Simulation == Sim_RegenNum ||
            AuxiliarMain::choose_Simulation == Sim_SlotsNum ||
            AuxiliarMain::choose_Simulation == Sim_OSNR) {
        cout << "La = Connections Arrival Rate. Enter LaNet: " << endl;
        cin >> AuxiliarMain::laNet; // La = taxa de chegada das conexoes;
    }

    if (AuxiliarMain::choose_Simulation == Sim_SlotsNum) {
        cout << "Enter the minimum Slots per Link number" << endl;
        cin >> Aux->SlotsNum_Min;
        cout << "Enter the maximum Slots per Link number" << endl;
        cin >> Aux->SlotsNum_Max;
    }

    if (AuxiliarMain::choose_Simulation == Sim_AmpDist) {
        cout << "Enter..." << endl;
        cout << "the minimum distance between InLine amplifiers: ";
        cin >> AuxiliarMain::AmpDist_Min;
        cout << "the minimum distance between InLine amplifiers: ";
        cin >> AuxiliarMain::AmpDist_Max;
        cout << "#Points in the Result: ";
        cin >> Npoints;
        AuxiliarMain::AmpDist_Step = (AuxiliarMain::AmpDist_Max -
                                      AuxiliarMain::AmpDist_Min) / (Npoints - 1);
    }

    if (AuxiliarMain::consider_PhyImpair == _YES) {
        cout << "Enter the input power, in dBm: " << endl;
        cin >> op;
        AuxiliarMain::Config->set_Pin(op);
        cout << "Enter the fiber's reference power, in dBm: " << endl;
        cin >> op;
        AuxiliarMain::Config->set_PRef(op);

        if (AuxiliarMain::choose_Simulation != Sim_AmpDist) {
            cout << "Enter the distance between InLine Amplifiers (in km): " << endl;
            cin >> op;
            AuxiliarMain::Config->set_AmpDist(op);
        }

        cout << "\tBrodcasting and Select <" << Arch_BS << ">\n\tSpectrum Switching <"
             << Arch_SS << ">\nEnter the Node Architecture: " << endl;
        cin >> aux;
        Def::set_Architeture((NodeArchiteture) aux);
    }

    long double nR;

    if (AuxiliarMain::choose_Simulation != Sim_AmpDist) {
        cout << "Enter the minimum number of blocked requisitions before ending: ";
        cin >> nR;
        Def::set_NumReqBloqMin(nR);
    }
}

/**
 * @brief request_BitRate generates random number to represent BitRate.
 * @return a number in the range of Def::Possible_BitRates.
 */
int request_BitRate() {
    return floor(General::uniform(0.0, Def::get_numPossibleBitRates()));
}

/**
 * @brief request_Conn creates connection. Evokes routing algorithm. Finds route and creates connection. Schedules next event.
 * @param evt is the event being treated.
 * @param Config is the set of parameters over which the simulation runs.
 * @param MainAux is the set of parameters over which the simulation runs.
 */
void request_Conn(Event *evt, Def *Config, AuxiliarMain *MainAux) {
    if (AuxiliarMain::SupremeFlag) {
        int orN, deN, NslotsReq, NslotsUsed = 0, si, nBitRate;
        request_SourceDestinationPair(orN, deN);
        nBitRate = request_BitRate();

        if ((AuxiliarMain::choose_Simulation == Sim_AmpDist) ||
                (AuxiliarMain::choose_Simulation == Sim_SlotsNum)) {
            nBitRate = Def::get_numPossibleBitRates() - 1;
        }

        Config->TotalData += Def::Possible_BitRates.at(nBitRate);

        //Tries to allocate the requisition
        Route *route;
        long double OSNR = 0;

        Config->num_Req++;
        Config->num_ReqBitRate[nBitRate]++;

        if (MainAux->choose_NetworkType == TransparentNetwork) {
            ModulationScheme Schemes[num_ModulationSchemes] = {_64QAM, _16QAM, _4QAM};

            for (int Scheme = 0; Scheme < num_ModulationSchemes; Scheme++) {
                evt->Scheme = Schemes[Scheme];
                NslotsReq = eval_SlotsNeeded(Def::Possible_BitRates.at(nBitRate), evt->Scheme);
                assert((NslotsReq > 0));

                if (AuxiliarMain::choose_RoutingAlgorithm == Routing_LOR_A ||
                        AuxiliarMain::choose_RoutingAlgorithm == Routing_LOR_AlPos) {
                    RWA::Dijkstra(MainAux, Config, orN, deN, nBitRate, evt->Scheme);
                }

                for (unsigned int i = 0;
                        i < MainAux->All_Routes[orN * Def::NumNodes + deN].size(); i++) {
                    //Tries the ith route for the orN-deN pair
                    route = MainAux->All_Routes[orN * Def::NumNodes + deN].at(i);

                    if (Scheme == 0 && AuxiliarMain::FlagRP_SQP) {
                        Regenerators::SQP_evaluateRoute(route, nBitRate, Aux);
                    }

                    NslotsUsed = 0;
                    si = -1;
                    try_ToConnect(route, NslotsReq, NslotsUsed, si, Config);
                    assert(((NslotsUsed == 0) || (NslotsUsed == NslotsReq)));
                    //Remove if connection may be sufficed by less slots than requested

                    if (NslotsUsed > 0) {   //Connection accepted
                        assert((NslotsUsed <= NslotsReq && si >= 0 && si <= Def::SL - NslotsUsed));

                        if (AuxiliarMain::consider_PhyImpair == _YES) {
                            OSNR = eval_OSNR(route, Config, MainAux);
                        }

                        if (AuxiliarMain::consider_PhyImpair == _NO ||
                                OSNR >= Def::get_OSNRThreshold(evt->Scheme,
                                                               Def::Possible_BitRates.at(nBitRate))) {   //Accepts Connection
                            //Insert connection
                            int L_or, L_de;

                            for (int c = 0; c < route->get_NumHops(); c++) {
                                L_or = route->get_Node(c);
                                L_de = route->get_Node(c + 1);

                                for (int s = si; s < si + NslotsUsed; s++) {
                                    assert((Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or *
                                                               Def::NumNodes + L_de] == false));
                                    Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes +
                                                       L_de] =
                                                           true;
                                    //Slots marked as used
                                }
                            }

                            long double Time = General::exponential(AuxiliarMain::mu);

                            insert_Connection(route, si, NslotsUsed, Time, MainAux);

                            Config->num_HopsPerRoute += route->get_NumHops();
                            Config->net_Occupancy += NslotsUsed * route->get_NumHops();
                            Config->TotalTime_Scheme[nBitRate] += Time;
                            Config->num_ReqAccepScheme[Scheme] += 1;
                            Config->TotalData_Scheme[Scheme] += Def::Possible_BitRates.at(nBitRate);
                            break;
                        } else { //connection blocked due to QoT
                            NslotsUsed = 0;

                            if (Scheme == num_ModulationSchemes - 1) {
                                account_QoTBlocking(NslotsReq, NslotsUsed, Config);
                            }
                        }
                    }
                }

                if (NslotsUsed != 0) {
                    break;
                }
            }
        } else if (MainAux->choose_NetworkType == TranslucentNetwork) {

            if (AuxiliarMain::choose_RoutingAlgorithm == Routing_LOR_A ||
                    AuxiliarMain::choose_RoutingAlgorithm == Routing_LOR_AlPos) {
                RWA::Dijkstra(MainAux, Config, orN, deN, nBitRate);
            }

            NslotsReq = eval_SlotsNeeded(Def::Possible_BitRates.at(nBitRate), _4QAM);

            for (unsigned int i = 0;
                    i < MainAux->All_Routes[orN * Def::NumNodes + deN].size();
                    i++) {
                route = MainAux->All_Routes[orN * Def::NumNodes + deN].at(i);

                if (AuxiliarMain::choose_RAAlgorithm == RA_FLR) {
                    if (Regenerators::RA_FLR(route, nBitRate, Config,
                                             MainAux)) {   //Connection Accepted
                        NslotsUsed = NslotsReq;
                    }
                } else if (AuxiliarMain::choose_RAAlgorithm == RA_FNS) {
                    if (Regenerators::RA_FNS(route, nBitRate, Config,
                                             MainAux)) {   //Connection Accepted
                        NslotsUsed = NslotsReq;
                    }
                } else if (AuxiliarMain::choose_RAAlgorithm == RA_Both) {
                    if (Regenerators::RA_Both(route, nBitRate, Config,
                                              MainAux)) {   //Connection Accepted
                        NslotsUsed = NslotsReq;
                    }
                }

                if (NslotsUsed != 0) {
                    break;
                }
            }
        }

        Config->num_SlotsReq += NslotsReq;

        //verifies how much connections and slots have been blocked
        account_Blocking(NslotsReq, NslotsUsed, nBitRate, Config);
        //defines new event
        long double IAT = General::exponential(
                              AuxiliarMain::laNet);    //Inter-arrival time
        set_RequestEvent(evt, MainAux->sim_Time + IAT);
        assert((evt->type == Req));
        schedule_Event(evt, MainAux);
    }
}

/**
 * @brief refresh_Noise refresh link's noises.
 * @param Config is the set of parameters over which the refreshing happens.
 */
void refresh_Noise(Def *Config) {
    AuxiliarMain::Tree->reset_Tree();

    for (int i = 0; i < Def::NumNodes; i++) {
        for (int j = 0; j < Def::NumNodes; j++) {
            AuxiliarMain::LinkSet[i].at(j).refresh_Link(Config);
        }
    }
}

/**
 * @brief request_SourceDestinationPair creates randomly a origin-destination pair.
 * @param orN is a pointer to the id of the origin node.
 * @param deN is a pointer to the id of the destination node.
 */
void request_SourceDestinationPair(int &orN, int &deN) {
    orN = rand() % Def::NumNodes;
    deN = rand() % (Def::NumNodes - 1);

    if (deN >= orN) {
        deN += 1;
    }

    if ((orN < 0) || (orN >= Def::NumNodes) || (deN < 0) ||
            (deN >= Def::NumNodes) ||
            (deN == orN)) {
        cout << "Error in request_SourceDestinationPair";
        cin.get();
    }
}

/**
 * @brief remove_Conn removes connection from network, releasing all of its slots.
 * @param evt is the Descending Event being evaluated.
 * @param Config is the set of parameters over which the simulation is being run.
 * @param Aux is the set of parameters over which the simulation is being run.
 */
void remove_Conn(Event *evt, Def *Config, AuxiliarMain *Aux) {
    int L_or, L_de;

    for (int hop = 0; hop < evt->conn->getNHops(); hop++) {
        L_or = evt->conn->getRoute()->get_Node(hop);
        L_de = evt->conn->getRoute()->get_Node(hop + 1);

        for (int s = evt->conn->getFirstSlot(hop);
                s <= evt->conn->getLastSlot(hop); s++) {
            assert(((s >= 0) && (s < Def::SL)));
            assert((Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or *
                                       Def::NumNodes + L_de] == true));
            Config->Topology_S[s * Def::NumNodes * Def::NumNodes + L_or * Def::NumNodes +
                               L_de] =
                                   false;
        }
    }

    for (int node = 0; node < Def::NumNodes; node++) {
        if (evt->Used_Regenerators[node] != 0) {
            Aux->Network.at(node).release_Regenerators(
                evt->Used_Regenerators[node]);  //release used regenerators
            evt->Used_Regenerators_Number -= evt->Used_Regenerators[node];
            evt->Used_Regenerators[node] = 0;
        }
    }

    assert((evt->Used_Regenerators_Number == 0));

    delete evt->conn;
    delete evt;
}

/**
 * @brief schedule_Event enqueues the event.
 * @param evt is the Event being enqueued.
 * @param Aux is the set of parameters over which the simulation is being run.
 */
void schedule_Event(Event *evt, AuxiliarMain *Aux) {
    Event *evtAux = Aux->first_Event, *evtAnt = NULL;

    while (evtAux != NULL) {
        if (evt->time < evtAux->time) {
            break;
        } else {
            evtAnt = evtAux;
            evtAux = evtAux->nextEvent;
        }
    }

    evt->nextEvent = evtAux;

    if (evtAnt == NULL) {
        Aux->first_Event = evt;
    } else {
        evtAnt->nextEvent = evt;
    }
}

/**
 * @brief set_RequestEvent creates a requisition event.
 * @param evt is a pointer to the event being set.
 * @param t is the time when the requisition happens.
 */
void set_RequestEvent(Event *evt, TIME t) {
    evt->time = t;
    evt->type = Req;
    evt->nextEvent = NULL;
    evt->conn = NULL;

    if ((AuxiliarMain::choose_Simulation == Sim_AmpDist) ||
            (AuxiliarMain::choose_Simulation == Sim_SlotsNum)) {
        evt->Scheme = _4QAM;
    }
}

/**
 * @brief Sim defines parameters before simulation
 * @param Aux is the set of parameters over which the simulation is being run.
 */
void Sim(AuxiliarMain *Aux) {
    Aux->Config->set_LaUniform(AuxiliarMain::laNet);
    //Def::setLaRandom(laNet);

    Simulate(Aux->Config, Aux);
}

/**
 * @brief SimAmpDist initiates a simulatation of In-Line Amplifiers Distance and Input OSNR that give enough quality to establish a connection in the diameter of the Topology.
 * @param Config is the set of parameters over which the simulation will run.
 * @param Aux is the set of parameters over which the simulation will run.
 */
void SimAmpDist(Def *Config, AuxiliarMain *Aux) {
    RWA::Dijkstra(Aux, Config);
    Event *evt = new Event;
    set_RequestEvent(evt, 0);
    cout << "Threshold: " << Def::get_OSNRThreshold(evt->Scheme,
            Def::GreatestBitRate) << "dB" << endl;

    for (long double osnr = AuxiliarMain::OSNR_Min; osnr <= AuxiliarMain::OSNR_Max;
            osnr += AuxiliarMain::OSNR_Step) {
        Config->set_OSNRin(osnr);

        for (long double dAmplif = AuxiliarMain::AmpDist_Min;
                dAmplif <= AuxiliarMain::AmpDist_Max;
                dAmplif += AuxiliarMain::AmpDist_Step) {
            Config->set_AmpDist(dAmplif);
            refresh_Noise(Config);
            set_RequestEvent(evt, 0);
            long double Max = - Def::MAX_DOUBLE, OSNRout;
            int orN, deN;

            for (int i = 0; i < Def::NumNodes; i++) {
                for (int j = 0; j < Def::NumNodes; j++) {
                    if (i == j) {
                        continue;
                    }

                    if (Max < Aux->All_Routes[i * Def::NumNodes + j].front()->get_Length()) {
                        Max = Aux->All_Routes[i * Def::NumNodes + j].front()->get_Length();
                        orN = i;
                        deN = j;
                    }
                }
            } //Finds longest link

            OSNRout = eval_OSNR(Aux->All_Routes[orN * Def::NumNodes + deN].at(0),
                                Config,
                                Aux);
            cout << "OSNRin = " << Config->get_OSRNin() << "dB, AmpDist = " <<
                 Config->get_AmpDist() << "km, OSNR = " << OSNRout << "dB" <<
                 endl; //first route

            if (OSNRout < Def::get_OSNRThreshold(evt->Scheme, Def::GreatestBitRate)) {
                AuxiliarMain::Result_DAmpLesserThanThresh << Config->get_AmpDist() << "\t" <<
                        Config->get_OSRNin() << endl;
            } else {
                AuxiliarMain::Result_DAmpGreaterThanThresh << Config->get_AmpDist() << "\t" <<
                        Config->get_OSRNin() << endl;
            }
        }
    }

    delete evt;

    if (AuxiliarMain::window != NULL) {
        AuxiliarMain::window->Simulation_Ended();
    }
}

/**
 * @brief SimOSNR initiates a simulation that evaluates the network's performance versus the input OSNR.
 * @param Aux is the set of parameters over which the simulation will run.
 */
void SimOSNR(AuxiliarMain *Aux) {
    /*OSNR Simulation*/
    for (long double osnr = AuxiliarMain::OSNR_Min; osnr <= AuxiliarMain::OSNR_Max;
            osnr += AuxiliarMain::OSNR_Step) {
        AuxiliarMain::Config->set_OSNRin(osnr);
        Sim(Aux);
    }

    if (AuxiliarMain::window != NULL) {
        AuxiliarMain::window->Simulation_Ended();
    }
}

/**
 * @brief SimPbReq initiates a simulation that evaluates the network's performance versus the network load, in Erlang.
 * @param Aux is the set of parameters over which the simulation will run.
 */
void SimPbReq(AuxiliarMain *Aux) {
    /*Blocking Probability Simulation*/
    for (AuxiliarMain::laNet = AuxiliarMain::LaNet_Min;
            AuxiliarMain::laNet <= AuxiliarMain::LaNet_Max;
            AuxiliarMain::laNet += AuxiliarMain::LaNet_Step) {
        if (AuxiliarMain::SupremeFlag == true) {
            Sim(Aux);
        }
    }

    if (AuxiliarMain::window != NULL) {
        AuxiliarMain::window->Simulation_Ended();
    }
}

/**
 * @brief SimRegeneratorNumber initiates a translucent simulation that evaluates the network's performance versus the number of regenerators per node.
 */
void SimRegeneratorNumber() {
    AuxiliarMain::choose_RPAlgorithm = RP_NDF;

    for (long double RegPorNo = AuxiliarMain::RegPerNode_Min;
            RegPorNo <= AuxiliarMain::RegPerNode_Max; RegPorNo += 2) {
        if (!AuxiliarMain::SupremeFlag) {
            continue;
        }

        AuxiliarMain::RegNumber_PerNode = RegPorNo;
        AuxiliarMain::RegNumber_Total = RegPorNo * Def::NumNodes;
        Regenerators::refresh_Regenerators(Aux);
        implement_TranslucentNetwork();

        long double PbReq = simulate_Network(AuxiliarMain::Config, Aux);
        cout << "Num. Reg. Per Node\t" << RegPorNo << "\tPbReq " << PbReq << endl;
        AuxiliarMain::Result << RegPorNo << "\t" << PbReq << endl;

        if (AuxiliarMain::window != NULL) {
            AuxiliarMain::window->Refresh();
            AuxiliarMain::window->Slot_Screen4_RefreshProgressBar();
        }
    }

    if (AuxiliarMain::window != NULL) {
        AuxiliarMain::window->Simulation_Ended();
    }
}

/**
 * @brief SimSlotsNum initiates a simulation that evaluates the network's performance versus the number of slots per link.
 * @param Aux is the set of parameters over which the simulation will run.
 */
void SimSlotsNum(AuxiliarMain *Aux) {
    for (int Slots = Aux->SlotsNum_Min; Slots <= Aux->SlotsNum_Max;
            Slots += Aux->SlotsNum_Min) {
        if (!AuxiliarMain::SupremeFlag) {
            continue;
        }

        delete []Aux->Config->Topology_S;
        Def::set_SL(Slots);
        Aux->Config->Topology_S = new bool[Def::SL * Def::NumNodes *
                                           Def::NumNodes];

        for (int i = 0; i < Def::SL * Def::NumNodes * Def::NumNodes; i++) {
            Aux->Config->Topology_S[i] = false;
        }

        Simulate(Aux->Config, Aux);
    }

    if (AuxiliarMain::window != NULL) {
        AuxiliarMain::window->Simulation_Ended();
    }
}

/**
 * @brief SimWhiskers repeats a simulation to evaluate statistical trends in the blocking probability results.
 */
void SimWhiskers() {
    srand(time(NULL));
    #pragma omp parallel for schedule(dynamic)

    for (int i = 1; i <= Def::maxSim_Whiskers; i++) {
        if (!AuxiliarMain::SupremeFlag) {
            continue;
        }

        AuxiliarMain *BigodeAux = new AuxiliarMain();
        Def *BigodeDef;

        BigodeDef = new Def();

        long double PbReq = simulate_Network(BigodeDef, BigodeAux);
        cout << "Iteration " << i << "\tPbReq " << PbReq << endl;
        Aux->Result << PbReq << endl;
        delete BigodeAux;
        delete BigodeDef;

        if (AuxiliarMain::window != NULL) {
            AuxiliarMain::window->Refresh();
            AuxiliarMain::window->Slot_Screen4_RefreshProgressBar();
        }
    }

    if (AuxiliarMain::window != NULL) {
        AuxiliarMain::window->Simulation_Ended();
    }
}

/**
 * @brief simulate_Network simulates a network without the overhead of Simulate, or its output.
 * @param Config is the set of parameters over which the simulation will run.
 * @param MainAux is the set of parameters over which the simulation will run.
 * @return the blocking probability from the simulation.
 */
long double simulate_Network(Def *Config, AuxiliarMain *MainAux) {
    if (AuxiliarMain::SupremeFlag)  {

        if (MainAux->Network.empty()) {
            MainAux->refresh_Nodes();
        }

        if (MainAux->choose_NetworkType == TranslucentNetwork) {
            for (int i = 0; i < Def::NumNodes; i++) {
                MainAux->Network.at(i).set_NodeType(Aux->Network.at(i).get_NodeType());
                MainAux->Network.at(i).set_NumberRegenerators(Aux->Network.at(
                            i).get_NumberRegenerators());
            }
        }

        clear_Memory(Config, MainAux);

        Config->set_LaUniform(AuxiliarMain::laNet);

        if ((AuxiliarMain::choose_Simulation != Sim_RegenNum) && (MainAux != Aux))
            for (int i = 0; i < Def::NumNodes * Def::NumNodes; i++) {
                while (!MainAux->All_Routes[i].empty()) {
                    delete MainAux->All_Routes[i].back();
                    MainAux->All_Routes[i].pop_back();
                }

                vector<Route *> ().swap(MainAux->All_Routes[i]);
            }

        MainAux->first_Event = new Event;

        if (((AuxiliarMain::choose_RoutingAlgorithm == Routing_MH) ||
                (AuxiliarMain::choose_RoutingAlgorithm == Routing_SP) ||
                (AuxiliarMain::choose_RoutingAlgorithm == Routing_OSNRR)) &&
                (AuxiliarMain::choose_Simulation != Sim_RegenNum))
            for (int i = 0; i < Def::NumNodes * Def::NumNodes; i++) {
                MainAux->All_Routes[i] = Aux->All_Routes[i];
            }

        set_RequestEvent(MainAux->first_Event, MainAux->sim_Time);

        while ((Config->num_ReqBloq < Def::get_NumReqBloqMin()) &&
                (Config->num_Req < Def::get_NumReqMax()) && AuxiliarMain::SupremeFlag) {
            Event *curEvent = MainAux->first_Event;
            MainAux->first_Event = MainAux->first_Event->nextEvent;
            MainAux->sim_Time = curEvent->time;

            if (curEvent->type == Req) {
                request_Conn(curEvent, Config, MainAux);
            } else if (curEvent->type == Desc) {
                remove_Conn(curEvent, Config, MainAux);
            }
        }
    }

    long double PbReq = Config->num_ReqBloq / Config->num_Req;
    return PbReq;
}

/**
 * @brief Simulate initiates simulation, evokes memory cleaning. Request events accordingly to the event type, until Simulation's ending.
 * @param Config is the set of parameters over which the simulation is being run.
 * @param Aux is the set of parameters over which the simulation is being run.
 */
void Simulate(Def *Config, AuxiliarMain *Aux) {
    if (AuxiliarMain::SupremeFlag) {
        clear_Memory(Config, Aux);
        //Creates first event as requisition
        Aux->first_Event = new Event;
        set_RequestEvent(Aux->first_Event, Aux->sim_Time);

        while ((Config->num_ReqBloq < Def::get_NumReqBloqMin()) &&
                (Config->num_Req < Def::get_NumReqMax())) {
            Event *curEvent = Aux->first_Event;
            Aux->first_Event = Aux->first_Event->nextEvent;
            Aux->sim_Time = curEvent->time;

            if (curEvent->type == Req) {
                request_Conn(curEvent, Config, Aux);
            } else if (curEvent->type == Desc) {
                remove_Conn(curEvent, Config, Aux);
            }
        }

        cout << "Simulation Time = " << Aux->sim_Time << "  numReq = " <<
             Config->num_Req << endl;

        if (AuxiliarMain::choose_Simulation == Sim_PbReq ||
                AuxiliarMain::choose_Simulation == Sim_Whiskers) {
            cout << "nu0 = " << AuxiliarMain::laNet << "   PbReq= " <<
                 eval_BlockingProbability(Config) << "   PbAc = " << eval_AcceptationProbability(
                     Config) << "   PbSlots = " << (long double) Config->num_SlotsBloq /
                 Config->num_SlotsReq << " AvgHops = " << (long double)
                 Config->num_HopsPerRoute / (Config->num_Req - Config->num_ReqBloq) <<
                 " netOcc = " << (long double) Config->net_Occupancy << endl;
            AuxiliarMain::Result << AuxiliarMain::laNet << "\t" << (long double)
                                 Config->num_ReqBloq / Config->num_Req << "\t" << (long double)(
                                     1.0 - Config->num_ReqBloq / Config->num_Req) << "\t" <<
                                 (long double) Config->num_SlotsBloq / Config->num_SlotsReq << "\t" <<
                                 (long double) Config->num_HopsPerRoute / (Config->num_Req - Config->num_ReqBloq)
                                 << "\t" << Config->net_Occupancy << endl;
            AuxiliarMain::Result_2 << AuxiliarMain::laNet << "\t" <<
                                   Config->num_ReqBloqDueToQOT / Config->num_Req << "\t" <<
                                   (1.0 - Config->num_ReqBloqDueToQOT / Config->num_ReqBloq) *
                                   Config->num_ReqBloq / Config->num_Req << endl;
        } else if (AuxiliarMain::choose_Simulation == Sim_OSNR) {
            cout << "OSNR = " << Config->get_OSRNin() << "   PbReq = " <<
                 eval_BlockingProbability(Config) << "   PbAc = " << eval_AcceptationProbability(
                     Config) << "   PbSlots = " << (long double) Config->num_SlotsBloq /
                 Config->num_SlotsReq << " AvgHops = " << (long double)
                 Config->num_HopsPerRoute / (Config->num_Req - Config->num_ReqBloq) <<
                 " netOcc = " << (long double) Config->net_Occupancy << endl;
            AuxiliarMain::Result << Config->get_OSRNin() << "\t" <<
                                 (long double)                             Config->num_ReqBloq / Config->num_Req
                                 << "\t" << (long double) Config->num_SlotsBloq / Config->num_SlotsReq << "\t"
                                 << (long double) Config->num_HopsPerRoute / (Config->num_Req -
                                         Config->num_ReqBloq) << "\t" << Config->net_Occupancy << endl;
        } else if (AuxiliarMain::choose_Simulation == Sim_SlotsNum) {
            cout << "NSlots = " << Def::SL << "\t PbReq Phy = " <<
                 Config->num_ReqBloqDueToQOT / Config->num_Req << "\t PbReq Netw = " <<
                 (1.0 - Config->num_ReqBloqDueToQOT / Config->num_ReqBloq) *
                 Config->num_ReqBloq / Config->num_Req << endl;
            AuxiliarMain::Result << Def::SL << "\t" << Config->num_ReqBloq / Config->num_Req
                                 << "\t" << Config->num_ReqBloqDueToQOT / Config->num_Req << "\t" <<
                                 (1.0 - Config->num_ReqBloqDueToQOT / Config->num_ReqBloq) *
                                 Config->num_ReqBloq / Config->num_Req << endl;
        }

        {
            eval_BlockingProbabilityPerBitRate(Config);
            eval_AcceptationProbabilityPerBitRate(Config);
            eval_AverageBitRate(Config, Aux);
            eval_AcceptationPerScheme(Config);
        }

        if (AuxiliarMain::window != NULL) {
            AuxiliarMain::window->Refresh();
            AuxiliarMain::window->Slot_Screen4_RefreshProgressBar();
        }
    }
}

/**
 * @brief try_ToConnect Tries to allocate the spectrum with contiguity. Returns si, the initial slot, or -1 if the allocation hasn't succeeded.
 * @param route is the route in which the algorithm will try to allocate the spectrum.
 * @param NslotsReq is the number of required slots.
 * @param NslotsUsed is a pointer to the number of used slots. Will be 0 if it's impossible to allocate.
 * @param si is the initial slot of the requisition. Will be -1 if it's impossible to allocate.
 * @param Config is the set of parameters over which the simulation runs.
 */
void try_ToConnect(const Route *route, const int NslotsReq, int &NslotsUsed,
                   int &si, Def *Config) {
    si = -1, NslotsUsed = 0;

    switch (AuxiliarMain::choose_SpectrumAssignmentAlgorithm) {
        case RD:
            RWA::Random(route, NslotsReq, NslotsUsed, si, Config);
            break;

        case FF:
            RWA::FirstFit(route, NslotsReq, NslotsUsed, si, Config);
            break;

        case MU:
            RWA::MostUsed(route, NslotsReq, NslotsUsed, si, Config);
            break;

        default:
            cout << "Algorithm Undefined" << endl;
    }
}
