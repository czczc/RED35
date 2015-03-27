{
    gROOT->Reset();
    gROOT->ProcessLine(".x loadClasses.C" );

    GuiController *gc = new GuiController(gClient->GetRoot(), 1200, 900);
}
