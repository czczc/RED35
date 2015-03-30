#include "ControlWindow.h"
#include "InfoWindow.h"

#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"
#include "TGListBox.h"
#include "TGLabel.h"
#include "TGDoubleSlider.h"

#include <iostream>
using namespace std;

ControlWindow::ControlWindow(const TGWindow *p, int w, int h)
    :TGVerticalFrame(p, w, h)
{
    fInfoWindow = new InfoWindow(this, 200, 200);
    AddFrame(fInfoWindow, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    // Navigation Frame
    fNavigationFrame = new TGHorizontalFrame(this, w, 100, kFixedWidth);
    AddFrame(fNavigationFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    prevButton = new TGTextButton(fNavigationFrame, "< Prev");
    fNavigationFrame->AddFrame(prevButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    eventEntry = new TGNumberEntryField(fNavigationFrame, -1, 0, TGNumberFormat::kNESInteger);
    eventEntry->SetDefaultSize(80, 20);
    fNavigationFrame->AddFrame(eventEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

    nextButton = new TGTextButton(fNavigationFrame, "Next >");
    fNavigationFrame->AddFrame(nextButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    

    // Display Option Selection
    fDisplayFrame = new TGHorizontalFrame(this, w, 100);
    AddFrame(fDisplayFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 1, 1));
    displayButtonGroup = new TGButtonGroup(fDisplayFrame, "Display Properties", kHorizontalFrame); 
    fDisplayFrame->AddFrame(displayButtonGroup, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    rawButton = new TGRadioButton(displayButtonGroup,  "Raw    "); 
    calibButton = new TGRadioButton(displayButtonGroup, "Calib    "); 
    hitsButton = new TGRadioButton(displayButtonGroup, "Hits    "); 
    rawButton->SetState(kButtonDown);

    // zoom control button frame  
    fZoomControlFrame = new TGGroupFrame(this, "Display Options", kVerticalFrame);
    fZoomControlFrame->SetTitlePos(TGGroupFrame::kLeft);
    AddFrame(fZoomControlFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 1, 1));

    fZoomButtonFrame = new TGHorizontalFrame(fZoomControlFrame, w, 100);
    fZoomControlFrame->AddFrame(fZoomButtonFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    autoZoomButton = new TGTextButton(fZoomButtonFrame, "Auto Zoom");
    fZoomButtonFrame->AddFrame(autoZoomButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    unZoomButton = new TGTextButton(fZoomButtonFrame, "UnZoom");
    fZoomButtonFrame->AddFrame(unZoomButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    // XaxisFrame Frame
    fXaxisFrame = new TGHorizontalFrame(fZoomControlFrame, w, 100);
    fZoomControlFrame->AddFrame(fXaxisFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    xrangeButton = new TGTextButton(fXaxisFrame, "X Range");
    fXaxisFrame->AddFrame(xrangeButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    tdcMinEntry = new TGNumberEntryField(fXaxisFrame, -1, 0, TGNumberFormat::kNESInteger);
    tdcMinEntry->SetDefaultSize(40, 20);
    tdcMinEntry->SetIntNumber(0);
    fXaxisFrame->AddFrame(tdcMinEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

    tdcMaxEntry = new TGNumberEntryField(fXaxisFrame, -1, 0, TGNumberFormat::kNESInteger);
    tdcMaxEntry->SetDefaultSize(50, 20);
    tdcMaxEntry->SetIntNumber(3200);
    fXaxisFrame->AddFrame(tdcMaxEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));


    // ZaxisFrame Frame
    fZaxisFrame = new TGHorizontalFrame(fZoomControlFrame, w, 100);
    fZoomControlFrame->AddFrame(fZaxisFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));

    zrangeButton = new TGTextButton(fZaxisFrame, "Color Scale");
    fZaxisFrame->AddFrame(zrangeButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

    ztColorEntry = new TGNumberEntryField(fZaxisFrame, -1, 0, TGNumberFormat::kNESInteger);
    ztColorEntry->SetDefaultSize(40, 20);
    ztColorEntry->SetIntNumber(0);
    fZaxisFrame->AddFrame(ztColorEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

    utColorEntry = new TGNumberEntryField(fZaxisFrame, -1, 0, TGNumberFormat::kNESInteger);
    utColorEntry->SetDefaultSize(40, 20);
    utColorEntry->SetIntNumber(0);
    fZaxisFrame->AddFrame(utColorEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

    vtColorEntry = new TGNumberEntryField(fZaxisFrame, -1, 0, TGNumberFormat::kNESInteger);
    vtColorEntry->SetDefaultSize(40, 20);
    vtColorEntry->SetIntNumber(0);
    fZaxisFrame->AddFrame(vtColorEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));
    
    // Induction Signal Group
    inductionSigButtonGroup = new TGButtonGroup(fZoomControlFrame, "Show Induction Signal", kHorizontalFrame); 
    fZoomControlFrame->AddFrame(inductionSigButtonGroup, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    inductionSigButton_pos = new TGRadioButton(inductionSigButtonGroup, "Positive    ");
    inductionSigButton_neg = new TGRadioButton(inductionSigButtonGroup, "Negative    ");
    inductionSigButton_both = new TGRadioButton(inductionSigButtonGroup, "Both    ");
    inductionSigButton_pos->SetState(kButtonDown);

    // APA button group
    apaButtonGroup = new TGButtonGroup(fZoomControlFrame, "Show APA", kHorizontalFrame);
    fZoomControlFrame->AddFrame(apaButtonGroup, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    apaButton_1 = new TGCheckButton(apaButtonGroup, "1  ");
    apaButton_2 = new TGCheckButton(apaButtonGroup, "2  ");
    apaButton_3 = new TGCheckButton(apaButtonGroup, "3  ");
    apaButton_4 = new TGCheckButton(apaButtonGroup, "4  ");
    apaButton_1->SetState(kButtonDown);
    apaButton_2->SetState(kButtonDown);
    apaButton_3->SetState(kButtonDown);
    apaButton_4->SetState(kButtonDown);

    // MC group frame
    fMCGroupFrame = new TGGroupFrame(this, "Monte Carlo Truth", kVerticalFrame);
    int hTracks = 255;
    fMCTracksFrame = new TGHorizontalFrame(fMCGroupFrame, w, hTracks);
    fDaughterTracksFrame = new TGVerticalFrame(fMCTracksFrame, w, hTracks);
    fParentSiblingTracksFrame = new TGVerticalFrame(fMCTracksFrame, w, hTracks);
    

    TGLabel *labelDaughterTracks = new TGLabel(fDaughterTracksFrame, "Children (MeV)");
    fDaughterTracksFrame->AddFrame(labelDaughterTracks, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    
    fDaughterTracksListBox = new TGListBox(fDaughterTracksFrame);
    fDaughterTracksFrame->AddFrame(fDaughterTracksListBox, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fDaughterTracksListBox->Resize(w/2, hTracks-40);


    TGLabel *labelParentTracks = new TGLabel(fParentSiblingTracksFrame, "Parents (MeV)");
    fParentSiblingTracksFrame->AddFrame(labelParentTracks, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    
    fParentTracksListBox = new TGListBox(fParentSiblingTracksFrame);
    fParentSiblingTracksFrame->AddFrame(fParentTracksListBox, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fParentTracksListBox->Resize(w/2, 40);


    TGLabel *labelSiblingTracks = new TGLabel(fParentSiblingTracksFrame, "Siblings (MeV)");
    fParentSiblingTracksFrame->AddFrame(labelSiblingTracks, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    
    fSiblingTracksListBox = new TGListBox(fParentSiblingTracksFrame);
    fParentSiblingTracksFrame->AddFrame(fSiblingTracksListBox, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fSiblingTracksListBox->Resize(w/2, hTracks-120);

    showMCButton = new TGCheckButton(fParentSiblingTracksFrame, "Show MC ");
    fParentSiblingTracksFrame->AddFrame(showMCButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    showMCButton->SetState(kButtonUp);

    fMCTracksFrame->AddFrame(fParentSiblingTracksFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fMCTracksFrame->AddFrame(fDaughterTracksFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    fMCGroupFrame->AddFrame(fMCTracksFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    AddFrame(fMCGroupFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 0, 0));

}

ControlWindow::~ControlWindow()
{
    // delete prev;
    // delete eventEntry;
    // delete next;
    // delete fNavigationFrame;
    // delete fMCTracksFrame;
}
