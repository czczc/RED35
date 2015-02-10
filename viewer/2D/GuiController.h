#ifndef GUI_CONTROLLER_H
#define GUI_CONTROLLER_H

#include "TGFrame.h"
#include "RQ_OBJECT.h"
#include "TGString.h"
#include <vector>
#include <map>

class MainWindow;
class ViewWindow;
class InfoWindow;
class ControlWindow;
class MCEvent;
class MCGeometry;
class TCanvas;
class TH2F;
class TLine;
class TMarker;
class TDatabasePDG;

class GuiController
{
    RQ_OBJECT("MyMainFrame")
public:
    GuiController(const TGWindow *p, int w, int h);
    virtual ~GuiController();

    void Open(const char* filename);
    void Reload();
    void InitConnections();
    void DrawPixels();
    void Modified();

    // slots
    void Prev();
    void Next();
    void AutoZoom();
    void UnZoom(bool redraw=true);
    void SyncRangeZT();
    void SyncRangeUT();
    void SyncRangeVT();
    void SyncXaxis();
    void UpdateZaxis();
    void UpdateInductionSig(int id);
    void UpdateAPA(int id);
    void UpdateShowMC();
    void UpdatePalette(int id);
    void UpdateDisplayOption(int id);
    void MCTrackSelected(int id);
    void ProcessCanvasEvent(Int_t ev, Int_t x, Int_t y, TObject *selected);
    void HandleMenu(int id);

    void InitTracksList();
    void AutoZoom(TH2F* hist, bool zoomY=true);
    void DrawTrack(int id);
    void HideTrack();
    void SetTheme(int theme);

    // utilities
    double KE(float* momentum);  // KE
    TGString PDGName(int pdg);

    MainWindow *mw;
    ViewWindow *vw;
    InfoWindow *iw;
    ControlWindow *cw;
    MCEvent *event;
    MCGeometry *geom;

    TCanvas *can;

    int currentEvent;
    int xMin_now, xMax_now;

    TLine* trackLineZ;
    TMarker* trackStartPointZ;
    TLine* trackLineU;
    TMarker* trackStartPointU;
    TLine* trackLineV;
    TMarker* trackStartPointV;

    TDatabasePDG *dbPDG;

    int currentPalette;
    int currentDisplayOption;
    int currentTheme;
    int currentInductionSig;
    bool currentShowMC;
    int currentTrackId;

};

#endif