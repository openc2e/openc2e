////////////////////////////////////////////////////////////////////////////////
#if !defined(CMACRO_PARSE_H)
#define CMACRO_PARSE_H

#include "../../engine/CAOS/CAOSDescription.h"

//
// Public Constants
//

// The different classes of token
#define UNCLASSIFIED_TOKEN		-1
#define	BAD_TOKEN				1 
#define	ERROR_TOKEN				2  
#define	COMMENT_TOKEN			3  
#define	NEWLINE_TOKEN			4  
#define	NORMAL_COMMAND_TOKEN	5  
#define	RELATIVE_OPERATOR_TOKEN	7  
#define	STRING_CONSTANT_TOKEN	8  
#define	DECIMAL_CONSTANT_TOKEN	9  
#define LOGICAL_OPERATOR_TOKEN	10
#define	LABEL_TOKEN				11 
#define BYTE_STRING_TOKEN       12
#define LVRV_TOKEN				13

// Parsing error status
#define PARSE_FAIL_END		-1   // Ran out of tokens
#define PARSE_FAIL_GENERAL	-2   // General syntax error
#define PARSE_OK			1    // No problem

// Types of script
#define INSTALL_SCRIPT 1   // iscr ... [endm]
#define EVENT_SCRIPT   2   // scrp ... endm
#define REMOVE_SCRIPT  4   // rscr ... [endm]
#define OUTSIDE_SCRIPT 8   // Blank lines and comments can exist 
						   // outside of the above three script types 
						   // without causing a syntax error


//
// Public Types
//


// This structure holds details about each token
struct TokenStructure {
	char *Name;				// Token string
	int   Size;				// Length of the token string
	int   PositionBefore;   // Character position before parsing
	int   PositionAfter;	// Character position after parsing
	int   TokenClass;		// Token classification, eg STRING_TOKEN
	UINT  Value;			// Holds a 32-bit representation of tokens
							// of exactly four characters. For other
							// tokens, Value is 0
};

// This macro performs the encoding of four-character tokens into 
// their 32-bit representations
#define ENCODE(a, b, c, d) ((a<<24) | (b<<16) | (c<<8) | (d))

// This structure holds details about each command
struct CommandStructure {
	int   StartIndex;		// Token index where the oommand begins
	int   EndIndex;			// Token index where the command ends
	int   ParseStatus;		// Status of the command, eg PARSE_OK
	char *ErrorString;      // Gives a description of what went wrong
							// with the command (Can be NULL, see code)
	char *RTFString;		// An RTF representation of the command
							// (Can be NULL, see the code)
	int   RTFStringSize;    // The length of the RTF representation
	int   IndentLevel;		// The indentation level of the command
							// starting at zero. Can be -1 for badly 
							// nested commands (see the code)
	int   ScriptClass;		// The classsification of script that the
							// command belongs to, eg EVENT_SCRIPT
	int   FGSEValues[4];	// If the command is part of an event 
							// script, this stores the family/genus/ 
							// species/event IDs of the owning script.
							// Otherwise it is [-1,-1,-1,-1]
	int   LineNumber;		// The line number of the command starting
							// at one
};


// This structure holds details about each script
struct ScriptStructure {
	int   StartCommandIndex;// Command index where the script begins
	int   EndCommandIndex;	// Command index where the script ends
	int   FGSEValues[4];	// Stores the family/genus/species/event 
							// IDs for event scripts. For install and
							// remove scripts, stores [-1,-1,-1,-1]
	char *InjectString;		// Stores the injectable macro code string
							// for the script. (Can be NULL or the 
							// empty string, see the code)
};


// This structure holds details about a command, lvalue or rvalue
struct TableEntryStructure {
	UINT KeywordValue;				
	char *ArgumentSpecification;
	TableEntryStructure * ExtraKeywordTable;
	int TokenClass;
};


//
// The class definition
//

class CMacroParse {
public:
	static CAOSDescription theCAOSDescription;

private:

	//
	// Private constants
	//

	enum {
		DEFAULT_TOKEN_COUNT	  = 10000,
		DEFAULT_COMMAND_COUNT =	5000,
		DEFAULT_ERROR_COUNT	  = 500,
		MAX_DEPTH			  = 100,
		MAX_TOKEN_SIZE        = 500,
		MAX_ERROR_SIZE        = 300,
		MAX_RTF_LINE          = 50000,
		MAX_INJECT_SIZE       = 50000
	};

	// Types of syntactical block
	enum {
		SCRIPT_BLOCK = 0,	// scrp ... endm
		REPEAT_BLOCK,		// reps ... repe
		SUBROUTINE_BLOCK,	// subr	... retn
		IF_BLOCK,			// doif ... elif
							// doif ... else
							// doif ... endi
		ELIF_BLOCK,			// elif ... elif
							// elif ... else
							// elif ... endi
		ELSE_BLOCK,			// else ... endi
		LOOP_BLOCK,			// loop ... ever
							// loop ... untl
		ENUMERATOR_BLOCK	// enum ... next
							// esee ... next
							// etch ... next
							// epas ... next
							// econ ... next
	};


// RTF Constants
#define RTF_HEADER "{\\rtf1\\ansi\\deff0\\deftab720{\\fonttbl{\\f0\\fmodern Courier New;}}\r\n{\\colortbl\\red128\\green64\\blue64;\\red255\\green0\\blue0;\\red200\\green0\\blue200;\\red0\\green0\\blue255;\\red0\\green128\\blue0;\\red0\\green0\\blue0;}\r\n"
#define RTF_TRAILER "}"
#define RTF_EMPTY RTF_HEADER RTF_TRAILER

	//
	// Private Member Variables
	//

	char	   *m_RTF;
	char	   *m_RTFBody;
	char	   *m_RTFTrailer;
	int			m_RTFStringSizeTotal;
	int			m_ParseLineStatus;
	int			m_NestingStack[MAX_DEPTH];
	int			m_IndentLevel;
	int			m_ScriptClass;
	BOOL		m_InstallScriptAlreadySeen;
	BOOL		m_RemoveScriptAlreadySeen;
	int			m_FGSEValues[4];
	int			m_TokenIndex;
	char		m_ErrorString[MAX_ERROR_SIZE];
	CPtrArray	m_TokenList;
	int			m_TokenCount; 
	CPtrArray	m_CommandList;
	int			m_CommandCount;
	CPtrArray	m_ScriptList;
	int			m_EventScriptCount;
	int			m_ScriptCount;
	CUIntArray  m_ErrorIndexList;
	int			m_ErrorCount;

	
	//
	// Private Member Functions
	//

	void Cleanup();
	void Initialize();
	BOOL CheckBlockCommandTable(UINT TokenValue);
	TableEntryStructure * GetTableEntry
		(TableEntryStructure * Table, UINT KeywordValue);
	void BeginBlock(int Block);
	int EndBlock(UINT TerminatingTokenValue);
	void TokenizeString(char * StringToTokenize);
	BOOL IsDecimalConstant(TokenStructure * Token);
	BOOL IsStringConstant(TokenStructure * Token);
	BOOL IsByteString(TokenStructure * Token);
	BOOL IsRelativeOperator(TokenStructure * Token);
	BOOL IsLogicalOperator(TokenStructure * Token);
	BOOL IsCommandHeader(TokenStructure * Token);
	TokenStructure *GetNextKeyword();
	int ParseDecimalConstant();
	int ParseStringConstant();
	int ParseByteString();
	int ParseRelativeOperator();
	int ParseLabel();
	int ParseVariable();
	int ParseDecimalRV();
	int ParseStringRV();
	int ParseAgentRV();
	int ParseAnyRV(int& type);
	int ParseRelation();
	int ParseCondition();
	int ParseOpSpec(TokenStructure *Token, int Class, const OpSpec* op, char *OnErrorString);
	int ParseCommand
		(BOOL    HandleBlocks,
		 int   & IndentLevel, 
		 BOOL  & ScriptClass,
		 int     FGSEValues[4]);



public:
	
	//
	// Public Functions
	//
	
	CMacroParse();
	~CMacroParse();
	
	static bool LoadSyntax(const std::string& filename);

	void Parse(char * MacroCode);
	void GetParseResults
		(CPtrArray  & TokenList,
		 int		& TokenCount,
		 CPtrArray	& CommandList,
		 int		& CommandCount,
		 CUIntArray & ErrorIndexList,
		 int		& ErrorCount);
	void GetScriptInformation
		(CPtrArray  & ScriptList,
		 int        & ScriptCount);
	void ParseLine(char * MacroCode);
	void GetParseLineResults
		(CPtrArray  & TokenList,
		 int		& TokenCount,
		 int		& ParseStatus,
		 char *     & ErrorString);

	char *GetRTFHeader();
	char *GetRTFBody();
	char *GetRTFTrailer();
	char *GetRTF();

	void Nullify();
};


#endif // CMACRO_PARSE_H

