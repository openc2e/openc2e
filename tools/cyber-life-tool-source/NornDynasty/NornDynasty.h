// $Id: NornDynasty.h,v 1.5 2000/09/06 15:48:29 firving Exp $

// Pretend to be lots of Norns in several pools of worlds,
// breeding and moving like Norns should.  This is for
// testing database structures, network feeds, and anything
// else.

#ifndef NORN_DYNASTY_H
#define NORN_DYNASTY_H

#define THREAD_SAFE

// Choose between a direct MySQL feed (HistoryFeed), or 
// a feed via Babel (BabelHistoryFeed).
#define BABEL_FEED

#ifdef BABEL_FEED
	#include <windows.h>
	#include <winsock.h>
	#include "../../../c2e/server/HistoryFeed/BabelHistoryFeed.h"
#else
	#include "../../../c2e/server/HistoryFeed/HistoryFeed.h"
#endif

#ifdef THREAD_SAFE
#include <BabelCriticalSection.h>
#endif

#include "../../../c2e/engine/Creature/History/HistoryStore.h"

// Polymorphism by typedef!
#ifdef BABEL_FEED
	typedef BabelHistoryFeed OurFeed;
#else
	typedef HistoryFeed OurFeed;
#endif

class NornDynasty
{
public:
	// Returns false upon an error
	static bool NewWorld(OurFeed* feed, int initialNorns, const std::string& user);
	static bool UpdateDynasty();

	// Various input paramaters - all documented in test.py
	static int ourLifeStageLength;
	static float ourBreedGoes;
	static int ourChanceOfSplice;
	static float ourDeathGoes;
	static float ourCloneGoes;
	static float ourExportGoes;
	static float ourImportGoes;
	static int ourChanceDuplicateEvent;

	// Output statistics
	static int GetEventCount() { return ourEventCount; }
	static int GetNornCount() { return ourNorns.size(); }
	static int GetMotherDiedWhileInWombCount() { return ourMotherDiedWhileInWomb; }

private:

	static int CalculateGoes(float factor, int norns, int worlds);

	class Exception : std::exception
	{
	public:
		Exception(const std::string& what);
		virtual const char * what();
	private:
		std::string myWhat;
	};

	class World
	{
	public:
		World(OurFeed* feed, const std::string& user);
		
		// Data members
		std::string myUID, myName, myUser;
		int myWorldTick;
		int myJustTickedBy;

		OurFeed* myFeed;
	};

	class Norn
	{
	public:
		Norn(World* world, int genus,
			std::string parent1Moniker = "", std::string parent2Moniker = "");
		~Norn();

		void Event(::LifeEvent::EventType type, const std::string& moniker1, const std::string& moniker2);
		static void StaticEvent(OurFeed* feed, World* world, const std::string& monikerTarget, ::LifeEvent::EventType type, const std::string& moniker1, const std::string& moniker2);

		void Age(int amount);
		void Die();

		Norn* MakeLove(Norn* love);
		Norn* Splice(Norn* other);

		void Lay();
		void Laid();
		void Hatch();

		void ExportOrWarpOut(bool warp);
		void ImportOrWarpIn(World* world);

		// Data members
		std::string myMoniker;
		::CreatureHistory myHistory;
		int myEventCount;

		enum State
		{
			StateConceived,
			StateLaid,
			StateBorn,
			StateDead,
			StateExported,
			StateWarped,
		};

		State myState;
		World* myWorld;
		int myAgeInTicks;
		int myLifeStage;
		Norn* myPregnant;
		std::string myMumMoniker, myDadMoniker;

		OurFeed* myFeed;
	};

	// Data members - Norn uses them also
	friend class NornDynasty::Norn;

	// The real world time that we are simulating, so
	// it's actually not a very real real time.
	static int ourRealTime;

	// Number of life events added to the database
	static int ourEventCount;
	
	// Double checking
	static int ourPopCheck;
	static int ourMotherDiedWhileInWomb;

	// Look up Norn by moniker
	static std::map<std::string, Norn*> ourMonikerToNorn;

	static bool ourInitialised;
	static std::vector<World*> ourWorlds;
	static std::vector<Norn*> ourNorns;

#ifdef THREAD_SAFE
	// Make the public interface thread safe
	static CBabelCriticalSection ourCritical;
#endif
};

#endif

