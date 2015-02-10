{
    gROOT->Reset();
    gROOT->ProcessLine(".x load3dClasses.C" );

    Gui3DController *w = new Gui3DController();
    w->Run();
}