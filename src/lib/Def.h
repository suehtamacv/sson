#ifndef Def_H
#define Def_H

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "General.h"

/**
 * @brief The ModulationScheme enum is used to choose the modulation scheme used on each connection.
 */
enum ModulationScheme {
    _4QAM = 4, _16QAM = 16, _64QAM = 64
};
/**
 * @brief num_ModulationSchemes is the number of modulation schemes.
 */
const int num_ModulationSchemes = 3;

/**
 * @brief The NodeArchiteture enum is used to choose between two architectures of node, Broadcast
 * and Select or Spectrum Switching.
 * Arch_Null is compliance and shouldn't be used.
 */
enum NodeArchiteture {
    Arch_Null, Arch_BS, Arch_SS
};

typedef long double TIME;

using namespace std;

/**
 * @brief The Def class implements the set of parameters on which the simulation depends.
 */
class Def {
  public:
    /**
     * @brief Def is Def's constructor.
     */
    Def();
    ~Def();

    /**
     * @brief Possible_BitRates is a vector containing all possible bitrates.
     */
    static std::vector<long double> Possible_BitRates;
   /**
     * @brief GreatestBitRate is the value of the greatest bitrate available.
     */
    static long double GreatestBitRate;

    /**
     * @brief NumNodes is the number of nodes in the Topology.
     */
    static int NumNodes;
    /**
     * @brief SL is the number of slots per link.
     */
    static int SL;
    /**
     * @brief SR is the maximum number of slots that can be requisited. It is usually the same as Def::SL.
     */
    static int SR;

    /**
     * @brief Node_Degree is a vector with the degree of each node.
     */
    static vector<int> Node_Degree;

    /**
     * @brief MAX_DOUBLE is the greatest Double possible.
     */
    static double MAX_DOUBLE;
    /**
     * @brief MAX_INT is the greatest Int possible.
     */
    static int MAX_INT;
    /**
     * @brief MAX_LONGDOUBLE is the greatest Long Double possible.
     */
    static long double MAX_LONGDOUBLE;
    /**
     * @brief maxSim_Whiskers is the Number of Repetitions in Whiskers Simulation.
     */
    static int maxSim_Whiskers;
    /**
     * @brief net_Occupancy is the total number of slots succesfully required during the Simulation.
     */
    long double net_Occupancy;
    /**
     * @brief num_HopsPerRoute is the average number of hops per route.
     */
    long double num_HopsPerRoute;
    /**
     * @brief num_Req is the number of requisitions simulated.
     */
    long double num_Req;
    /**
     * @brief num_ReqBloq is the number of requisitions blocked.
     */
    long double num_ReqBloq;
    /**
     * @brief num_ReqBloqDueToQOT is the number of requisitions blocked due to QoT.
     */
    long double num_ReqBloqDueToQOT;
    /**
     * @brief num_SlotsBloq is the number of blocked slots.
     */
    long double num_SlotsBloq;
    /**
     * @brief num_SlotsReq is the number of required slots.
     */
    long double num_SlotsReq;
    /**
     * @brief num_SlotsBloqDueToQOT is the number of slots blocked due to QoT.
     */
    long double num_SlotsBloqDueToQOT;
    /**
     * @brief num_ReqBitRate is a vector of Def::num_PossibleBitRates with the number of requisitions simulated by bitrate.
     */
    long double *num_ReqBitRate;
    /**
     * @brief num_ReqBloqBitRate is a vector of Def::num_PossibleBitRates with the number of requisitions simulated blocked by bitrate.
     */
    long double *num_ReqBloqBitRate;
    /**
     * @brief num_ReqAccepScheme is a vector of num_ModulationSchemes with the number of requisitions established by modulation scheme.
     */
    long double *num_ReqAccepScheme;
    /**
     * @brief TotalTime_Scheme is a vector of num_ModulationSchemes with the total time of established connections, per modulation scheme.
     */
    long double *TotalTime_Scheme;
    /**
     * @brief TotalData_Scheme is a vector of num_ModulationSchemes with the total amount of data transfered, per modulation scheme.
     */
    long double *TotalData_Scheme;
    /**
     * @brief TotalData is the total amount of data transfered.
     */
    long double TotalData;
    /**
     * @brief Topology_S is a matrix of Def::NumNodes * Def::NumNodes * Def::SL indicating whether the slot in determinated link is available or not.
     */
    bool *Topology_S;

    /**
     * @brief clear_NodeDegree clears Def::Node_Degree.
     */
    void clear_NodeDegree();
    /**
     * @brief get_BSlot returns the Bandwidth of a slot, in Hz.
     * @return the Bandwidth of a slot, in Hz.
     */
    static long double get_BSlot();
    /**
     * @brief get_BRef returns the reference Bandwidth, in Hz.
     * @return the reference Bandwidth, in Hz.
     */
    static long double get_BRef();
    /**
     * @brief get_NodeDegree returns the degree of the i-th node.
     * @param i is the i-th node.
     * @return the degree of the i-th node.
     */
    static int get_NodeDegree(int i);
    /**
     * @brief get_lambda returns the wavelength.
     * @return the wavelength.
     */
    static long double get_lambda(void);
    /**
     * @brief get_LaNet returns the network load in the s-th slot.
     * @param s is the s-th slot.
     * @return the network load in the s-th slot.
     */
    double get_LaNet(int s);
    /**
     * @brief get_OSNRThreshold returns the OSNR quality threshold, in dB.
     * @param Scheme is the Modulation Scheme.
     * @param BitRate is the Bitrate, in bps.
     * @return the OSNR quality threshold, in dB, in order to have a 10**-3 bit error rate.
     */
    static double get_OSNRThreshold(ModulationScheme Scheme, long double BitRate);
    /**
     * @brief get_NumNodes returns the number of nodes in the network.
     * @return the number of nodes in the network.
     */
    static int get_NumNodes();
    /**
     * @brief get_NumReqBloqMin returns the minimum number of blocked requisitions until the end of simulation.
     * @return the minimum number of blocked requisitions until the end of simulation.
     */
    static long double get_NumReqBloqMin();
    /**
     * @brief get_NumReqMax returns the maximum number of requisitions until simulation's ending.
     * @return the maximum number of requisitions until simulation's ending.
     */
    static long double get_NumReqMax();
    /**
     * @brief set_NumReqMax sets the maximum number os requisitions until simulation's ending.
     * @param the new maximum number ofrequisitions until simulation's ending.
     */
    static void set_NumReqMax(long double);
    /**
     * @brief set_NumNodes sets the number of nodes in the network.
     * @param N the number of nodes in the network.
     */
    static void set_NumNodes(int N);
    /**
     * @brief get_SL returns the number of slots per link.
     * @return the number of slots per link.
     */
    static int get_SL();
    /**
     * @brief get_SR returns the maximum number of slots that can be requisited.
     * @return the maximum number of slots that can be requisited.
     */
    static int get_SR();
    /**
     * @brief set_Architeture sets the Node Architecture.
     * @param Arch is the Node Architecture.
     */
    static void set_Architeture(NodeArchiteture Arch);
    /**
     * @brief set_AmpDist sets the distance between In-Line Amplifiers.
     * @param Dist is the distance between In-Line Amplifiers.
     */
    void set_AmpDist(long double Dist);
    /**
     * @brief set_LSSS sets the loss, in dB, of the SSS elements.
     * @param L is the loss, in dB, of the SSS elements.
     */
    static void set_LSSS(long double L);
    /**
     * @brief set_Pin sets the power, in dBm, of the transmitters.
     * @param P is the power, in dBm, of the transmitters.
     */
    void set_Pin(long double P);
    /**
     * @brief set_PRef sets the power, in dBm, used in the fibers.
     * @param P is the power, in dBm, used in the fibers.
     */
    void set_PRef(long double P);

    /**
     * @brief set_BSlot sets the bandwidth of a slot.
     * @param B is the bandwidth, in GHz.
     */
    static void set_BSlot(double B);
    /**
     * @brief set_BRef_Norm sets the reference bandwidth.
     * @param B is the bandwidth.
     */
    static void set_BRef_Norm(double B);
    /**
     * @brief set_BRef sets the reference bandwidth.
     * @param B is the bandwidth, in GHz.
     */
    static void set_BRef(double B);
    /**
     * @brief set_NodeDegree appends Degree to Def::Node_Degree.
     * @param Degree is a new evaluated Node Degree.
     */
    static void set_NodeDegree(int Degree);
    /**
     * @brief set_LaCheck confirms wether the traffic is normalized
     * @param la is the total network load, in Erlang.
     */
    void set_LaCheck(double);
    /**
     * @brief set_LaRandom configurates random traffic between the slots.
     * @param la is the total network load, in Erlang.
     */
    void set_LaRandom(double la);
    /**
     * @brief set_LaUniform configurates uniform traffic between the slots.
     * @param la is the total network load, in Erlang.
     */
    void set_LaUniform(double la);
    /**
     * @brief set_NumReqBloqMin configurates the minimum number of bloqued requisitions until simulation's ending.
     * @param N is the minimum number of bloqued requisitions until simulation's ending.
     */
    static void set_NumReqBloqMin(long double N);
    /**
     * @brief set_SL sets the maximum number of slots that can be requisited.
     * @param SL is the maximum number of slots that can be requisited.
     */
    static void set_SL(int SL);
    /**
     * @brief set_SR sets the maximum number of slots that can be requisited.
     * @param SR is the maximum number of slots that can be requisited.
     */
    static void set_SR(int SR);
    /**
     * @brief set_OSNRin sets the OSNR of the transmitter.
     * @param OSNR is the OSNR of the transmitter.
     */
    static void set_OSNRin(long double OSNR);

    /**
     * @brief get_Architeture returns the Node Architectures.
     * @return the Node Architectures.
     */
    static NodeArchiteture get_Architeture();
    /**
     * @brief get_Pin returns the transmitter power, in Watts.
     * @return the transmitter power, in Watts.
     */
    long double get_Pin();
    /**
     * @brief get_Pref returns the reference power, in Watts.
     * @return the reference power, in Watts.
     */
    long double get_Pref();
    /**
     * @brief get_OSRNin returns the input OSNR, in dB.
     * @return the input OSNR, in dB.
     */
    static long double get_OSRNin();
    /**
     * @brief get_LSSS returns the losses in SSS elements, in dB.
     * @return the losses in SSS elements, in dB.
     */
    static long double get_LSSS();
    /**
     * @brief get_AmpDist returns the distance between In-Line Amplifiers, in km.
     * @return the distance between In-Line Amplifiers, in km.
     */
    long double get_AmpDist();
    /**
     * @brief get_FAmp sets the amplifier's noise factor, in linear unities.
     * @param the new amplifier's noise factor, in linear unities.
     */
    static void set_FAmp(long double);
    /**
     * @brief get_FAmp returns the amplifier's noise factor, in linear unities.
     * @return the amplifier's noise factor, in linear unities.
     */
    static long double get_FAmp();
    /**
     * @brief set_freq sets the frequency, in Hz.
     * @param the new frequency, in Hz.
     */
    static void set_freq(long double);
    /**
     * @brief get_freq returns the frequency, in Hz.
     * @return the frequency, in Hz.
     */
    static long double get_freq();
    /**
     * @brief get_numPossibleBitRates returns the number of possible bitrates.
     * @return the number of possible bitrates.
     */
    static int get_numPossibleBitRates();
    static long double NF_Amp; //Amplifier Noise Factor
  private:
    vector<double> LaNet; //Traffic Through Each Slot
    static long double num_ReqBloqMin; //Minimum Number of Blocked Requisitions
    static long double num_ReqMax; //Maximum Number of Requisitions

    static long double P_in; //Input Power
    static long double P_ref; //Reference Power
    static long double OSNR_in; //Input OSNR
    static long double L_SSS; //Devices Losses
    static long double Amp_Distance; //Distance Between InLine Amplifiers
    static NodeArchiteture Architeture; //Node Architetures
    static long double lambda; //Wavelength
    static long double B_Slot; //Slot Bandwidth
    static long double B_Ref; //Reference Bandwidth
    static long double F_Amp; //Amplifier Noise Factor
    static long double freq;
    static long double get_snrb(
        ModulationScheme);  //Threshold SNRb

};
#endif /* DEF_H */
