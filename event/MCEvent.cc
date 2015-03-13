#include "MCEvent.h"
#include "MCGeometry.h"
#include "MCChannel.h"

#include <iostream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TLine.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

using namespace std;

MCEvent::MCEvent(){}

MCEvent::MCEvent(const char* filename)
{
    geom = new MCGeometry();

    raw_wfADC = new std::vector<std::vector<int> >;
    raw_wfTDC = new std::vector<std::vector<int> >;
    calib_wfADC = new std::vector<std::vector<int> >;
    calib_wfTDC = new std::vector<std::vector<int> >;
    mc_daughters = new std::vector<std::vector<int> >;  // daughters id of this track; vector


    mc_trackPosition = new TObjArray();
    mc_trackMomentum = new TObjArray();

    rootFile = new TFile(filename);
    simTree = (TTree*)rootFile->Get("/Event/Sim");
    nEvents = simTree->GetEntries();

    optionDisplay = kRAW;      // default display raw signal
    optionInductionSignal = 1; // default draw positive signal only
    for (int i=0; i<4; i++) showAPA[i] = true;

    InitBranchAddress();
    InitHistograms();
}

MCEvent::~MCEvent()
{
    delete hPixelZT;  // delete histogram, then close file
    delete hPixelUT;
    delete hPixelVT;
    delete raw_wfADC;
    delete raw_wfTDC;
    delete calib_wfADC;
    delete calib_wfTDC;
    delete mc_daughters;
    delete mc_trackPosition;
    delete mc_trackMomentum;

    rootFile->Close();
    delete rootFile;
    delete geom;
    
}

void MCEvent::InitBranchAddress()
{
    simTree->SetBranchAddress("eventNo" , &eventNo);
    simTree->SetBranchAddress("runNo"   , &runNo);
    simTree->SetBranchAddress("subRunNo", &subRunNo);

    simTree->SetBranchAddress("raw_Nhit"     , &raw_Nhit);
    simTree->SetBranchAddress("raw_channelId", &raw_channelId);
    simTree->SetBranchAddress("raw_charge"   , &raw_charge);
    simTree->SetBranchAddress("raw_time"     , &raw_time);
    simTree->SetBranchAddress("raw_wfADC"    , &raw_wfADC);
    simTree->SetBranchAddress("raw_wfTDC"    , &raw_wfTDC);

    simTree->SetBranchAddress("calib_Nhit"     , &calib_Nhit);
    simTree->SetBranchAddress("calib_channelId", &calib_channelId);
    simTree->SetBranchAddress("calib_wfADC"    , &calib_wfADC);
    simTree->SetBranchAddress("calib_wfTDC"    , &calib_wfTDC);

    simTree->SetBranchAddress("mc_Ntrack"       , &mc_Ntrack);
    simTree->SetBranchAddress("mc_id"           , &mc_id);
    simTree->SetBranchAddress("mc_pdg"          , &mc_pdg);
    simTree->SetBranchAddress("mc_mother"       , &mc_mother);
    simTree->SetBranchAddress("mc_daughters"    , &mc_daughters);
    simTree->SetBranchAddress("mc_startXYZT"    , &mc_startXYZT);
    simTree->SetBranchAddress("mc_endXYZT"      , &mc_endXYZT);
    simTree->SetBranchAddress("mc_startMomentum", &mc_startMomentum);
    simTree->SetBranchAddress("mc_endMomentum"  , &mc_endMomentum);

    simTree->SetBranchAddress("mc_trackPosition"  , &mc_trackPosition);
    simTree->SetBranchAddress("mc_trackMomentum"  , &mc_trackMomentum);

    simTree->SetBranchAddress("no_hits"    , &no_hits);
    simTree->SetBranchAddress("hit_channel", &hit_channel);
    simTree->SetBranchAddress("hit_peakT"  , &hit_peakT);
    simTree->SetBranchAddress("hit_charge" , &hit_charge);

    simTree->SetBranchAddress("reco_nTrack"    , &reco_nTrack);
    simTree->SetBranchAddress("reco_trackPosition"  , &reco_trackPosition);

}


void MCEvent::InitHistograms()
{
    // hPixelZT = new TH2F("hPixelZT", "Z vs T", 3200, 0-0.5, 3200-0.5, 400, 0-0.5, 400-0.5);
    // hPixelUT = new TH2F("hPixelUT", "U vs T", 3200, 0-0.5, 3200-0.5, 400, 0-0.5, 400-0.5);
    // hPixelVT = new TH2F("hPixelVT", "V vs T", 3200, 0-0.5, 3200-0.5, 400, 0-0.5, 400-0.5);
    const double xPerTDC = 0.0802815;
    hPixelZT = new TH2F("hPixelZT", "Z (|_ collection Y wire) vs X (drift axis)", 3200, -1, -1+3200*xPerTDC, 359, 0, 0+359*0.449055);
    hPixelUT = new TH2F("hPixelUT", "V (|_  induction U wire ) vs X (drift axis)", 3200, -1, -1+3200*xPerTDC, 510, -53, -53+510*0.487811);
    hPixelVT = new TH2F("hPixelVT", "U (|_  induction V wire ) vs X (drift axis)", 3200, -1, -1+3200*xPerTDC, 510, -168, -168+510*0.500144);

    hPixelZT->GetXaxis()->SetTitle("x [cm]");
    hPixelUT->GetXaxis()->SetTitle("x [cm]");
    hPixelVT->GetXaxis()->SetTitle("x [cm]");

    hPixelZT->GetYaxis()->SetTitle("z [cm]");
    hPixelUT->GetYaxis()->SetTitle("v [cm]");
    hPixelVT->GetYaxis()->SetTitle("u [cm]");
}

void MCEvent::GetEntry(int entry)
{
    Reset();
    simTree->GetEntry(entry);

    // reco_trackPosition->Print();
    // TClonesArray *pos = (TClonesArray*)(*reco_trackPosition)[reco_nTrack-1];
    // pos->Print();

    ProcessTracks();
    ProcessChannels();
}

void MCEvent::Reset()
{
    (*raw_wfADC).clear();
    (*raw_wfTDC).clear();
    (*calib_wfADC).clear();
    (*calib_wfTDC).clear();
    (*mc_daughters).clear();

    trackIndex.clear();
    trackParents.clear();
    trackChildren.clear();
    trackSiblings.clear();

    zBintoWireHash.clear();
    uBintoWireHash.clear();
    vBintoWireHash.clear();

    raw_NZchannels = 0;
    raw_NUchannels = 0;
    raw_NVchannels = 0;
    raw_ZchannelId.clear();
    raw_UchannelId.clear();
    raw_VchannelId.clear();

    hit_NZchannels = 0;
    hit_NUchannels = 0;
    hit_NVchannels = 0;
    hit_ZchannelId.clear();
    hit_UchannelId.clear();
    hit_VchannelId.clear();
}

void MCEvent::ProcessChannels()
{
    for (int i=0; i<raw_Nhit; i++) {
        int channelId = raw_channelId[i];
        MCChannel channel = geom->fChannels[channelId];
        int plane = channel.planes[0];
        if (plane == 0) {
            raw_NUchannels++;
            raw_UchannelId.push_back(channelId);
        }
        else if (plane == 1) {
            raw_NVchannels++;
            raw_VchannelId.push_back(channelId);
        }
        else {
            raw_NZchannels++;
            raw_ZchannelId.push_back(channelId);
        }
    }

    for (int i=0; i<no_hits; i++) {
        int channelId = hit_channel[i];
        MCChannel channel = geom->fChannels[channelId];
        int plane = channel.planes[0];
        if (plane == 0) {
            hit_NUchannels++;
            hit_UchannelId.push_back(channelId);
        }
        else if (plane == 1) {
            hit_NVchannels++;
            hit_VchannelId.push_back(channelId);
        }
        else {
            hit_NZchannels++;
            hit_ZchannelId.push_back(channelId);
        }
    }
}

void MCEvent::FillPixel(int yView, int xView)
{
    int wirePlane = 2;
    TH2F *h = 0;
    map<int, int> *m = 0;
    if (yView == kZ && xView == kT) {
        wirePlane = 2;
        h = hPixelZT;
        m = &zBintoWireHash;
    }
    else if (yView == kU && xView == kT) {
        wirePlane = 0;
        h = hPixelUT;
        m = &uBintoWireHash;
    }
    else if (yView == kV && xView == kT) {
        wirePlane = 1;
        h = hPixelVT;
        m = &vBintoWireHash;
    }
    else {
        cout << "no such combination view: " << yView << " vs " << xView;
    }
    if (!h) return;
    h->Reset();

    if (optionDisplay == kRAW || optionDisplay == kCALIB) {
        for (int i=0; i<raw_Nhit; i++) {
            int channelId = raw_channelId[i];
            MCChannel channel = geom->fChannels[channelId];
            int plane = channel.planes[0];
            if ( ! ((yView == kU && plane == 0) || 
                    (yView == kV && plane == 1) ||
                    (yView == kZ && plane == 2))
               ) {
                continue; // skip other channels
            }

            // MCChannel channel = geom->fChannels[channels->at(i)];
            // cout << yView << " View channel " << channels[i] << ": " << endl;
            for (int j=0; j<channel.Nwires; j++) {
                int wire = channel.wires[j];
                int tpc = channel.tpcs[j];
                // int plane = channel.planes[j];

                // skip short drift chamber
                if (tpc % 2 == 0) continue; 
                // only show designated APA's
                if (!showAPA[(tpc-1)/2]) continue;

                double y = _ProjectionY(yView, tpc, wire);
                int ybin = h->GetYaxis()->FindBin(y);
                (*m)[ybin] = channel.Encode(wirePlane, tpc, wire);
                // if (ybin == 216) {cout << (*m)[ybin] << " " << y << " " << ybin << " " << tpc << endl;}
                // if (channelId == 191) {cout << (*m)[ybin] << " " <<  y << " " << ybin << " " << tpc << endl;}
                int id = 0;
                vector<int>& tdcs = (*raw_wfTDC).at(id);
                vector<int>& adcs = (*raw_wfADC).at(id);
                int size_tdc = tdcs.size();

                if (optionDisplay == kRAW) {
                    id = TMath::BinarySearch(raw_Nhit, raw_channelId, channel.channelNo);
                    tdcs = (*raw_wfTDC).at(id);
                    adcs = (*raw_wfADC).at(id);
                    size_tdc = tdcs.size();
                }
                else {  // calib wire
                    id = TMath::BinarySearch(calib_Nhit, calib_channelId, channel.channelNo);
                    if (calib_channelId[id] == channel.channelNo) {
                        tdcs = (*calib_wfTDC).at(id);
                        adcs = (*calib_wfADC).at(id);
                        size_tdc = tdcs.size();
                    }
                    else {
                        cout << "cannot find raw channel " << channel.channelNo << " in calib wire, skipping" << endl; 
                        size_tdc = 0;
                    }
                }
                for (int i_tdc=0; i_tdc<size_tdc; i_tdc++) {
                    // double x = tdcs[i_tdc];
                    double x = geom->ProjectionX(tpc, tdcs[i_tdc], wirePlane);
                    // cout << tpc << " " << tdcs[i_tdc] << " " << x << endl;
                    int weight = adcs[i_tdc];
                    if (weight>1e4) {
                        cout << weight << endl;
                    }
                    if (yView == kZ) {
                        if (weight>0) h->Fill(x, y, weight);
                    }
                    else {
                        if (optionInductionSignal == 1) {
                            if (weight>0) h->Fill(x, y, weight);
                        }
                        else if (optionInductionSignal == -1) {
                            if (weight<0) h->Fill(x, y, -weight);
                        }
                        else if (optionInductionSignal == 0) {
                            h->Fill(x, y, fabs(weight));
                        }
                    }
                }
            }
        }
    } // raw & calib done
    else if (optionDisplay == kHITS) {
        for (int i=0; i<no_hits; i++) {
            int channelId = hit_channel[i];
            MCChannel channel = geom->fChannels[channelId];
            int plane = channel.planes[0];
            if ( ! ((yView == kU && plane == 0) || 
                    (yView == kV && plane == 1) ||
                    (yView == kZ && plane == 2))
               ) {
                continue; // skip other channels
            }
            for (int j=0; j<channel.Nwires; j++) {
                int wire = channel.wires[j];
                int tpc = channel.tpcs[j];

                // skip short drift chamber
                if (tpc % 2 == 0) continue; 
                // only show designated APA's
                if (!showAPA[(tpc-1)/2]) continue;

                double x = geom->ProjectionX(tpc, hit_peakT[i], wirePlane);
                double y = _ProjectionY(yView, tpc, wire);
                h->Fill(x, y, hit_charge[i]);
            }
        }
    } // hits done
    
}

double MCEvent::_ProjectionY(int yView, int tpc, int wire)
{
    double y = 0;
    if (yView == kZ) {
        // y = (double)wire;
        y = geom->ProjectionZ(tpc, wire);
        // cout << tpc << " " << wire << " " << y << endl;
    }
    else if (yView == kU) {
        y = geom->ProjectionU(tpc, wire);
    }
    else if (yView == kV) {
        y = geom->ProjectionV(tpc, wire);
    }
    else {
        cout << "I cannot recognize yView: " << yView << endl;
    }
    return y;
}


void MCEvent::PrintGeomInfo()
{
    geom->PrintInfo();
}


void MCEvent::ProcessTracks()
{
    for (int i=0; i<mc_Ntrack; i++) {
        trackIndex[mc_id[i]] = i;
    }
    for (int i=0; i<mc_Ntrack; i++) {
        vector<int> parents;
        if (!IsPrimary(i)) {
            parents.push_back(trackIndex[mc_mother[i]]);  // for single gen, parent size == 1;
        }
        trackParents.push_back(parents);

        vector<int> children;
        int nChildren = (*mc_daughters).at(i).size();
        for (int j=0; j<nChildren; j++) {
            children.push_back(trackIndex[(*mc_daughters).at(i).at(j)]);
        }
        trackChildren.push_back(children);

    }
    for (int i=0; i<mc_Ntrack; i++) {
        vector<int> siblings;
        if ( IsPrimary(i) ) {
            for (int j=0; j<mc_Ntrack; j++) {
                if( IsPrimary(j) ) {
                    siblings.push_back(j);
                }
            }
        }
        else {
            // siblings are simply children of the mother
            int mother = trackIndex[mc_mother[i]];
            int nSiblings = trackChildren.at(mother).size();
            for (int j=0; j<nSiblings; j++) {
                siblings.push_back(trackChildren.at(mother).at(j));
            }
        }
        trackSiblings.push_back(siblings);
    }

}


void MCEvent::PrintInfo(int level)
{
    cout << "run/subRun/event (total) : " 
        << runNo << "/" 
        << subRunNo << "/" 
        << eventNo << " ("
        << nEvents << ")"
        << endl;

    cout << "hit channels (raw, calib, hits): " << raw_Nhit << ", " << calib_Nhit << ", " << no_hits << endl;
    cout << "Z channels: " << raw_NZchannels << ", " << hit_NZchannels << endl;
    cout << "U channels: " << raw_NUchannels << ", " << hit_NUchannels << endl;
    cout << "V channels: " << raw_NVchannels << ", " << hit_NVchannels << endl;

    // print mc info
    if (level > 0) {
        cout << "MC tracks:" << mc_Ntrack;
        for (int i=0; i<mc_Ntrack; i++) {
            cout << "\n              id: " << mc_id[i];
            cout << "\n             pdg: " << mc_pdg[i];
            cout << "\n          mother: " << mc_mother[i];
            cout << "\n      Ndaughters: " << (*mc_daughters).at(i).size();
            cout << "\n      start XYZT: (" << mc_startXYZT[i][0] << ", " << mc_startXYZT[i][1] << ", " << mc_startXYZT[i][2] << ", " << mc_startXYZT[i][3] << ")";
            cout << "\n        end XYZT: (" << mc_endXYZT[i][0] << ", " << mc_endXYZT[i][1] << ", " << mc_endXYZT[i][2] << ", " << mc_endXYZT[i][3] << ")";
            cout << "\n  start momentum: (" << mc_startMomentum[i][0] << ", " << mc_startMomentum[i][1] << ", " << mc_startMomentum[i][2] << ", " << mc_startMomentum[i][3] << ")";
            cout << "\n    end momentum: (" << mc_endMomentum[i][0] << ", " << mc_endMomentum[i][1] << ", " << mc_endMomentum[i][2] << ", " << mc_endMomentum[i][3] << ")";

            cout << endl;
        }
    }

    // print raw channel info
    if (level > 1) {
        cout << "raw_channelId: ";
        for (int i=0; i<raw_Nhit; i++) {
            cout << raw_channelId[i] << " ";
        }
        cout << endl << endl;
        // cout << "raw_charge: ";
        // for (int i=0; i<raw_Nhit; i++) {
        //     cout << raw_charge[i] << " ";
        // }
        // cout << endl << endl;
        // cout << "raw_time: ";
        // for (int i=0; i<raw_Nhit; i++) {
        //     cout << raw_time[i] << " ";
        // }
        // cout << endl << endl;

        cout << "Z channels: ";
        for (int i=0; i<raw_NZchannels; i++) {
            cout << raw_ZchannelId.at(i) << " ";
        }
        cout << endl << endl;
        cout << "U channels: ";
        for (int i=0; i<raw_NUchannels; i++) {
            cout << raw_UchannelId.at(i) << " ";
        }
        cout << endl << endl;
        cout << "V channels: ";
        for (int i=0; i<raw_NVchannels; i++) {
            cout << raw_VchannelId.at(i) << " ";
        }
        cout << endl << endl;

        cout << "first hit channel non zero samples: ";
        cout << (*raw_wfADC)[0].size() << endl;
        for (size_t i=0; i<(*raw_wfADC)[0].size(); i++) {
            cout << (*raw_wfADC)[0].at(i) << " ";
        }
        cout << endl << endl;
        cout << "tdc samples: ";
        for (size_t i=0; i<(*raw_wfADC)[0].size(); i++) {
            cout << (*raw_wfTDC)[0].at(i) << " ";
        }
        cout << endl << endl;
    }

}

