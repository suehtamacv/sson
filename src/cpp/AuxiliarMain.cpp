#include "AuxiliarMain.h"
#include <QDir>
#include <ctime>

Def *AuxiliarMain::Config = new Def();
Answers AuxiliarMain::isGraphic = _YES;
Answers AuxiliarMain::isDebugVersion = _YES;
string AuxiliarMain::Results_Path;
string AuxiliarMain::Installed_Path;
ofstream AuxiliarMain::Result;
ofstream AuxiliarMain::Result_2;
ofstream AuxiliarMain::Result_PbReqPerRate;
ofstream AuxiliarMain::Result_PaReqPerRate;
ofstream AuxiliarMain::Result_AvgRate;
ofstream AuxiliarMain::Result_DAmpGreaterThanThresh;
ofstream AuxiliarMain::Result_DAmpLesserThanThresh;
ofstream AuxiliarMain::Result_Schemes;
ifstream AuxiliarMain::TopolNFSNet;
ifstream AuxiliarMain::TopolPBell;
ifstream AuxiliarMain::TopolReduced8;
ifstream AuxiliarMain::TopolReduced4;
ifstream AuxiliarMain::TopolNSFNetMod;
ifstream AuxiliarMain::TopolTopology1;
ifstream AuxiliarMain::TopolTopology2;
ifstream AuxiliarMain::TopolEuropean;
ifstream AuxiliarMain::TopolGerman;
ifstream AuxiliarMain::TopolUSBackbone;

long double AuxiliarMain::laNet;
long double AuxiliarMain::OSNR_Min;
long double AuxiliarMain::OSNR_Max;
long double AuxiliarMain::OSNR_Step;
long double AuxiliarMain::LaNet_Min;
long double AuxiliarMain::LaNet_Max;
long double AuxiliarMain::LaNet_Step;
long double AuxiliarMain::AmpDist_Min;
long double AuxiliarMain::AmpDist_Max;
long double AuxiliarMain::AmpDist_Step;
long double AuxiliarMain::RegPerNode_Min;
long double AuxiliarMain::RegPerNode_Max;
int AuxiliarMain::SlotsNum_Min;
int AuxiliarMain::SlotsNum_Max;
long double AuxiliarMain::mu;
long double *AuxiliarMain::Topology;
bool AuxiliarMain::FlagRP_TLP = false;
bool AuxiliarMain::FlagRP_SQP = false;
bool AuxiliarMain::FlagRP_MU = false;
bool AuxiliarMain::FlagRP_MSU = false;
vector<Link> *AuxiliarMain::LinkSet;
SpectrumAssignment_Algorithm AuxiliarMain::choose_SpectrumAssignmentAlgorithm;
Routing_Algorithm AuxiliarMain::choose_RoutingAlgorithm;
Answers AuxiliarMain::consider_PhyImpair;
Topologies AuxiliarMain::choose_Topology;
Simulations AuxiliarMain::choose_Simulation;
NetworkType AuxiliarMain::choose_NetworkType;
RA_Algorithm AuxiliarMain::choose_RAAlgorithm;
RP_Algorithm AuxiliarMain::choose_RPAlgorithm;
Polarization AuxiliarMain::choose_PolarizationModes = Pol_Double;
OSNR_Tree *AuxiliarMain::Tree;
int AuxiliarMain::RegNumber_PerNode;
int AuxiliarMain::RegNumber_Total;
int AuxiliarMain::LNMax;
MainWindow *AuxiliarMain::window;
SSON_Thread *AuxiliarMain::TheGraphicThread;
SSON_Thread *AuxiliarMain::TheTextualThread;
The_Translations AuxiliarMain::Translation = Translation_English;

bool AuxiliarMain::SupremeFlag = true;

AuxiliarMain::AuxiliarMain() {
    All_Routes = new vector<Route *>[Def::NumNodes * Def::NumNodes];

    for (int i = 0; i < Def::NumNodes * Def::NumNodes; i++) {
        while (!All_Routes[i].empty()) {
            delete All_Routes[i].back();
            All_Routes[i].pop_back();
        }
    }

    first_Event = NULL;
    RP_TLP_NodeUsage = new int[Def::NumNodes];
    RP_SQP_NodeUsage = new int[Def::NumNodes];
    RP_MSU_NodeUsage = new int[Def::NumNodes];
    RP_MU_NodeUsage = new int[Def::NumNodes];

    for (int i = 0; i < Def::NumNodes; i++) {
        RP_TLP_NodeUsage[i] = 0;
        RP_SQP_NodeUsage[i] = 0;
        RP_MSU_NodeUsage[i] = 0;
        RP_MU_NodeUsage[i] = 0;
    }

    Network.clear();
}

void AuxiliarMain::setFiles() {
    if (isDebugVersion == _YES) {
        AuxiliarMain::Installed_Path = string(getenv("HOME")) +
                                       "/.local/share/sson/";
    } else {
        AuxiliarMain::Installed_Path = "/usr/share/sson/";
    }

    AuxiliarMain::TopolNFSNet.open(Installed_Path +
                                   "Topologies/TopologyNFS.txt");
    AuxiliarMain::TopolPBell.open(Installed_Path +
                                  "Topologies/TopologyPBell.txt");
    AuxiliarMain::TopolReduced8.open(Installed_Path +
                                     "Topologies/TopologyRed8.txt");
    AuxiliarMain::TopolReduced4.open(Installed_Path +
                                     "Topologies/TopologyRed4.txt");
    AuxiliarMain::TopolNSFNetMod.open(Installed_Path +
                                      "Topologies/TopologyNFSMod.txt");
    AuxiliarMain::TopolTopology1.open(Installed_Path +
                                      "Topologies/TopologyTop1.txt");
    AuxiliarMain::TopolTopology2.open(Installed_Path +
                                      "Topologies/TopologyTop2.txt");
    AuxiliarMain::TopolEuropean.open(Installed_Path +
                                     "Topologies/TopologyEuropean.txt");
    AuxiliarMain::TopolGerman.open(Installed_Path +
                                   "Topologies/TopologyGerman.txt");
    AuxiliarMain::TopolUSBackbone.open(Installed_Path +
                                       "Topologies/TopologyUSBackbone.txt");

    time_t t = time(0);
    struct tm *now = localtime(&t);

    AuxiliarMain::Results_Path = string(getenv("HOME")) + "/.sson/" +
                                 to_string(now->tm_year + 1900) + "-" + to_string(now->tm_mon + 1) + "-" +
                                 to_string(now->tm_mday) + "/" + to_string(now->tm_hour) + "-" + to_string(
                                     now->tm_min) + "-" + to_string(now->tm_sec) + "/";

    if (!QDir(QString::fromStdString(Results_Path)).exists()) {
        QDir().mkpath(QString::fromStdString(Results_Path));
    } //Didn't wanted to use Qt, but here am I.

    AuxiliarMain::Result.open(Results_Path + "Result.txt");
    AuxiliarMain::Result_2.open(Results_Path + "Result_2.txt");
    AuxiliarMain::Result_PbReqPerRate.open(Results_Path +
                                           "Result_PbReqPerRate.txt");
    AuxiliarMain::Result_PaReqPerRate.open(Results_Path +
                                           "Result_PaReqPerRate.txt");
    AuxiliarMain::Result_AvgRate.open(Results_Path + "AvgRate.txt");
    AuxiliarMain::Result_DAmpGreaterThanThresh.open(Results_Path +
            "DAmpGreaterThanThreshold.txt");
    AuxiliarMain::Result_DAmpLesserThanThresh.open(Results_Path +
            "DAmpLesserThanThreshold.txt");
    AuxiliarMain::Result_Schemes.open(Results_Path + "Result_Schemes.txt");
}

AuxiliarMain::~AuxiliarMain() {
    delete[] All_Routes;
    delete[] RP_TLP_NodeUsage;
    delete[] RP_SQP_NodeUsage;
    delete[] RP_MU_NodeUsage;
    delete[] RP_MSU_NodeUsage;
}

void AuxiliarMain::refresh_Nodes() {
    for (int i = 0; i < Def::NumNodes; i++) {
        this->Network.push_back(Node(i));
    }
}
