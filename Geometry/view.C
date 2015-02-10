{
  TGeoManager::Import("lbne35t4apa.gdml");
  // gGeoManager->GetTopVolume()->Draw("ogl"); // opengl viewer
  TBrowser b("b", gGeoManager);

  gGeoManager->GetTopVolume()->Draw();
}
