#include "Node.h"
#include "General.h"
#include "Def.h"
#include "Constant.h"
#include "Regenerators.h"

Node::Node(int who) {
    assert((who >= 0));
    whoami = who;
    Loss_SSS = General::dBtoLin(-1 * Def::get_LSSS());
    Loss_SSSorDiv = -1;
    NodeType = TransparentNode;

    NumRegenerators = 0;
    NumAvailableRegenerators = 0;
    NumUsedRegenerators = 0;
}

void Node::eval_PotAmplGain(Def *Config) {
    PotAmplGain = General::dBtoLin(General::LintodB(Config->get_Pref() / 1E-3L) -
                                   TransmPower + Config->get_LSSS());
    eval_PotAmplNoise();
}

void Node::eval_PotAmplNoise() {
    long double freq = Def::get_freq();
    PotAmplNoise = 0.5 * AuxiliarMain::choose_PolarizationModes * Def::get_FAmp() *
                   (PotAmplGain - 1.0) * Constant::h * freq * Def::get_BRef();
}

long double Node::get_Loss() {
    return Loss_SSS;
}

long double Node::get_PotAmplGain(Def *Config) {
    eval_PotAmplGain(Config);
    return PotAmplGain;
}

long double Node::get_PotAmplNoise(Def *Config) {
    eval_PotAmplGain(Config);
    return PotAmplNoise;
}

void Node::set_TransmPower(long double power) {
    TransmPower = power;
}

void Node::set_NodeType(NodeTypes type) {
    NodeType = type;
}

Node::NodeTypes Node::get_NodeType() {
    return NodeType;
}

void Node::set_NumberRegenerators(long double NR) {
    assert((NR >= 0));

    if (NR > 0) {
        assert((NodeType == TranslucentNode));
    }

    NumRegenerators = NR;
    NumAvailableRegenerators = NR;
    NumUsedRegenerators = 0;
}

long double Node::get_NumberRegenerators() {
    return NumRegenerators;
}

long double Node::get_NumberAvailableRegenerators() {
    if (AuxiliarMain::FlagRP_MU ||
            AuxiliarMain::FlagRP_MSU) { //Those algorithms consider opaque network
        return Def::MAX_INT;
    } else {
        return NumAvailableRegenerators;
    }
}

long double Node::get_NumberUsedRegenerators() {
    return NumUsedRegenerators;
}

long double Node::request_Regenerators(long double BitRate) {
    assert((NodeType == TranslucentNode));
    int NumReg = ceil(BitRate / Regenerators::BR);


    if (AuxiliarMain::FlagRP_MU ||
            AuxiliarMain::FlagRP_MSU) {  //Those algorithms consider opaque network
        NumUsedRegenerators += NumReg;
        return NumReg;
    } else if (NumAvailableRegenerators >= NumReg) {
        NumUsedRegenerators += NumReg;
        NumAvailableRegenerators -= NumReg;
        return NumReg;
    } else {
        return 0;
    }
}

void Node::release_Regenerators(long double NumReg) {
    assert((NodeType == TranslucentNode));
    assert(NumRegenerators - NumAvailableRegenerators >= NumReg);

    NumUsedRegenerators -= NumReg;
    assert((NumUsedRegenerators >= 0));

    if (!(AuxiliarMain::FlagRP_MU ||
            AuxiliarMain::FlagRP_MSU)) { //Not considering opaque network
        NumAvailableRegenerators += NumReg;
    }
}

long double Node::get_LossSSSorDiv() {
    if (Loss_SSSorDiv == -1) {
        if (Def::get_Architeture() == Arch_SS) {
            Loss_SSSorDiv = General::dBtoLin(-1 * Def::get_LSSS());
        } else if (Def::get_Architeture() == Arch_BS) {
            Loss_SSSorDiv = 1.0 / (Def::get_NodeDegree(whoami) + 1.0);
        }
    }

    return Loss_SSSorDiv;
}
