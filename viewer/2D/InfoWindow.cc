#include "InfoWindow.h"
#include "MCEvent.h"
#include "MCChannel.h"

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"
#include "TMath.h"
#include "TLatex.h"
#include "TColor.h"
#include "TStyle.h"

#include <iostream>
#include <vector>
using namespace std;


InfoWindow::InfoWindow(const TGWindow *p, int w,int h)
    :TRootEmbeddedCanvas("ECanvas", p, w, h)
{
    SetTheme(0); // night theme
    can = GetCanvas();
    InitCanvas();
}

InfoWindow::~InfoWindow()
{
}

void InfoWindow::SetStyle(int theme)
{
  if (theme==0) {
    gStyle->SetFrameFillColor(TColor::GetColor(float(0.1), float(0.1), float(0.1)));
  } else {
    gStyle->SetFrameFillColor(kWhite);
  }
}

void InfoWindow::SetTheme(int i)
{
    theme = i;
    if (theme == 0) { // night theme
        colors[0] = kWhite;
        colors[1] = kYellow;
        colors[2] = kGreen;
    }
    else { // day theme
        colors[0] = kBlack;
        colors[1] = kRed;
        colors[2] = kBlue;    
    }
}

void InfoWindow::InitCanvas()
{
}

void InfoWindow::ClearCanvas()
{
    can->cd();    
    size_t size = listOfDrawables.size();
    for (size_t i=0; i!=size; i++) {
        delete listOfDrawables[i];
    }
    listOfDrawables.clear();
    can->Clear();
}

void InfoWindow::DrawEventInfo(MCEvent *ev)
{
    ClearCanvas();
    vector<TString> lines;
    lines.push_back(Form("Run: %i", ev->runNo));
    lines.push_back(Form("Event: %i", ev->eventNo));
    lines.push_back(Form("Hits: %i", ev->no_hits));
    lines.push_back(Form("Hit Channels: %i / %i / %i", ev->raw_NZchannels, ev->raw_NUchannels, ev->raw_NVchannels));
    lines.push_back(Form("Photons: %d", ev->CountDetected));
    float startx = 0.05;
    float starty = 0.9;
    size_t size = lines.size();
    for (size_t i=0; i<size; i++) {
        TLatex* tex = new TLatex(startx, starty-i*0.1, lines[i].Data());
        listOfDrawables.push_back(tex);
        tex->SetNDC();
        tex->SetTextFont(102);
        tex->SetTextSize(0.08);
        tex->Draw();
    }

    UpdateCanvas();
}


void InfoWindow::DrawWire(int channelId, MCEvent *ev, int wirehash)
{
    ClearCanvas();
    //SetStyle(theme);    
    int plane, tpc, wire;
    MCChannel::Decode(wirehash, plane, tpc, wire);

    // Container
    TString name = Form("channel_%i", channelId);
    TString title = Form("Channel %i: Plane %i, TPC %i, Wire %i", channelId, plane, tpc, wire);
    TH2F *hh = new TH2F(name, title, 3200, 0, 3200, 100, -500, 500); 
    listOfDrawables.push_back(hh);
    hh->Draw();


    // Raw signal
    int id = TMath::BinarySearch(ev->raw_Nhit, ev->raw_channelId, channelId);
    if (!(ev->raw_channelId[id] == channelId)) {
        cout << "cannot find channel " << channelId << endl;
        UpdateCanvas();
        return;
    }
    vector<int>& tdcs = (*(ev->raw_wfTDC)).at(id);
    vector<int>& adcs = (*(ev->raw_wfADC)).at(id);
    int size_tdc = tdcs.size();
    TH1F *h = new TH1F(name+"_wf", title, 3200, 0, 3200); 
    listOfDrawables.push_back(h);
    h->SetLineColor(colors[0]);
    for (int i=0; i<size_tdc; i++) {
        h->SetBinContent(tdcs[i], adcs[i]);
    }
    h->Draw("same");
    hh->GetXaxis()->SetRangeUser(tdcs.at(0)-10, tdcs.at(tdcs.size()-1)+10);
    hh->GetYaxis()->SetRangeUser(h->GetMinimum()-10, h->GetMaximum()*1.5);

    // Calibrated signal
    int id2 = TMath::BinarySearch(ev->calib_Nhit, ev->calib_channelId, channelId);
    if (!(ev->calib_channelId[id2] == channelId)) {
        cout << "cannot find calib channel " << channelId << endl;
        return;
    }
    vector<int>& tdcs2 = (*(ev->calib_wfTDC)).at(id2);
    vector<int>& adcs2 = (*(ev->calib_wfADC)).at(id2);
    int size_tdc2 = tdcs2.size();
    TH1F *h2 = new TH1F(name+"_calib", title, 3200, 0, 3200); 
    listOfDrawables.push_back(h2);
    h2->SetLineColor(colors[1]);
    for (int i=0; i<size_tdc2; i++) {
        h2->SetBinContent(tdcs2[i], adcs2[i]);
    }
    h2->Draw("same");

    // Hits
    for (int i=0; i<ev->no_hits; i++) {
        if (!(channelId == ev->hit_channel[i])) continue;
        float pt = ev->hit_peakT[i];
        TLine *l = new TLine(pt, h->GetMinimum()-10, pt, h->GetMaximum()*1.5);
        listOfDrawables.push_back(l);
        l->SetLineStyle(3);
        l->SetLineColor(colors[2]);
        l->SetLineWidth(2);
        l->Draw();
    }

    UpdateCanvas();
}

void InfoWindow::UpdateCanvas()
{
    can->Modified();
    can->Update();
}
