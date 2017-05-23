Int_t determineCombinedEventsKept(TString& algA, Float_t threshA, TString& algB, Float_t threshB)
{
  std::cout << "Determining how many zero bias events kept when using combined algorithm of " << algA << " at: " << threshA << " and "
  << algB << " at: " << threshB << std::endl;
  TString fileName = "../myData/ZeroBias2016new.13Runs.root";
	TFile * 2016Data = TFile::Open(fileName, "READ");
	Int_t nbins = 60;
	Double_t metMin = 0.0;
	Double_t metMax = 500.0;
  Int_t numRndm = 0;
  Int_t nentries = tree->GetEntries();
  Float_t algAMET, algBMET;
  Int_t passrndm;

  tree->SetBranchAddress("passrndm",&passrndm);
  tree->SetBranchAddress(algA,&algAMET);
  tree->SetBranchAddress(algB,&algBMET);

  Int_t counter = 0;

  for (Int_t i  = 0 ; i < nentries ;i++)
  {
    tree->GetEntry(i);
    if ((passrndm > 0.1) && (algAMET > threshA) && (algBMET > threshB))
    {
      counter++;
    }
  }

std::cout << "Combined alg kept: " << counter << " events" << std::endl;

return(0);


}
