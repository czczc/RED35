{
    gROOT->Reset();
    gROOT->ProcessLine(".x loadClasses.C" );

    Gui3DController *w = new Gui3DController();
    w->Run();
}