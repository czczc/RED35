#include "MainWindow.h"
#include "ViewWindow.h"
#include "ControlWindow.h"
#include "PDControl.h"
#include "TGLayout.h"
#include "TGMenu.h"

#include "TString.h"

#include <iostream>
using namespace std;


MainWindow::MainWindow(const TGWindow *p, int w,int h)
    : TGMainFrame(p, w, h)
{
    InitMenu();

    fViewAndControlFrame = new TGHorizontalFrame(this, w, h, kFixedWidth);
    AddFrame(fViewAndControlFrame, new TGLayoutHints(kLHintsExpandX  | kLHintsExpandY, 2, 2, 2, 2));

    int controlWindowWidth = 240;
    int PDControlWidth = 260;
    fViewWindow = new ViewWindow(fViewAndControlFrame, w-controlWindowWidth-PDControlWidth, h);
    fViewAndControlFrame->AddFrame(fViewWindow, new TGLayoutHints(
        // kLHintsTop | kFixedWidth, 2, 2, 2, 2));
        kLHintsTop | kLHintsExpandX  | kLHintsExpandY, 2, 2, 2, 2));

    fControlWindow = new ControlWindow(fViewAndControlFrame, controlWindowWidth, h);
    fViewAndControlFrame->AddFrame(fControlWindow, new TGLayoutHints(kLHintsTop | kLHintsExpandY  | kFixedWidth, 2, 2, 2, 2));

    //TGMatrixLayout(fPDControl, 2, 2, 2, 2); 
    fPDControl = new PDControl(fViewAndControlFrame, PDControlWidth, h);
    fViewAndControlFrame->AddFrame(fPDControl, new TGLayoutHints(kLHintsTop | kLHintsExpandY  | kFixedWidth, 2, 2, 2, 2));

    // set main window stats
    SetWindowName("LBNE 35t Event Viewer");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

void MainWindow::InitMenu()
{
    fMenuBar = new TGMenuBar(this, 10, 100);
    AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

    fMenuFile = new TGPopupMenu(gClient->GetRoot());
    fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
    fMenuFile->AddSeparator();
    fMenuFile->AddEntry(new TGHotString("E&xit"), M_FILE_EXIT);

    fMenuBar->AddPopup(new TGHotString("&File"), fMenuFile, fMenuBarItemLayout);

}


MainWindow::~MainWindow()
{
    CleanUp();
}

void MainWindow::CleanUp()
{
    // delete fViewWindow;
    // delete fMenuBar;
}
