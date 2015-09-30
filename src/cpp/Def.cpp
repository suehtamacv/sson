#include "Def.h"
#include "math.h"
#include "Constant.h"
#include "AuxiliarMain.h"
#include <limits>

//Inicializa constantes estáticas
vector<int> Def::Node_Degree(0);
double Def::MAX_DOUBLE = std::numeric_limits<double>::max();
int Def::MAX_INT = std::numeric_limits<int>::max();
long double Def::MAX_LONGDOUBLE = std::numeric_limits<double>::max();
int Def::NumNodes;
long double Def::num_ReqBloqMin = 0.0;
long double Def::num_ReqMax = 1E7; //dez milhoes de requisicoes
std::vector<long double> Def::Possible_BitRates = {10E9, 40E9, 100E9, 160E9, 400E9 };
long double Def::GreatestBitRate = 400E9;
int Def::SL = 0;
int Def::SR = 0;
int Def::maxSim_Whiskers = 0;
long double Def::P_in = 1E-3;
long double Def::P_ref = 1E-3;
long double Def::L_SSS = 5.0;
long double Def::freq = 193.4E12;
NodeArchiteture Def::Architeture = Arch_SS;
long double Def::lambda = 1550E-9;
long double Def::B_Slot = 12.5E9;
long double Def::B_Ref = 12.5E9;
long double Def::NF_Amp = 5.0;
long double Def::F_Amp = General::dBtoLin((long double) Def::NF_Amp);
long double Def::Amp_Distance = 80;
long double Def::OSNR_in = 30;

Def::Def() {
    num_ReqAccepScheme = new long double[num_ModulationSchemes];
    num_ReqBitRate = new long double[Def::get_numPossibleBitRates()];
    num_ReqBloqBitRate = new long double[Def::get_numPossibleBitRates()];
    Topology_S = new bool[Def::NumNodes * Def::NumNodes * Def::SL];

    for (int i = 0; i < Def::SL; i++)
        for (int j = 0; j < Def::NumNodes; j++)
            for (int k = 0; k < Def::NumNodes; k++) {
                Topology_S[i * Def::NumNodes * Def::NumNodes + Def::NumNodes * j + k] = false;
            }

    num_SlotsBloq = 0.0;
    num_SlotsBloqDueToQOT = 0.0;
    num_ReqBloqDueToQOT = 0.0;
    num_SlotsReq = 0.0;
    num_ReqBloq = 0.0;
    num_HopsPerRoute = 0.0;
    num_Req = 0.0;
    net_Occupancy = 0.0;
    TotalTime_Scheme = new long double[Def::get_numPossibleBitRates()];
    TotalData_Scheme = new long double[num_ModulationSchemes];
    TotalData = 0;
}

Def::~Def() {
    delete[] num_ReqAccepScheme;
    delete[] num_ReqBitRate;
    delete[] num_ReqBloqBitRate;
    delete[] Topology_S;
    delete[] TotalTime_Scheme;
    delete[] TotalData_Scheme;
}

void Def::set_BSlot(double BSlot) {
    assert((BSlot > 0));
    Def::B_Slot = BSlot * 1E9;
}

void Def::set_BRef_Norm(double BRef) {
    assert((BRef > 0));
    Def::B_Ref = BRef;
}

void Def::set_BRef(double BRef) {
    assert((BRef > 0));
    Def::B_Ref = BRef * 1E9;
}

long double Def::get_lambda() {
    return lambda;
}

void Def::set_OSNRin(long double osnr) {
    assert((osnr >= 0));
    Def::OSNR_in = osnr;
}

void Def::clear_NodeDegree() {
    Node_Degree.clear();
}

int Def::get_NodeDegree(int No) {
    assert((No < NumNodes));
    return Node_Degree.at(No);
}

double Def::get_LaNet(int Lr) {
    assert((Lr > 0 && Lr <= SR));
    return LaNet.at(Lr);
}

double Def::get_OSNRThreshold(ModulationScheme Scheme, long double BitRate) {
    return General::LintodB(0.5 * BitRate * get_snrb(Scheme) /
                            Def::get_BRef());
}

long double Def::get_BRef() {
    return B_Ref;
}

int Def::get_SL() {
    return SL;
}

long double Def::get_NumReqMax() {
    return num_ReqMax;
}

void Def::set_NumReqMax(long double Num) {
    num_ReqMax = Num;
}

long double Def::get_snrb(ModulationScheme Scheme) {
    switch (Scheme) {
        case _4QAM:
            return General::dBtoLin(6.8);
            break;

        case _16QAM:
            return General::dBtoLin(10.5);
            break;

        case _64QAM:
            return General::dBtoLin(14.8);
            break;

        default:
            return 0;
    }
}

int Def::get_SR() {
    return SR;
}

int Def::get_NumNodes() {
    return NumNodes;
}

long double Def::get_NumReqBloqMin() {
    return num_ReqBloqMin;
}

void Def::set_NodeDegree(int Grau) {
    assert((Node_Degree.size() < (unsigned) NumNodes));
    Node_Degree.push_back(Grau);
}

void Def::set_LaCheck(double la) {
    double sum = 0.0;

    for (int Lr = 1; Lr <= SR; Lr++) {
        sum += LaNet.at(Lr);
    }

    assert((sum > 0.999 * la && sum < 1.001 * la));
}

void Def::set_LaRandom(double la) {
    LaNet.clear();
    LaNet.push_back(-1.0);    //Lr=0 can't be requested
    double sum = 0.0;

    for (int Lr = 1; Lr <= SR; Lr++) {
        LaNet.push_back(General::uniform(0.0, 1.0));
        sum += LaNet.at(Lr);
    }

    for (int Lr = 1; Lr <= SR; Lr++) {
        LaNet.at(Lr) = (LaNet.at(Lr) / sum) * la;
    }

    set_LaCheck(la); //Checks
}

void Def::set_LaUniform(double la) {
    LaNet.clear();
    LaNet.push_back(-1.0);    //Lr=0 can't be requested

    for (int Lr = 1; Lr <= SR; Lr++) {
        LaNet.push_back((double) la / SR);
    }

    set_LaCheck(la);    //Checks
}

void Def::set_NumNodes(int x) {
    NumNodes = x;
}

void Def::set_NumReqBloqMin(long double x) {
    assert((x > 0));
    num_ReqBloqMin = x;
}

void Def::set_SL(int x) {
    assert((x > 0));
    SL = x;
}

void Def::set_SR(int x) {
    SR = x;
}

void Def::set_Pin(long double p) {
    P_in = 1E-3 * General::dBtoLin(p);    //Conversion to Watt
}

void Def::set_LSSS(long double l) {
    assert((l > 0));
    L_SSS = l;
}

void Def::set_AmpDist(long double d) {
    Amp_Distance = d;
}

void Def::set_Architeture(NodeArchiteture a) {
    Architeture = a;
}

long double Def::get_Pin() {
    return P_in;
}

long double Def::get_OSRNin() {
    return OSNR_in;
}

long double Def::get_LSSS() {
    return L_SSS;
}

long double Def::get_AmpDist() {
    return Amp_Distance;
}

long double Def::get_BSlot() {
    return B_Slot;
}

void Def::set_FAmp(long double Num) {
    F_Amp = Num;
}

long double Def::get_FAmp() {
    return F_Amp;
}

int Def::get_numPossibleBitRates() {
    return Possible_BitRates.size();
}

NodeArchiteture Def::get_Architeture() {
    return Architeture;
}

long double Def::get_Pref() {
    return P_ref;
}

void Def::set_PRef(long double P) {
    P_ref = 1E-3 * General::dBtoLin(P);
}

void Def::set_freq(long double Num) {
    freq = Num;
}

long double Def::get_freq() {
    return freq;
}
