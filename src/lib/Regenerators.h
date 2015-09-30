#ifndef REGENERATORS_H
#define REGENERATORS_H

#include "Route.h"
#include "Event.h"
#include "AuxiliarMain.h"


/**
 * @brief The Regenerators class implements functions specificly related to translucent networks.
 */
class Regenerators {
  public:
    /**
     * @brief RP_NDF is the Nodal Degree First Regenerator Placement Algorithm.
     * @param N is the total number of regenerators.
     * @param X is the number of regenerators per node.
     */
    static void RP_NDF(int N, int X);
    /**
     * @brief RP_CNF is the Centered Node First Regenerator Placement Algorithm.
     * @param N is the total number of regenerators.
     * @param X is the number of regenerators per node.
     */
    static void RP_CNF(int N, int X);
    /**
     * @brief RP_TLP is the Traffic Load Prediction Based Regenerator Placement Algorithm.
     * @param N is the total number of regenerators.
     * @param X is the number of regenerators per node.
     */
    static void RP_TLP(int N, int X);
    /**
     * @brief RP_SQP is the Signal Quality Prediction Based Regenerator Placement Algorithm.
     * @param N is the total number of regenerators.
     * @param X is the number of regenerators per node.
     */
    static void RP_SQP(int N, int X);

    /**
     * @brief RP_MU is the Most Used Regenerator Placement Algorithm.
     * @param N is the total number of regenerators.
     * @param X is the number of regenerators per node.
     */
    static void RP_MU(int N, int X);

    /**
     * @brief RP_MSU is the Most Simultaneously Used Regenerator Placement Algorithm.
     * @param NX is the total number of regenerators, in the network.s
     */
    static void RP_MSU(int NX);

    /**
     * @brief RA_FLR is the First Longest Reach Regenerator Assignment Algorithm.
     * @param route is the route.
     * @param nBitRate is the id of the Bitrate of the requisition, in the range of Def::num_PossibleBitRates.
     * @param Config is the set of parameters in which the simulation occurs.
     * @param Aux is the set of parameters in which the simulation occurs.
     * @return true if succeed to establish.
     */
    static bool RA_FLR(Route *route, int nBitRate, Def *Config,
                       AuxiliarMain *Aux);
    /**
     * @brief RA_FNS is the First Narrowest Spectrum Regenerator Assignment Algorithm.
     * @param route is the route.
     * @param nBitRate is the id of the Bitrate of the requisition, in the range of Def::num_PossibleBitRates.
     * @param Config is the set of parameters in which the simulation occurs.
     * @param Aux is the set of parameters in which the simulation occurs.
     * @return true if succeed to establish.
     */
    static bool RA_FNS(Route *route, int nBitRate, Def *Config,
                       AuxiliarMain *Aux);

    /**
     * @brief RA_Both is a Heuristic between RA_FLR and RA_FNS to the Regenerator Assignment Problem.
     * @param route is the route.
     * @param nBitRate is the id of the Bitrate of the requisition, in the range of Def::num_PossibleBitRates.
     * @param Config is the set of parameters in which the simulation occurs.
     * @param Aux is the set of parameters in which the simulation occurs.
     * @return true if succeed to establish.
     */
    static bool RA_Both(Route *route, int nBitRate, Def *Config,
                        AuxiliarMain *Aux);

    /**
     * @brief refresh_Regenerators sets the number of regenerators in each node to zero.
     */
    static void refresh_Regenerators(AuxiliarMain *);

    /**
     * @brief SQP_evaluateRoute is used to evaluate which counters should be incremented in the RP_SQP Algorithm.
     * @param route is the route.
     * @param nBitRate is the id of the Bitrate of the requisition, in the range of Def::num_PossibleBitRates.
     * @param Aux is the set of parameters in which the simulation occurs.
     */
    static void SQP_evaluateRoute(Route *route,
                                  int nBitRate, AuxiliarMain *Aux);
    /**
     * @brief set_BR sets the bitrate of each regenerator.
     * @param the new bitrate.
     */
    static void set_BR(long double);

    /**
     * @brief get_BR returns the bitrate of each regenerator.
     * @return the bitrate of the regenerator.
     */
    static long double get_BR();

    /**
     * @brief SQP_LNMax is a vector with the value of the counter in each node of the network.
     */
    static int *SQP_LNMax;

    /**
     * @brief BR is the Bitrate of each regenerator.
     */
    static long double BR;

    /**
     * @brief RA_Both_DecisionRatio is the turning point between RA_FLR and RA_FNS Behaviour in RA_Both Algorithm.
     */
    static long double RA_Both_DecisionRatio;

    /**
     * @brief MainAux is a general-purpose set of parameters over which the RP simulations run.
     */
    static AuxiliarMain *MainAux;
  private:
    static bool existsSpectrumAndQuality(Route *route, long double BitRate,
                                         Def *Config,
                                         AuxiliarMain *Aux);
    static bool existsSpectrumAndQuality(Route *route, long double BitRate,
                                         Def *Config,
                                         AuxiliarMain *Aux, ModulationScheme Scheme);
    static ModulationScheme BestScheme(Route *route, long double BitRate,
                                       Def *Config, AuxiliarMain *Aux);
    static void SQP_evalLNMax();
    static int SQP_getLNMax(ModulationScheme Scheme, int nBitRate);
};

#endif // REGENERATORS_H
