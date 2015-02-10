{
    TString include = ".include ";
    TString load = ".L ";

    gSystem->Load("libGLEW");
    gSystem->Load("libFTGL");
    TString prefix = "../event";
    gROOT->ProcessLine( include + prefix );
    gROOT->ProcessLine( load + prefix + "/MCChannel.cc+" );
    gROOT->ProcessLine( load + prefix + "/MCGeometry.cc+" );
    gROOT->ProcessLine( load + prefix + "/MCEvent.cc+" );

    TString prefix = "../viewer/3D";
    gROOT->ProcessLine( include + prefix );
    gROOT->ProcessLine( load + prefix + "/Gui3DController.cc+" );

}