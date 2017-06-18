#include "TString.h"
#include "TROOT.h"

Int_t generateEfficiencies(Float_t metl1Thresh = 30.0, const Float_t frac = 1e-4, const TString& folderName = "", const TString& zeroBiasFileName = "ZeroBias2016R307195R311481Runs56.root", const TString& muonFilename = "PhysicsMain2016.Muons.noalgL1XE45R3073065R311481Runs9B.root")
{
    /*
    This program should just call threeefficiencies.c for all of the 15 combinations of algorithms
    */
    gROOT->ProcessLine(".L threeEfficiencies.C");
    TString algArray[6] = {"mettopoclem","metcell","metmht","mettopocl","mettopoclps","mettopoclpuc"};
    //TString algArray[6] = {"metl1","metcell","metmht","mettopocl","mettopoclps","mettopoclpuc"};
    for (Int_t i = 0 ; i < 6 ; i++)
    {
	for (Int_t j = i+1 ; j < 6 ; j++)
	{
	    gROOT->ProcessLine(".x threeEfficiencies.C(\"" + algArray[i] + "\",\"" + algArray[j] + "\"," + Form("%.7f",metl1Thresh) + "," + Form("%.7f",frac) + ",\""+zeroBiasFileName+"\",\""+muonFilename+"\")");
	}
    }

  return(0);

}