#ifndef MCEVENT_H
#define MCEVENT_H

#include <map>
#include <vector>
#include "TTree.h"

class MCGeometry;
class TFile;
class TH2F;
class TH2Poly;
class TObjArray;
class TGraph;

class MCEvent {
public:
    TFile *rootFile;
    TTree *simTree;
    TTree *opTree[4];
    MCGeometry *geom;

    enum LIMITS {
        MAX_CHANNEL = 2048,
        MAX_TRACKS = 2000,
        MAX_HITS = 20000,
	MAX_OPDET = 8,
    };

    enum VIEW {
        kU = 0,
        kV = 1, 
        kZ = 2,
        kT = -1,
    };

    enum DISPLAY {
        kRAW = 1,
        kCALIB = 2,
        kHITS = 3,
	kTRUEHITS = 4,
	kCLUSTERS = 5,
        kTRACK = 6,
    };
 
   int nEvents;

    // simTree Leafs
    int eventNo;
    int runNo;
    int subRunNo;

    int raw_Nhit;  // number of hit channels
    int raw_channelId[MAX_CHANNEL];  // hit channel id; size == raw_Nhit
    int raw_charge[MAX_CHANNEL];  // hit channel charge (simple alg); size == raw_Nhit
    int raw_time[MAX_CHANNEL];  // hit channel time (simple alg); size == raw_Nhit
    std::vector<std::vector<int> > *raw_wfADC;
    std::vector<std::vector<int> > *raw_wfTDC;

    int calib_Nhit;  // number of hit channels
    int calib_channelId[MAX_CHANNEL];  // hit channel id; size == raw_Nhit
    std::vector<std::vector<int> > *calib_wfADC;
    std::vector<std::vector<int> > *calib_wfTDC;

    int mc_Ntrack;  // number of tracks in MC
    int mc_id[MAX_TRACKS];  // track id; size == mc_Ntrack
    int mc_pdg[MAX_TRACKS];  // track particle pdg; size == mc_Ntrack
    int mc_mother[MAX_TRACKS];  // mother id of this track; size == mc_Ntrack
    float mc_startXYZT[MAX_TRACKS][4];  // start position of this track; size == mc_Ntrack
    float mc_endXYZT[MAX_TRACKS][4];  // end position of this track; size == mc_Ntrack
    float mc_startMomentum[MAX_TRACKS][4];  // start momentum of this track; size == mc_Ntrack
    float mc_endMomentum[MAX_TRACKS][4];  // end momentum of this track; size == mc_Ntrack
    std::vector<std::vector<int> > *mc_daughters;  // daughters id of this track; vector
    TObjArray* mc_trackPosition;
    TObjArray* mc_trackMomentum;

    int    no_hits;                  //number of hits
    int    hit_channel[MAX_HITS];    //channel ID
    float  hit_peakT[MAX_HITS];      //peak time
    float  hit_charge[MAX_HITS];     //charge (area)
    float  hit_wireID[MAX_HITS];
    float  hit_tpc[MAX_HITS];
    float  hit_plane[MAX_HITS];

    int    nthits;
    int    thit_channel[MAX_HITS];
    float  thit_peakT[MAX_HITS];
    float  thit_charge[MAX_HITS];
    int    thit_wireID[MAX_HITS];
    int    thit_tpc[MAX_HITS];
    int    thit_plane[MAX_HITS];

    int    nclhits;
    int    chit_cryostat[MAX_HITS];
    int    chit_tpc[MAX_HITS];
    int    chit_plane[MAX_HITS];
    int    chit_charge[MAX_HITS];
    float  chit_peakT[MAX_HITS];
    int    chit_wire[MAX_HITS];
    int    chit_channel[MAX_HITS];
    int    chit_cluster[MAX_HITS];

    int reco_nTrack;    // number of tracks
    TObjArray* reco_trackPosition;

    // Photon detectors variables
    int   AllPhoton_OpChannel;
    float AllPhoton_Time;
    int   DetPhoton_OpChannel;
    float DetPhoton_Time;
    int   CountOpDetAll[MAX_OPDET];
    int   CountOpDetDetected[MAX_OPDET];
    int   CountAll;
    int   CountDetected;

    // derived variables
    int raw_NZchannels;
    int raw_NUchannels;
    int raw_NVchannels;
    std::vector<int> raw_ZchannelId;
    std::vector<int> raw_UchannelId;
    std::vector<int> raw_VchannelId;

    int hit_NZchannels;
    int hit_NUchannels;
    int hit_NVchannels;
    std::vector<int> hit_ZchannelId;
    std::vector<int> hit_UchannelId;
    std::vector<int> hit_VchannelId;

    std::map<int, int> trackIndex;
    std::vector<std::vector<int> > trackParents;
    std::vector<std::vector<int> > trackChildren;
    std::vector<std::vector<int> > trackSiblings;


    // histograms
    TH2F *hPixelZT;
    TH2F *hPixelUT;
    TH2F *hPixelVT;
    TH2Poly *hOpDetAll;
    std::map<int, int> zBintoWireHash;
    std::map<int, int> uBintoWireHash;
    std::map<int, int> vBintoWireHash;

    int optionDisplay; 
    int optionInductionSignal;  // 1:pos; 0:both; -1:neg
    bool showAPA[4];

    //-------------------------------------
    MCEvent();
    MCEvent(const char* filename);
    virtual ~MCEvent();

    //  methods
    TTree* Tree() { return simTree; }
    void GetEntry(int i);
    void Reset();
    void PrintGeomInfo();
    void PrintInfo(int level=0);  // print the current event(entry) info
    void InitBranchAddress();
    void InitHistograms();
    void ProcessTracks();
    void ProcessChannels();
    bool IsPrimary(int i) { return mc_mother[i] == 0; }
    
    void FillPixel(int yView, int xView);  // T=-1, U=0, V=1, Z=2
    void FillOpDet();

    TGraph * PlotTracks(int yView, int xView, bool IsMC, int trackID); // T=-1, U=0, V=1, Z=2
private:
    double _ProjectionY(int yView, int tpc, int wire);
};

#endif
