#include "mincerMacros.h"


TFile* threeEfficiencies( const TString& AlgAName , const TString& AlgBName ) 
{

    //GLOBAL DEFINITION 
    userInfo* parameters = new userInfo();
    parameters->Read_Parameter_File("./parameters.txt");

    parameters->Print();

    //FILES
    TString zerobiasFileName = parameters->Get_ThreshFileName();
    TString muonFilename = parameters->Get_MuonFileName();

    //THREEFF BENCHMARK
    TBenchmark* threeEfficienciesBenchmark = new TBenchmark();

    //TOTAL TIMER
    threeEfficienciesBenchmark->Start("Three Efficiencies");

    //MUON FILE; MUON TREE
    TString muonFilePath = "../myData/"+muonFilename;
    TFile * muonFile = TFile::Open(muonFilePath, "READ");
    TTree* myMuonTree = (TTree*)muonFile->Get("tree");
    Int_t muonNentries = myMuonTree->GetEntries();
    parameters->Set_MuonNentries( muonNentries );

    //ZBTREE
    std::cout << "MuonNentries: " << muonNentries << "\n" << std::endl;
    TString zerobiasFilePath = "../myData/"+ zerobiasFileName;
    TFile * zeroBiasFile = TFile::Open(zerobiasFilePath, "READ");
    TTree* zeroBiasTree = (TTree*)zeroBiasFile->Get("tree");
    Int_t zerobiasNentries = zeroBiasTree->GetEntries();
    parameters->Set_PassnoalgNentries( zerobiasNentries );


    Float_t passnoalgcut = parameters->Get_Passnoalgcut();
    Float_t passrndmcut = parameters->Get_Passrndmcut();

    TString xlabel = "MET [GeV]";
    TString yaxis = "Events";



    //initialize variables to be used later
    Float_t metl1thresh = parameters->Get_MetL1Thresh();
    Float_t actintCut = parameters->Get_ActintCut();
    Int_t muonNbins = 200;
    Int_t nbins = parameters->Get_Nbins();
    Double_t metMin = parameters->Get_MetMin();
    Double_t metMax = parameters->Get_MetMax();
    Int_t NumbPassnoAlgPassProcess1WithActintCut = 0; Int_t counter1 = 0; Int_t counter2 = 0; Int_t counter3 = 0;
    Int_t passrndm, numPassMuon,passmuon,passmuvarmed,cleanCutsFlag,recalBrokeFlag;
    Float_t algAMET,algBMET,metoffrecal,mexoffrecal,meyoffrecal,mexoffrecalmuon, zb_actint,
            meyoffrecalmuon, metl1,metcell,metrefmuon,mexrefmuon,meyrefmuon,metoffrecalmuon;
    Int_t passnoalgL1XE10,passnoalgL1XE30,passnoalgL1XE40,passnoalgL1XE45;
    Float_t algAMETx1thresh,algBMETx1thresh;
    Float_t algAMETx2thresh,algBMETx2thresh;


    //ZB BRANCHES
    zeroBiasTree->SetBranchAddress("passrndm", &passrndm);
    zeroBiasTree->SetBranchAddress(AlgAName,&algAMET);
    zeroBiasTree->SetBranchAddress(AlgBName,&algBMET);
    zeroBiasTree->SetBranchAddress("metl1",&metl1);
    zeroBiasTree->SetBranchAddress("passnoalgL1XE10",&passnoalgL1XE10);
    zeroBiasTree->SetBranchAddress("passnoalgL1XE30",&passnoalgL1XE30);
    zeroBiasTree->SetBranchAddress("passnoalgL1XE40",&passnoalgL1XE40);
    zeroBiasTree->SetBranchAddress("passnoalgL1XE45",&passnoalgL1XE45);
    zeroBiasTree->SetBranchAddress("actint",&zb_actint);
    //MUON BRANCHES
    myMuonTree->SetBranchAddress("passmu26med", &passmuon);
    myMuonTree->SetBranchAddress("passmu26varmed", &passmuvarmed);
    myMuonTree->SetBranchAddress("passcleancuts", &cleanCutsFlag);
    myMuonTree->SetBranchAddress("recalbroke", &recalBrokeFlag);
    myMuonTree->SetBranchAddress("metoffrecal", &metoffrecal);
    myMuonTree->SetBranchAddress("mexoffrecal", &mexoffrecal);
    myMuonTree->SetBranchAddress("meyoffrecal", &meyoffrecal);
    myMuonTree->SetBranchAddress("metoffrecalmuon", &metoffrecalmuon);
    myMuonTree->SetBranchAddress("mexoffrecalmuon", &mexoffrecalmuon);
    myMuonTree->SetBranchAddress("meyoffrecalmuon", &meyoffrecalmuon);
    myMuonTree->SetBranchAddress("metrefmuon", &metrefmuon);
    myMuonTree->SetBranchAddress("mexrefmuon", &mexrefmuon);
    myMuonTree->SetBranchAddress("meyrefmuon", &meyrefmuon);

    //ZB INDIVID HISTS
    TH1F *algAMETHist = new TH1F(AlgAName, "algA", nbins, metMin, metMax);
    TH1F *algBMETHist = new TH1F(AlgBName, "algB", nbins, metMin, metMax);

    //start timer on zerobias determination of thresholds
    threeEfficienciesBenchmark->Start("ZeroBias Thresholds");

    //IN DETERMINE THRESH I COMPUTE THRESHOLD AFTER ALSO CUTTING ON METL1 TO MAKE HISTOGRAMS
    Int_t numPassnoalgPassProcess1AlgA = 0;
     
    determineZeroBiasThresh( parameters );

    const Float_t AlgAIndividThresh = parameters->Get_IndividAlgAThresh(); 
    const Float_t AlgBIndividThresh = parameters->Get_IndividAlgBThresh(); 


    std::cout << "Returned to threeEfficiencies.C" << std::endl;
    std::cout << "AlgAThresh: " << AlgAIndividThresh << std::endl;
    std::cout << "AlgBThresh: " << AlgBIndividThresh << std::endl;
    std::cout << "Using METL1THRESH: " << metl1thresh << std::endl;

    NumbPassnoAlgPassProcess1WithActintCut = 0 ;
	for (Int_t k = 0; k < zerobiasNentries; k++)
	{
	    zeroBiasTree->GetEntry(k);
        Bool_t isL1 = metl1 > metl1thresh;
        Bool_t isactint = zb_actint > actintCut;
        Bool_t isPassnoalg = passnoalgL1XE10 > passnoalgcut || passnoalgL1XE30 > passnoalgcut ||
        passnoalgL1XE40 > passnoalgcut || passnoalgL1XE45 > passnoalgcut;
        Bool_t isPassrndm = passrndm > passrndmcut;

	    if ( ( isL1 ) && ( isactint ) && (isPassnoalg || isPassrndm)) 
        {
    		algAMETHist->Fill(algAMET);
    		algBMETHist->Fill(algBMET);
            NumbPassnoAlgPassProcess1WithActintCut++;
	    }
	}


    //the individual fraction needed such that when both algs constrained to keep the same fraction individually,
    //keep the proper amount when combined

    Float_t bisectionIndividFrac;

    //start bisection timer
    threeEfficienciesBenchmark->Start("Bisection");

    //run BISECTION
    bisection( parameters , algAMETHist, algBMETHist , zeroBiasTree );
    
    const Float_t CombinedThreshAlgA = parameters->Get_CombinedAlgAThresh();
    const Float_t CombinedThreshAlgB = parameters->Get_CombinedAlgBThresh();

    //end bisection timer
    threeEfficienciesBenchmark->Show("Bisection");

    //END ZEROBIAS TIMER
    threeEfficienciesBenchmark->Show("ZeroBias Thresholds");


    TString astring = AlgAName + " > " + Form(" %.2f", AlgAIndividThresh );
    TString bstring = AlgBName + " > " + Form(" %.2f", AlgBIndividThresh );
    TString cstring = AlgAName+ " > " + Form(" %.2f", CombinedThreshAlgA) + " and " + AlgBName + " > " + Form(" %.2f", CombinedThreshAlgB);
    TString dstring = (TString) "L1 > " + Form(" %.2f" , metl1thresh);

    TEfficiency* Ateff  = new TEfficiency(astring , "Efficiency", muonNbins, metMin, metMax);
    TEfficiency* Bteff  = new TEfficiency(bstring , "Efficiency", muonNbins, metMin, metMax);
    TEfficiency* Cteff  = new TEfficiency(cstring,  "Efficiency", muonNbins, metMin, metMax);
    TEfficiency* Dteff  = new TEfficiency(dstring,  "Efficiency", muonNbins, metMin, metMax);//combined just L1 cut, 0 on others

    threeEfficienciesBenchmark->Start("Fill TEfficiencies");

    Float_t algAmuonMET = 0;
    Float_t algBmuonMET = 0;
    Float_t muonMetl1 = 0;
    Float_t muonActint = 0;
    myMuonTree->SetBranchAddress(AlgAName,&algAmuonMET);
    myMuonTree->SetBranchAddress(AlgBName,&algBmuonMET);
    myMuonTree->SetBranchAddress("metl1",&muonMetl1);
    myMuonTree->SetBranchAddress("actint", &muonActint);

    std::cout << "Starting to fill TEfficiencies.." << std::endl;

    Int_t NumMuonPassProcess1WithActintCut = 0 ;

    Bool_t isMuon;
    Bool_t isClean;
    for (Int_t l = 0 ; l < muonNentries ; l++)
    {
        myMuonTree->GetEntry(l);
        isMuon = (passmuvarmed > 0.1 || passmuon > 0.1);
        isClean = (cleanCutsFlag > 0.1) && (recalBrokeFlag < 0.1);

        if ( isMuon && isClean && muonMetl1 > metl1thresh)
        {
            NumMuonPassProcess1WithActintCut++;
        }


        if ( isMuon && isClean && ( muonActint > actintCut ))
    	{
            if ( passTransverseMassCut(metoffrecal,mexoffrecal,meyoffrecal,metoffrecalmuon,mexoffrecalmuon,meyoffrecalmuon) )
            {
        	    Float_t metnomu = computeMetNoMu(  mexoffrecal , meyoffrecal , mexoffrecalmuon , meyoffrecalmuon );

                Ateff->Fill((algAmuonMET > AlgAIndividThresh ) && (muonMetl1 > metl1thresh) && ( muonActint > actintCut ), metnomu);
        	    Bteff->Fill((algBmuonMET > AlgBIndividThresh ) && (muonMetl1 > metl1thresh)&& ( muonActint > actintCut ), metnomu);
        	    Cteff->Fill(((algAmuonMET > CombinedThreshAlgA) && (algBmuonMET > CombinedThreshAlgB)
                && ( muonActint > actintCut )&& (muonMetl1 > metl1thresh)), metnomu);
                Dteff->Fill((muonMetl1 >= metl1thresh) && ( muonActint > actintCut ), metnomu);
            }
    	}
    }

    threeEfficienciesBenchmark->Show("Fill TEfficiencies");

    const TString canvName = AlgAName + " and " + AlgBName + " Combined Efficiency" + ";Offline Recalibrated MET w/o Muon term [GeV];Efficiency";

    TCanvas* efficiencyCanvas = new TCanvas("Efficiency Canvas", "Efficiency Canvas");

    efficiencyCanvas->RangeAxis(0,0,500,1.0);

    efficiencyCanvas->SetTitle(canvName);

    Ateff->SetLineColor(kBlue);
    Cteff->SetLineColor(kRed);
    Bteff->SetLineColor(kGreen);
    Dteff->SetLineColor(kBlack);

    Ateff->Draw();
    Bteff->Draw("same");
    Cteff->Draw("same");
    Dteff->Draw("same");

    TLegend *legend = new TLegend(0.57,0.15,0.9, 0.4 ,"","NDC");
    legend->AddEntry(Ateff, astring);
    legend->AddEntry(Bteff, bstring);
    legend->AddEntry(Cteff, cstring);
    legend->AddEntry(Dteff, dstring);
    legend->Draw();

    //compute number muon events actually kept using external macro
    Int_t muonEventsCombined = determineMuonEventsKeptCombined( AlgAName , CombinedThreshAlgA , AlgBName , CombinedThreshAlgB , muonFilename );

    parameters->Set_NumPassNoAlgPassProcess1( NumbPassnoAlgPassProcess1WithActintCut );
    parameters->Set_NumMuonPassProcess1( NumMuonPassProcess1WithActintCut );

    Int_t NumberMuonEventsProcess2CombinedActintCut = (Cteff->GetPassedHistogram())->GetEntries() ;
    Int_t NumberMuonEventsProcess2AlgAActintCut = (Ateff->GetPassedHistogram())->GetEntries() ;
    Int_t NumberMuonEventsProcess2AlgBActintCut = (Bteff->GetPassedHistogram())->GetEntries() ;

    parameters->Set_NumMuonPassProcess2Combined( NumberMuonEventsProcess2CombinedActintCut);
    parameters->Set_NumMuonPassProcess2AlgA( NumberMuonEventsProcess2AlgAActintCut);
    parameters->Set_NumMuonPassProcess2AlgB( NumberMuonEventsProcess2AlgBActintCut);

    parameters->Set_NumPassNoAlgPassProcess2Combined( NumberMuonEventsProcess2CombinedActintCut);
    //TODO: pass numPassnoalgPassProcess2AlgA from determinezerobiasthresh to set it here
    //parameters->setNumPassNoAlgPassProcess2AlgA( numPassnoalgPassProcess2AlgA);
    //parameters->setNumPassNoAlgPassProcess2AlgB( numPassnoalgPassProcess2AlgB);


    parameters->Set_NumMuonKeptCombinedAtThresh( muonEventsCombined );
    parameters->Set_NumTotal((Ateff->GetTotalHistogram())->GetEntries());
    parameters->Set_ActintCut(actintCut);
    parameters->Set_AlgAName(AlgAName);
    parameters->Set_AlgBName(AlgBName);

    TString fileName = "./RootFiles/" + AlgAName + "_" + AlgBName + "Efficiencies.root";
    //if file already exists, not opened
    TFile* rootFile = new TFile(fileName,"CREATE");

    //TODO: add filehandling to use one root file for a given combination, and have it create subfolders within the root file to
    //store different runs
    if ( !( rootFile->IsOpen() ) )
        {
            TString suffix = "";
            std::cout << "Unable to open file" << std::endl;
            std::cout << "Enter run number: ";
            std::cin >> suffix;
            suffix = "(" + suffix + ")";
            fileName.Insert( (fileName.Length()-5),suffix);
            //try again, but this time if user inputs, can overwrite
            rootFile->Open( fileName ,"RECREATE");
        } 
    std::cout << "Root file successfully opened" << std::endl;

    efficiencyCanvas->Print("./Pictures/" + AlgAName + "_" + AlgBName + "_efficiencies.tiff");
    Ateff->Write( AlgAName + " Efficiency" );
    Bteff->Write( bstring );
    Cteff->Write( cstring );
    Dteff->Write( "METL1" );
    efficiencyCanvas->Write("efficiencyCanvas");
    
    parameters->Print();
    parameters->Write("parameters");
  
    Float_t realtime, cputime;
    threeEfficienciesBenchmark->Summary(realtime ,cputime);

    rootFile->Close();

    return( rootFile );
    }
