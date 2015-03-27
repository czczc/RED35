#ifndef PD_CONTROL_H
#define PD_CONTROL_H

#include "TGFrame.h"

class PDWindow;
class RecoWindow;

class TGButtonGroup;
class TGRadioButton;
class TGListBox;
class TGDoubleVSlider;
class TGCheckButton;
class TGLabel;

class PDControl: public TGVerticalFrame
{
public:
    PDControl(const TGWindow *p, int w, int h);
    virtual ~PDControl();

    TGVerticalFrame    *fColorScaleFrame; 
    TGButtonGroup      *paletteButtonGroup;
    TGRadioButton      *rainbowPaletteButton; 
    TGRadioButton      *grayPaletteButton;
    TGRadioButton      *summerPaletteButton;
    TGRadioButton      *grayinvPaletteButton;


    TGGroupFrame     *fRecoGroupFrame;
    TGListBox          *fRecoTracksListBox;
    TGCheckButton      *showRecoButton;

    PDWindow         *fPDWindow;
    RecoWindow       *fRecoWindow;

    ClassDef(PDControl, 0)
};

#endif
