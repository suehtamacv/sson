#include "Link.h"
#include "AuxiliarMain.h"
#include "math.h"
#include "Def.h"
#include "assert.h"
#include "Constant.h"

Link::Link(Node *NSource, Node *NDestination, double Leng) {
    assert((Leng > 0));
    Source = NSource;
    Destination = NDestination;
    Length = Leng;
    eval_Losses();

    if (NSource != NULL) {   //if isn't a dummy Link
        eval_NumAmplifiers(AuxiliarMain::Config);
        eval_LinkGain();
        eval_LinkNoise();
        eval_PreamplifierGainNoise(AuxiliarMain::Config);
    }
}

void Link::eval_NumAmplifiers(Def *Config) {
    Num_Amplifiers = floor(Length / Config->get_AmpDist());

    if (ceil(Length / Config->get_AmpDist()) == Num_Amplifiers) {
        Num_Amplifiers--;
    }
}

void Link::eval_LinkGain() {
    Link_Gain = General::dBtoLin(1.0 * Num_Amplifiers * (General::LintodB(
                                     L_DCF) + General::LintodB(L_FB)) / (1.0 * Num_Amplifiers + 1.0));
}

long double Link::get_LinkGain() {
    return Link_Gain;
}

void Link::eval_LinkNoise() {
    if (Num_Amplifiers == 0) {
        Noise_Gain = 0;
    } else {
        double freq = Def::get_freq();

        Noise_Gain = 0.5 * AuxiliarMain::choose_PolarizationModes * Num_Amplifiers *
                     Def::get_FAmp() *
                     Constant::h * freq * Def::get_BRef();
        Noise_Gain *= (1.0L - pow(L_FB * L_DCF, -1.0 / (Num_Amplifiers + 1.0)));
    }
}

void Link::eval_Losses() {
    long double dDCF = 0;
    dDCF = (dDCF > 0 ? dDCF : -dDCF);
    L_FB = pow(10, 0.1 * Constant::alphaFB * Length);
    L_DCF = pow(10, 0.1 * Constant::alphaDCF * dDCF);
}

long double Link::get_LinkNoise() {
    return Noise_Gain;
}

double Link::get_Length() {
    return Length;
}

long double Link::get_LinkLoss() {
    return L_FB * L_DCF;
}

long double Link::get_PreamplifierGain() {
    return Preamplifier_Gain;
}

long double Link::get_PreamplifierNoise() {
    return Preamplifier_Noise;
}

void Link::eval_PreamplifierGainNoise(Def *Config) {
    eval_Losses();
    long double freq = Def::get_freq();

    if (Def::get_Architeture() == Arch_SS) {
        Preamplifier_Gain = General::dBtoLin((General::LintodB(L_FB) /
                                              (Num_Amplifiers + 1.0)) + Def::get_LSSS());
    } else if (Def::get_Architeture() == Arch_BS) {
        Preamplifier_Gain = General::dBtoLin((General::LintodB(L_FB) /
                                              (Num_Amplifiers + 1.0)) + 10.0 * log10(Config->get_NodeDegree(
                                                      Destination->whoami) + 1));
    }

    Preamplifier_Noise = 0.5 * AuxiliarMain::choose_PolarizationModes *
                         Def::get_FAmp() *
                         (Preamplifier_Gain - 1.0) * Constant::h * freq * Def::get_BRef();
}

void Link::refresh_Link(Def *Config) {
    eval_Losses();

    if (Source != NULL) {   //se nao e o no "infinito"
        eval_NumAmplifiers(Config);
        eval_LinkGain();
        eval_LinkNoise();
        eval_PreamplifierGainNoise(Config);
    }
}

Node *Link::get_SourceNode() {
    return Source;
}

Node *Link::get_DestinationNode() {
    return Destination;
}

void Link::set_Length(long double dist) {
    assert((dist > 0));
    Length = dist;
}

int Link::get_NumAmplifiers() {
    return Num_Amplifiers;
}

long double Link::get_LongestLink() {
    long double Max = 0;

    for (int i = 0; i < Def::NumNodes; i++) {
        for (unsigned j = 0; j < AuxiliarMain::LinkSet[i].size(); j++) {
            if (Max < AuxiliarMain::LinkSet[i].at(j).get_Length()) {
                Max = AuxiliarMain::LinkSet[i].at(j).get_Length();
            }
        }
    }

    return Max;
}
