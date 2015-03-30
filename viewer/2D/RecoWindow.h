#ifndef RECO_WINDOW_H
#define RECO_WINDOW_H

#include "TGFrame.h"

class TGButtonGroup;
class TGRadioButton;
class TGListBox;
class TGCheckButton;
class TGLabel;

class RecoWindow : public TGHorizontalFrame
{
 public:
    RecoWindow(const TGWindow *p, int w, int h);
    virtual ~RecoWindow();

    TGVerticalFrame    *fColorScaleFrame; 
    TGButtonGroup      *paletteButtonGroup;
    TGRadioButton      *rainbowPaletteButton; 
    TGRadioButton      *grayPaletteButton;
    TGRadioButton      *summerPaletteButton;
    TGRadioButton      *grayinvPaletteButton;


    TGGroupFrame       *fRecoGroupFrame;
    TGVerticalFrame    *fRecoTracksFrame;
    TGListBox          *fRecoTracksListBox;
    TGCheckButton      *showRecoButton;

    ClassDef(RecoWindow, 0)
};

#endif
