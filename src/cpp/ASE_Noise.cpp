/**
 * @file ASE_Noise.cpp Contains functions that do things related to ASE Noise evaluation.
*/

#include "Connection.h"
#include "AuxiliarMain.h"

/**
* @brief eval_OSNR evaluates the Optical Signal-To-Noise Ratio of a route.
* @param route is the route in which the OSNR is evaluated.
* @param Config is the set of parameters in which the evaluation occurs.
* @param Aux is the set of parameters in which the evaluation occurs.
* @return the OSNR on the receiver node
*/
long double eval_OSNR(Route *route, Def *Config, AuxiliarMain *Aux) {
    long double osnr;
    vector <Link *> *LinkSet = route->get_Nodes();

    if (OSNR_Tree::use_Tree) {
        osnr = Aux->Tree->get_OSNR(route);

        if (osnr != Def::MAX_LONGDOUBLE) {
            return osnr;
        }
    }

    long double Potencia = Config->get_Pin();
    long double Ruido = Config->get_Pin() / General::dBtoLin(
                            Config->get_OSRNin());


    for (unsigned i = 0; i < LinkSet->size(); i++) {
        Potencia *= LinkSet->at(i)->get_SourceNode()->get_Loss();
        Ruido *= LinkSet->at(i)->get_SourceNode()->get_Loss();; //Mux Losses

        if (i != 0) {
            LinkSet->at(i)->get_SourceNode()->set_TransmPower(General::LintodB(
                        Config->get_Pref() / 1E-3L));
        } else {
            LinkSet->at(i)->get_SourceNode()->set_TransmPower(General::LintodB(
                        Config->get_Pin() / 1E-3L));
        }

        Potencia *= LinkSet->at(i)->get_SourceNode()->get_PotAmplGain(Config);
        Ruido *= LinkSet->at(i)->get_SourceNode()->get_PotAmplGain(Config);
        Ruido += LinkSet->at(i)->get_SourceNode()->get_PotAmplNoise(
                     Config);  //Power Amplifier Losses

        Potencia /= LinkSet->at(i)->get_LinkLoss();
        Ruido /= LinkSet->at(i)->get_LinkLoss();
        Potencia *= LinkSet->at(i)->get_LinkGain();
        Ruido *= LinkSet->at(i)->get_LinkGain();
        Ruido += LinkSet->at(i)->get_LinkNoise();   //Link Losses

        Potencia *= LinkSet->at(i)->get_PreamplifierGain();
        Ruido *= LinkSet->at(i)->get_PreamplifierGain();
        Ruido += LinkSet->at(i)->get_PreamplifierNoise(); //Preamplifier Losses
        Potencia *= LinkSet->at(i)->get_DestinationNode()->get_LossSSSorDiv();
        Ruido *= LinkSet->at(i)->get_DestinationNode()->get_LossSSSorDiv(); //Network Devices Losses
    }

    osnr = 10 * log10(Potencia / Ruido);

    if (OSNR_Tree::use_Tree) {
        Aux->Tree->refresh_OSNR(route, osnr);
    }

    return osnr;
}

/**
 * @brief account_QoTBlocking accounts quality blockings in the respective Def object.
 *
 * When a connection is blocked due to no quality, this function accounts it in the respective Def object.
 * @param NslotsReq is the number of slots required by the connection.
 * @param NslotsUsed is the number of slots used by the connection. If it was blocked, should be zero.
 * @param Config is the set of parameters in which the blocking is accounted.
 */
void account_QoTBlocking(int NslotsReq, int NslotsUsed, Def *Config) {
    if (NslotsUsed <= 0) {   //Connection blocked
        Config->num_ReqBloqDueToQOT++;
    }

    Config->num_SlotsBloqDueToQOT += (NslotsReq - NslotsUsed);
}
