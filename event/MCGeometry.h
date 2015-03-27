#ifndef MCGEOMETRY_H
#define MCGEOMETRY_H

#include "MCChannel.h"
#include <map>

#define NCHANNELS 2048
#define NOPDETS   8
class TFile;
class TTree;
class TString;

class MCGeometry {
public:
    TTree *geoTree;

    MCChannel fChannels[NCHANNELS];
    std::map<int, int> wireToChannel; // hash map to find channel no. given wire hash

    MCGeometry(const char* mapFileName="../Geometry/ChannelWireMap.txt", const char* rootFileName="../data/sample.root");
    virtual ~MCGeometry();

    //  methods
    void ReadChanneleWireMap(const char* mapFileName);
    void PrintInfo();
    double ProjectionZ(int tpc, int wire); // projection to Z coordinate for collection plane
    double ProjectionU(int tpc, int wire); // projection to perpendicular coordinate of U plane
    double ProjectionV(int tpc, int wire); // projection to perpendicular coordinate of V plane
    double ProjectionX(int tpc, int tdc, int plane);  // projection to X coordinate for drifting

    void LoadPDGeometryTree();
    void GetPDGeometry();

    bool IsPointInside(double x, double y, double z, double scale = 1.);

    Float_t OpDetCenterY[NOPDETS];
    Float_t OpDetCenterZ[NOPDETS]; 
    Float_t OpDetHalfWidths[NOPDETS];
    Float_t OpDetHalfHeights[NOPDETS];

};

#endif
