#include "common/creaturesImage.h"
#include "common/readfile.h"
#include "openc2e/PathResolver.h"
#include "openc2e/World.h"
#include "openc2e/caosScript.h"
#include "openc2e/caosVM.h"
#include "openc2e/imageManager.h"

#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

class Openc2eTestHelper {
  public:
	static std::shared_ptr<creaturesImage> addBlnkSprite() {
		shared_array<uint8_t> pureblack(2 * 41 * 18);
		std::vector<Image> images(2);
		for (size_t i = 0; i < images.size(); ++i) {
			images[i].width = 41;
			images[i].height = 18;
			images[i].format = if_rgb565;
			images[i].data = pureblack;
		}

		std::shared_ptr<creaturesImage> img(new creaturesImage("blnk"));
		img->images = images;

		world.gallery->addImage(img);
		return img;
	}
};

static void run_script(const std::string& dialect, const std::string& s) {
	std::stringstream stream(s);
	caosScript script(dialect, "");
	script.parse(stream);

	caosVM vm(nullptr);
	try {
		vm.runEntirely(script.installer);
	} catch (caosException& e) {
		ADD_FAILURE() << e.prettyPrint();
	}
}

TEST(caos, unknown_dialect) {
	ASSERT_ANY_THROW(run_script("unknown dialect", ""));
}

TEST(caos, assert) {
	for (auto dialect : getDialectNames()) {
		run_script(dialect, "dbg: asrt 1 eq 1");
		EXPECT_NONFATAL_FAILURE(run_script(dialect, "dbg: asrt 1 eq 2"), "");

		run_script(dialect, "dbg: asrf 1 eq 2");
		EXPECT_NONFATAL_FAILURE(run_script(dialect, "dbg: asrf 1 eq 1"), "");

		EXPECT_NONFATAL_FAILURE(run_script(dialect, "dbg: fail"), "");
	}
}

TEST(caos, barewords) {
	auto sprite = Openc2eTestHelper::addBlnkSprite();
	run_script("c3", R"(
		setv va00 5
		new: simp 3 2 1 "blnk" 2 0 0
		emit 1 _p1_
		subr eye-roll
		retn
	)");
	world.gallery = std::make_unique<imageManager>();
}

TEST(caos, file) {
	struct auto_data_directories {
		decltype(data_directories) original_data_directories;
		auto_data_directories() {
			original_data_directories = data_directories;
			data_directories = {DataDirectory(".")};
		}
		~auto_data_directories() { data_directories = original_data_directories; }
	} auto_directories;
	run_script("c3", R"(
		file oope 1 "testfile" 0
		outs "hello\n"
		outv 2
		outs "\n"
		outv 5.0
		file oclo

		file iope 1 "testfile"
		sets va00 innl
		dbg: asrt va00 eq "hello"

		setv va00 inni
		dbg: asrt va00 eq 2

		setv va00 innf
		dbg: asrt va00 eq 5.0

		file iclo
	)");
}

TEST(caos, flow) {
	run_script("c3", R"(
		* unit tests for the non-ifblock flow stuff 
		* fuzzie, 06/06/04

		* test LOOP .. UNTL
		SETV VA00 0
		LOOP
		 ADDV VA00 1
		UNTL VA00 eq 3
		DBG: ASRT VA00 eq 3

		* test LOOP .. EVER
		SETS VA00 CAOS 1 0 0 0 "SETV VA00 3 LOOP DOIF VA00 EQ 0 STOP ENDI OUTV VA00 SUBV VA00 1 EVER" 0 0 VA01
		DBG: ASRT VA00 EQ "321"

		* test REPS .. REPE
		SETV VA00 0
		SETV VA01 4
		REPS VA01
		 SETV VA01 0
		 ADDV VA00 1
		REPE
		DBG: ASRT VA00 eq 4

		* test GSUB
		SETV VA00 0
		GSUB test
		DBG: ASRT VA00 eq 1

		* GSUB loops
		SETV VA00 5
		REPS 4
		  GSUB t2
		REPE
		DBG: ASRT VA00 eq 9

		* for GSUB test
		SUBR test
		 SETV VA00 1
		RETN

		SUBR t2
		 ADDV VA00 1
		RETN
	)");
}

TEST(caos, ifblocks) {
	run_script("c3", R"(
		* unit tests for DOIF/ELIF/ELSE/ENDI blocks and comparison operators
		* fuzzie, 06/06/04

		* test equality
		DBG: ASRT 1 eq 1
		DBG: ASRT 0.5 eq 0.5

		* test non-equality
		DBG: ASRT 1 ne 2
		DBG: ASRT 0.5 ne 0.7

		* test greater than
		DBG: ASRF 1 > 2

		* test less than
		DBG: ASRT 1 < 2

		* test le #1
		DBG: ASRT 1 <= 1

		* test le #2
		DBG: ASRT 1 <= 2

		* test le #3
		DBG: ASRF 2 <= 1

		* test ge #1
		DBG: ASRT 1 >= 1

		* test ge #2
		DBG: ASRF 1 >= 2

		* test ge #3
		DBG: ASRT 2 >= 1

		* test AND
		DBG: ASRT 1 eq 1 AND 2 eq 2

		* test OR #1
		DBG: ASRT 1 eq 1 OR 1 eq 2

		* test OR #2
		DBG: ASRF 1 eq 2 OR 2 eq 3

		* test embedded if blocks
		DOIF 1 eq 2
		 DOIF 1 eq 1
		  DBG: FAIL
		 ELSE
		  DBG: FAIL
		 ENDI
		ELSE
		 DOIF 1 eq 1
		 ELSE
		  DBG: FAIL
		 ENDI
		ENDI

		* test ELIF
		DOIF 1 eq 2
		 DBG: FAIL
		ELIF 1 eq 1
		ELSE
		 DBG: FAIL
		ENDI

		* test associativity
		* (1 == 2 && 2 == 1) || 1 == 1
		DBG: ASRT 1 gt 2 and 2 lt 1 or 1 eq 1

		* test AND and OR, ie, ordering
		DBG: ASRT 1 ne 1 and 2 eq 3 and 4 eq 5 or 1 eq 1

		DBG: ASRT "a" lt "b"

		SETV VA00 MOWS

		* test AND #2
		DBG: ASRF 1 eq 2 AND 2 eq 2

		*test chained elifs
		DOIF 1 eq 2
			DBG: FAIL
		ELIF 1 eq 3
			DBG: FAIL
		ELIF 2 eq 3
			DBG: FAIL
		ELSE
		ENDI

		* test more chained elifs, with nesting
		DOIF 1 eq 1
			SETV VA00 0
			DOIF 3 eq 1
				DBG: FAIL
			ELIF 4 eq 4
				SETV VA00 3
			ENDI
		ELSE
			DBG: FAIL
		ENDI
		DBG: ASRT VA00 eq 3

		* test that only one elif branch is evaluated
		SETV VA00 0
		DOIF 1 eq 2
			DBG: FAIL
		ELIF 1 eq 1
			SETV VA00 1
		ELIF 2 eq 2
			SETV VA00 2
		ENDI
		DBG: ASRF VA00 eq 2 or VA00 eq 0        
	)");
}

TEST(caos, parse_comment_end) {
	run_script("c3", "** Make sure we can handle a file ending with a comment");
}

TEST(caos, parsing) {
	run_script("c3", R"(
		* ensure that single-quoted characters are parsed correctly
		dbg: asrt 'C' = 67
	)");
}

TEST(caos, simpleagent) {
	auto sprite = Openc2eTestHelper::addBlnkSprite();
	run_script("c3", R"(
		* unit tests for the simple agent stuff 
		* fuzzie, 06/06/04

		* test NULL
		SETA VA00 NULL
		DBG: ASRT VA00 eq NULL

		* test NEW: SIMP
		NEW: SIMP 3 2 1 "blnk" 2 0 0
		DBG: ASRF TARG eq NULL

		* test ATTR
		ATTR 3575 * all attributes relevant to agents
		DBG: ASRT ATTR eq 3575

		* test FMLY/SPCS/GNUS
		SETV VA00 0
		DOIF SPCS eq 1
		 ADDV VA00 1
		ENDI
		DOIF GNUS eq 2
		 ADDV VA00 1
		ENDI
		DOIF FMLY eq 3
		 ADDV VA00 1
		ENDI
		DBG: ASRT VA00 eq 3

		* test OV00
		SETV OV00 1
		DBG: ASRT OV00 eq 1

		* test ENUM
		SETV VA00 0
		NEW: SIMP 3 2 1 "blnk" 2 0 0
		SETV OV00 2
		ENUM 3 2 1
		 ADDV VA00 OV00
		NEXT
		DBG: ASRT VA00 eq 3

		* make sure TARG is reset after ENUM
		DBG: ASRT TARG eq NULL

		* make sure ENUM doesn't happen if there are no agents
		RTAR 3 2 1
		SETV VA00 0
		ENUM 20 25 65530
		 SETV VA00 1
		NEXT
		DBG: ASRT VA00 eq 0

		* make sure TARG is reset after unsuccessful ENUM
		DBG: ASRT TARG eq NULL

		RTAR 3 2 1

		* test POSE setting
		POSE 1
		DBG: ASRT POSE eq 1

		* make sure ANIM doesn't immediately change current POSE
		ANIM [0]
		DBG: ASRT POSE eq 1
	)");
	world.gallery = std::make_unique<imageManager>();
}

TEST(caos, special_lexing) {
	auto special_lexing_script = R"(
		SETV VAR0 0
		GSUB go_to_bed
		DBG: ASRT VAR0 EQ 1

		SUBR go_to_bed
		  DBG: ASRT VAR0 EQ 0
		  ADDV VAR0 1
		RETN

		DBG: ASRT VAR0 EQ 1
	)";
	run_script("c1", special_lexing_script);
	run_script("c2", special_lexing_script);
}

TEST(caos, numbers) {
	run_script("c3", R"(
		setv va00 0
		setv va00 1
		setv va00 -5
		setv va00 0.4
		setv va00 -3.2
		setv va00 -.4
		setv va00 .3
		setv va00 3.
	)");
}

TEST(caos, strings) {
	run_script("c3", R"(
		* tests for strings
		* fuzzie, 14/07/08

		* test string concaternation
		SETS VA00 "he"
		ADDS VA00 "llo"
		DBG: ASRT VA00 eq "hello"

		* test strings don't always match
		DBG: ASRF "meep" eq "moop"

		* test LOWA
		DBG: ASRT LOWA "HELLO" eq "hello"

		* test UPPA
		DBG: ASRT UPPA "hello" eq "HELLO"

		* test STRL (string length)
		DBG: ASRT STRL "hello" eq 5

		* test SUBS (substring)
		DBG: ASRT SUBS "moohello" 4 5 eq "hello"

		* test integer-to-string
		DBG: ASRT VTOS 1 eq "1"

		* test float-to-string
		DBG: ASRT VTOS 1.0 eq "1.000000"

		* test string-to-integer
		DBG: ASRT STOI "54.6" eq 54

		* test string-to-float
		DBG: ASRT STOF "54.6" eq 54.6

		* search for string
		DBG: ASRT SINS "moohellomoo" 1 "hello" eq 4

		DBG: ASRT SINS "moohellomoo" 2 "moo" eq 9

		* failed search for string #1
		DBG: ASRT SINS "moohellomoo" 5 "hello" eq -1

		* failed search for string #2
		DBG: ASRT SINS "moohellomoo" 1 "moop" eq -1

		* read char at index
		DBG: ASRT CHAR "hello" 2 eq 'e'

		* set char at index
		SETS VA00 "moop"
		CHAR VA00 3 'e'
		DBG: ASRT VA00 eq "moep"    
	)");
}

TEST(caos, timeslice) {
	GTEST_SKIP();
	auto timeslice_script = R"(
		* unit tests for time slicing
		* nornagon 04/08/07

		* setv
		setv var0 100
		dbg: tslc var0
		setv var1 42
		subv var0 dbg: tslc
		dbg: asrt var0 eq 0

		* doif
		setv var0 100
		dbg: tslc var0

		doif 1 eq 2
			dbg: fail
		elif 3 eq 4
			dbg: fail
		else
			setv var1 2
		endi

		subv var0 dbg: tslc
		dbg: asrt var0 eq 0

		* math
		setv var0 100
		dbg: tslc var0
		setv var1 0
		addv var1 17
		divv var1 3
		mulv var1 32
		modv var1 6
		rndv var1 -5 5
		andv var1 3
		orrv var1 8
		subv var1 9
		negv var1
		subv var0 dbg: tslc
		dbg: asrt var0 eq 0

		* new: simp
		setv var0 100
		dbg: tslc var0
		new: simp blnk 1 0 1 0
		subv var0 dbg: tslc
		dbg: asrt var0 eq 1
		kill targ

		* kill
		new: simp blnk 1 0 1 0
		setv var0 100
		dbg: tslc var0
		kill targ
		subv var0 dbg: tslc
		dbg: asrt var0 eq 1

		* pose
		new: simp blnk 48 0 1 0
		setv var0 100
		dbg: tslc var0
		pose 0
		subv var0 dbg: tslc
		dbg: asrt var0 eq 1
		kill targ

		* anim
		new: simp blnk 48 0 1 0
		setv var0 100
		dbg: tslc var0
		anim [0101]
		subv var0 dbg: tslc
		dbg: asrt var0 eq 1
		kill targ

		* loop..untl
		setv var0 100
		dbg: tslc var0
		setv var1 0
		loop
			addv var1 1
		untl var1 ge 3
		subv var0 dbg: tslc
		dbg: asrt var0 eq 0

		* stim
		setv var0 100
		dbg: tslc var0
		stim shou 100 1 30 0 0 0 0 0 0 0 0 0
		subv var0 dbg: tslc
		dbg: asrt var0 eq 0
	)";
	run_script("c1", timeslice_script);
	run_script("c2", timeslice_script);
}

TEST(caos, variables) {
	run_script("c3", R"(
		* unit tests for variables
		* fuzzie, 06/06/04

		* test setv
		SETV VA00 1
		DBG: ASRT VA00 eq 1

		* test subv
		SETV VA00 4
		SUBV VA00 2
		DBG: ASRT VA00 eq 2

		* test subv on an integer and float
		setv va00 1
		subv va00 0.25
		dbg: asrt va00 = 0.75
		setv va00 1.5
		subv va00 1
		dbg: asrt va00 = 0.5

		* test addv
		SETV VA00 4
		ADDV VA00 2
		DBG: ASRT VA00 eq 6

		* test addv on an integer and float
		setv va00 1
		addv va00 0.25
		dbg: asrt va00 = 1.25
		setv va00 1.5
		addv va00 1
		dbg: asrt va00 = 2.5

		* test mulv
		SETV VA00 4
		MULV VA00 2
		DBG: ASRT VA00 eq 8

		* test mulv on an integer and float
		setv va00 1
		mulv va00 0.25
		dbg: asrt va00 = 0.25
		setv va00 1.5
		mulv va00 2
		dbg: asrt va00 = 3.0

		* test negv
		SETV VA00 4
		NEGV VA00
		DBG: ASRT VA00 eq -4

		* test modv
		SETV VA00 7
		MODV VA00 2
		DBG: ASRT VA00 eq 1

		* test that you can have independent va00/va01
		SETV VA01 8
		SETV VA00 7
		DBG: ASRT VA01 eq 8 AND VA00 eq 7

		* test setv with va00/va01, from nornagon
		SETV VA00 1
		SETV VA01 0
		SETV VA00 VA01
		DBG: ASRT VA00 eq VA01

		* test if variables are zero by default
		DBG: ASRT VA99 eq 0
	)");
}

TEST(caos, vector) {
	run_script("c3", R"(
		VEC: SETV VA00 VEC: MAKE 42 43
		VEC: GETC VA00 VA01 VA02
		DBG: ASRT VA01 EQ 42 AND VA02 EQ 43

		SETV VA00 -179
		SETV VA03 1

		LOOP
		* VA00 = angle
		* VA01 = unit vector (VA00)
		* VA42 = angle of VA01
		* VA02 = difference of VA00 and VA42
			VEC: SETV VA01 VEC: UNIT VA00
			VEC: MULV VA01 5
			SETV VA02 VEC: ANGL VA01
			SETV VA42 VA02
			DOIF VA02 < 0
			  ADDV VA02 360
			ENDI
			SUBV VA02 VA00
			ABSV VA02

			SETV VA10 VEC: MAGN VA01
			SUBV VA10 5
			ABSV VA10

			DOIF VA02 > 0.01 AND VA20 > 0.01 OR VA10 > 0.01
				DBG: OUTS "# nok@"
				DBG: OUTV VA00
				SETV VA03 0
				DBG: OUTS " got: "
				DBG: OUTV VA42
				DBG: OUTV VA20
				DBG: OUTS " v="
				DBG: OUTV VA01
				DBG: OUTS "\n"
				DBG: FAIL
			ENDI
			ADDV VA00 1
		UNTL VA00 > 180

		DBG: ASRT VA03 EQ 1    
	)");
}
