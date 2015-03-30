#include "RecoWindow.h"

#include "TGButtonGroup.h"
#include "TGButton.h"
#include "TGListBox.h"
#include "TGLabel.h"
//#include "TGCheckButton.h"

#include <iostream>
using namespace std;

RecoWindow::RecoWindow(const TGWindow *p, int w, int h)
    : TGHorizontalFrame(p, w, h)
{

    // Reco tracks frame                                                                                                                                                                                                                                                                   
    fRecoGroupFrame = new TGGroupFrame(this, "Reconstructed", kVerticalFrame);// | kFixedWidth | kLHintsExpandY);
    int hTracks = 285;
    fRecoTracksFrame = new TGVerticalFrame(this, w/2+50, hTracks-100);
    TGLabel *labelRecoTracks = new TGLabel(fRecoTracksFrame, "Reco Tracks (MeV)");
    fRecoTracksFrame->AddFrame(labelRecoTracks, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fRecoTracksListBox = new TGListBox(fRecoTracksFrame);
    fRecoTracksListBox->Resize(w/2+40, hTracks-80);
    fRecoTracksFrame->AddFrame(fRecoTracksListBox, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fRecoGroupFrame->AddFrame(fRecoTracksFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));
    showRecoButton = new TGCheckButton(fRecoGroupFrame, "Show Reco ");
    fRecoGroupFrame->AddFrame(showRecoButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    showRecoButton->SetState(kButtonUp);


    // Palette Selection                                                                                                                                                                                                                                                                   
    fColorScaleFrame = new TGVerticalFrame(this, w/2, hTracks, kVerticalFrame | kLHintsExpandY);
    paletteButtonGroup = new TGButtonGroup(fColorScaleFrame, "Color Theme", kVerticalFrame | kLHintsExpandY);
    rainbowPaletteButton = new TGRadioButton(paletteButtonGroup, "Rainbow");
    grayPaletteButton = new TGRadioButton(paletteButtonGroup, "Film");
    summerPaletteButton = new TGRadioButton(paletteButtonGroup, "Sea");
    grayinvPaletteButton = new TGRadioButton(paletteButtonGroup, "Ink");
    rainbowPaletteButton->SetState(kButtonDown);
    fColorScaleFrame->AddFrame(paletteButtonGroup, new TGLayoutHints(kLHintsTop | kLHintsExpandY, 2, 2, 2, 2));

    AddFrame(fRecoGroupFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandY, 2, 2, 0, 0));
    AddFrame(fColorScaleFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandY, 2, 2, 2, 2));
}

RecoWindow::~RecoWindow()
{

}
