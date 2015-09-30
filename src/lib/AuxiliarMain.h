#ifndef MAIN_AUXILIAR_H
#define MAIN_AUXILIAR_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Event.h"
#include "Node.h"
#include "Link.h"
#include "Connection.h"
#include "Constant.h"
#include "OSNR_Tree.h"
#include "MainWindow.h"
#include "SSON_Thread.h"

/**
 * @brief The SpectrumAssignment enum is used to choose the Spectrum Assignment Algorithm. _Spectrum_Null is compliance and shouldn't be used.
 */
enum SpectrumAssignment_Algorithm {
    _Spectrum_Null, RD /*random*/, FF /*first-fit*/, MU /*most-used*/
};

/**
 * @brief The NetworkType enum is used to choose the type of network, whether Transparent or Translucent. _Network_Null is compliance and shouldn't be used.
 */
enum NetworkType {
    _Network_Null, TransparentNetwork, TranslucentNetwork
};

/**
 * @brief The Answers enum is a general enum used as a boolean variable. _Answer_Null is compliance and shouldn't be used.
 */
enum Answers {
    _Answer_Null, _NO, _YES
};

/**
 * @brief The Topologies enum is used to choose the topology used during the simulations. _Top_Null is compliance and shouldn't be used.
 */
enum Topologies {
    _Top_Null, Top_PacificBell, Top_NSFNet, Top_NFSNetMod, Top_PointToPoint4, Top_PointToPoint8, Top_Top1, Top_Top2, Top_European, Top_German, Top_USBackbone
};

/**
 * @brief The Simulations enum is used to choose the simulation type. _Sim_Null is compliance and shouldn't be used.
 */
enum Simulations {
    _Sim_Null, Sim_PbReq, Sim_OSNR, Sim_AmpDist, Sim_SlotsNum, Sim_Whiskers, Sim_RegenNum
};

/**
 * @brief The RP_Algorithm enum is used to choose the Regenerator Placement (RP) algorithm. _RP_Null is compliance and shouldn't be used.
 */
enum RP_Algorithm {
    _RP_Null, RP_NDF, RP_CNF, RP_TLP, RP_SQP, RP_MU, RP_MSU
};

/**
 * @brief The RA_Algorithm enum is used to choose the Regenerator Assignment (RA) algorithm. _RA_Null is compliance and shouldn't be used.
 */
enum RA_Algorithm {
    _RA_Null, RA_FLR, RA_FNS, RA_Both
};

/**
 * @brief The RoutingAlgorithm enum is used to choose the Routing Algorithm. _Routing_Null is compliance and shouldn't be used.
 */
enum Routing_Algorithm {
    _Routing_Null, Routing_MH, Routing_SP, Routing_LOR_AlPos, Routing_LOR_A, Routing_OSNRR
};

/**
 * @brief The Polarization enum is used to choose the number of polarizations used to transmit the optical signal. _Pol_Null is compliance and shouldn't be used.
 */
enum Polarization {
    _Pol_Null, Pol_Single = 1, Pol_Double = 2
};

/**
 * @brief The AuxiliarMain class stores parameters to the simulations.
 */
class AuxiliarMain {
  public:
    AuxiliarMain();
    ~AuxiliarMain();

    static bool SupremeFlag;
    /**
     * @brief refresh_Nodes creates new Nodes to this AuxiliarMain object.
     */
    void refresh_Nodes();

    /**
     * @brief Config is a standard Def object, used when no other is given.
     */
    static Def *Config;

    /**
     * @brief Results_Path is the path where the Results datafiles are saved.
     */
    static string Results_Path;
    /**
     * @brief Installed_Path is the path where the SSON is installed.
     */
    static string Installed_Path;
    /**
     * @brief isGraphic says whether the SSON is being run with graphic interface or not.
     */
    static Answers isGraphic;

    /**
     * @brief Result is the standard Result datafile.
     */
    static ofstream Result;

    /**
     * @brief Result_2 is an auxiliar Result datafile.
     */
    static ofstream Result_2;
    /**
     * @brief Result_PbReqPerRate is an auxiliar Result datafile.
     */
    static ofstream Result_PbReqPerRate;
    /**
     * @brief Result_PaReqPerRate is an auxiliar Result datafile.
     */
    static ofstream Result_PaReqPerRate;
    /**
     * @brief Result_AvgRate is an auxiliar Result datafile.
     */
    static ofstream Result_AvgRate;
    /**
     * @brief Result_Schemes is an auxiliar Result datafile.
     */
    static ofstream Result_Schemes;
    /**
     * @brief Result_DAmpGreaterThanThresh is an auxiliar Result datafile.
     */
    static ofstream Result_DAmpGreaterThanThresh;
    /**
     * @brief Result_DAmpLesserThanThresh is an auxiliar Result datafile.
     */
    static ofstream Result_DAmpLesserThanThresh;

    /**
     * @brief TopolNFSNet is the NSFNet Topology.
     */
    static ifstream TopolNFSNet;
    /**
     * @brief TopolPBell is the Pacific Bell Topology.
     */
    static ifstream TopolPBell;
    /**
     * @brief TopolReduced8 is a Point-to-Point Topology with 8 Nodes.
     */
    static ifstream TopolReduced8;
    /**
     * @brief TopolReduced4 is a Point-to-Point Topology with 4 Nodes.
     */
    static ifstream TopolReduced4;
    /**
     * @brief TopolNSFNetMod is a modified and reduced NSFNet Topology.
     */
    static ifstream TopolNSFNetMod;
    /**
     * @brief TopolTopology1 is the Topology1 Topology.
     */
    static ifstream TopolTopology1;
    /**
     * @brief TopolTopology2 is the Topology2 Topology.
     */
    static ifstream TopolTopology2;
    /**
     * @brief TopolEuropean is the European Topology.
     */
    static ifstream TopolEuropean;
    /**
     * @brief TopolGerman is the German Topology.
     */
    static ifstream TopolGerman;
    /**
     * @brief TopolUSBackbone is the US Backbone Topology.
     */
    static ifstream TopolUSBackbone;

    //Needed in main_SSON.cpp

    /**
     * @brief first_Event is the current Event being evaluated.
     */
    Event *first_Event;
    /**
     * @brief laNet is the network load in Erlang.
     */
    static long double laNet;

    /**
     * @brief OSNR_Min is the minimum Input OSNR simulated.
     */
    static long double OSNR_Min;
    /**
     * @brief OSNR_Max is the maximum Input OSNR simulated.
     */
    static long double OSNR_Max;
    /**
     * @brief OSNR_Step is the step between simulated Input OSNRs.
     */
    static long double OSNR_Step;

    /**
     * @brief LaNet_Min is the mininum Network Load, in Erlang, simulated.
     */
    static long double LaNet_Min;
    /**
     * @brief LaNet_Max is the maximum Network Load, in Erlang, simulated.
     */
    static long double LaNet_Max;
    /**
     * @brief LaNet_Step is the step between simulated Network Loads, in Erlang.
     */
    static long double LaNet_Step;

    /**
     * @brief AmpDist_Min is the minimum distance between In-Line Amplifiers, in km.
     */
    static long double AmpDist_Min;
    /**
     * @brief AmpDist_Max is the maximum distance between In-Line Amplifiers, in km.
     */
    static long double AmpDist_Max;
    /**
     * @brief AmpDist_Step is the step between simulated distance between In-Line Amplifiers, in km.
     */
    static long double AmpDist_Step;

    /**
     * @brief RegPerNode_Min is the minimum number of regenerators per node.
     */
    static long double RegPerNode_Min;
    /**
     * @brief RegPerNode_Max is the maximum number of regenerators per node.
     */
    static long double RegPerNode_Max;

    /**
     * @brief SimSlotsNum_minSlots is the minimum number of slots per link.
     */
    static int SlotsNum_Min;
    /**
     * @brief SimSlotsNum_maxSlots is the maximum number of slots per link.
     */
    static int SlotsNum_Max;

    /**
     * @brief mu is the connections deactivation rate.
     */
    static long double mu;
    /**
     * @brief Topology is the adjacency matrix of the chosen topology.
     */
    static long double *Topology;

    /**
     * @brief FlagRP_TLP is marked as true, if simulating RP-TLP.
     */
    static bool FlagRP_TLP;
    /**
     * @brief RP_TLP_NodeUsage is a vector used by RP-TLP Algorithm, indicating the regenerator usage of each node.
     */
    int *RP_TLP_NodeUsage;
    /**
     * @brief FlagRP_SQP is marked as true, if simulating RP-SQP.
     */
    static bool FlagRP_SQP;
    /**
     * @brief RP_SQP_NodeUsage is a vector used by RP-SQP Algorithm, indicating the regenerator usage of each node.
     */
    int *RP_SQP_NodeUsage;
    /**
     * @brief FlagRP_MU is marked as true, if simulating RP-MU.
     */
    static bool FlagRP_MU;
    /**
     * @brief RP_MU_NodeUsage is a vector used by RP-MU Algorithm, indicating the regenerator usage of each node.
     */
    int *RP_MU_NodeUsage;
    /**
     * @brief FlagRP_MSU is marked as true, if simulating RP-MSU.
     */
    static bool FlagRP_MSU;
    /**
     * @brief RP_MSU_NodeUsage is a vector used by RP-MSU Algorithm, indicating the regenerator usage of each node.
     */
    int *RP_MSU_NodeUsage;
    /**
     * @brief RegNumber_Total is the total number of regenerators.
     */
    static int RegNumber_Total;
    /**
     * @brief RegNumber_PerNode is the number of regenerators per node.
     */
    static int RegNumber_PerNode;
    /**
     * @brief LNMax is a parameter to the RP-SQP Algorithm.
     */
    static int LNMax;

    /**
     * @brief sim_Time is the total time taken by the simulation, measured in simulation's unities.
     */
    TIME sim_Time;
    /**
     * @brief All_Routes is a vector of vector of pointers to all the routes in the network that were found by the Routing Algorithm.
     */
    vector<Route *> *All_Routes;
    /**
     * @brief Network is a vector containing all the Nodes of this AuxiliarMain.
     */
    vector<Node> Network;
    /**
     * @brief Tree is the tree that caches the OSNRs.
     */
    static OSNR_Tree *Tree;
    /**
     * @brief LinkSet is a matrix containing all the links in the network.
     */
    static vector<Link> *LinkSet;
    /**
     * @brief choose_SpectrumAssignAlgorithm is where the chosen Spectrum Assignment Algorithm is saved.
     */
    static SpectrumAssignment_Algorithm choose_SpectrumAssignmentAlgorithm;
    /**
     * @brief choose_RoutingAlgorithm is where the chosen Routing Algorithm is saved.
     */
    static Routing_Algorithm choose_RoutingAlgorithm;
    /**
     * @brief consider_PhyImpair is a compliance value, that determines if Quality is considered to establish a connection at all.
     */
    static Answers consider_PhyImpair;
    /**
     * @brief choose_Topology is where the chosen Topology is saved.
     */
    static Topologies choose_Topology;
    /**
     * @brief choose_NetworkType is where the chosen Network Type is saved.
     */
    static NetworkType choose_NetworkType;
    /**
     * @brief choose_Simulation is where the chosen Simulation is saved.
     */
    static Simulations choose_Simulation;
    /**
     * @brief choose_RAAlgorithm is where the chosen Regenerator Assignment Algorithm is saved.
     */
    static RA_Algorithm choose_RAAlgorithm;
    /**
     * @brief choose_RPAlgorithm is where the chosen Regenerator Placement Algorithm is saved.
     */
    static RP_Algorithm choose_RPAlgorithm;
    /**
     * @brief choose_PolarizationModes is where the chosen number of possible polarizations is saved.
     */
    static Polarization choose_PolarizationModes;

    //Graphic Interface
    /**
     * @brief window is the Graphic Interface.
     */
    static MainWindow *window;
    /**
     * @brief TheGraphicThread is the Thread that executes the Graphic Interface.
     */
    static SSON_Thread *TheGraphicThread;
    /**
     * @brief TheTextualThread is the Thread that executes the Textual Interface.
     */
    static SSON_Thread *TheTextualThread;
    /**
     * @brief Translation selects the language in which the graphic interface will be presented
     */
    static The_Translations Translation;
    /**
     * @brief isDebugVersion indicates whether SSON is being compiled as debug or release version.
     */
    static Answers isDebugVersion;
    static void setFiles();
};

#endif /* MAIN_AUXILIAR_H */
