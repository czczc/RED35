#ifndef MCCHANNEL_H
#define MCCHANNEL_H

#include <vector>

class MCChannel {
public:
    int channelNo;
    int Nwires;
    std::vector<int> tpcs;
    std::vector<int> planes;
    std::vector<int> wires;
    std::vector<int> hashes; // internal hash for a wire
    std::vector<std::vector<int> > opchannel;

    MCChannel();
    virtual ~MCChannel();

    //  methods
    int Encode(int plane, int tpc, int wire);
    static void Decode(int hash, int& plane, int& tpc, int& wire);
    void PrintInfo();
};

#endif
