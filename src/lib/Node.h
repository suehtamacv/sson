#ifndef NODE_H
#define NODE_H

#include "Def.h"

/**
 * @brief The Node class implements a node in the route.
 */
class Node {
  public:

    /**
     * @brief The NodeTypes enum is used to choose the type of node. A TransparentNode has no regenerators, and a TranslucentNode has.
     * The NodeType_Null is compliance and mustn't be used.
     */
    enum NodeTypes {
        NodeType_Null, TransparentNode, TranslucentNode
    };

    /**
     * @param who is a unique identifier to the node. Should not be altered nor be repeated.
     */
    Node(int);

    /**
     * @brief get_whoami returns the Node's unique identifier
     * @return the Node's unique identifier
     */
    int get_whoami();

    /**
     * @brief whoami is the node unique identifier
     */
    int whoami;

    /**
     * @return the (linear) loss in the SSS element
     */
    long double get_Loss();

    /**
     * @return the (linear) loss in the SSS element, if SS Arch, or the loss in the splitter, if BS Arch.
     */
    long double get_LossSSSorDiv();

    /**
     * @param Config is the set of parameters over which the gain is evaluated.
     * @return the (linear) gain of the Booster Amplifier.
     */
    long double get_PotAmplGain(Def *Config);

    /**
     * @param Config is the set of parameters over which the noise is evaluated.
     * @return the noise (in watts) generated by the Booster Amplifier.
     */
    long double get_PotAmplNoise(Def *Config);

    /**
     * @brief Node::set_TransmPower configurates the power of the transmitting device in the node.
     * @param power the power, in dBm, of the transmitting device.
     */
    void set_TransmPower(long double);


    /**
     * @brief Node::set_NodeType configurates the type of node.
     * @param type is either TransparentNode or TranslucentNode.
     */
    void set_NodeType(NodeTypes type);

    /**
     * @return the type of the node
     */
    NodeTypes get_NodeType();

    /**
     * @brief Node::set_NumberRegenerators, in translucent nodes, configurates the number of optical regenerators installed in the node.
     * @param NR the number of optical regenerators installed in the node.
     */
    void set_NumberRegenerators(long double);

    /**
     * @return the number of installed regenerators in the node.
     */
    long double get_NumberRegenerators();

    /**
     * @return the number of regenerators in the node that aren't being used.
     */
    long double get_NumberAvailableRegenerators();

    /**
     * @return the number of regenerators in the node that are being used.
     */
    long double get_NumberUsedRegenerators();

    /**
     * @param BitRate is the the bit rate of the connection, in bits per second, as defined in Def::Possible_BitRates.
     * @return the number greater than zero of succesfully reserved regenerators, or zero if the regenerators couldn't be reserved.
     */
    long double request_Regenerators(long double BitRate);    //BitRate in Gbps

    /**
     * @brief Node::release_Regenerators releases the regenerators that are no longer being used.
     * @param NumReg is the number of regenerators to be released.
     */
    void release_Regenerators(long double NumReg);
  private:
    long double Loss_SSS;
    long double Loss_SSSorDiv;
    long double PotAmplGain;
    long double PotAmplNoise;
    long double TransmPower;

    long double NumRegenerators;
    long double NumAvailableRegenerators;
    long double NumUsedRegenerators;

    /**
     * @brief caches the value of the (linear) gain of the Booster Amplifier.
     * @param Config is the set of parameters over which the gain is evaluated.
     */
    void eval_PotAmplGain(Def *Config);

    /**
     * @brief Node::eval_PotAmplNoise caches the value of the noise power (in watts) generated by the Booster Amplifier.
     */
    void eval_PotAmplNoise();
    NodeTypes NodeType;
};

#endif //NODE_H
