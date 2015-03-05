#ifndef GUI3D_CONTROLLER_H
#define GUI3D_CONTROLLER_H

#include "RQ_OBJECT.h"

class TGTextButton;
class TGNumberEntryField;
class TGMainFrame;
class TDatabasePDG;
class TEveTrackList;

class MCEvent;

class Gui3DController
{
    RQ_OBJECT("3DCotroller")
public:
    Gui3DController();
    virtual ~Gui3DController();

    void InitGeometry();
    void InitNavigationFrame();
    void InitEvent();
    void Reload();
    void AddMCTracks();
    void AddRecoTracks();
    void ProjectionView();
    void Run();

    // slots
    void Prev();
    void Next();
    void Jump();

    MCEvent *event;
    int currentEvent;
    TDatabasePDG *dbPDG;
    TEveTrackList *mcTrackList;
    TEveTrackList *recoTrackList;

    // GUIs
    TGMainFrame        *frmMain;
    TGTextButton       *prevButton, *nextButton;
    TGNumberEntryField *eventEntry; 

};

#endif