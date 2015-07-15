#include "GuiController.h"
#include "MainWindow.h"
#include "ViewWindow.h"
#include "InfoWindow.h"
#include "ControlWindow.h"
#include "PDControl.h"
#include "PDWindow.h"
#include "RecoWindow.h"
#include "MCEvent.h"
#include "MCGeometry.h"
#include "MCChannel.h"

#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"
#include "TApplication.h"
#include "TGMenu.h"
#include "TGFileDialog.h"
#include "TGListBox.h"
#include "TGString.h"
#include "TGDoubleSlider.h"
#include "TGLabel.h"
#include "TSystem.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TFrame.h"
#include "TColor.h"
#include "TH2F.h"
#include "TH2Poly.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TGraph.h"
#include "TList.h"
#include "TObject.h"
#include "TLatex.h"
#include "TMarker.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TExec.h"
#include "TSystem.h"

#include <exception>
#include <iostream>
#include <map>
using namespace std;

GuiController::GuiController(const TGWindow *p, int w, int h, const char* filename)
{

    // InitPDGMap();
    dbPDG = new TDatabasePDG();

    currentPalette = 1;  // dark rainbow
    currentTheme = 0; // night theme
    currentDisplayOption = 1; // display raw signal
    currentInductionSig = 0;  // positive signal
    currentShowMC = false;  // do not show MC
    currentShowReco = false; // do not show Reco
    currentMCTrackId = 1;  // first track
    currentRecoTrackId = 0;

    trackMCLineZ = 0;
    trackStartPointZ = new TMarker(0,0,24);
    trackStartPointZ->SetMarkerColor(kWhite);
    trackStartPointZ->SetMarkerSize(1.0);

    trackMCLineU = 0;
    trackStartPointU = new TMarker(0,0,24);
    trackStartPointU->SetMarkerColor(kWhite);
    trackStartPointU->SetMarkerSize(1.0);

    trackMCLineV = 0;
    trackStartPointV = new TMarker(0,0,24);
    trackStartPointV->SetMarkerColor(kWhite);
    trackStartPointV->SetMarkerSize(1.0);

    trackRecoLineZ = 0;
    trackRecoLineU = 0;
    trackRecoLineV = 0;

    event = 0;
    mw = new MainWindow(p, w, h);
    vw = mw->fViewWindow;
    cw = mw->fControlWindow;
    pc = mw->fPDControl;
    rw = mw->fPDControl->fRecoWindow;
    iw = mw->fControlWindow->fInfoWindow;
    pw = mw->fPDControl->fPDWindow;
    can = vw->can;
    pd_can = pw->can;

    xMin_now = 0;
    xMax_now = 3200;

    if (!filename) {
        OpenDialog();
    }
    else {
        Open(filename);
    }


    InitConnections();

}

GuiController::~GuiController()
{
  /*
    delete trackMCLineZ;
    delete trackMCLineU;
    delete trackMCLineV;
    delete trackRecoLineZ;
    delete trackRecoLineU;
    delete trackRecoLineV;
  */
}

void GuiController::InitConnections()
{
    mw->fMenuFile->Connect("Activated(int)", "GuiController", this, "HandleMenu(int)");

    cw->prevButton->Connect("Clicked()", "GuiController", this, "Prev()");
    cw->nextButton->Connect("Clicked()", "GuiController", this, "Next()");
    cw->eventEntry->Connect("ReturnPressed()", "GuiController", this, "Jump()");

    cw->autoZoomButton->Connect("Clicked()", "GuiController", this, "AutoZoom()");
    cw->unZoomButton->Connect("Clicked()", "GuiController", this, "UnZoom()");
    cw->xrangeButton->Connect("Clicked()", "GuiController", this, "SyncXaxis()");
    cw->zrangeButton->Connect("Clicked()", "GuiController", this, "UpdateZaxis()");
    cw->inductionSigButtonGroup->Connect("Clicked(int)", "GuiController", this, "UpdateInductionSig(int)");
    cw->apaButtonGroup->Connect("Clicked(int)", "GuiController", this, "UpdateAPA(int)");
    cw->showMCButton->Connect("Clicked()", "GuiController", this, "UpdateShowMC()");
    cw->displayButtonGroup->Connect("Clicked(int)", "GuiController", this, "UpdateDisplayOption(int)");

    cw->fSiblingTracksListBox->Connect("Selected(int)", "GuiController", this, "MCTrackSelected(int)");
    cw->fDaughterTracksListBox->Connect("Selected(int)", "GuiController", this, "MCTrackSelected(int)");
    cw->fParentTracksListBox->Connect("Selected(int)", "GuiController", this, "MCTrackSelected(int)");

    rw->fRecoTracksListBox->Connect("Selected(int)", "GuiController", this, "RecoTrackSelected(int)");
    rw->showRecoButton->Connect("Clicked()", "GuiController", this, "UpdateShowReco()");
    rw->paletteButtonGroup->Connect("Clicked(int)", "GuiController", this, "UpdatePalette(int)");

    can->GetPad(1)->Connect("RangeChanged()", "GuiController", this, "SyncRangeZT()");
    can->GetPad(2)->Connect("RangeChanged()", "GuiController", this, "SyncRangeUT()");
    can->GetPad(3)->Connect("RangeChanged()", "GuiController", this, "SyncRangeVT()");

    can->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "GuiController",
           this, "ProcessCanvasEvent(Int_t,Int_t,Int_t,TObject*)");
    pd_can->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "GuiController",
			       this, "ProcessPDCanvasEvent(Int_t,Int_t,Int_t,TObject*)");

}


void GuiController::ProcessCanvasEvent(Int_t ev, Int_t x, Int_t y, TObject *selected)
{
    if (ev == 11) { // clicked
        if (!(selected->IsA() == TH2F::Class())) return;
	//Modified();
        TVirtualPad* pad = can->GetClickSelectedPad();
        double xx = pad->AbsPixeltoX(x);
        double yy = pad->AbsPixeltoY(y);
        TH2F *h = (TH2F*)selected;
        int biny = h->GetYaxis()->FindBin(yy);
        TString name = h->GetName();
        map<int, int> *m = 0;
             if (name == "hPixelZT") m = &(event->zBintoWireHash);
        else if (name == "hPixelUT") m = &(event->uBintoWireHash);
        else if (name == "hPixelVT") m = &(event->vBintoWireHash);
        else { cout << "not recognized: " << name << endl; return;}
        int wirehash = (*m)[biny];
        int channelNo = geom->wireToChannel[wirehash];
        cout
            // << "event: " << ev
            << "x: " << xx
            << ", y: " << yy
            << ", h: " << name
            << ", biny: " << biny
            << ", wire hash: " << wirehash
            << ", channel: " << channelNo
            << endl;
        iw->DrawWire(channelNo, event, wirehash);
    }

}

void GuiController::ProcessPDCanvasEvent(Int_t ev, Int_t x, Int_t y, TObject *selected)
{
    if (ev == 11) {
        if (!(selected->IsA() == TH2Poly::Class())) return;
	TVirtualPad* pad = pd_can->GetClickSelectedPad();
	double xx = pad->AbsPixeltoX(x);
	double yy = pad->AbsPixeltoY(y);
	TH2Poly *h = (TH2Poly*)selected;
	int bin = 0;
	if (h->FindBin(xx, yy) < 0) {
    	    bin = 8;
	} else {
	    bin = h->FindBin(xx, yy)-1;
	}
	pw->DrawOpDetChannel(bin, event);
    }
}

void GuiController::AutoZoom()
{
    AutoZoom(event->hPixelZT);//, false);
    AutoZoom(event->hPixelUT, false);
    AutoZoom(event->hPixelVT, false);
    Modified();
}

void GuiController::UnZoom(bool redraw)
{
    xMin_now = 1;
    xMax_now = 3200;
    event->hPixelZT->GetXaxis()->SetRange(xMin_now, xMax_now);
    event->hPixelUT->GetXaxis()->SetRange(xMin_now, xMax_now);
    event->hPixelVT->GetXaxis()->SetRange(xMin_now, xMax_now);
    event->hPixelZT->GetYaxis()->SetRange(1, 343);
    event->hPixelUT->GetYaxis()->SetRange(1, 510);
    event->hPixelVT->GetYaxis()->SetRange(1, 495);
    cw->tdcMinEntry->SetIntNumber(xMin_now);
    cw->tdcMaxEntry->SetIntNumber(xMax_now);
    if (redraw) Modified();

}

void GuiController::SyncRangeZT()
{
    xMin_now = event->hPixelZT->GetXaxis()->GetFirst();
    xMax_now = event->hPixelZT->GetXaxis()->GetLast();
    event->hPixelUT->GetXaxis()->SetRange(xMin_now, xMax_now);
    event->hPixelVT->GetXaxis()->SetRange(xMin_now, xMax_now);
    cw->tdcMinEntry->SetIntNumber(xMin_now);
    cw->tdcMaxEntry->SetIntNumber(xMax_now);
    cout << "Range Changed: " << xMin_now << ", " << xMax_now << endl;
    Modified();
}

void GuiController::UpdateZaxis()
{
    int zt = cw->ztColorEntry->GetNumber();
    int ut = cw->utColorEntry->GetNumber();
    int vt = cw->vtColorEntry->GetNumber();

    int min = 0;
    // if (currentTheme == 1) min = 0;
    event->hPixelZT->GetZaxis()->SetRangeUser(min, zt);
    event->hPixelUT->GetZaxis()->SetRangeUser(min, ut);
    event->hPixelVT->GetZaxis()->SetRangeUser(min, vt);
    Modified();
}

void GuiController::SyncRangeUT()
{
    xMin_now = event->hPixelUT->GetXaxis()->GetFirst();
    xMax_now = event->hPixelUT->GetXaxis()->GetLast();
    event->hPixelZT->GetXaxis()->SetRange(xMin_now, xMax_now);
    event->hPixelVT->GetXaxis()->SetRange(xMin_now, xMax_now);
    cw->tdcMinEntry->SetIntNumber(xMin_now);
    cw->tdcMaxEntry->SetIntNumber(xMax_now);
    cout << "Range Changed: " << xMin_now << ", " << xMax_now << endl;
    Modified();
}

void GuiController::SyncRangeVT()
{
    xMin_now = event->hPixelVT->GetXaxis()->GetFirst();
    xMax_now = event->hPixelVT->GetXaxis()->GetLast();
    event->hPixelZT->GetXaxis()->SetRange(xMin_now, xMax_now);
    event->hPixelUT->GetXaxis()->SetRange(xMin_now, xMax_now);
    cw->tdcMinEntry->SetIntNumber(xMin_now);
    cw->tdcMaxEntry->SetIntNumber(xMax_now);
    cout << "Range Changed: " << xMin_now << ", " << xMax_now << endl;
    Modified();
}

void GuiController::SyncXaxis()
{
    double x1 = cw->tdcMinEntry->GetNumber();
    double x2 = cw->tdcMaxEntry->GetNumber();

    event->hPixelZT->GetXaxis()->SetRange(x1, x2);
    event->hPixelUT->GetXaxis()->SetRange(x1, x2);
    event->hPixelVT->GetXaxis()->SetRange(x1, x2);

    Modified();
}

void GuiController::UpdatePalette(int id)
{
    if (id == currentPalette) return;
         if (id == 1) vw->PaletteRainbow();
    else if (id == 2) vw->PaletteGray();
    else if (id == 3) vw->PaletteSummer();
    else if (id == 4) vw->PaletteGrayInv();
    currentPalette = id;

    if (id==1 || id==2) currentTheme = 0;
    else currentTheme = 1;
    SetTheme(currentTheme);

    Modified();
    pd_can->GetPad(1)->Modified();
    pd_can->Update();
    cout << "changing theme: " << id << endl;
}

void GuiController::UpdateDisplayOption(int id)
{
    if(id == currentDisplayOption) return;
    event->optionDisplay = id;
    currentDisplayOption = id;

    Reload();
    cout << "changing display to option: " << id << endl;
}

void GuiController::UpdateInductionSig(int id)
{
    if (id == currentInductionSig) return;
         if (id == 1) event->optionInductionSignal = 1;
    else if (id == 2) event->optionInductionSignal = -1;
    else if (id == 3 || id == 4) event->optionInductionSignal = 0;
    currentInductionSig = id;

    Reload();
    cout << "changing induction signal: " << id << endl;
}


void GuiController::UpdateAPA(int id)
{
    TGCheckButton *b = (TGCheckButton*)cw->apaButtonGroup->GetButton(id);
    if (b->IsDown()) event->showAPA[id-1] = true;
    else event->showAPA[id-1] = false;

    Reload();
    cout << "toggle APA " << id << ": " << b->IsDown() << endl;
}


void GuiController::UpdateShowMC()
{
    if (cw->showMCButton->IsDown()) {
        if (currentShowMC == true) return;
        currentShowMC = true;
        MCTrackSelected(currentMCTrackId);
    }
    else {
        if (currentShowMC == false) return;
        currentShowMC = false;
        HideMCTrack();
    }

}

void GuiController::UpdateShowReco()
{
    if (rw->showRecoButton->IsDown()) {
        if (currentShowReco == true) return;
        currentShowReco = true;
        RecoTrackSelected(currentRecoTrackId);
    }
    else {
        if (currentShowReco == false) return;
        currentShowReco = false;
        HideRecoTrack();
    }

}


void GuiController::SetTheme(int theme)
{
    // if (theme == 0) {
    //     // by default, empty bins are not painted, which is overwritten if there is negative bin content
    //     // event->hPixelZT->SetMinimum(-1);
    //     // event->hPixelUT->SetMinimum(-1);
    //     // event->hPixelVT->SetMinimum(-1);
    // }
    // else {
    //     event->hPixelZT->SetMinimum(0);
    //     event->hPixelUT->SetMinimum(0);
    //     event->hPixelVT->SetMinimum(0);
    // }
    vw->SetTheme(theme);
    iw->SetTheme(theme);
    pw->SetStyle(theme);
}

void GuiController::Modified()
{
    //vw->SetStyle();
    can->GetPad(1)->Modified();
    // can->GetPad(1)->Update();
    can->GetPad(2)->Modified();
    // can->GetPad(2)->Update();
    can->GetPad(3)->Modified();
    // can->GetPad(3)->Update();
    can->Update();
}


void GuiController::DrawPixels()
{
    TH2F *h = 0;
    //TExec *pal = new TExec("pal", "GuiController::SetPalette(currentPalette)");

    can->cd(1);
    event->FillPixel(2, -1); // ZT
    h = event->hPixelZT;
    //pal->Draw();
    h->Draw("colz");
    cw->ztColorEntry->SetIntNumber(h->GetBinContent(h->GetMaximumBin()));


    can->cd(2);
    event->FillPixel(0, -1); // UT
    h = event->hPixelUT;
    //pal->Draw();
    h->Draw("colz");
    cw->utColorEntry->SetIntNumber(h->GetBinContent(h->GetMaximumBin()));


    can->cd(3);
    event->FillPixel(1, -1); // VT
    h = event->hPixelVT;
    //pal->Draw();
    h->Draw("colz");
    cw->vtColorEntry->SetIntNumber(h->GetBinContent(h->GetMaximumBin()));

    //SetTheme(currentTheme);
    //SetPalette(currentPalette);
    Modified();
}


void GuiController::DrawTrack(int id, bool IsMC)
{
    int trackID = event->trackIndex[id];
    int i=0;
    if(IsMC) i=trackID;
    else     i=id;

    //SetTheme(currentTheme);
    //SetPalette(currentPalette);
    //Modified();

    if(IsMC) {
      trackStartPointZ->SetX(event->mc_startXYZT[i][0]);
      trackStartPointZ->SetY(event->mc_startXYZT[i][2]);
      if(trackMCLineZ) {delete trackMCLineZ; trackMCLineZ=NULL;}
      trackMCLineZ = (TGraph*)event->PlotTracks(2, -1, IsMC, i);
      can->cd(1);
      trackMCLineZ->Draw();
      trackStartPointZ->Draw();

      trackStartPointU->SetX(event->mc_startXYZT[i][0]);
      trackStartPointU->SetY((event->mc_startXYZT[i][1]+event->mc_startXYZT[i][2])*TMath::Sqrt(2)/2);
      if(trackMCLineU) {delete trackMCLineU; trackMCLineU=NULL;}
      trackMCLineU = (TGraph*)event->PlotTracks(0, -1, IsMC, i);
      can->cd(2);
      trackMCLineU->Draw();
      trackStartPointU->Draw();

      trackStartPointV->SetX(event->mc_startXYZT[i][0]);
      trackStartPointV->SetY((event->mc_startXYZT[i][1]-event->mc_startXYZT[i][2])*TMath::Sqrt(2)/2);
      if(trackMCLineV) {delete trackMCLineV; trackMCLineV=NULL;}
      trackMCLineV = (TGraph*)event->PlotTracks(1, -1, IsMC, i);
      can->cd(3);
      trackMCLineV->Draw();
      trackStartPointV->Draw();
    } else {
      if(event->reco_nTrack==0) return;
      if(trackRecoLineZ) {delete trackRecoLineZ; trackRecoLineZ=NULL;}
      trackRecoLineZ = (TGraph*)event->PlotTracks(2, -1, IsMC, i);
      can->cd(1);
      trackRecoLineZ->Draw();
      if(trackRecoLineU) {delete trackRecoLineU; trackRecoLineU=NULL;}
      trackRecoLineU = (TGraph*)event->PlotTracks(0, -1, IsMC, i);
      can->cd(2);
      trackRecoLineU->Draw();
      if(trackRecoLineV) {delete trackRecoLineV; trackRecoLineV=NULL;}
      trackRecoLineV = (TGraph*)event->PlotTracks(1, -1, IsMC, i);
      can->cd(3);
      trackRecoLineV->Draw();
    }

    Modified();
}


void GuiController::HideMCTrack()
{
    float faraway = -1000.;
    if(trackMCLineZ) {delete trackMCLineZ; trackMCLineZ=NULL;}
    if(trackMCLineU) {delete trackMCLineU; trackMCLineU=NULL;}
    if(trackMCLineV) {delete trackMCLineV; trackMCLineV=NULL;}

    trackStartPointZ->SetX(faraway);
    trackStartPointZ->SetY(faraway);
    trackStartPointU->SetX(faraway);
    trackStartPointU->SetY(faraway);
    trackStartPointV->SetX(faraway);
    trackStartPointV->SetY(faraway);
    can->cd(1);
    //trackMCLineZ->Draw();
    trackStartPointZ->Draw();
    can->cd(2);
    //trackMCLineU->Draw();
    trackStartPointU->Draw();
    can->cd(3);
    //trackMCLineV->Draw();
    trackStartPointV->Draw();
    Modified();
}

void GuiController::HideRecoTrack()
{
    //float faraway = -1000.;
    if(trackRecoLineZ) {delete trackRecoLineZ; trackRecoLineZ=NULL;}
    if(trackRecoLineU) {delete trackRecoLineU; trackRecoLineU=NULL;}
    if(trackRecoLineV) {delete trackRecoLineV; trackRecoLineV=NULL;}
    //trackRecoLineZ->SetPoint(1,faraway, faraway);
    //trackRecoLineU->SetPoint(1,faraway, faraway);
    //trackRecoLineV->SetPoint(1,faraway, faraway);
    //can->cd(1);
    //trackRecoLineZ->Draw();
    //can->cd(2);
    //trackRecoLineU->Draw();
    //can->cd(3);
    //trackRecoLineV->Draw();
    Modified();
}

void GuiController::Prev()
{
    if (currentEvent==0) {
        currentEvent = event->nEvents-1;
    }
    else {
        currentEvent--;
    }
    Reload();
}


void GuiController::Next()
{
    if (currentEvent==event->nEvents-1) {
        currentEvent = 0;
    }
    else {
        currentEvent++;
    }
    Reload();
}

void GuiController::Jump()
{
    currentEvent = int(cw->eventEntry->GetNumber());
    if (currentEvent>=event->nEvents-1) {
        currentEvent = event->nEvents-1;
    }
    else if (currentEvent < 0) {
        currentEvent=0;
    }
    Reload();
}

void GuiController::Reload()
{
    cw->eventEntry->SetNumber(currentEvent);
    event->GetEntry(currentEvent);
    // event->PrintInfo(1);
    event->PrintInfo();
    iw->DrawEventInfo(event);
    // InitTracksList();
    MCTrackSelected(1); // select the first track
    RecoTrackSelected(0);
    // cw->showMCButton->SetState(kButtonUp);
    //UpdateShowMC();
    DrawPixels();
    pw->DrawOpDets(event);
    pw->DrawOpDetChannel(8,event);
}

// void GuiController::InitTracksList()
// {

    // cw->fSiblingTracksListBox->RemoveAll();
    // cw->fParentTracksListBox->RemoveAll();
    // cw->fDaughterTracksListBox->RemoveAll();
    // TGString name;
    // name.Form("%s (%.1f)", PDGName(event->mc_pdg[0]).Data(), KE(event->mc_startMomentum[0])*1000);
    // int id = event->mc_id[0];
    // currentTrackId = id;
    // cw->fSiblingTracksListBox->AddEntry(name, id);
// }

void GuiController::MCTrackSelected(int id)
{
    currentMCTrackId = id;
    //UpdateShowMC();
    //cw->showMCButton->SetState(kButtonDown);
    //currentShowMC = true;

    cw->fDaughterTracksListBox->RemoveAll();
    int i = event->trackIndex[id];
    int nDaughter = (*(event->mc_daughters)).at(i).size();
    for (int j=0; j<nDaughter; j++) {
        int idx = event->trackChildren.at(i).at(j);
        TGString name;
        name.Form("%s (%.1f)", PDGName(event->mc_pdg[idx]).Data(), KE(event->mc_startMomentum[idx])*1000);
        cw->fDaughterTracksListBox->AddEntry(name, event->mc_id[idx]);
        // cout << name.Data() << event->mc_id[idx] << endl;
    }
    cw->fDaughterTracksListBox->Layout();

    cw->fParentTracksListBox->RemoveAll();
    int nParent = event->trackParents.at(i).size();
    for (int j=0; j<nParent; j++) {
        int idx = event->trackParents.at(i).at(j);
        TGString name;
        name.Form("%s (%.1f)", PDGName(event->mc_pdg[idx]).Data(), KE(event->mc_startMomentum[idx])*1000);
        cw->fParentTracksListBox->AddEntry(name, event->mc_id[idx]);
        // cout << name.Data() << event->mc_id[idx] << endl;
    }
    cw->fParentTracksListBox->Layout();

    cw->fSiblingTracksListBox->RemoveAll();
    int nSiblings = event->trackSiblings.at(i).size();
    for (int j=0; j<nSiblings; j++) {
        int idx = event->trackSiblings.at(i).at(j);
        TGString name;
        name.Form("%s (%.1f)", PDGName(event->mc_pdg[idx]).Data(), KE(event->mc_startMomentum[idx])*1000);
        cw->fSiblingTracksListBox->AddEntry(name, event->mc_id[idx]);
        // cout << name.Data() << event->mc_id[idx] << endl;
    }
    cw->fSiblingTracksListBox->Layout();


    cw->fSiblingTracksListBox->Select(id);
    if (currentShowMC) DrawTrack(id, true);
    cout << PDGName(event->mc_pdg[i]) <<": trackID: " << event->trackIndex[id] << ", id: " << id << ", daughters: " << nDaughter << endl;
}

void GuiController::RecoTrackSelected(int id)
{
    currentRecoTrackId = id;
    //currentShowReco = true;

    rw->fRecoTracksListBox->RemoveAll();
    for (int j=0; j<event->reco_nTrack; j++) {
        TGString name;
        name.Form("Track # %2d", j+1);//, KE(event->mc_startMomentum[idx])*1000);
        rw->fRecoTracksListBox->AddEntry(name, j);
        // cout << name.Data() << event->mc_id[idx] << endl;
    }
    rw->fRecoTracksListBox->Layout();

    rw->fRecoTracksListBox->Select(id);
    if (currentShowReco) DrawTrack(id, false);
    cout << "Reco track ID: "<< id <<endl;
}


void GuiController::Open(const char* filename)
{
    cout << "here" << endl;

    if (event) delete event;
    event = new MCEvent(filename);
    geom = event->geom;
    currentEvent = 0;
    Reload();
}


void GuiController::HandleMenu(int id)
{
    // const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    switch (id) {
        case M_FILE_OPEN:
            OpenDialog();
            break;

        case M_FILE_EXIT:
            gApplication->Terminate(0);
            break;
    }
}

void GuiController::OpenDialog()
{
    const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    TString currentDir(gSystem->WorkingDirectory());
    static TString dir("../data");
    TGFileInfo fi;
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), mw, kFDOpen, &fi);
    dir = fi.fIniDir;
    gSystem->cd(currentDir.Data());

    if (fi.fFilename) {
        // UnZoom();
        cout << "open file: " << fi.fFilename << endl;
        Open(fi.fFilename);
    }
    else {
        gApplication->Terminate(0);
    }

}

void GuiController::AutoZoom(TH2F* hist, bool zoomX)
{
    // zooms in on where the action is i.e., where there is counts
    Int_t xMin = hist->GetNbinsX();
    Int_t xMax = 0;
    Int_t yMin = hist->GetNbinsY();
    Int_t yMax = 0;

    for(Int_t i = 1; i <= hist->GetNbinsX(); i++){
        for(Int_t j = 1; j <= hist->GetNbinsY(); j++){

            if(hist->GetBinContent(i, j) <= 1) continue;

            if(i < xMin) xMin = i;
            if(j < yMin) yMin = j;
            if(i > xMax) xMax = i;
            if(j > yMax) yMax = j;
        }
    }

    hist->GetYaxis()->SetRange(yMin, yMax);
    if (zoomX) {
        hist->GetXaxis()->SetRange(xMin, xMax);
        xMin_now = xMin;
        xMax_now = xMax;
    }
    else {
        hist->GetXaxis()->SetRange(xMin_now, xMax_now);
    }
}

double GuiController::KE(float* momentum)
{
    TLorentzVector particle(momentum);
    return particle.E()-particle.M();
}


TGString GuiController::PDGName(int pdg)
{
    TParticlePDG *p = dbPDG->GetParticle(pdg);
    if (p == 0) {
        if (pdg>1e9) {
            int z = (pdg - 1e9) / 10000;
            int a = (pdg - 1e9 - z*1e4) / 10;
            TGString name;
            if (z == 18) name = "Ar";

            else if (z == 17) name = "Cl";
            else if (z == 19) name = "Ca";
            else if (z == 16) name = "S";
            else if (z == 15) name = "P";
            else if (z == 14) name = "Si";
            else if (z == 1) name = "H";
            else if (z == 2) name = "He";

            else return pdg;
            return Form("%s-%i", name.Data(), a);
        }
        return pdg;
    }
    else {
        return p->GetName();
    }
}

/*
void GuiController::SetPalette(int i)
{

  switch (i) {
  case 1:
    PaletteRainbow();
    break;
  case 2:
    PaletteGray();
    break;
  case 3:
    PaletteFire();
    break;
  case 4:
    PaletteSummer();
    break;
  case 5:
    PaletteGrayInv();
    break;
  default:
    PaletteRainbow();
  }
  gROOT->ForceStyle();

}

void GuiController::PaletteRainbow()
{
  gStyle->SetFrameFillColor(TColor::GetColor(float(0.1), float(0.1), float(0.1)));

  // http://diana.parno.net/thoughts/?p=28
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

}

void GuiController::PaletteGray()
{
  // gray scale, night
  gStyle->SetFrameFillColor(TColor::GetColor(float(0.1), float(0.1), float(0.1)));
  const UInt_t Number = 2;
  Int_t nb=50;
  Double_t Red[Number]   = { 0.15, 1.00};
  Double_t Green[Number] = { 0.15, 1.00};
  Double_t Blue[Number]  = { 0.15, 1.00};
  Double_t Stops[Number] = { 0, 1.00};
  TColor::CreateGradientColorTable(Number,Stops,Red,Green,Blue,nb);
  gStyle->SetNumberContours(nb);
}

void GuiController::PaletteSummer()
{
  gStyle->SetFrameFillColor(kWhite);
  const Int_t NRGBs = 256;
  const Int_t NCont = 256;
  Double_t stops[NRGBs] = {0.000,0.004,0.008,0.012,0.016,0.020,0.024,0.027,0.031,0.035,0.039,0.043,0.047,0.051,0.055,0.059,0.063,0.067,0.071,0.075,0.078,0.082,0.086,0.090,0.094,0.098,0.102,0.106,0.110,0.114,0.118,0.122,0.125,0.129,0.133,0.137,0.141,0.145,0.149,0.153,0.157,0.161,0.165,0.169,0.173,0.176,0.180,0.184,0.188,0.192,0.196,0.200,0.204,0.208,0.212,0.216,0.220,0.224,0.227,0.231,0.235,0.239,0.243,0.247,0.251,0.255,0.259,0.263,0.267,0.271,0.275,0.278,0.282,0.286,0.290,0.294,0.298,0.302,0.306,0.310,0.314,0.318,0.322,0.325,0.329,0.333,0.337,0.341,0.345,0.349,0.353,0.357,0.361,0.365,0.369,0.373,0.376,0.380,0.384,0.388,0.392,0.396,0.400,0.404,0.408,0.412,0.416,0.420,0.424,0.427,0.431,0.435,0.439,0.443,0.447,0.451,0.455,0.459,0.463,0.467,0.471,0.475,0.478,0.482,0.486,0.490,0.494,0.498,0.502,0.506,0.510,0.514,0.518,0.522,0.525,0.529,0.533,0.537,0.541,0.545,0.549,0.553,0.557,0.561,0.565,0.569,0.573,0.576,0.580,0.584,0.588,0.592,0.596,0.600,0.604,0.608,0.612,0.616,0.620,0.624,0.627,0.631,0.635,0.639,0.643,0.647,0.651,0.655,0.659,0.663,0.667,0.671,0.675,0.678,0.682,0.686,0.690,0.694,0.698,0.702,0.706,0.710,0.714,0.718,0.722,0.725,0.729,0.733,0.737,0.741,0.745,0.749,0.753,0.757,0.761,0.765,0.769,0.773,0.776,0.780,0.784,0.788,0.792,0.796,0.800,0.804,0.808,0.812,0.816,0.820,0.824,0.827,0.831,0.835,0.839,0.843,0.847,0.851,0.855,0.859,0.863,0.867,0.871,0.875,0.878,0.882,0.886,0.890,0.894,0.898,0.902,0.906,0.910,0.914,0.918,0.922,0.925,0.929,0.933,0.937,0.941,0.945,0.949,0.953,0.957,0.961,0.965,0.969,0.973,0.976,0.980,0.984,0.988,0.992,0.996,1.000,};

  Double_t blue[NRGBs]   = {1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.999,0.998,0.998,0.998,0.998,0.998,0.998,0.998,0.998,0.997,0.997,0.997,0.997,0.997,0.996,0.996,0.996,0.996,0.995,0.995,0.995,0.994,0.994,0.994,0.993,0.993,0.992,0.992,0.991,0.991,0.990,0.990,0.989,0.989,0.988,0.987,0.986,0.986,0.985,0.984,0.983,0.982,0.981,0.980,0.979,0.978,0.977,0.976,0.975,0.974,0.972,0.971,0.969,0.968,0.966,0.965,0.963,0.961,0.960,0.958,0.956,0.954,0.952,0.950,0.947,0.945,0.943,0.940,0.938,0.935,0.932,0.930,0.927,0.924,0.921,0.918,0.915,0.911,0.908,0.904,0.901,0.897,0.893,0.890,0.886,0.882,0.877,0.873,0.869,0.864,0.860,0.855,0.851,0.846,0.841,0.836,0.831,0.825,0.820,0.815,0.809,0.804,0.798,0.792,0.787,0.781,0.775,0.769,0.762,0.756,0.750,0.743,0.737,0.731,0.724,0.717,0.711,0.704,0.697,0.690,0.683,0.676,0.669,0.662,0.655,0.648,0.641,0.633,0.626,0.619,0.612,0.604,0.597,0.590,0.582,0.575,0.568,0.560,0.553,0.546,0.539,0.531,0.524,0.517,0.510,0.502,0.495,0.488,0.481,0.474,0.467,0.460,0.454,0.447,0.440,0.433,0.427,0.420,0.414,0.407,0.401,0.395,0.389,0.382,0.376,0.371,0.365,0.359,0.353,};
  Double_t green[NRGBs] = {0.878,0.878,0.877,0.876,0.875,0.874,0.873,0.873,0.872,0.871,0.870,0.869,0.868,0.867,0.865,0.864,0.863,0.862,0.861,0.860,0.858,0.857,0.856,0.854,0.853,0.851,0.850,0.848,0.847,0.845,0.844,0.842,0.840,0.839,0.837,0.835,0.833,0.831,0.829,0.827,0.825,0.823,0.821,0.819,0.817,0.815,0.812,0.810,0.808,0.805,0.803,0.800,0.798,0.795,0.792,0.790,0.787,0.784,0.781,0.779,0.776,0.773,0.770,0.767,0.764,0.760,0.757,0.754,0.751,0.747,0.744,0.740,0.737,0.733,0.730,0.726,0.723,0.719,0.715,0.711,0.707,0.703,0.699,0.695,0.691,0.687,0.683,0.679,0.675,0.671,0.666,0.662,0.657,0.653,0.649,0.644,0.639,0.635,0.630,0.626,0.621,0.616,0.611,0.607,0.602,0.597,0.592,0.587,0.582,0.577,0.572,0.567,0.562,0.557,0.552,0.547,0.541,0.536,0.531,0.526,0.521,0.515,0.510,0.505,0.500,0.494,0.489,0.484,0.478,0.473,0.468,0.462,0.457,0.451,0.446,0.441,0.435,0.430,0.425,0.419,0.414,0.409,0.403,0.398,0.393,0.387,0.382,0.377,0.371,0.366,0.361,0.356,0.351,0.345,0.340,0.335,0.330,0.325,0.320,0.315,0.310,0.305,0.300,0.295,0.290,0.285,0.280,0.275,0.271,0.266,0.261,0.257,0.252,0.247,0.243,0.238,0.234,0.230,0.225,0.221,0.216,0.212,0.208,0.204,0.200,0.196,0.192,0.188,0.184,0.180,0.176,0.172,0.168,0.165,0.161,0.157,0.154,0.150,0.147,0.143,0.140,0.137,0.133,0.130,0.127,0.124,0.121,0.118,0.115,0.112,0.109,0.106,0.103,0.100,0.098,0.095,0.092,0.090,0.087,0.085,0.082,0.080,0.077,0.075,0.073,0.071,0.069,0.066,0.064,0.062,0.060,0.058,0.056,0.054,0.053,0.051,0.049,0.047,0.046,0.044,0.042,0.041,0.039,0.038,0.036,0.035,0.033,0.032,0.031,0.029,0.028,0.027,0.026,0.025,0.023,0.022,};
  Double_t red[NRGBs]  = {0.716,0.714,0.711,0.708,0.705,0.703,0.700,0.697,0.694,0.690,0.687,0.684,0.681,0.677,0.674,0.671,0.667,0.663,0.660,0.656,0.652,0.649,0.645,0.641,0.637,0.633,0.628,0.624,0.620,0.616,0.611,0.607,0.603,0.598,0.593,0.589,0.584,0.579,0.575,0.570,0.565,0.560,0.555,0.550,0.545,0.540,0.535,0.530,0.524,0.519,0.514,0.508,0.503,0.498,0.492,0.487,0.481,0.476,0.470,0.465,0.459,0.454,0.448,0.443,0.437,0.432,0.426,0.420,0.415,0.409,0.403,0.398,0.392,0.387,0.381,0.375,0.370,0.364,0.359,0.353,0.347,0.342,0.336,0.331,0.326,0.320,0.315,0.309,0.304,0.299,0.293,0.288,0.283,0.278,0.273,0.268,0.263,0.258,0.253,0.248,0.243,0.238,0.233,0.229,0.224,0.219,0.215,0.210,0.206,0.201,0.197,0.193,0.189,0.184,0.180,0.176,0.172,0.168,0.164,0.161,0.157,0.153,0.149,0.146,0.142,0.139,0.136,0.132,0.129,0.126,0.123,0.120,0.117,0.114,0.111,0.108,0.105,0.102,0.100,0.097,0.095,0.092,0.090,0.087,0.085,0.083,0.080,0.078,0.076,0.074,0.072,0.070,0.068,0.066,0.065,0.063,0.061,0.059,0.058,0.056,0.055,0.053,0.052,0.050,0.049,0.048,0.047,0.045,0.044,0.043,0.042,0.041,0.040,0.039,0.038,0.037,0.036,0.035,0.034,0.033,0.032,0.032,0.031,0.030,0.030,0.029,0.028,0.028,0.027,0.026,0.026,0.025,0.025,0.024,0.024,0.023,0.023,0.023,0.022,0.022,0.021,0.021,0.021,0.020,0.020,0.020,0.020,0.019,0.019,0.019,0.019,0.018,0.018,0.018,0.018,0.018,0.017,0.017,0.017,0.017,0.017,0.017,0.016,0.016,0.016,0.016,0.016,0.016,0.016,0.016,0.016,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.015,0.014,0.014,0.014,0.014,0.014,0.014,0.014,};

  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}

void GuiController::PaletteGrayInv()
{
  gStyle->SetFrameFillColor(kWhite);
  const UInt_t Number = 2;
  Int_t nb=50;
  Double_t Red[Number]   = { 0.8, 0.0};
  Double_t Green[Number] = { 0.8, 0.0};
  Double_t Blue[Number]  = { 0.8, 0.0};
  Double_t Stops[Number] = { 0, 1.00};
  TColor::CreateGradientColorTable(Number,Stops,Red,Green,Blue,nb);
  gStyle->SetNumberContours(nb);
}

void GuiController::PaletteFire()
{
  gStyle->SetPalette(53);
}
*/
