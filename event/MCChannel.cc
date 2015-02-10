#include "MCChannel.h"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

MCChannel::MCChannel()
{
}

MCChannel::~MCChannel()
{
}

int MCChannel::Encode(int plane, int tpc, int wire) { return plane*100000+tpc*10000+wire; }
    
void MCChannel::Decode(int hash, int& plane, int& tpc, int& wire)
{
    int tmp = hash;
    plane = tmp / 100000;
    tmp = tmp - plane*100000;
    tpc = tmp / 10000;
    wire = tmp - tpc*10000;
}

void MCChannel::PrintInfo()
{
    cout << "Channel " << channelNo << "\n";
    cout << "Nwires  " << Nwires << "\n";

    cout << "Plane ";
    for (int i=0; i<Nwires; i++) {
        cout << planes.at(i) << " ";
    }
    cout << "\n";

    cout << "TPC ";
    for (int i=0; i<Nwires; i++) {
        cout << tpcs.at(i) << " ";
    }
    cout << "\n";

    cout << "Wire ";
    for (int i=0; i<Nwires; i++) {
        cout << wires.at(i) << " ";
    }
    cout << "\n";

    cout << "Hash ";
    for (int i=0; i<Nwires; i++) {
        cout << hashes.at(i) << " ";
    }
    cout << "\n";

    cout << endl;
}

