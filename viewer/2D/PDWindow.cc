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
#include "TLatex.h"
#include "TRandom3.h"
#include "RVersion.h"

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
    can->cd(1)->SetMargin(0.07, 0.13, 0.1, 0.1);
    can->cd(2)->SetLeftMargin(0.1);
    //can->cd(2)->SetRightMargin(0.1);

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

    ev->hOpDetWaveformCount->Draw("colz");
    //ev->hOpDetAll->SetFillColor(1);
    //TExec *pal = new TExec("pal","gStyle->SetPalette(53)");
    //ev->hOpDetAll->Draw("colz");
    //pal->Draw();
    //ev->hOpDetAll->Draw("col same");
    UpdateCanvas(1);
    //delete pal;
}

void PDWindow::DrawOpDetChannel(int op, double lowbin, double highbin, MCEvent *ev)
{
    ClearCanvas(2);
    if (lowbin < ev->hOpDetWaveformCount->GetXaxis()->GetBinUpEdge(1) || highbin > ev->hOpDetWaveformCount->GetXaxis()->GetBinLowEdge(ev->hOpDetWaveformCount->GetNbinsX())) {
      UpdateCanvas(2);
      return;
    }
    std::map<int, int> waveformcount;
    std::map<int, int> drawnwaveformcount;
    std::map<int, int> drawnchannelcount;
    std::vector<TH1D*> allHist;
    TLatex txt;
    txt.SetTextSize(0.05);
    double vshift = 0;
    TH1D sumHist("sumHist",Form("OpDet %i,  %2.3f ~ %2.3f #musec", op, lowbin, highbin), 100, lowbin, highbin);
    sumHist.SetNdivisions(100,"Y");
    sumHist.GetXaxis()->SetTitle("time [#musec]");
    sumHist.GetYaxis()->SetLabelSize(0);
    sumHist.Draw();

    for (size_t n = 0; n < ev->timestamp->at(op).size(); ++n) {
      double timestamp = ev->timestamp->at(op).at(n);
      int channel = ev->OpChannelToOpDet->at(op).at(n);
      std::map<int,int>::iterator it = waveformcount.find(channel);
      if (it == waveformcount.end()) waveformcount[channel]=0;
      waveformcount[channel]++;
      if (timestamp > highbin || timestamp < lowbin) continue;
      TH1D *tmp = (TH1D*)ev->averageWaveform->FindObject(Form("avgwaveform_channel_%i_%i", channel, waveformcount[channel]-1))->Clone();
      if (tmp) {
	tmp->SetName(Form("c%i_%i_%i", channel, waveformcount[channel]-1, rnd.Integer(1000)));
	std::map<int,int>::iterator itr = drawnwaveformcount.find(channel);
	if (itr == drawnwaveformcount.end()) drawnwaveformcount[channel]=drawnwaveformcount.size();
	allHist.push_back(tmp);
	drawnchannelcount[allHist.size()-1]=channel;
	if (allHist.size()==1) {
	  vshift = allHist.at(0)->Integral()/allHist.at(0)->GetNbinsX();
	  allHist.at(0)->Draw("same][");
	} else {
	  for (int i = 1; i <= allHist.back()->GetNbinsX(); ++i) {
	    allHist.back()->SetBinContent(i, allHist.back()->GetBinContent(i)+vshift*drawnwaveformcount[channel]);
	  }
	  allHist.back()->Draw("same][");
	}
      }
    }
    if (allHist.size()) {
      sumHist.SetMaximum(allHist.at(0)->GetMaximum()*(drawnwaveformcount.size()+1));
      sumHist.SetMinimum(allHist.at(0)->GetMinimum()*0.5);
      double x = sumHist.GetXaxis()->GetBinLowEdge(1)-sumHist.GetNbinsX()*sumHist.GetBinWidth(1)/15.;
      for (size_t i = 0; i < allHist.size(); ++i) {
	allHist.at(i)->SetLineColor(kRed);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
	txt.DrawLatex(0.02, (double)drawnwaveformcount[drawnchannelcount[i]]/(drawnwaveformcount.size()+1.), Form("%i",drawnchannelcount[i]));
#else
	txt.DrawLatex(x, allHist.at(i)->GetBinContent(1), Form("%i",drawnchannelcount[i]));
#endif
      }
    }
    UpdateCanvas(2);
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
