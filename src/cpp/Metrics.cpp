/**
  * @file Metrics.cpp Contains functions that evaluate several metrics to the simulation.
  **/


#include "Def.h"
#include "AuxiliarMain.h"

/**
 * @brief eval_BlockingProbability evaluates the blocking probability of the simulation that happened over Config.
 *
 * The Blocking Probability is aproximated by the number of Blocked Requisitions over the total number of Requisitions.
 *
 * @param Config is the set of parameters over which the simulation happened.
 * @return the blocking probability.
 */
long double eval_BlockingProbability(Def *Config) {
    return Config->num_ReqBloq / Config->num_Req;
}

/**
 * @brief eval_AcceptationProbability evaluates the acceptation probability of the simulation that happened over Config.
 *
 * The Blocking Probability is aproximated by the number of Accepted Requisitions over the total number of Requisitions.
 *
 * @param Config is the set of parameters over which the simulation happened.
 * @return the acceptation probability.
 */
long double eval_AcceptationProbability(Def *Config) {
    return 1.0 - eval_BlockingProbability(Config);
}

/**
 * @brief eval_BlockingProbabilityPerBitRate prints in AuxiliarMain::Result_PbReqPerRate the blocking probability of each Bitrate defined in Def::Possible_BitRates.
 * @param Config is the set of parameters over which the simulation happened.
 */
void eval_BlockingProbabilityPerBitRate(Def *Config) {
    for (int i = 0; i < Def::get_numPossibleBitRates(); i++) {
        AuxiliarMain::Result_PbReqPerRate << AuxiliarMain::laNet << "\t" <<
                                          Def::Possible_BitRates.at(i) << "\t"
                                          << Config->num_ReqBloqBitRate[i] / Config->num_ReqBitRate[i] << endl;
    }
}

/**
 * @brief eval_AcceptationProbabilityPerBitRate prints in AuxiliarMain::Result_PaReqPerRate the acceptation probability of each Bitrate defined in Def::Possible_BitRates.
 * @param Config is the set of parameters over which the simulation happened.
 */
void eval_AcceptationProbabilityPerBitRate(Def *Config) {
    for (int i = 0; i < Def::get_numPossibleBitRates(); i++) {
        AuxiliarMain::Result_PaReqPerRate << AuxiliarMain::laNet << "\t" <<
                                          Def::Possible_BitRates.at(i) <<
                                          "\t" << 1.0 - Config->num_ReqBloqBitRate[i] / Config->num_ReqBitRate[i] << endl;
    }
}

/**
 * @brief eval_AverageBitRate evaluates the average bitrate of the network, and prints it in AuxiliarMain::Result_AvgRate.
 *
 * In AuxiliarMain::Result_AvgRate, the result is written in the first column with the Network Load and in the second column the Average Bitrate in bps.
 *
 * @param Config is the set of parameters over which the simulation happened.
 * @param Aux is the set of parameters over which the simulation happened.
 */
void eval_AverageBitRate(Def *Config, AuxiliarMain *Aux) {
    long double AvgBitRate = 0;

    for (int i = 0; i < Def::get_numPossibleBitRates(); i++) {
        AvgBitRate  += Def::Possible_BitRates.at(i) * Config->TotalTime_Scheme[i];
    }

    AvgBitRate  /= Aux->sim_Time;
    cout << "Average BitRate is " << AvgBitRate  / pow(10, 9) << " Gbps" << endl;
    AuxiliarMain::Result_AvgRate << AuxiliarMain::laNet << "\t" << AvgBitRate  <<
                                 endl;
}

/**
 * @brief eval_AcceptationPerScheme evaluates the acceptation probability of requisitions per Modulation Scheme defined in ModulationScheme.
 * @param Config is the set of parameters over which the simulation happened.
 */
void eval_AcceptationPerScheme(Def *Config) {
    for (int i = 0; i < num_ModulationSchemes; i++) {
        AuxiliarMain::Result_Schemes << i << "\t" << Config->num_ReqAccepScheme[i] /
                                     Config->num_Req << "\t" << Config->TotalData_Scheme[i] / Config->TotalData <<
                                     endl;
    }
}
