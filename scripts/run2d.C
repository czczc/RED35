{
    gROOT->Reset();
    gROOT->ProcessLine(".x loadClasses.C" );

    // defalut: GuiController(const TGWindow *p, int w, int h,
    //   const char* filename=0,
    //   int nTDCTicks=3200,
    //   double xPerTDC=0.0802815);
    GuiController *gc = new GuiController(gClient->GetRoot(), 1200, 900, 0, 3200, 0.0802815);
}
