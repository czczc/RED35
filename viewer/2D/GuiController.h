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
class PDControl;
class PDWindow;
class RecoWindow;
class MCEvent;
class MCGeometry;
class TCanvas;
class TH2F;
class TH2Poly;
class TDatabasePDG;
class TGraph;
class TMarker;

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
    void Jump();
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
    void UpdateShowReco();
    void UpdatePalette(int id);
    void UpdateDisplayOption(int id);
    void MCTrackSelected(int id);
    void RecoTrackSelected(int id);// added by xiaoyue
    void ProcessCanvasEvent(Int_t ev, Int_t x, Int_t y, TObject *selected);
    void ProcessPDCanvasEvent(Int_t ev, Int_t x, Int_t y, TObject *selected);
    void HandleMenu(int id);

    void OpenDialog();

    // void InitTracksList();
    void AutoZoom(TH2F* hist, bool zoomY=true);
    void DrawTrack(int id, bool IsMC);
    void HideMCTrack();
    void HideRecoTrack();
    void SetTheme(int theme);

    //void PaletteRainbow();
    //void PaletteGray();
    //void PaletteSummer();
    //void PaletteGrayInv();
    //void PaletteFire();
    //void SetPalette(int i);

    // utilities
    double KE(float* momentum);  // KE
    TGString PDGName(int pdg);

    MainWindow *mw;
    ViewWindow *vw;
    InfoWindow *iw;
    ControlWindow *cw;
    PDControl     *pc;
    RecoWindow    *rw;
    PDWindow      *pw;
    MCEvent *event;
    MCGeometry *geom;

    TCanvas *can;
    TCanvas *pd_can;
    int currentEvent;
    int xMin_now, xMax_now;

    TGraph* trackMCLineZ;
    TGraph* trackMCLineU;
    TGraph* trackMCLineV;
    TGraph* trackRecoLineZ;
    TGraph* trackRecoLineU;
    TGraph* trackRecoLineV;

    TMarker *trackStartPointZ;
    TMarker *trackStartPointU;
    TMarker *trackStartPointV;

    TDatabasePDG *dbPDG;

    int currentPalette;
    int currentDisplayOption;
    int currentTheme;
    int currentInductionSig;
    bool currentShowMC;
    bool currentShowReco;
    int currentMCTrackId;
    int currentRecoTrackId;

};

#endif
