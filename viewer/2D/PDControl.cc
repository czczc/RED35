#include "PDControl.h"
#include "PDWindow.h"
#include "RecoWindow.h"

#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"
#include "TGListBox.h"
#include "TGLabel.h"
#include "TGDoubleSlider.h"
#include "TGLayout.h"

#include <iostream>
using namespace std;

PDControl::PDControl(const TGWindow *p, int w, int h)
  :TGVerticalFrame(p, w, h)
{


    fPDWindow = new PDWindow(this, w, 495);
    fRecoWindow = new RecoWindow(this, w, h-495);
    //this->ChangeOptions((this->GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

    /*
    // Reco tracks frame
    fRecoGroupFrame = new TGGroupFrame(this, "Reconstructed", kVerticalFrame | kFixedWidth);
    int hTracks = 260;
    fRecoGroupFrame->SetWidth(w-150);
    TGLabel *labelRecoTracks = new TGLabel(fRecoGroupFrame, "Reco Tracks (MeV)");
    fRecoGroupFrame->AddFrame(labelRecoTracks, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));    
    fRecoTracksListBox = new TGListBox(fRecoGroupFrame);
    fRecoGroupFrame->AddFrame(fRecoTracksListBox, new TGLayoutHints(kLHintsTop | kLHintsExpandY, 2, 2, 2, 2));
    fRecoTracksListBox->Resize(130, hTracks-40);
    showRecoButton = new TGCheckButton(fRecoGroupFrame, "Show Reco ");
    fRecoGroupFrame->AddFrame(showRecoButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    showRecoButton->SetState(kButtonUp);

    // Palette Selection
    fColorScaleFrame = new TGVerticalFrame(this, 100, hTracks, kFixedWidth);
    paletteButtonGroup = new TGButtonGroup(fColorScaleFrame, "Color Theme", kVerticalFrame); 
    rainbowPaletteButton = new TGRadioButton(paletteButtonGroup, "Rainbow"); 
    grayPaletteButton = new TGRadioButton(paletteButtonGroup, "Film"); 
    summerPaletteButton = new TGRadioButton(paletteButtonGroup, "Sea"); 
    grayinvPaletteButton = new TGRadioButton(paletteButtonGroup, "Ink"); 
    rainbowPaletteButton->SetState(kButtonDown);
    fColorScaleFrame->AddFrame(paletteButtonGroup, new TGLayoutHints(kLHintsTop | kLHintsExpandY, 2, 2, 2, 2));
    */
    //SetLayoutManager(new TGXYLayout(this));
    //this->SetLayoutManager(TGLayoutManager *1);

    //SetLayoutManager(new TGMatrixLayout(this, 2, 2, 2, 2)); 

    AddFrame(fPDWindow,
    //new TGXYLayoutHints(900, 0, w, 498, 
    //TGXYLayoutHints::kLRubberX |
    //TGXYLayoutHints::kLRubberY |
    //TGXYLayoutHints::kLRubberH ));
	     new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    //AddFrame(fRecoGroupFrame,
    //, new TGXYLayoutHints(900, 498, 200, hTracks,
    //TGXYLayoutHints::kLRubberX |
    //TGXYLayoutHints::kLRubberY |
    //TGXYLayoutHints::kLRubberH ));
    //new TGLayoutHints(kLHintsTop | kLHintsExpandY, 2, 2, 2, 2));
    //AddFrame(fColorScaleFrame,
    //, new TGXYLayoutHints(1100, 498, 150, hTracks,
    //TGXYLayoutHints::kLRubberX |
    //TGXYLayoutHints::kLRubberY |
    //TGXYLayoutHints::kLRubberH ));
    //new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandY, 2, 2, 2, 2));
    AddFrame(fRecoWindow, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

}

PDControl::~PDControl()
{

}
