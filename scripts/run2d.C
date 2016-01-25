void run2d(const char* fileName=0)
{
  if(fileName){
    std::cout << "RED35 - Opening fileName \"" << fileName << "\"" << std::endl;
    bool file_does_not_exist = gSystem->AccessPathName(fileName); //This function returns FALSE if the file exists!
    if(file_does_not_exist){
      std::cout << "RED35 - File does not exist!" << std::endl;
      return;
    }
  }
  else{
    std::cout << "RED35 - You didn't supply a filename ( root -l run2d.C\(\"path/to/file.root\"\) )" << std::endl;
  }
    gROOT->Reset();
    gROOT->ProcessLine(".x loadClasses.C" );

    // defalut: GuiController(const TGWindow *p, int w, int h,
    //   const char* filename=0,
    //   int nTDCTicks=3200,
    //   double xPerTDC=0.0802815);
    GuiController *gc = new GuiController(gClient->GetRoot(), 1200, 900, fileName, 3200, 0.0802815);
}
