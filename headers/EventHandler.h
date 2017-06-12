// does not work with AliRoot CINT, must use ACLiC!!!

#include "TApplication.h"
#include "TPythia6.h"
#include "TClonesArray.h"
#include "TTimeStamp.h"
#include "TPythia6.h"
#include "UTKdefaults.h"


//Use UTK defaults for EventHandler() constructor
using namespace UTKdefaults;

//////////////////////////////////////////////////////////////////
// 
// EventHandler class
// 
// This is a TPythia6 wrapper class which sets creates a TPythia6
// object and sets a psuedrandom seed on class construction
//
// 
class EventHandler 
{
public:
	// Constructor
	// EventHandler( Int_t tune 		- Pythia tune to use for event generation
	//               std::string frame	- Pythia frame - e.g. "cms"
	//               std::string beam	- Pythia beam type - e.g. "p"
	//               std::string targ	- Pythia target type - e.g. "p"
	//               Int_t energy		- beam energy in MeV
	//               Double_t minPTHard	- Bias for Event generation - 0 = min bias)
	//				 Int_t seedmodifier - Int added to seed for batch jobs
	EventHandler(Int_t tune, std::string frame, std::string beam, std::string targ, Int_t energy, Double_t minPTHard, Int_t seedmodifier);
	
	EventHandler(Int_t seedmodifier);// constructor - use defaults
	EventHandler();// constructor - use defaults
	~EventHandler(); //destructor
	TClonesArray* GenerateEvent(); //get particles from pythia
		
private:
	TPythia6 *pythia;
	void SetSeed(Int_t);
	Int_t eTune;
	std::string eFrame;
	std::string eBeam;
	std::string eTarget;
	Int_t eEnergy;
	Double_t eMinPtCut;
	TClonesArray* particles;

};
	
void EventHandler::SetSeed(Int_t mod)
{
  TTimeStamp timestamp;
  UInt_t time = (timestamp.GetSec() - static_cast<long> (1446000000));
  UInt_t seed = (time + timestamp.GetNanoSec()+mod) % 900000000;
  if(seed<=900000000) 
  {
    pythia->SetMRPY(1, seed);                   // set seed
    pythia->SetMRPY(2, 0);                      // use new seed
  } 
  else {
    cout << "error: time " << seed << " is not valid" << endl;
    exit(2);
  }
}

/* Constructor:
 * EventHander(
 * 		Int_t 	 tune	- Pythia tune
 * 		string 	 frame 	- Pythia frame
 * 		string 	 beam 	- Pythia beam
 * 		string 	 targ 	- Pythia Target
 * 		Int_t 	 energy	- event energy (MeV)
 * 		Double_t minPT	- event minimumPT cut (i.e. bias, MeV)
 *      Int_t    seedmodifier - Int added to seed for batch jobs
 * 				);
 * *******************************/
EventHandler::EventHandler(Int_t tune, std::string frame, std::string beam, std::string targ, Int_t energy, Double_t minPTHard=PtHardCut, Int_t seedmodifier = 0):
	eTune(tune), eFrame(frame), eBeam(beam), eTarget(targ),
	eEnergy(energy), eMinPtCut(minPTHard)
{
	pythia = new TPythia6;
	SetSeed(seedmodifier);
	pythia -> SetMSTP(5, eTune);

	if (eMinPtCut > 0.0) {
		pythia -> SetCKIN(3,eMinPtCut);

	} 
	pythia -> Initialize(eFrame.c_str(),eBeam.c_str(),eTarget.c_str(),eEnergy);	
	
	particles = (TClonesArray*) pythia ->GetListOfParticles();
}

EventHandler::EventHandler(Int_t seedmodifier=0):
	eTune(Tune), eFrame(pyFrame), eBeam(pyBeam), eTarget(pyTarget),
	eEnergy(pyEnergy), eMinPtCut(PtHardCut)
{
	
	pythia = new TPythia6;
	SetSeed(seedmodifier);
	pythia -> SetMSTP(5, eTune);
	if (eMinPtCut > 0.0) {
		pythia -> SetCKIN(3,eMinPtCut);
	}
	pythia -> Initialize(eFrame.c_str(),eBeam.c_str(),eTarget.c_str(),eEnergy);	
	particles = (TClonesArray*) pythia ->GetListOfParticles();
}
EventHandler::EventHandler():
	eTune(Tune), eFrame(pyFrame), eBeam(pyBeam), eTarget(pyTarget),
	eEnergy(pyEnergy), eMinPtCut(PtHardCut)
{
	
	pythia = new TPythia6;
	
	SetSeed(0);
	pythia -> SetMSTP(5, eTune);
	if (eMinPtCut > 0.0) {
		pythia -> SetCKIN(3,eMinPtCut);
	}
	pythia -> Initialize(eFrame.c_str(),eBeam.c_str(),eTarget.c_str(),eEnergy);	
	particles = (TClonesArray*) pythia ->GetListOfParticles();
}

EventHandler::~EventHandler()
{
	delete pythia;
	delete particles;
}

//TClonesArray* EventHandler::GenerateEvent()
// Run a Pythia Event, pass TClonesArray* of particles
TClonesArray* EventHandler::GenerateEvent()
{
	pythia -> GenerateEvent();
	return particles;
}
