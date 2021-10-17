// $Id: NornDynasty.cpp,v 1.18 2001/04/24 15:01:36 firving Exp $

// TODO: Tests of User text and name changing

#include "../../../c2e/engine/UniqueIdentifier.h"
#include "NornDynasty.h"

#include <exception>
#include <sstream>
#include <fstream>

#ifdef _WIN32
#include <time.h>
#endif

// External values - these can be changed from outside
// as they are public
int NornDynasty::ourLifeStageLength = 10000; 
float NornDynasty::ourBreedGoes = 0.05f;
int NornDynasty::ourChanceOfSplice = 250;
float NornDynasty::ourDeathGoes = 0.0001f;
float NornDynasty::ourCloneGoes = 0.00005f;
float NornDynasty::ourExportGoes = 0.0001f;
float NornDynasty::ourImportGoes = 0.0002f;
int NornDynasty::ourChanceDuplicateEvent = 10;

// Internal values
int NornDynasty::ourRealTime = 10000;
int NornDynasty::ourEventCount = 0;
int NornDynasty::ourPopCheck = 0;
int NornDynasty::ourMotherDiedWhileInWomb = 0;
std::map<std::string, NornDynasty::Norn*> NornDynasty::ourMonikerToNorn;
bool NornDynasty::ourInitialised = false;
std::vector<NornDynasty::World*> NornDynasty::ourWorlds;
std::vector<NornDynasty::Norn*> NornDynasty::ourNorns;
CBabelCriticalSection NornDynasty::ourCritical;

NornDynasty::Exception::Exception(const std::string& what)
{
	myWhat = what;
}

const char* NornDynasty::Exception::what()
{
	return myWhat.c_str();
}

NornDynasty::World::World(OurFeed* feed, const std::string& user)
{
	myFeed = feed;

	// Construct a randomly made word
	myUID = GenerateUniqueIdentifier("", "");
	const char* WorldNames[] = { "Flubble's world", "Bibble's world", "Space odyssey", "Snuffleville", "Medtown",
		"Home of the dancing Shee", "Land of Mel", "Jurtain show room", "Hogwarts", "Azakaban" };
	myName = WorldNames[rand() % 10];
	myUser = user;

	// Start counters
	myWorldTick = 0;
	myJustTickedBy = 0;
}

NornDynasty::Norn::Norn(World* world, int genus,
		std::string parent1Moniker, std::string parent2Moniker)
	: myFeed(world->myFeed)
{
	ourPopCheck++;

	myWorld = world;
	myAgeInTicks = 0;
	myLifeStage = 0;
	myState = StateConceived;
	myPregnant = NULL;
	myMumMoniker = "";
	myDadMoniker = "";

	myEventCount = 0;

	if (genus == 0)
	{
		if (rand() % 6 != 0)
			genus = 0; // Norn
		else
			genus = rand() % 3 + 1; // Maybe not just Norns ;-)
	}

	myMoniker = TryGenerateUniqueMoniker(parent1Moniker, parent2Moniker, genus);
	::NornDynasty::ourMonikerToNorn[myMoniker] = this;





	// Fill in history with appropriate random files
	myHistory.myMoniker = myMoniker;
	myHistory.myGender = rand() % 2 + 1;

	// Choose suitable values for history members
	// (SPEED: This should probably be cached if we need to)
	std::string nameFile = (myHistory.myGender == 1 ? "boysnames" : "girlsnames");
	std::ifstream in(nameFile.c_str());
	if (!in.good())
		throw Exception("Couldn't find/open file " + nameFile);
	std::vector<std::string> names;
	while (in.good())
	{
		std::string name;
		std::getline(in, name);
		names.push_back(name);
	}
	myHistory.myName = names[rand() % names.size()];

	// Test swear word filter
	if (rand() % 1000 == 1)
		myHistory.myName = "fucking shitter";

	if (myHistory.myName.empty())
		myHistory.myName = "Muggles";


	myHistory.myGenus = genus;
	myHistory.myVariant = rand() % 8;
	myHistory.myCrossoverMutationCount = rand() % 10;
	myHistory.myCrossoverCrossCount = rand() % 20;


	if (!myFeed->AddCreatureToDatabase(myHistory))
		throw Exception("Failed to add new Norn to database");



	if (rand() % ourChanceDuplicateEvent == 0)
	{
		if (!myFeed->AddCreatureToDatabase(myHistory))
			throw Exception("Failed with duplicate event Norn adding test");
	}

	if (!myFeed->ChangeName(myHistory.myName, myMoniker))
		throw Exception("Failed to change name in new Norn creation");
}



void NornDynasty::Norn::Event(::LifeEvent::EventType type, const std::string& moniker1, const std::string& moniker2)
{
	StaticEvent(myFeed, myWorld, myMoniker, type, moniker1, moniker2);
}

// static
void NornDynasty::Norn::StaticEvent(OurFeed* feed, World* world, const std::string& monikerTarget, ::LifeEvent::EventType type, const std::string& moniker1, const std::string& moniker2)
{
	if (world == NULL)
		throw Exception("Expected a world for an event");

	Norn* norn = ::NornDynasty::ourMonikerToNorn[monikerTarget];

	::LifeEvent event;

	event.myEventType = type;
	event.myWorldTick = world->myWorldTick;
	event.myAgeInTicks = norn ? norn->myAgeInTicks : -1;
	event.myRealWorldTime = ourRealTime;
	event.myLifeStage = norn ? norn->myLifeStage : -1;

	event.myUserText = "";
	event.myPhoto = "";

	event.myRelatedMoniker1 = moniker1;
	event.myRelatedMoniker2 = moniker2;

	event.myWorldName = world->myName;
	event.myWorldUniqueIdentifier = world->myUID;
	event.myNetworkUser = world->myUser;

	int solidIndex = -10;
	if (event.CreaturePresentForEvent())
		solidIndex = ourMonikerToNorn[monikerTarget]->myEventCount++;
	// we still feed in events that we should to test that
	// the feed copes with it well (-10 for solid index in the
	// database would show that this isn't working).

	if (!feed->AddLifeEventToDatabase(solidIndex, event, monikerTarget))
		throw Exception("Failed to add life event to database");

	if (rand() % ourChanceDuplicateEvent == 0)
	{
		// Test duplicates are being ignored
		if (!feed->AddLifeEventToDatabase(solidIndex, event, monikerTarget))
			throw Exception("Failed to add test duplicate life event to database");
	}

	if (event.CreaturePresentForEvent())
		ourEventCount++;
}

NornDynasty::Norn::~Norn()
{
	ourPopCheck--;
	::NornDynasty::ourMonikerToNorn[myMoniker] = NULL;
}

void NornDynasty::Norn::Age(int amount)
{
	if (myPregnant && myPregnant->myState != StateConceived)
			throw Exception("Pregnant with a beyond-conceived child");

	if (myState == StateLaid && rand() % 5 == 0)
		Hatch();

	// Only born, living creatures age
	if (myState == StateExported || myState == StateWarped)
		throw Exception("Tried to age exported/warped Creature");
	if (myState != StateBorn)
		return;

	int oldAge = myAgeInTicks;
	myAgeInTicks += amount;
	int newAge = myAgeInTicks;
	int stageLength = ourLifeStageLength;
	// vary life stage lenghts by +-5%
	stageLength += rand() % (ourLifeStageLength / 10) - (ourLifeStageLength / 5);
	if ((newAge / stageLength) > (oldAge / stageLength))
	{
		Event(::LifeEvent::typeNewLifeStage, "", "");
		myLifeStage++;

		if (myLifeStage == 7)
			Die();
	}

	if (myPregnant && rand() % 5 == 0)
		Lay();
}

void NornDynasty::Norn::Die()
{
	// Only born, living creatures die
	if (myState != StateBorn)
		return;

	Event(::LifeEvent::typeDied, "", "");
	myState = StateDead;

	if (myPregnant)
	{
		++ourMotherDiedWhileInWomb;
		myPregnant->myState = StateDead;
	}
}

NornDynasty::Norn* NornDynasty::Norn::MakeLove(Norn* love)
{
	if (love->myWorld != myWorld)
		throw Exception("Trying to breed when in different ourWorlds");	
	if (myState != StateBorn || love->myState != StateBorn)
		return NULL;

	// Too young!
	if (myLifeStage < 3 || love->myLifeStage < 3)
		return NULL;

	// Females make love to males in this world
	if (myHistory.myGender == 1 && love->myHistory.myGender == 2)
		return love->MakeLove(this);

	// Only non-pregnant girls get pregnant, and only
	// with boys.  Oh, and they should be the same
	// species (genus) as well.
	if (!myPregnant && myHistory.myGender == 2 && love->myHistory.myGender == 1
		&& myHistory.myGenus == love->myHistory.myGenus)
	{
		myPregnant = new Norn(myWorld, myHistory.myGenus, myMoniker, love->myMoniker);	
		myPregnant->Event(::LifeEvent::typeConceived, myMoniker, love->myMoniker);
		Event(::LifeEvent::typeBecamePregnant, myPregnant->myMoniker, love->myMoniker);
		love->Event(::LifeEvent::typeImpregnated, myPregnant->myMoniker, myMoniker);
		myPregnant->myMumMoniker = myMoniker;
		myPregnant->myDadMoniker = love->myMoniker;
		return myPregnant;
	}

	return NULL;
}

NornDynasty::Norn* NornDynasty::Norn::Splice(Norn* other)
{
	Norn* spliced = new Norn(myWorld,
		rand() % 2 == 1 ? myHistory.myGenus : other->myHistory.myGenus,
		myMoniker, other->myMoniker);
	spliced->Event(::LifeEvent::typeSpliced, myMoniker, other->myMoniker);
	spliced->myState = StateBorn;
	return spliced;
}

void NornDynasty::Norn::Lay()
{
	if (myPregnant)
	{
		Event(::LifeEvent::typeLaidEgg, myPregnant->myMoniker, "");
		myPregnant->Laid();
		myPregnant = NULL;
	}
}

void NornDynasty::Norn::Laid()
{
	if (myState != StateConceived)
		return ;
	Event(::LifeEvent::typeLaid, "", "");
	myState = StateLaid;
}

void NornDynasty::Norn::Hatch()
{
	if (myState != StateLaid)
		return;
	Event(::LifeEvent::typeBorn, myMumMoniker, myDadMoniker);
	if (!myMumMoniker.empty())
		StaticEvent(myFeed, myWorld, myMumMoniker, ::LifeEvent::typeChildBorn, myMoniker, myDadMoniker);
	if (!myDadMoniker.empty())
		StaticEvent(myFeed, myWorld, myDadMoniker, ::LifeEvent::typeChildBorn, myMoniker, myMumMoniker);
	myState = StateBorn;
}

void NornDynasty::Norn::ExportOrWarpOut(bool warp)
{
	// Sorry, we don't export pregnant people yet (too tiring!)
	if (myPregnant)
		return;
	if (myState != StateBorn)
		return;

	if (myWorld == NULL)
		throw Exception("Expected to be in a world when not exported");
	
	if (warp)
	{
		Event(::LifeEvent::typeWarpedOut, "", "");
		myState = StateWarped;
	}
	else
	{
		Event(::LifeEvent::typeExported, "", "");
		myState = StateExported;
	}

	myWorld = NULL;
}

void NornDynasty::Norn::ImportOrWarpIn(World* world)
{
	if (myState != StateExported && myState != StateWarped)
		return;

	if (myWorld != NULL)
		throw Exception("Expected not to be in a world when exported");
	myWorld = world;
	

	if (myState == StateExported)
		Event(::LifeEvent::typeImported, "", "");
	else if (myState == StateWarped)
		Event(::LifeEvent::typeWarpedIn, "", "");
	else
		throw Exception("Internal error: Import when not warped or exported");
	myState = StateBorn;

	return;
}

// static
int NornDynasty::CalculateGoes(float factor, int norns, int worlds)
{
	float floatGoes = factor * (float)norns * (float)worlds;
	int goes = (int)floor(floatGoes);
	if (rand() % 100000 < (floatGoes - goes) * 100000)
		goes++;
	return goes;
}

// static
bool NornDynasty::NewWorld(OurFeed* feed, int initialNorns, const std::string& user)
{
#ifdef THREAD_SAFE
	CBabelCritSect crit(ourCritical);
#endif

	// One off initialisation
	if (!ourInitialised)
	{
		srand(time(NULL));
		ourInitialised = true;
	}

	try
	{
		// Make the world
		World* world = new World(feed, user);
		ourWorlds.push_back(world);

		// Make some initial Norns
		for (int norn = 0; norn < initialNorns; ++norn)
		{
			Norn* norn = new Norn(world, 0);
			ourNorns.push_back(norn);
			norn->Event(::LifeEvent::typeEngineered, "", "aint-really-exist.gen");
			norn->Laid();
			norn->Hatch();
		}
	}
	catch (Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "Unknown exception caught while making a world" << std::endl;
		return false;
	}

	return true;
}


bool NornDynasty::UpdateDynasty()
{
#ifdef THREAD_SAFE
	CBabelCritSect crit(ourCritical);
#endif

	if (ourNorns.size() <= 0 || ourWorlds.size() <= 0)
		return false;

	try
	{
		// Increment world tick by varying amounts
		for (int world = 0; world < ourWorlds.size(); ++world)
		{
			int amount = rand() % 100 + 1;
			ourWorlds[world]->myWorldTick += amount;
			ourWorlds[world]->myJustTickedBy = amount;
		}

		// All Norns in that world age
		for (int norn = 0; norn < ourNorns.size(); ++norn)
		{
			if (ourNorns[norn]->myWorld)
				ourNorns[norn]->Age(ourNorns[norn]->myWorld->myJustTickedBy);
		}

		// Breed two random creatures together
		// Check they're the opposite gender and same species
		{
			// Make list of Norns in each world
			std::map< World*, std::vector<Norn*> > nornsInWorld;
			for (int i = 0; i < ourNorns.size(); ++i)
					nornsInWorld[ourNorns[i]->myWorld].push_back(ourNorns[i]);
				
			int goes = CalculateGoes(ourBreedGoes, ourNorns.size(), 1);
			for (int go = 0; go < goes; ++go)
			{
				Norn* parent1 = ourNorns[rand() % ourNorns.size()];
				if (parent1->myWorld)
				{				
					std::vector<Norn*>& nornsHere = nornsInWorld[parent1->myWorld];
					if (nornsHere.size() < 1)
						throw Exception("Breeding: But at least I must be there?!");

					// Pick one at random
					Norn* parent2 = nornsHere[rand() % nornsHere.size()];

					Norn* newNorn;
					
					if (rand() % ourChanceOfSplice == 0)
						newNorn = parent1->Splice(parent2);
					else
						newNorn = parent1->MakeLove(parent2);

					if (newNorn)
						ourNorns.push_back(newNorn);
				}
			}
		}

		// Clone a creature	(can be natural when imported into world it
		// is already in)

		// Randomly kill a creature
		{
			int goes = CalculateGoes(ourDeathGoes, ourNorns.size(), 1);
			for (int go = 0; go < goes; ++go)
			{
				Norn* piranhaDeath = ourNorns[rand() % ourNorns.size()];
				piranhaDeath->Die();
			}
		}

		// Export/warpout a creature
		{
			int goes = CalculateGoes(ourExportGoes, ourNorns.size(), 1);
			for (int go = 0; go < goes; ++go)
			{
				Norn* exportMe = ourNorns[rand() % ourNorns.size()];
				bool warp = (rand() % 2 == 0);
				exportMe->ExportOrWarpOut(warp);	
			}
		}

		// Import/warp in a creature who isn't in a world at the moment
		{
			int goes = CalculateGoes(ourImportGoes, ourNorns.size(), 1);
			for (int go = 0; go < goes; ++go)
			{
				Norn* exportMe = ourNorns[rand() % ourNorns.size()];
				World* world = ourWorlds[rand() % ourWorlds.size()];
				exportMe->ImportOrWarpIn(world);	
			}
		}

		// Clone a creature (like when you import one twice into
		// the same world, or you TWIN one)
		{
			int goes = CalculateGoes(ourCloneGoes, ourNorns.size(), 1);
			for (int go = 0; go < goes; ++go)
			{
				Norn* cloneMe = ourNorns[rand() % ourNorns.size()];
				if (cloneMe->myWorld)
				{
					Norn* clone = new Norn(cloneMe->myWorld,
						 cloneMe->myHistory.myGenus, cloneMe->myMoniker, "");
					ourNorns.push_back(clone);
					clone->myState = cloneMe->myState;
					clone->myAgeInTicks = cloneMe->myAgeInTicks;
					clone->myLifeStage = cloneMe->myLifeStage;
					clone->Event(::LifeEvent::typeCloned, cloneMe->myMoniker, "");
					cloneMe->Event(::LifeEvent::typeClonedSource, clone->myMoniker, "");
				}
			}
		}

		// Double imports to different ourWorlds

		// Stop using a world, make a new world?

		// Cull dead creatures
		std::vector<Norn*> swapNorns;
		{
			for (int norn = 0; norn < ourNorns.size(); ++norn)
			{
				if (ourNorns[norn]->myState != Norn::StateDead)
					swapNorns.push_back(ourNorns[norn]);
				else
					delete ourNorns[norn];
			}
		}
		std::swap(swapNorns, ourNorns);

		// Check population
		if (ourNorns.size() != ourPopCheck)
			throw Exception("Internal population accounting error");

		// Increase the faked real time
		ourRealTime++;
	}
	catch (Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "Unknown exception caught while making a dynasty" << std::endl;
		return false;
	}

	if (ourNorns.size() == 0)
		std::cout << std::endl << "All your Norns, Grendels and Ettins died!" << std::endl;

	return ourNorns.size() > 0;
}

