// example batch mode
// root -l batch.C (however it doesn't work in batch mode "-b")
{
    gROOT->Reset();
    gROOT->ProcessLine(".x loadClasses.C" );
    // gROOT->SetBatch();

    GuiController *gc = new GuiController(gClient->GetRoot(), 900, 900, "../data/xy.root");
    TString filename;
    for (int i=0; i!=gc->event->nEvents; i++) {
        filename.Form("batch_%i.png", i);
        gc->vw->can->SaveAs(filename.Data());
        gc->Next();
    }
}
