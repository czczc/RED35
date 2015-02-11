#include "Gui3DController.h"
#include "MCEvent.h"

#include "TEveManager.h"
#include "TEveGeoNode.h"
#include "TEveVSDStructs.h"
#include "TEvePathMark.h"
#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TEveBrowser.h"
#include "TEveWindow.h"
#include "TEveViewer.h"

#include "TGLViewer.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

#include "TGFrame.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGFileDialog.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TString.h"
#include "TColor.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>
using namespace std;

Gui3DController::Gui3DController()
{
    dbPDG = new TDatabasePDG();
    mcTrackList = 0;

    TEveManager::Create();
    InitGeometry();
    InitNavigationFrame();
    InitEvent();

    ProjectionView();
}


Gui3DController::~Gui3DController()
{
}


void Gui3DController::InitEvent()
{
    const char *filetypes[] = {"ROOT files", "*.root", 0, 0};
    static TString dir("../data");
    TGFileInfo fi;
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), frmMain, kFDOpen, &fi);
    dir = fi.fIniDir;
    cout << fi.fFilename << endl;
    event = new MCEvent(fi.fFilename);
    currentEvent = 0;
    Reload();
}


void Gui3DController::InitGeometry()
{
    TGeoManager::Import("../Geometry/lbne35t4apa_v3_nowires.gdml");
    // TGeoManager::Import("../Geometry/lbne35t4apa.root");
    gGeoManager->DefaultColors();
    TGeoNode* world = gGeoManager->GetTopNode();
    TGeoNode *det = world->GetDaughter(0);
    TGeoNode *cryo = det->GetDaughter(0);
    TEveGeoTopNode* top = new TEveGeoTopNode(gGeoManager, world);
    gEve->AddGlobalElement(top);

    int nDaughters = world->GetNdaughters();
    for (int i=0; i<nDaughters; i++) {
      TGeoNode *node = world->GetDaughter(i);
      TString name(node->GetName());
      if (!name.Contains("DetEnclosure")) {
        node->SetInvisible();
        node->SetAllInvisible();
      }
    }

    nDaughters = det->GetNdaughters();
    for (int i=0; i<nDaughters; i++) {
      TGeoNode *node = det->GetDaughter(i);
      TString name(node->GetName());
      // if (name.Contains("Foam") || name.Contains("Steel") 
      //     || name.Contains("Concrete") || name.Contains("Neck")) {
      if (! (node == cryo)) {
        node->SetInvisible();
        node->SetAllInvisible();
      }
    }

    nDaughters = cryo->GetNdaughters();
    for (int i=0; i<nDaughters; i++) {
        TGeoNode *node = cryo->GetDaughter(i);
        TString name(node->GetName());
        if (name.Contains("Cathode")) {
            node->GetVolume()->SetLineColor(kCyan);
            node->GetVolume()->SetTransparency(70);
        }
        // else if (name.Contains("TPC")) {
        //     node->GetVolume()->SetTransparency(100);
        // }
        else if (name.Contains("APA")) {
            node->GetVolume()->SetLineColor(kRed);
            // node->GetVolume()->SetTransparency(10);
        }
        else {
            node->SetInvisible();
            node->SetAllInvisible();
        }

    }


    // nDaughters = cryo->GetNdaughters();
    // for (int i=0; i<nDaughters; i++) {
    //   TGeoNode *node = cryo->GetDaughter(i);
    //   TString name(node->GetName());
    //   if (name.Contains("TPC") || name.Contains("Steel") 
    //       || name.Contains("Concrete") || name.Contains("Argon")) {
    //     node->SetInvisible();
    //     node->SetAllInvisible();
    //   }
    // }

}

void Gui3DController::InitNavigationFrame()
{
   // Create minimal GUI for event navigation.

   TEveBrowser* browser = gEve->GetBrowser();
   browser->StartEmbedding(TRootBrowser::kLeft);

   frmMain = new TGMainFrame(gClient->GetRoot(), 200, 600);
   frmMain->SetWindowName("Navigation GUI");
   frmMain->SetCleanup(kDeepCleanup);

   TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain);
   frmMain->AddFrame(hf, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
   {
        prevButton = new TGTextButton(hf, "< Prev");
        hf->AddFrame(prevButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
        prevButton->Connect("Clicked()", "Gui3DController", this, "Prev()");

        eventEntry = new TGNumberEntryField(hf, -1, 0, TGNumberFormat::kNESInteger);
        eventEntry->SetDefaultSize(60, 20);
        hf->AddFrame(eventEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

        nextButton = new TGTextButton(hf, "Next >");
        hf->AddFrame(nextButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
        nextButton->Connect("Clicked()", "Gui3DController", this, "Next()");

   }
   

   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();

   browser->StopEmbedding();
   browser->SetTabTitle("Events", 0);
}


void Gui3DController::AddMCTracks()
{
    if (mcTrackList) {
        mcTrackList->RemoveElements();
    }
    else {
        mcTrackList = new TEveTrackList();
        mcTrackList->GetPropagator()->SetStepper(TEveTrackPropagator::kRungeKutta);
        mcTrackList->GetPropagator()->SetMaxR(1e4);
        mcTrackList->GetPropagator()->SetMaxZ(1e4);
        gEve->AddElement(mcTrackList);
    }    
    TString name;
    name.Form("Event %i: MC Tracks", currentEvent);
    mcTrackList->SetName(name.Data());

    int mc_Ntrack = event->mc_Ntrack;  // number of tracks in MC
    TEveRecTrackD *rc = 0;
    TEveTrack* track = 0;

    int colors[6] = {kMagenta, kGreen, kYellow, kRed, kCyan, kWhite};
    int nTrackShowed = 0;
    for (int i=0; i<mc_Ntrack; i++) {
        int mc_id = event->mc_id[i];
        int mc_pdg = event->mc_pdg[i];
        TParticlePDG *p = dbPDG->GetParticle(mc_pdg);
        if (p == 0) {
            continue; // skip unknown particles
        }
        if ( fabs(p->Charge()) < 1e-2 ) {
            continue;  // skip neutral particles
        }
        rc = new TEveRecTrackD();
        rc->fV.Set(event->mc_startXYZT[i][0], event->mc_startXYZT[i][1], event->mc_startXYZT[i][2]);
        track = new TEveTrack(rc, mcTrackList->GetPropagator());
        TString s;
        s.Form("%i: %s", mc_id, p->GetName());
        track->SetName(s.Data());
        track->SetLineColor( colors[nTrackShowed % 6] );
        track->SetLineWidth(2);

        TClonesArray *pos = (TClonesArray*)(*event->mc_trackPosition)[i];
        int nPoints = pos->GetEntries();
        for (int k=1; k<nPoints; k++) {
            TLorentzVector* l = (TLorentzVector*)(*pos)[k];
            TEvePathMarkD pm(TEvePathMarkD::kReference);
            pm.fV.Set(l->X(), l->Y(), l->Z());
            track->AddPathMark(pm);
            // delete pm;
        }
                
        mcTrackList->AddElement(track);
        track->MakeTrack();
        nTrackShowed++;
    }

}


void Gui3DController::AddRecoTracks()
{
    if (recoTrackList) {
        recoTrackList->RemoveElements();
    }
    else {
        recoTrackList = new TEveTrackList();
        recoTrackList->GetPropagator()->SetStepper(TEveTrackPropagator::kRungeKutta);
        recoTrackList->GetPropagator()->SetMaxR(1e4);
        recoTrackList->GetPropagator()->SetMaxZ(1e4);
        gEve->AddElement(recoTrackList);
    }    
    TString name;
    name.Form("Event %i: Reco Tracks", currentEvent);
    recoTrackList->SetName(name.Data());

    int nTrack = event->reco_nTrack;  // number of tracks in MC
    TEveRecTrackD *rc = 0;
    TEveTrack* track = 0;

    int colors[6] = {kMagenta, kGreen, kYellow, kRed, kCyan, kWhite};
    int nTrackShowed = 0;
    for (int i=0; i<nTrack; i++) {
        TClonesArray *pos = (TClonesArray*)(*event->reco_trackPosition)[i];
        TLorentzVector* l = (TLorentzVector*)(*pos)[0];

        rc = new TEveRecTrackD();
        rc->fV.Set(l->X(), l->Y(), l->Z());
        track = new TEveTrack(rc, recoTrackList->GetPropagator());
        TString s;
        s.Form("track %i", i);
        track->SetName(s.Data());
        track->SetLineColor( colors[nTrackShowed % 6] );
        track->SetLineStyle(3);

        int nPoints = pos->GetEntries();
        for (int k=1; k<nPoints; k++) {
            l = (TLorentzVector*)(*pos)[k];
            TEvePathMarkD pm(TEvePathMarkD::kReference);
            pm.fV.Set(l->X(), l->Y(), l->Z());
            track->AddPathMark(pm);
        }
                
        recoTrackList->AddElement(track);
        track->MakeTrack();
        nTrackShowed++;
    }

}


void Gui3DController::ProjectionView()
{
    TEveViewer            *f3DView;
    TEveViewer            *fXYView;
    TEveViewer            *fXZView;   
    TEveViewer            *fZYView;   


    // frames
    TEveWindowSlot* slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* packH = slot->MakePack();
    packH->SetElementName("Projections");
    packH->SetHorizontal();
    packH->SetShowTitleBar(kFALSE);

    slot = packH->NewSlot();
    TEveWindowPack* pack0 = slot->MakePack();
    pack0->SetShowTitleBar(kFALSE);
    TEveWindowSlot*  slotLeftTop   = pack0->NewSlot();
    TEveWindowSlot* slotLeftBottom = pack0->NewSlot();

    slot = packH->NewSlot();
    TEveWindowPack* pack1 = slot->MakePack();
    pack1->SetShowTitleBar(kFALSE);
    TEveWindowSlot* slotRightTop    = pack1->NewSlot();
    TEveWindowSlot* slotRightBottom = pack1->NewSlot();

    // scenes
    slotLeftTop->MakeCurrent();
    f3DView = gEve->SpawnNewViewer("3D View", "");
    f3DView->AddScene(gEve->GetGlobalScene());
    f3DView->AddScene(gEve->GetEventScene());
    f3DView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspYOZ);
    f3DView->GetGLViewer()->SetGuideState(1, true, false, 0);
    // f3DView->GetGLViewer()->CurrentCamera().RotateRad(-0.5, -2);
    // f3DView->GetGLViewer()->DoDraw();

    slotRightTop->MakeCurrent();
    fXYView = gEve->SpawnNewViewer("XY View", "");
    fXYView->AddScene(gEve->GetGlobalScene());
    fXYView->AddScene(gEve->GetEventScene());
    fXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    fXYView->GetGLViewer()->SetGuideState(1, true, false, 0);

    slotRightBottom->MakeCurrent();
    fXZView = gEve->SpawnNewViewer("XZ View", "");
    fXZView->AddScene(gEve->GetGlobalScene());
    fXZView->AddScene(gEve->GetEventScene());
    fXZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
    fXZView->GetGLViewer()->SetGuideState(1, true, false, 0);

    slotLeftBottom->MakeCurrent();
    fZYView = gEve->SpawnNewViewer("ZY View", "");
    fZYView->AddScene(gEve->GetGlobalScene());
    fZYView->AddScene(gEve->GetEventScene());
    fZYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZnOY);
    fZYView->GetGLViewer()->SetGuideState(1, true, false, 0);

}

void Gui3DController::Run()
{
    gEve->FullRedraw3D(kTRUE);
    TGLViewer *v = gEve->GetDefaultGLViewer();
    // v->UseLightColorSet();
    v->CurrentCamera().RotateRad(-0.5, -2);
    // v->SetCurrentCamera(TGLViewer::kCameraPerspXOZ);
    v->SetGuideState(1, true, false, 0);
    v->DoDraw();
}


void Gui3DController::Prev()
{
    int maxEvent = event->nEvents-1;

    if (currentEvent == 0) {
        currentEvent = maxEvent;
    }
    else {
        currentEvent--;
    }
    cout << "prev: current event: " << currentEvent << endl;
    Reload();
}


void Gui3DController::Next()
{
    int maxEvent = event->nEvents-1;

    if (currentEvent == maxEvent) {
        currentEvent = 0;
    }
    else {
        currentEvent++;
    }
    cout << "next: current event: " << currentEvent << endl;
    Reload();
}

void Gui3DController::Reload()
{
    eventEntry->SetNumber(currentEvent);
    event->GetEntry(currentEvent);
    // event->PrintInfo(1);
    event->PrintInfo();
    AddMCTracks();
    AddRecoTracks();
    gEve->DoRedraw3D();
}
