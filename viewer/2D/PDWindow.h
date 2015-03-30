#ifndef PD_WINDOW_H
#define PD_WINDOW_H

#include "TRootEmbeddedCanvas.h"

#include <vector>

#define MAXCOLORS 6

class MCEvent;
class TObject;

class PDWindow: public TRootEmbeddedCanvas
{
public:
    PDWindow(const TGWindow *p, int w,int h);
    virtual ~PDWindow();

    void InitCanvas();
    void ClearCanvas(int i);
    void UpdateCanvas(int i);

    void DrawOpDets(MCEvent *ev);
    void DrawOpDetChannel(int i, MCEvent *ev);

    void SetStyle(int theme);

    TCanvas* can; 

    ClassDef(PDWindow, 0)
};

#endif
