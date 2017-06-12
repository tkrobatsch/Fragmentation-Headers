#ifndef __CINT__
#include "TApplication.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TMCParticle.h"
#include "TParticle.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/Selector.hh"
#include "TRandom2.h"
#include "TTimeStamp.h"
#include "TF1.h"
#include "UTKdefaults.h"
#endif

//other git test

namespace UTKdefaults {

	// Default efficiency cut curve
	Double_t UTKeff(Double_t *pt,Double_t *z);
}



using namespace UTKdefaults;

//////////////////////////////////////////////////////////////////
// 
// ParticleHandler class
// 
// This class takes in an TClonesArray of TMCParticles from Pythia,
// culls particles which would not be detectable,
// loads the particles into fastjet-friendly PsuedoJets, and
// applies detector efficiency algorithm to detector level particles
//
// 
class ParticleHandler
{
public:
	// Constructor
	// ParticleHandler( TClonesArray particles	- Array of TMCParticles to process
	//                  Double_t maxeta			- maximum pseudorapidity allowed for particles, has a default
	//                  Double_t minPThadron	- Min threshold pt (GeV) for Hadron particles, has a default
	//                  Double_t minPTphoton	- Min threshold pt (GeV) for Gamma, e+ and e-), has a default
	//					TF1* efficiencyFunc		- Efficiency Function to apply to Hadrons
	ParticleHandler(TClonesArray* particles,Double_t maxeta, Double_t minPThadron, Double_t minPTphoton, TF1* efficiencyFunc);
	
	~ParticleHandler();
	vector<fastjet::PseudoJet> GetAllParticles();
	vector<fastjet::PseudoJet> GetDetectorParticles(bool ApplyEfficiency);
	static vector<fastjet::PseudoJet> BackgroundTMCPtoFJ(TClonesArray* particles,Double_t maxeta, Double_t minPThadron, Double_t minPTphoton);
	static Bool_t detectableParticle(Int_t KF);
	static Bool_t qualifyParticle(Int_t KF);

private:
	TClonesArray* phParticles;
	vector<fastjet::PseudoJet> *particlesAll;
	vector<fastjet::PseudoJet> *particlesDetector;
	vector<Double_t> detectorPT;
	vector<Int_t> KFcodeDet;
	TRandom2 *rng;
	TF1 *Efficiency;
	bool newTF1;

};

ParticleHandler::ParticleHandler(TClonesArray* particles,Double_t maxeta = maximum_eta, Double_t minPThadron = MinHadrPt, Double_t minPTphoton=MinPhotPt,TF1* efficiencyFunc = nullptr) :
	phParticles(particles), Efficiency(efficiencyFunc)
{
	if (Efficiency == nullptr){
		Efficiency = new TF1("Eff",UTKeff,0.3,80,1);
		newTF1 = true;
	}
	else newTF1 = false;
	
	
	Int_t nPart = phParticles->GetEntries();
	TTimeStamp timestamp;
	
	UInt_t time = (timestamp.GetSec() - static_cast<long> (1446000000));
	UInt_t seed = (time + timestamp.GetNanoSec()) % 900000000;
	//delete timestamp;
	rng = new TRandom2(seed);
	Double_t pt, p, eta, phi;
	TMCParticle *MPart;
	particlesAll = new vector<fastjet::PseudoJet>;
	particlesDetector = new vector<fastjet::PseudoJet>;
	Int_t KF;
	Float_t DCA_precalc = TMath::Power(PrimaryMaxDCA * (1000.0/1000000.0),2.0);
	for (Int_t part=0; part<nPart; part++) {
      MPart = (TMCParticle *) particles->At(part); //indexing the particles array for the current member in loop
      KF =MPart->GetKF();
	  Float_t Vx = MPart -> GetVx(); //x coordinate of particle vertex in mm
	  Float_t Vy = MPart -> GetVy(); //y coordinate of particle vertex in mm
	  Float_t Vz = MPart -> GetVz(); //z coordinate of particle vertex in mm
	  
	  Float_t DCA_calc = (Vx*Vx) + (Vy*Vy) + (Vz*Vz); //answer in microns  
		
	  if (qualifyParticle(KF) && DCA_calc < DCA_precalc) {
		
		pt =  TMath::Sqrt(MPart->GetPx() * MPart->GetPx() + MPart->GetPy() * MPart->GetPy());
		p =  TMath::Sqrt(MPart->GetPx() * MPart->GetPx() + MPart->GetPy() * MPart->GetPy() +  MPart->GetPz() * MPart->GetPz()); 
		eta = 0.5*TMath::Log((p+MPart->GetPz())/(p-MPart->GetPz()));
		phi = TMath::Pi()+TMath::ATan2(-MPart->GetPy(),-MPart->GetPx()); //answer in radians
		bool ptOK = false;
		if(TMath::Abs(eta) < maxeta){
			switch (KF) {
				case GAMMA:
				case EPLUS:
				case EMINUS:
					if (pt > minPTphoton) ptOK = true;
					break;
				default:
					if (pt > minPThadron) ptOK = true;
			}
			if (ptOK){
				
				particlesAll->push_back(fastjet::PseudoJet( MPart->GetPx(), MPart->GetPy() , MPart->GetPz() , MPart->GetEnergy())); //storing into fastjet
				//particlesAll->back().set_user_index(part);
				if (detectableParticle(KF)) {
					particlesDetector->push_back(fastjet::PseudoJet( MPart->GetPx(), MPart->GetPy() , MPart->GetPz() , MPart->GetEnergy() ) ); //storing into fastjet
					particlesDetector->back().set_user_index(part|DETECTABLE);
					particlesAll->back().set_user_index(part|DETECTABLE);
					detectorPT.push_back(pt);
					KFcodeDet.push_back(KF);
				}
				else particlesAll->back().set_user_index(part);
			}
				
		}
	  }
    }
}




ParticleHandler::~ParticleHandler()
{
	delete particlesAll;
	delete particlesDetector;
	delete rng;
	
	if (newTF1) delete Efficiency;
}
	
vector<fastjet::PseudoJet> ParticleHandler::GetAllParticles()
{
	return *particlesAll;
}

vector<fastjet::PseudoJet> ParticleHandler::GetDetectorParticles(bool ApplyEfficiency)
{
	if (ApplyEfficiency) {
		
		Double_t pt;
		vector<fastjet::PseudoJet> culled;
		Int_t numPart = particlesDetector->size();
		Int_t KFtmp;
		for (Int_t n = 0; n<numPart;n++){
			KFtmp = KFcodeDet[n];
			if (KFtmp!=EMINUS && KFtmp!=EPLUS && KFtmp!=GAMMA)
			{
				pt = detectorPT[n];
				Float_t odds = Efficiency->Eval(pt);
				if (rng->Binomial(1,odds)==1){
					culled.push_back((*particlesDetector)[n]);
				}
			
			}
			else (culled.push_back((*particlesDetector)[n]));
			
		}
		
		return culled;
	}
	else return *particlesDetector;
}



Bool_t ParticleHandler::detectableParticle(Int_t KF) {
  switch (KF) {
    case PIPLUS:
    case PIMINUS:
    case KPLUS:
    case KMINUS:
    case PPLUS:
    case PMINUS:
    case GAMMA:
    case ETA:
      return true;
    default:
      return false;
  }
}
Bool_t ParticleHandler::qualifyParticle(Int_t KF) {
  switch (KF) {
    case PIPLUS:
    case PIMINUS:
    case PIZERO:
    case KPLUS:
    case KMINUS:
    case KZEROSHORT:
    case KZEROLONG:
    case PPLUS:
    case PMINUS:
    case LAMBDAZERO:
    case ANTILAMBDAZERO:
    case GAMMA:
    case ETA:
    case NEUTRON:
	case NUE:
	case NUMU:
	case NUTAU:
	case ANTINUE:
	case ANTINUMU:
	case ANTINUTAU:
      return true;
    default:
      return false;
  }
}

Double_t UTKdefaults::UTKeff(Double_t *pt,Double_t *z) 
{
	return (7.0/9.0)*2.26545*TMath::Exp(-TMath::Power(9.99977e-01/pt[0],7.85488e-02));
}//efficiency	

////////////////////////////////
// For Future Usage
vector<fastjet::PseudoJet> BackgroundTMCPtoFJ(TClonesArray* particles,Double_t maxeta, Double_t minPThadron, Double_t minPTphoton){
		vector<fastjet::PseudoJet> bgParticles;
		return bgParticles;
}