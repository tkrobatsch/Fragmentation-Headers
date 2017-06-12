#ifndef __CINT__
#include "TApplication.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TMCParticle.h"
#include "TParticle.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/Selector.hh"
#include "UTKdefaults.h"
using namespace std;
#endif

// Set Default Fastjet Parameters

using namespace UTKdefaults;


class FastjetHandler
{
public:
	// Constructor
	// FastjetHandler( Strategy        	   strat  - e.g. fastjet::Best
	//                 RecombinationScheme scheme - e.g. fastjet::BIpt_scheme
	//                 JetAlgorthm	       algo	  - KT, anti-kt, SIScone, etc
	//                 Double_t 		   minGeV - Minimum PT to qualify as a jet in fj
	//                 Double_t 		   maxGeV - Maximum PT to qualify as a jet in fj
	//                 Double_t 		   maxEta - Maximum psuedorapidity allowed for jet.
	//                 Double_t 		   jetRadius - radius for FJ algorithm )
	FastjetHandler(fastjet::Strategy strat, fastjet::RecombinationScheme scheme,
					fastjet::JetAlgorithm algo, Double_t minGeV, Double_t maxGeV,
					Double_t maxEta, Double_t jetRadius); //constructor
	// Constructor
	// FastjetHandler() Uses defaults set above
	FastjetHandler();
	~FastjetHandler(); // deconstructor
	
	// Run FJ clustering
	vector<fastjet::PseudoJet> Cluster(const vector<fastjet::PseudoJet>&);
	
	// This NEEDS To be public to prevent seg faults for checking jet constituents
	fastjet::ClusterSequence *fjClusterSeq;
private:
	fastjet::Strategy fjStrategy;
	fastjet::RecombinationScheme fjScheme;
	fastjet::JetAlgorithm fjAlgorithm;
	Double_t fjMinJetPT;
	Double_t fjMaxJetPT;
	Double_t fjJetRadius;
	Double_t fjMaxEta;
	
	fastjet::Selector fjSelector;
	fastjet::JetDefinition *fjJetDef;
	
};

// Constructor
// FastjetHandler( Strategy        	   strat  - e.g. fastjet::Best
//                 RecombinationScheme scheme - e.g. fastjet::BIpt_scheme
//                 JetAlgorthm	       algo	  - KT, anti-kt, SIScone, etc
//                 Double_t 		   minGeV - Minimum PT to qualify as a jet in fj
//                 Double_t 		   maxGeV - Maximum PT to qualify as a jet in fj
//                 Double_t 		   maxEta - Maximum psuedorapidity allowed for jet.
//											    algo will adjust this down by jetRadius
//                 Double_t 		   jetRadius - radius for FJ algorithm )
FastjetHandler::FastjetHandler(fastjet::Strategy strat,fastjet::RecombinationScheme scheme,
					fastjet::JetAlgorithm algo,Double_t minGeV,
					Double_t maxGeV, Double_t maxEta, Double_t jetRadius):
	fjStrategy(strat),fjScheme(scheme),fjAlgorithm(algo),
	fjMinJetPT(minGeV),fjMaxJetPT(maxGeV),fjJetRadius(jetRadius),fjMaxEta(maxEta)
{
	fjJetDef = new fastjet::JetDefinition(fjAlgorithm,fjJetRadius,fjScheme,fjStrategy);
	fastjet::Selector selectEta = fastjet::SelectorEtaRange(-fjMaxEta,fjMaxEta);
	fastjet::Selector selectPT = fastjet::SelectorPtRange(fjMinJetPT,fjMaxJetPT);
	fjSelector = selectEta && selectPT;
	fjClusterSeq = nullptr;
}


// Constructor - uses defaults
FastjetHandler::FastjetHandler():
	fjStrategy(strategy),fjScheme(recombScheme),fjAlgorithm(algorithm),
	fjMinJetPT(MinJetGeV),fjMaxJetPT(MaxJetPt),fjJetRadius(Jet_Radius),fjMaxEta(MaxEta)
{
	fjJetDef = new fastjet::JetDefinition(fastjet::antikt_algorithm,fjJetRadius,fjScheme,fjStrategy);
	fastjet::Selector selectEta = fastjet::SelectorEtaRange(-fjMaxEta+fjJetRadius,fjMaxEta-fjJetRadius);
	fastjet::Selector selectPT = fastjet::SelectorPtRange(fjMinJetPT,fjMaxJetPT);
	fjSelector = selectEta && selectPT;
	
	
}

// FastjetHandler::Cluster(vector<fastjet::PseudoJet> &fjInputs)
// runs fastjet cluster with parameters set on construction
vector<fastjet::PseudoJet> FastjetHandler::Cluster(const vector<fastjet::PseudoJet> &fjInputs)
{
	vector<fastjet::PseudoJet> InclusiveJets;
	if (fjClusterSeq != nullptr) delete fjClusterSeq;
	fjClusterSeq = new fastjet::ClusterSequence(fjInputs,*fjJetDef);
	InclusiveJets = sorted_by_pt(fjClusterSeq->inclusive_jets(fjMinJetPT));
	
	vector<fastjet::PseudoJet> SelectedJets;
	SelectedJets = fjSelector(InclusiveJets);
	return SelectedJets;
}


FastjetHandler::~FastjetHandler()
{
	delete fjJetDef;
	delete fjClusterSeq;
}

