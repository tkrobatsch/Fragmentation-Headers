// Author: James Courtland Neuhaus <jneuhau1@vols.utk.edu>
// Update: 2017-05-31
// Copyright: 2017 (C) James Courtland Neuhaus
// For the licensing terms see $ROOTSYS/LICENSE.
//
//////////////////////////////////////////////////////////////
//
//  Default settings for use in the UTKdefaults namespace
//

#ifndef __CINT__
#include "TApplication.h"
#include "TMath.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/Selector.hh"
#endif

#ifndef __UTKDEFAULTS__
#define __UTKDEFAULTS__
namespace UTKdefaults {
	
	////////////////////////////////////////////////
	// DEFAULT JET MATCHING SETTINGS 
	// implemented in EventHandler.h
	const Float_t matchR = 0.2; // Maximum allowable dR offset for a JetMatch,  3.22 makes irrelevant
	
	////////////////////////////////////////////////
	// DEFAULT PYTHIA SETTINGS 
	// implemented in EventHandler.h
	const Int_t			Tune		= 100;
	const std::string	pyFrame		= "cms"; // "cms" for colliding beam exeriment
	const std::string	pyBeam		= "p"; // beam type
	const std::string	pyTarget	= "p"; // target type
	const Int_t			pyEnergy	= 2760; // for "cms" represents the total energy of the system in GeV 
	const Double_t		PtHardCut	= 00.0; // Pt Threshold for pythia primary particles 

	////////////////////////////////////////////////	
	// DEFAULT FASTJET SETTINGS 
	// implemented in FastJetHandler.h
	const fastjet::Strategy             strategy     = fastjet::Best;
	const fastjet::RecombinationScheme  recombScheme = fastjet::BIpt_scheme;
	const fastjet::JetAlgorithm 		algorithm    = fastjet::antikt_algorithm;	
	const Double_t 						MinJetGeV    = 5.0; 
	const Double_t						Jet_Radius   = 0.2;
	const Double_t						MaxJetPt     = 10000.0;
	const Double_t						MinPartPt	 = 0.15;
	const Double_t						MinPhotonPt	 = 0.3;
	const Double_t						MaxEta		 = 0.7;

	////////////////////////////////////////////////	
	// DEFAULT PARTICLE QUALIFYING SETTINGS 
	// implemented in ParticleHandler.h
	const Double_t MinPhotPt	 = 0.3;
	const Double_t MinHadrPt	 = 0.15;  //Minimum pt in the EMCAL in SGeV/c (c = 1)
	const Double_t maximum_eta	 = 0.7;   //Maximum Psuedorapidity
	const Double_t PrimaryMaxDCA = 100.0; //Microns, cut all particles of DCA greater than this

	////////////////////////////////////////////////	
	// Particle KF Codes
	// http://pdg.lbl.gov/2007/reviews/montecarlorpp.pdf
	
	const Int_t PIPLUS = 211;
	const Int_t PIMINUS = -211;
	const Int_t PIZERO =111;
	const Int_t KPLUS = 321;
	const Int_t KMINUS = -321;
	const Int_t KZEROSHORT = 310;
	const Int_t KZEROLONG = 130;
	const Int_t PPLUS = 2212;
	const Int_t PMINUS = -2212;
	const Int_t LAMBDAZERO = 3122;
	const Int_t ANTILAMBDAZERO = -3122;
	const Int_t GAMMA = 22;
	const Int_t ETA = 221;
	const Int_t NEUTRON = 2112;
	const Int_t EPLUS = 11;
	const Int_t EMINUS = -11;
	const Int_t NUE = 12;
	const Int_t NUMU = 14;
	const Int_t NUTAU = 16;
	const Int_t ANTINUE = -12;
	const Int_t ANTINUMU = -14;
	const Int_t ANTINUTAU = -16;
	
	////////////////////////////////////////////////	
	// PseudoJet user_index flags
	// use bitwise and or bitwise xor to utilize
	const Int_t DETECTABLE = 0x10000000; 
	const Int_t BACKGROUND = 0x20000000;
	const Int_t ANTIPART   = 0x40000000;

} // namespace UTKdefaults


#endif
