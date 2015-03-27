#include "PDWindow.h"
#include "MCEvent.h"
#include "MCChannel.h"

#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2Poly.h"
#include "TStyle.h"
#include "TString.h"
#include "TCut.h"
#include "TColor.h"
#include "TLegend.h"
#include "TDirectory.h"
#include "TExec.h"

#include <iostream>
#include <vector>
using namespace std;


PDWindow::PDWindow(const TGWindow *p, int w,int h)
    :TRootEmbeddedCanvas("ECanvas", p, w, h)
{
    can = GetCanvas();
    InitCanvas();
    SetStyle(0);
}

PDWindow::~PDWindow()
{
}

void PDWindow::InitCanvas()
{
    can->Divide(1, 2, 0.005, 0.005);
}

void PDWindow::SetStyle(int theme)
{
    if (theme == 0) { // night theme
      can->cd();
      gPad->SetFillColor(TColor::GetColor(210,210,210));
      can->cd(1)->SetFrameFillColor(TColor::GetColor(float(0.1), float(0.1), float(0.1)));
      can->cd(2)->SetFrameFillColor(TColor::GetColor(float(0.1), float(0.1), float(0.1)));
    }
    else { // day theme
      can->cd();
      gPad->SetFillColor(kWhite);
      can->cd(1)->SetFrameFillColor(TColor::GetColor(float(0.1), float(0.1), float(0.1)));
      can->cd(2)->SetFrameFillColor(kWhite);
    }
    can->cd(1)->SetFillColor(kWhite);
    can->cd(2)->SetFillColor(kWhite);
    can->cd(1)->SetMargin(0.1, 0.13, 0.1, 0.1);
    can->cd(2)->SetLeftMargin(0.1);

}

void PDWindow::ClearCanvas(int i)
{
    can->cd(i);
    gPad->Clear();
}

void PDWindow::DrawOpDets(MCEvent *ev)
{
    ClearCanvas(1);
    ev->FillOpDet();
    //ev->hOpDetAll->SetFillColor(1);
    //TExec *pal = new TExec("pal","gStyle->SetPalette(53)");
    ev->hOpDetAll->Draw("colz");
    //pal->Draw();
    //ev->hOpDetAll->Draw("col same");
    UpdateCanvas(1);
    //delete pal;
}

void PDWindow::DrawOpDetChannel(int i, MCEvent *ev)
{
    ClearCanvas(2);
    TH1F *dummy = new TH1F("","",40,0,16000);
    dummy->SetName("dummy");
    TLegend *lg = new TLegend(0.6, 0.8, 0.95, 0.89);;
    lg->SetBorderSize(0);
    if (i==8) {
      TString cut_s(Form("eventNo==%d", ev->eventNo));
      TCut cut_c = cut_s.Data();
      dummy->SetTitle("Detected Photons");
      dummy->GetXaxis()->SetTitle("time [ns]");
      dummy->SetMaximum(ev->CountDetected/2.);
      can->cd(2);
      dummy->Draw();
      ev->opTree[0]->Draw("Time>>hOpDetTime(40,0,16000)", cut_c, "same");
      TH1F *hOpDetTime = (TH1F*)gDirectory->Get("hOpDetTime");
      hOpDetTime->SetLineColor(kOrange);
      lg->AddEntry((TObject*)0, Form("Total photon %d", ev->CountDetected), "");
      lg->Draw("same");
      UpdateCanvas(2);
    } else {
      TString cut_s(Form("eventNo==%d && OpChannel==%d", ev->eventNo, i));
      TCut cut_c = cut_s.Data();
      dummy->SetTitle(Form("Photon Detector %i", i+1));
      dummy->GetXaxis()->SetTitle("time [ns]");
      dummy->SetMaximum(ev->CountOpDetDetected[i]/2.);
      can->cd(2);
      dummy->Draw();
      ev->opTree[0]->Draw("Time>>hOpDetTime(40,0,16000)", cut_c, "same");
      TH1F *hOpDetTime = (TH1F*)gDirectory->Get("hOpDetTime");
      hOpDetTime->SetLineColor(kOrange);
      lg->AddEntry((TObject*)0, Form("Photon # %d", ev->CountOpDetDetected[i]), "");
      lg->Draw("same");
      UpdateCanvas(2);
    }
}

void PDWindow::UpdateCanvas(int i)
{
    can->cd(i)->Modified();
    can->cd(i)->Update();
}
