#include "mincerMacros.h"

Float_t computeMetNoMu( const Float_t mexoffrecal , const Float_t meyoffrecal , const Float_t mexoffrecalmuon , const Float_t meyoffrecalmuon )
{
    Float_t metnomu = sqrt(((mexoffrecal - mexoffrecalmuon) * (mexoffrecal - mexoffrecalmuon)) +
    ((meyoffrecal - meyoffrecalmuon)*(meyoffrecal - meyoffrecalmuon))); //compute metnomu
    return (metnomu);
}


Bool_t passTransverseMassCut( const Float_t metoffrecal , const Float_t mexoffrecal  , const Float_t meyoffrecal ,
                              const Float_t metoffrecalmuon , const Float_t mexoffrecalmuon , const Float_t meyoffrecalmuon )
{
    Float_t wLowerbnd = 40.0;
    Float_t wUpperbnd = 100.0;
    Float_t wValue = sqrt( 2.0 * metoffrecal * metoffrecalmuon * ( 1 + ( ( mexoffrecal * mexoffrecalmuon + meyoffrecal * meyoffrecalmuon ) /
                           ( metoffrecal * metoffrecalmuon ) ) ) );
    return( (( wValue >= wLowerbnd ) && ( wValue <= wUpperbnd )) );
}

Float_t computeThresh(const TH1F* target, const Float_t numberEventsToKeep)
{
    Int_t nbin = 0;
    //target is a right hand cumulative sum histogram
    target->GetBinWithContent( numberEventsToKeep , nbin , 5 , -1 , 20000 ); //if firstx<=0{firstx=1}; if lastx<firstx{lastx=fXaxis.GetNbinsX()};
    Float_t thresh = (target->GetXaxis())->GetBinCenter(nbin);
    return(thresh);
}


Float_t determineZeroBiasThresh( userInfo* parameters, const TString verbose )
{
    //this function determines thresh to keep proper trigger rate for process 2 on algs A and B
    //these thresholds are used on both passnoalg and muon data
    parameters->Print();
    const Float_t metL1Thresh      = parameters->Get_MetL1Thresh();
    const Float_t actintCut        = parameters->Get_ActintCut();
    const TString threshFileName   = parameters->Get_ThreshFileName();
    const TString algAName         = parameters->Get_AlgAName();
    const TString algBName         = parameters->Get_AlgBName();
    const Float_t frac             = parameters->Get_Frac();
	const Double_t metMin          = parameters->Get_MetMin();
	const Double_t metMax          = parameters->Get_MetMax();
    const Int_t numberEventsToKeep = parameters->Get_NumberEventsToKeep();
	const Int_t nbins              = parameters->Get_Nbins();
    const Float_t passnoalgcut     = parameters->Get_Passnoalgcut(); 
    const Float_t passrndmcut      = parameters->Get_Passrndmcut(); 
    const TString threshFilePath = "../myData/" + threshFileName;
	TFile *threshFileHandle = TFile::Open(threshFilePath, "READ");
	TTree *threshTree = (TTree*)(threshFileHandle->Get("tree"));
    const Int_t passnoAlgNentries = threshTree->GetEntries();
    
    Float_t actint = 0;
    Float_t algAMET,algBMET=0;
	Float_t metl1, algMET;
    Int_t numberEventsAlgAKept = 0;
    Int_t numberEventsAlgBKept = 0;
    Int_t passnoalgL1XE10 , passnoalgL1XE30 , passnoalgL1XE40 , passnoalgL1XE45,passrndm;
   

    if (verbose){
        std::cout << "DETERMINETHRESH.C" << std::endl;
        std::cout << "using passnoalg datafile: " << threshFileName << std::endl;
        std::cout << "algA: " << algAName << std::endl;
        std::cout << "algB: " << algBName << std::endl;
        std::cout << "using L1 thresh: " << metL1Thresh << std::endl;
        std::cout << "fraction of events to keep: " << frac << std::endl;
        std::cout << "passnoalg nentries: " << passnoAlgNentries << std::endl;
    }


    TH1F *AlgAHist = new TH1F(algAName, algAName, nbins, metMin, metMax);
    TH1F *AlgBHist = new TH1F(algBName, algBName, nbins, metMin, metMax);

    //set branch address for zerobias branches
	threshTree->SetBranchAddress(algAName,&algAMET);
	threshTree->SetBranchAddress(algBName,&algBMET);
	threshTree->SetBranchAddress("metl1",&metl1);
    threshTree->SetBranchAddress("passnoalgL1XE10",&passnoalgL1XE10);
    threshTree->SetBranchAddress("passnoalgL1XE30",&passnoalgL1XE30);
    threshTree->SetBranchAddress("passnoalgL1XE40",&passnoalgL1XE40);
    threshTree->SetBranchAddress("passnoalgL1XE45",&passnoalgL1XE45);
    threshTree->SetBranchAddress("actint",&actint);
    threshTree->SetBranchAddress("passrndm",&passrndm);

    for (Int_t k = 0; k < passnoAlgNentries; k++)
	{
		threshTree->GetEntry(k);
        Bool_t passl1 = metl1 > metL1Thresh ;
        Bool_t passactint = actint > actintCut;
        Bool_t isPassnoalg = passnoalgL1XE10 > passnoalgcut || passnoalgL1XE30 > passnoalgcut || passnoalgL1XE40 > passnoalgcut || passnoalgL1XE45 > passnoalgcut;
        Bool_t isPassrndm = passrndm > passrndmcut;

		if ( ( passl1 ) && ( passactint ) && ( isPassnoalg || isPassrndm ))
		{
		    AlgAHist->Fill(algAMET);
		    AlgBHist->Fill(algBMET);
		}
    }

    //compute the threshold to keep appropriate fraction
    TH1F *AlgAtarget = (TH1F*) AlgAHist->GetCumulative(kFALSE);
    TH1F *AlgBtarget = (TH1F*) AlgBHist->GetCumulative(kFALSE);
	Float_t AlgAThresh = computeThresh(AlgAtarget, numberEventsToKeep);
	Float_t AlgBThresh = computeThresh(AlgBtarget, numberEventsToKeep);


    if (verbose){
        std::cout << algAName << " threshold: " << AlgAThresh << std::endl;
        std::cout << algBName << " threshold: " << AlgBThresh << std::endl;
        std::cout << "target number events to keep: " << numberEventsToKeep << std::endl;
    }
    
    //determine number of events kept at determined threshold (gives idea of error due to binning)
	for (Int_t l = 0 ; l < passnoAlgNentries ; l++)
	{
		threshTree->GetEntry(l);
        Bool_t passl1 = metl1 > metL1Thresh ;
        Bool_t passactint = actint > actintCut;
        Bool_t isPassnoalg = passnoalgL1XE10 > passnoalgcut || passnoalgL1XE30 > passnoalgcut || passnoalgL1XE40 > passnoalgcut || passnoalgL1XE45 > passnoalgcut;
        Bool_t isPassrndm = passrndm > passrndmcut;


		if (( passl1 ) && ( passactint ) && ( isPassnoalg || isPassrndm ) )
		{
            if (algAMET > AlgAThresh){
                numberEventsAlgAKept++;
            }
            if (algBMET > AlgBThresh){
                numberEventsAlgBKept++;
            }
		}
	}


    if (verbose){
        std::cout << "number of events " << algAName << " kept at threshold: " << numberEventsAlgAKept << std::endl;
        std::cout << "number of events " << algBName << " kept at threshold: " << numberEventsAlgBKept << std::endl;
    }

    
    parameters->Set_AlgAIndividThresh( AlgAThresh );
    parameters->Set_AlgBIndividThresh( AlgBThresh );

    threshFileHandle->Close();
	return(0);
}

Float_t determineMuonEventsKeptCombined( const TString& algA, const Float_t threshA,
                                         const TString& algB, Float_t threshB,
                                         const TString& muonFileName)
{

    //this function determines process2 for muon events on combined alg


    //TODO: proof lite this
    const Float_t metL1Thresh = 50.0;
    //display algs and thresholds
    std::cout << "Determining fraction of muon events kept when using combined algorithm of " << algA << " at: " << threshA << ", "
    << algB << " at: " << threshB << " and metl1 at: " << metL1Thresh << std::endl;

    //get muon tree
    TString muonFilePath = "../myData/"+muonFileName;
    TFile * muonFile = TFile::Open(muonFilePath, "READ");
    TTree* muonTree = (TTree*)muonFile->Get("tree");
    Int_t muonNentries = muonTree->GetEntries();

    //initialize variables
    Float_t algAMET, algBMET,metl1, wValue;
    Int_t passmuon,passmuvarmed,recalBrokeFlag,cleanCutsFlag;
    Float_t metoffrecal,metoffrecalmuon,mexoffrecal,meyoffrecal,mexoffrecalmuon,meyoffrecalmuon,metrefmuon,mexrefmuon,meyrefmuon;
    Int_t numbPassedEvents = 0;
    Int_t numberMuonEvents = 0;


    muonTree->SetBranchAddress("passmu26med",&passmuon);
    muonTree->SetBranchAddress("passmu26varmed",&passmuvarmed);
    muonTree->SetBranchAddress("recalbroke",&recalBrokeFlag);
    muonTree->SetBranchAddress("passcleancuts",&cleanCutsFlag);
    muonTree->SetBranchAddress(algA,&algAMET);
    muonTree->SetBranchAddress("metl1",&metl1);
    muonTree->SetBranchAddress("metoffrecal", &metoffrecal);
    muonTree->SetBranchAddress("mexoffrecal", &mexoffrecal);
    muonTree->SetBranchAddress("meyoffrecal", &meyoffrecal);
    muonTree->SetBranchAddress("metoffrecalmuon", &metoffrecalmuon);
    muonTree->SetBranchAddress("mexoffrecalmuon", &mexoffrecalmuon);
    muonTree->SetBranchAddress("meyoffrecalmuon", &meyoffrecalmuon);
    muonTree->SetBranchAddress("metrefmuon", &metrefmuon);
    muonTree->SetBranchAddress("mexrefmuon", &mexrefmuon);
    muonTree->SetBranchAddress("meyrefmuon", &meyrefmuon);

    if ( algA != algB ) {//algs not the same, initialize second alg
         muonTree->SetBranchAddress( algB , &algBMET );
     }
    else {//algs are the same; set B to pass always
        threshB = threshA; algBMET = 0;
    }

    for (Int_t i  = 0 ; i < muonNentries ;i++)
      {
        muonTree->GetEntry(i);

        if (
               ((passmuon > 0.5) || (passmuvarmed > 0.5)) &&
               (cleanCutsFlag > 0.1 && recalBrokeFlag < 0.1) &&
               (passTransverseMassCut(metoffrecal ,mexoffrecal , meyoffrecal , metoffrecalmuon , mexoffrecalmuon , meyoffrecalmuon ))
           )
            {
              numberMuonEvents++;
            }
                if (((algAMET > threshA) && (algBMET > threshB) && (metl1 > metL1Thresh)))
                {
                    numbPassedEvents++;
                }
      }


    std::cout << "Number of muon events kept: " << numbPassedEvents << std::endl;
    Float_t frac = (Float_t) numbPassedEvents / (Float_t) numberMuonEvents;
    std::cout << "Fraction of muon events kept: " << frac << std::endl;

    muonFile->Close();


    return(numbPassedEvents);
}
