////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Caos.h"

// Don't touch this table - it stores the names of the fundamental block
// commands of the macro language
static UINT BlockCommandTable[] = {
	ENCODE('s','c','r','p'),
	ENCODE('e','n','d','m'),
	ENCODE('r','e','p','s'),
	ENCODE('r','e','p','e'),
	ENCODE('s','u','b','r'),
	ENCODE('r','e','t','n'),
	ENCODE('d','o','i','f'),
	ENCODE('e','l','i','f'),
	ENCODE('e','l','s','e'),
	ENCODE('e','n','d','i'),
	ENCODE('l','o','o','p'),
	ENCODE('u','n','t','l'),
	ENCODE('e','v','e','r'),
	ENCODE('e','n','u','m'), 
	ENCODE('e','t','c','h'), 
	ENCODE('e','s','e','e'),
	ENCODE('e','p','a','s'),
	ENCODE('n','e','x','t'),
	ENCODE('e','c','o','n'),
	// Table terminator
	0
};


CAOSDescription CMacroParse::theCAOSDescription;

void CMacroParse::Cleanup() 
{
	int i;
	CommandStructure *Command;
	TokenStructure *Token;
	ScriptStructure *Script;

	// Clean up the token list memory
	for (i=0; i<m_TokenCount; i++) {
		Token = (TokenStructure *)m_TokenList[i];
		delete [](Token->Name);
		delete Token;
	}
	// Clean up the command list memory
	for (i=0; i<m_CommandCount; i++) {
		Command = (CommandStructure *)m_CommandList[i];
		if (Command->ErrorString != NULL)
			delete [](Command->ErrorString);
		if (Command->RTFString != NULL)
			delete [](Command->RTFString);
		delete Command;
	}
	// Clean up the script list memory
	for (i=0; i<m_ScriptCount; i++) {
		Script = (ScriptStructure *)m_ScriptList[i];
		if (Script->InjectString != NULL)
			delete [](Script->InjectString);
		delete Script;
	}
	// Clean up RTF memory
	if (m_RTF != NULL)
		delete []m_RTF;
}


void CMacroParse::Initialize() {
	// Initialize member variables
	m_RTF = NULL;
	m_RTFBody = NULL;
	m_RTFTrailer = NULL;
	m_RTFStringSizeTotal = 0;
	m_ParseLineStatus = 0;
	m_IndentLevel = 0;
	m_ScriptClass = INSTALL_SCRIPT;
	m_InstallScriptAlreadySeen = FALSE;
	m_RemoveScriptAlreadySeen = FALSE;
	m_FGSEValues[0] = -1;
	m_FGSEValues[1] = -1;
	m_FGSEValues[2] = -1;
	m_FGSEValues[3] = -1;
	m_TokenIndex = 0;
	m_ErrorString[0] = '\0';
	m_TokenCount = 0;
	m_CommandCount = 0;
	m_ScriptCount = 0;
	m_EventScriptCount = 0;
	m_ErrorCount = 0;
	m_TokenList.SetSize(0);
	m_CommandList.SetSize(0);
	m_ScriptList.SetSize(0);
	m_ErrorIndexList.SetSize(0);
}


// Constructor
CMacroParse::CMacroParse() 
{
	// Initialize member variables
	Initialize();
}


// Destructor
CMacroParse::~CMacroParse()
{
	// Clean up memory usage
	Cleanup();	
}


BOOL CMacroParse::CheckBlockCommandTable(UINT TokenValue)
{
	UINT Value;
	int i;

	i = 0;
	while ((Value = BlockCommandTable[i]) != 0) {
		if (Value == TokenValue)
			return TRUE;
		i++;
	}
	return FALSE;
}


TableEntryStructure * CMacroParse::GetTableEntry
	(TableEntryStructure *Table, UINT KeywordValue)
{
	TableEntryStructure *t;

	t = Table;
	while (t->KeywordValue != 0) {
		if (t->KeywordValue == KeywordValue)
			return t;
		t++;
	}
	return NULL;
}


void CMacroParse::BeginBlock(int Block)
{
	// Push the supplied block type onto the nesting stack
    m_IndentLevel++;
    m_NestingStack[m_IndentLevel] = Block;
}



int CMacroParse::EndBlock(UINT TerminatingTokenValue)
{
    int CurrentBlock;
    BOOL OK;
    
    if (m_IndentLevel == 0) {
        // We've got a block-terminating token but we are at the outermost level
		switch (TerminatingTokenValue) {
        case ENCODE('e','n','d','m'):
			strcpy(m_ErrorString, "No matching 'scrp'");
			break;
        case ENCODE('r','e','p','e'):
			strcpy(m_ErrorString, "No matching 'reps'");
			break;
        case ENCODE('r','e','t','n'):
			strcpy(m_ErrorString, "No matching 'subr'");
			break;
		case ENCODE('e','l','i','f'):
			strcpy(m_ErrorString, "No matching 'doif' or 'elif'");
			break;
        case ENCODE('e','l','s','e'):
			strcpy(m_ErrorString, "No matching 'doif' or 'elif'");
			break;
        case ENCODE('e','n','d','i'):
			strcpy(m_ErrorString, "No matching 'doif', 'elif' or 'else'");
			break;
        case ENCODE('u','n','t','l'):
		case ENCODE('e','v','e','r'):
			strcpy(m_ErrorString, "No matching 'loop'");
			break;
        case ENCODE('n','e','x','t'):
			strcpy(m_ErrorString, "No matching 'enum', 'etch', 'esee', 'epas', 'econ'");
			break;
		} // switch
		return PARSE_FAIL_GENERAL;
    }
    
    // Which block are we currently in?
    CurrentBlock = m_NestingStack[m_IndentLevel];
    // Assume that there is a problem
    OK = FALSE;
    // Check that the terminating token is the right one for the current block
	switch (TerminatingTokenValue) {
	case ENCODE('e','n','d','m'):
        if (CurrentBlock == SCRIPT_BLOCK)
            OK = TRUE;
		break;
    case ENCODE('r','e','p','e'):
        if (CurrentBlock == REPEAT_BLOCK)
            OK = TRUE;
		break;
    case ENCODE('r','e','t','n'):
        if (CurrentBlock == SUBROUTINE_BLOCK)
            OK = TRUE;
		break;
	case ENCODE('e','l','i','f'):
        if ((CurrentBlock == IF_BLOCK) || 
			(CurrentBlock == ELIF_BLOCK))
            OK = TRUE;
		break;
    case ENCODE('e','l','s','e'):
        if ((CurrentBlock == IF_BLOCK) ||
			(CurrentBlock == ELIF_BLOCK))
            OK = TRUE;
		break;
    case ENCODE('e','n','d','i'):
        if ((CurrentBlock == IF_BLOCK) ||
			(CurrentBlock == ELIF_BLOCK) ||
            (CurrentBlock == ELSE_BLOCK))
            OK = TRUE;
		break;
    case ENCODE('u','n','t','l'):
	case ENCODE('e','v','e','r'):
        if (CurrentBlock == LOOP_BLOCK)
            OK = TRUE;
		break;
    case ENCODE('n','e','x','t'):
        if (CurrentBlock == ENUMERATOR_BLOCK)
            OK = TRUE;
		break;
	} // switch
    
    if (OK) {
        // The current block has been successfully terminated, so reduce the
		// indentation level, which is effectively a stack pop
        m_IndentLevel--;
        return PARSE_OK;
	}   
    
    // The terminating token wasn't the right one for the current block, so
    // figure out which terminating token IS right. Note that we remain inside
    // the current block, ie the indentation level does not change on a nesting 
	// error
    switch (CurrentBlock) {
	case SCRIPT_BLOCK:
            strcpy(m_ErrorString, "'endm' expected");
			break;
	case REPEAT_BLOCK:
            strcpy(m_ErrorString, "'repe' expected");
			break;
	case SUBROUTINE_BLOCK:
            strcpy(m_ErrorString, "'retn' expected");
			break;
	case IF_BLOCK:
            strcpy(m_ErrorString, "'elif', 'else' or 'endi' expected");
			break;
	case ELIF_BLOCK:
            strcpy(m_ErrorString, "'elif', 'else' or 'endi' expected");
			break;
	case ELSE_BLOCK:
            strcpy(m_ErrorString, "'endi' expected");
			break;
	case LOOP_BLOCK:
            strcpy(m_ErrorString, "'untl' or 'ever' expected");
			break;
	case ENUMERATOR_BLOCK:
            strcpy(m_ErrorString, "'next' expected");
			break;
    } // switch
    return PARSE_FAIL_GENERAL;
}



void CMacroParse::TokenizeString(char * StringToTokenize)
{
	char * CurrentPosition;
    char TokenBuffer[MAX_TOKEN_SIZE];
    char c;
    int TokenListSize;
	TokenStructure *Token;
	int i;
	BOOL terminated;
    
    m_TokenCount = 0;
	if (*StringToTokenize == 0) 
		// No tokens, so return
		return;
	CurrentPosition = StringToTokenize;
    // Create initial arrays
    TokenListSize = DEFAULT_TOKEN_COUNT;
    m_TokenList.SetSize(TokenListSize);
   
    while (*CurrentPosition != 0) {
		// Ensure that we have enough space in the arrays
        if (m_TokenCount >= TokenListSize) {
			TokenListSize = 2 * TokenListSize;
            m_TokenList.SetSize(TokenListSize);
		}

        // Get next non-tab, non-space character
        while ((c = *CurrentPosition) != 0) {
            if ((c == ' ') || (c == '\t'))
                CurrentPosition++;
            else
                break;   
        }

        if (*CurrentPosition == 0)
            // Reached the end of the string
            break;
     
        if (c == '\r') {
            // We've hit a newline (ie "\r\n"), so store it as a token
			Token = new TokenStructure;
			Token->PositionBefore = (int)(CurrentPosition - StringToTokenize);
			Token->PositionAfter = -1;
			Token->Name = new char[3];
			Token->Name[0] = '\r';
			Token->Name[1] = '\n';
			Token->Name[2] = '\0';
			Token->Size = 2;
			Token->Value = 0;
			Token->TokenClass = NEWLINE_TOKEN;
            m_TokenList[m_TokenCount] = Token;
			m_TokenCount++;
            CurrentPosition += 2;
		}
        else if (c == '*') {
			// We've hit a comment
			Token = new TokenStructure;
			Token->PositionBefore = (int)(CurrentPosition - StringToTokenize);
			Token->PositionAfter = -1;
			TokenBuffer[0] = c;
			i = 1;
			CurrentPosition++;
			while ((c = *CurrentPosition) != 0) {
				if (c == '\r') {
					// We've hit the end of the line which terminates the comment
					break;
				}
				else {
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;
				} 
			}  
			TokenBuffer[i] = '\0';	
			Token->Name = new char[i+1];
			memcpy(Token->Name, TokenBuffer, i+1);
			Token->Size = i;
			Token->Value = 0;
			Token->TokenClass = COMMENT_TOKEN;
			m_TokenList[m_TokenCount] = Token;
			m_TokenCount++;
		}

        else if (c == '[') {
            // We've hit a byte string
			Token = new TokenStructure;
			Token->PositionBefore = (int)(CurrentPosition - StringToTokenize);	
			Token->PositionAfter = -1;
			TokenBuffer[0] = c;
			i = 1;
			terminated = FALSE;
			CurrentPosition++;
			while ((c = *CurrentPosition) != 0) {
				if (c == '\r') {
					// We've hit a newline within the byte string
					break;
				}
				else if (c == ']') {
					// We've hit the end of the byte string
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;
					terminated = TRUE;
					break;
				}
				else {
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;
				}
			}  
			TokenBuffer[i] = '\0';
			Token->Name = new char[i+1];
			memcpy(Token->Name, TokenBuffer, i+1);
			Token->Size = i;
			Token->Value = 0;
			if (terminated) 
				Token->TokenClass = BYTE_STRING_TOKEN;
			else
				Token->TokenClass = UNCLASSIFIED_TOKEN;
			m_TokenList[m_TokenCount] = Token;
			m_TokenCount++;
        }
		else if (c == '"') {
            // We've hit a string constant
			Token = new TokenStructure;
			Token->PositionBefore = (int)(CurrentPosition - StringToTokenize);	
			Token->PositionAfter = -1;
			TokenBuffer[0] = c;
			i = 1;
			terminated = FALSE;
			CurrentPosition++;
			while ((c = *CurrentPosition) != 0) {
				if (c == '\r') {
					// We've hit a newline within the string constant
					break;
				}
				else if (c == '"') {
					// We've hit the end of the string constant
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;

					int backslashcount = 0;
					int j=i-2;
					while ((j>0) && (TokenBuffer[j--] == '\\'))
						backslashcount++;

					if ((backslashcount % 2) == 0)
						terminated = TRUE;
					else
						continue;
					break;
				}
				else {
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;
				}
			}  
			TokenBuffer[i] = '\0';
			Token->Name = new char[i+1];
			memcpy(Token->Name, TokenBuffer, i+1);
			Token->Size = i;
			Token->Value = 0;
			if (terminated) 
				Token->TokenClass = STRING_CONSTANT_TOKEN;
			else
				Token->TokenClass = UNCLASSIFIED_TOKEN;
			m_TokenList[m_TokenCount] = Token;
			m_TokenCount++;
        }
		else if (c == '\'') {
            // We've hit a character constant
			Token = new TokenStructure;
			Token->PositionBefore = (int)(CurrentPosition - StringToTokenize);	
			Token->PositionAfter = -1;
			TokenBuffer[0] = c;
			i = 1;
			CurrentPosition++;
			while ((c = *CurrentPosition) != 0) {
				if (c == '\r') {
					// We've hit a newline within the character constant
					break;
				}
				else if (c == '\'') {
					// We've hit the end of the character constant
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;
					break;
				}
				else {
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;
				}
			}  
			TokenBuffer[i] = '\0';
			Token->Name = new char[i+1];
			memcpy(Token->Name, TokenBuffer, i+1);
			Token->Size = i;
			Token->Value = 0;
			Token->TokenClass = UNCLASSIFIED_TOKEN;
			m_TokenList[m_TokenCount] = Token;
			m_TokenCount++;
        }
		else {
            // We've hit a general token
			Token = new TokenStructure;
			Token->PositionBefore = (int)(CurrentPosition - StringToTokenize);
			Token->PositionAfter = -1;
			TokenBuffer[0] = c;
			i = 1;
			CurrentPosition++;
			while ((c = *CurrentPosition) != 0) {
				if ((c == ' ') || (c == '\t')) {
					// Spaces and tabs terminate the token
	                CurrentPosition++;
					break;
				}
				else if (c == '\r') {
					// Leave CurrentPosition pointing at the newline - it will 
					// be picked up on the next iteration of the outer loop
					break;
				}
				else {
					TokenBuffer[i] = c;
					i++;
					CurrentPosition++;
				} 
			}  
			TokenBuffer[i] = 0;
			Token->Name = new char[i+1];
			memcpy(Token->Name, TokenBuffer, i+1);
			Token->Size = i;
			if (i == 4) {
				// Convert the token into a 32-bit value
				Token->Value = ENCODE(TokenBuffer[0], TokenBuffer[1],
					TokenBuffer[2], TokenBuffer[3]);
			}
			else 
				Token->Value = 0;
			// The token doesn't have a classification yet
			Token->TokenClass = UNCLASSIFIED_TOKEN;
			m_TokenList[m_TokenCount] = Token;
			m_TokenCount++;
		}   
	}
    // Shrink the token list to fit
	m_TokenList.SetSize(m_TokenCount);
}




BOOL CMacroParse::IsDecimalConstant(TokenStructure * Token)
{
	char *endptr;
	char *Name = Token->Name;
	int Size, i;

	Size = Token->Size;
	if (Name[0] == '%') {
		// Check for binary constant
		if ((Size < 2) || (Size > 33))
			return FALSE;
		for (i=1; i<Token->Size; i++) {
			if ((Name[i] != '0') && (Name[i] != '1'))
				return FALSE;
		}
		return TRUE;
	}
	if (Name[0] == '\'') {
		if ((Size < 3) || (Size > 4))
			return FALSE;
		if (Name[Size-1] != '\'')
			return FALSE;
		if (Size == 4) {
			if (Name[1] != '\\')
				return FALSE;
		}
		return TRUE;
	}
	strtod(Token->Name, &endptr);
	if (*endptr != '\0')
		return FALSE;
	else
		return TRUE;
}



BOOL CMacroParse::IsStringConstant(TokenStructure * Token)
{
	if (Token->TokenClass == STRING_CONSTANT_TOKEN) 
		return TRUE;
	else
		return FALSE;
}


BOOL CMacroParse::IsByteString(TokenStructure * Token)
{
	if (Token->TokenClass == BYTE_STRING_TOKEN) 
		return TRUE;
	else
		return FALSE;
}


BOOL CMacroParse::IsRelativeOperator(TokenStructure * Token)
{
	char *Name;

	if (Token->Size > 2)
		return FALSE;
	Name = Token->Name;
	if (Token->Size == 1) {
		if ((Name[0] == '=') || (Name[0] == '<') || (Name[0] == '>'))
			return TRUE;
		else
			return FALSE;
	}
	else if (((Name[0] == 'e') && (Name[1] == 'q')) ||
	    ((Name[0] == 'n') && (Name[1] == 'e')) ||
		((Name[0] == '<') && (Name[1] == '>')) ||
	    ((Name[0] == 'g') && (Name[1] == 't')) ||
	    ((Name[0] == 'l') && (Name[1] == 't')) ||
	    ((Name[0] == 'g') && (Name[1] == 'e')) ||
		((Name[0] == '>') && (Name[1] == '=')) ||
	    ((Name[0] == 'l') && (Name[1] == 'e')) ||
		((Name[0] == '<') && (Name[1] == '=')))
		return TRUE;	
	else
		return FALSE;
}


BOOL CMacroParse::IsLogicalOperator(TokenStructure * Token)
{
	char *Name;

	Name = Token->Name;
	if (Token->Size == 2) {
		if ((Name[0] == 'o') && (Name[1] == 'r'))
			return TRUE;
	}
	else if (Token->Size == 3) {
	    if ((Name[0] == 'a') && (Name[1] == 'n') && (Name[2] == 'd'))
			return TRUE;	
	}
	return FALSE;
} 



BOOL CMacroParse::IsCommandHeader(TokenStructure * Token)
{
	BOOL Found;

	const OpSpec* op = theCAOSDescription.FindCommand(Token->Name);
	if (op == NULL) {
		// Search the block command table
		Found = CheckBlockCommandTable(Token->Value);
	}
	else
		Found = TRUE;
	return Found;
}


TokenStructure * CMacroParse::GetNextKeyword()
{    
    TokenStructure *Token;
    
    // Check for the end of the token stream
	if (m_TokenIndex == m_TokenCount)
        return NULL;
    	
    Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;
    return Token;
}



int CMacroParse::ParseDecimalConstant()
{   
	TokenStructure *Token;
   
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "Decimal constant expected");
        return PARSE_FAIL_END;
    }
    
    Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;
    if (IsDecimalConstant(Token)) {
        Token->TokenClass = DECIMAL_CONSTANT_TOKEN;
        return PARSE_OK;
	}
	else {
        strcpy(m_ErrorString, "Decimal constant expected");
        Token->TokenClass = ERROR_TOKEN;
        return PARSE_FAIL_GENERAL;
    }
}


int CMacroParse::ParseStringConstant()
{   
    TokenStructure *Token;
    
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "String constant expected");
        return PARSE_FAIL_END;
    }
    
    Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;
    if (IsStringConstant(Token)) {
        return PARSE_OK;
	}
	else {
        strcpy(m_ErrorString, "String constant expected");
        Token->TokenClass = ERROR_TOKEN;
        return PARSE_FAIL_GENERAL;
    }
}


int CMacroParse::ParseByteString()
{   
    TokenStructure *Token;
    
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "Byte string expected");
        return PARSE_FAIL_END;
    }
    
    Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;
    if (IsByteString(Token)) {
        return PARSE_OK;
	}
	else {
        strcpy(m_ErrorString, "Byte string expected");
        Token->TokenClass = ERROR_TOKEN;
        return PARSE_FAIL_GENERAL;
    }
}

int CMacroParse::ParseRelativeOperator()
{    
	TokenStructure *Token;

    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "Relational operator expected");
        return PARSE_FAIL_END;
    }
    
	Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;
  
	if (IsRelativeOperator(Token)) {
        Token->TokenClass = RELATIVE_OPERATOR_TOKEN;
        return PARSE_OK;
	}
	else {
        strcpy(m_ErrorString, "Relational operator expected");
        Token->TokenClass = ERROR_TOKEN;
        return PARSE_FAIL_GENERAL;
    }
}



int CMacroParse::ParseLabel()
{
	TokenStructure * Token;
	BOOL OK = TRUE;
    
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "Label expected");
        return PARSE_FAIL_END;
    }
    
	Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;

	if (Token->Name == "")
		OK = FALSE;
	else if (!isalpha((unsigned char)(Token->Name[0])))
		OK = FALSE;

    if (OK) {
		Token->TokenClass = LABEL_TOKEN;
        return PARSE_OK;
	}
	else {
		strcpy(m_ErrorString, "Label expected");
        Token->TokenClass = ERROR_TOKEN;
        return PARSE_FAIL_GENERAL;
    }
}


int CMacroParse::ParseVariable()
{       
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "Variable expected");
        return PARSE_FAIL_END;
    }
    
	TokenStructure * Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;
     
	if (Token->Value != 0) {		
		const OpSpec* op = theCAOSDescription.FindVariable(Token->Name);
		int ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "Variable expected");
		return ParseStatus;
	}

	strcpy(m_ErrorString, "Variable expected");
	Token->TokenClass = ERROR_TOKEN;
	return PARSE_FAIL_GENERAL;
}



int CMacroParse::ParseDecimalRV()
{
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "Decimal r-value expected");
        return PARSE_FAIL_END;
    }
    
	TokenStructure *Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;

	if (IsDecimalConstant(Token)) {
		Token->TokenClass = DECIMAL_CONSTANT_TOKEN;
        return PARSE_OK;
	}
	if (Token->Value != 0)
	{
		int ParseStatus;
		const OpSpec* op;
		
		op = theCAOSDescription.FindVariable(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "Decimal r-value expected");

		if (ParseStatus != PARSE_OK)
		{
			op = theCAOSDescription.FindFloatRV(Token->Name);
			ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "Decimal r-value expected");

			if (ParseStatus != PARSE_OK)
			{
				op = theCAOSDescription.FindIntegerRV(Token->Name);
				ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "Decimal r-value expected");
			}
		}

		return ParseStatus;
	}
	
	strcpy(m_ErrorString, "Decimal r-value expected");
    Token->TokenClass = ERROR_TOKEN;
    return PARSE_FAIL_GENERAL;
}



int CMacroParse::ParseStringRV()
{
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "String r-value expected");
        return PARSE_FAIL_END;
    }
    
	TokenStructure * Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;

	if (IsStringConstant(Token)) {
        return PARSE_OK;
	}
	if (Token->Value != 0)
	{
		int ParseStatus;
		const OpSpec* op;

		op = theCAOSDescription.FindVariable(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "String r-value expected");

		if (ParseStatus != PARSE_OK)
		{
			op = theCAOSDescription.FindStringRV(Token->Name);
			ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "String r-value expected");
		}

		return ParseStatus;
	}
	
	strcpy(m_ErrorString, "String r-value expected");
    Token->TokenClass = ERROR_TOKEN;
    return PARSE_FAIL_GENERAL;
}


int CMacroParse::ParseAgentRV()
{
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "Agent r-value expected");
        return PARSE_FAIL_END;
    }
    
	TokenStructure *Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;

	if (Token->Value != 0)
	{
		int ParseStatus;
		const OpSpec* op;

		op = theCAOSDescription.FindAgentRV(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "Agent r-value expected");

		if (ParseStatus != PARSE_OK)
		{
			op = theCAOSDescription.FindVariable(Token->Name);
			ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "Agent r-value expected");
		}

		return ParseStatus;
	}
	
	strcpy(m_ErrorString, "Agent r-value expected");
    Token->TokenClass = ERROR_TOKEN;
    return PARSE_FAIL_GENERAL;
}


int CMacroParse::ParseAnyRV(int& type)
{
    // Check for the end of the token stream
    if (m_TokenIndex == m_TokenCount) {
        strcpy(m_ErrorString, "R-value expected");
        return PARSE_FAIL_END;
    }
    
	TokenStructure * Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
    m_TokenIndex++;

	if (IsDecimalConstant(Token)) {
		type = 0; // decimal
		Token->TokenClass = DECIMAL_CONSTANT_TOKEN;
        return PARSE_OK;
	}
	if (IsStringConstant(Token)) {
		type = 1; // string
        return PARSE_OK;
	}
	if (Token->Value != 0)
	{
		int ParseStatus;
		const OpSpec* op;
		
		type = 0;
		op = theCAOSDescription.FindFloatRV(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "R-value expected");
		if (ParseStatus == PARSE_OK)
			return PARSE_OK;
		op = theCAOSDescription.FindIntegerRV(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "R-value expected");
		if (ParseStatus == PARSE_OK)
			return PARSE_OK;

		type = 1;
		op = theCAOSDescription.FindStringRV(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "R-value expected");
		if (ParseStatus == PARSE_OK)
			return PARSE_OK;

		type = 2;
		op = theCAOSDescription.FindAgentRV(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "R-value expected");
		if (ParseStatus == PARSE_OK)
			return PARSE_OK;

		type = 3;
		op = theCAOSDescription.FindVariable(Token->Name);
		ParseStatus = ParseOpSpec(Token, LVRV_TOKEN, op, "R-value expected");
		if (ParseStatus == PARSE_OK)
			return PARSE_OK;

		type = -1;

		return ParseStatus;
	}
	
	strcpy(m_ErrorString, "R-value expected");
    Token->TokenClass = ERROR_TOKEN;
    return PARSE_FAIL_GENERAL;
}


int CMacroParse::ParseRelation()
{ 
	int ParseStatus;
	int type;
	int dummy;

	ParseStatus = ParseAnyRV(type);
	if (ParseStatus != PARSE_OK)
		return ParseStatus;
	ParseStatus = ParseRelativeOperator();
	if (ParseStatus != PARSE_OK)
		return ParseStatus;
	if (type == 0)
		ParseStatus = ParseDecimalRV();
	else if (type == 1)	
		ParseStatus = ParseStringRV();
	else if (type == 2)
		ParseStatus = ParseAgentRV();
	else // variable
		ParseStatus = ParseAnyRV(dummy);
	if (ParseStatus != PARSE_OK)
		return ParseStatus;
	return PARSE_OK;
}


int CMacroParse::ParseCondition()
{
	int ParseStatus;
	TokenStructure *Token;

	do {
		ParseStatus = ParseRelation();
		if (ParseStatus != PARSE_OK)
			return ParseStatus;
		Token = GetNextKeyword();
		if (Token == NULL) {
			return PARSE_OK;
		}
		if (IsLogicalOperator(Token)) {
			Token->TokenClass = LOGICAL_OPERATOR_TOKEN;
			continue;
		}
		else if (IsCommandHeader(Token)) {
			break;
		}
		else if (Token->TokenClass == NEWLINE_TOKEN) {
			break;
		}
		else {
			strcpy(m_ErrorString, "Logical operator expected");
			Token->TokenClass = ERROR_TOKEN;
		    return PARSE_FAIL_GENERAL;
		}
	} while (TRUE);		
	m_TokenIndex--;
	return PARSE_OK;
}

int CMacroParse::ParseOpSpec(TokenStructure *Token, int Class, const OpSpec* op, char *OnErrorString)
{
	if (op != NULL)
	{
		Token->TokenClass = Class;
		int ParseStatus = PARSE_OK;
		for(int arg = 0; arg < op->GetParameterCount(); ++arg )
		{
			switch( op->GetParameter(arg) )
			{
				case 'I':
					ParseStatus = ParseDecimalConstant();
					break;
				case 'S':
					ParseStatus = ParseStringConstant();
					break;
				case 'v':
					ParseStatus = ParseVariable();
					break;
				case 'i':
				case 'd':
				case 'f':
					ParseStatus = ParseDecimalRV();
					break;
				case 's':
					ParseStatus = ParseStringRV();
					break;
				case 'a':
					ParseStatus = ParseAgentRV();
					break;
				case 'b':
					ParseStatus = ParseByteString();
					break;
				case 'm':
					int dummy;
					ParseStatus = ParseAnyRV(dummy);
					break;
				case '#':
					ParseStatus = ParseLabel();
					break;
				case '*':
					{
						// we don't support any other arguments with a sub-command argument yet
						ASSERT(arg == 0 && op->GetParameterCount() == 1); 

						int oldToken = m_TokenIndex;
						TokenStructure *Keyword = GetNextKeyword();
						if (Keyword == NULL)
						{
							strcpy(m_ErrorString, OnErrorString);
							return PARSE_FAIL_END;
						}				
						op = theCAOSDescription.FindSubCommand(Keyword->Name, op);
						if (op == NULL)
						{
							m_TokenIndex = oldToken;
							strcpy(m_ErrorString, OnErrorString);
							return PARSE_FAIL_GENERAL;
						}
						else
						{
							ParseStatus = PARSE_OK;
							Keyword->TokenClass = NORMAL_COMMAND_TOKEN;
							arg = -1;
						}
					}
					break;
				case 'c':
					ParseStatus = ParseCondition();
					break;
				default:
					ASSERT(0);
			} // switch
			if (ParseStatus != PARSE_OK)
				return ParseStatus;
		}
		return PARSE_OK;
	}
	else {
		strcpy(m_ErrorString, OnErrorString);
		Token->TokenClass = ERROR_TOKEN;
		return PARSE_FAIL_GENERAL;
	}
}


int CMacroParse::ParseCommand
	(BOOL HandleBlocks, 
	 int & IndentLevel,
	 BOOL & ScriptClass,
	 int FGSEValues[4])
{   
    int ParseStatus, i, StartIndex;
    TokenStructure *Keyword;
	char *Family, *Genus, *Species, *Event;
	char *Dummy;

	if (HandleBlocks) {
		// Return current settings as default
		IndentLevel = m_IndentLevel;
		ScriptClass = m_ScriptClass;
		for (i=0; i<4; i++) 
			FGSEValues[i] = m_FGSEValues[i];
	}

    Keyword = GetNextKeyword();
    if (Keyword == NULL) {
        strcpy(m_ErrorString, "Command expected");
        return PARSE_FAIL_END;
    }
   
    // Check for blank line
    if (Keyword->TokenClass == NEWLINE_TOKEN) {
        if (HandleBlocks)
            IndentLevel = 0;	
        return PARSE_OK;
    }
    
    // Check for comment
    if (Keyword->TokenClass == COMMENT_TOKEN) {
        if (HandleBlocks)
            IndentLevel = 0;
        return PARSE_OK;
    }
  
    if (Keyword->Value != 0) {
		Keyword->TokenClass = NORMAL_COMMAND_TOKEN;
		if (Keyword->Value == ENCODE('i','s','c','r')) {
			if (HandleBlocks) {
				if (m_IndentLevel != 0) {
					strcpy(m_ErrorString, "Command cannot be nested inside other blocks");
					Keyword->TokenClass = ERROR_TOKEN;
					return PARSE_FAIL_GENERAL;
				}
				if (m_InstallScriptAlreadySeen) {
					strcpy(m_ErrorString, "Only one install script allowed");
					Keyword->TokenClass = ERROR_TOKEN;
					return PARSE_FAIL_GENERAL;
				}
				m_InstallScriptAlreadySeen = TRUE;
				m_ScriptClass = INSTALL_SCRIPT;
				ScriptClass = INSTALL_SCRIPT;		
				m_FGSEValues[0] = -1;
				m_FGSEValues[1] = -1;
				m_FGSEValues[2] = -1;
				m_FGSEValues[3] = -1;
				FGSEValues[0] = -1;
				FGSEValues[1] = -1;
				FGSEValues[2] = -1;
				FGSEValues[3] = -1;
			}
			return PARSE_OK;
		}

		else if (Keyword->Value == ENCODE('r','s','c','r')) {
			if (HandleBlocks) {
				if (m_IndentLevel != 0) {
					strcpy(m_ErrorString, "Command cannot be nested inside other blocks");
					Keyword->TokenClass = ERROR_TOKEN;
					return PARSE_FAIL_GENERAL;
				}
				if (m_RemoveScriptAlreadySeen) {
					strcpy(m_ErrorString, "Only one remove script allowed");
					Keyword->TokenClass = ERROR_TOKEN;
					return PARSE_FAIL_GENERAL;
				}
				m_RemoveScriptAlreadySeen = TRUE;
				m_ScriptClass = REMOVE_SCRIPT;
				ScriptClass = REMOVE_SCRIPT;		
				m_FGSEValues[0] = -1;
				m_FGSEValues[1] = -1;
				m_FGSEValues[2] = -1;
				m_FGSEValues[3] = -1;
				FGSEValues[0] = -1;
				FGSEValues[1] = -1;
				FGSEValues[2] = -1;
				FGSEValues[3] = -1;
			}
			return PARSE_OK;
		}

		else if (Keyword->Value == ENCODE('s','c','r','p')) {
			StartIndex = m_TokenIndex;
			for (i=0; i<4; i++) {
				ParseStatus = ParseDecimalConstant();
				if (ParseStatus != PARSE_OK)
					return ParseStatus;
			}
			if (HandleBlocks) {
				if (m_IndentLevel != 0) {
					strcpy(m_ErrorString, "Command cannot be nested inside other blocks");
					Keyword->TokenClass = ERROR_TOKEN;
					m_TokenIndex -= 4;
					return PARSE_FAIL_GENERAL;
				}
				m_ScriptClass = EVENT_SCRIPT;
				ScriptClass = EVENT_SCRIPT;
				// Store the script's family/genus/species/event IDs
				Family = ((TokenStructure *)(m_TokenList[StartIndex]))->Name;
				Genus = ((TokenStructure *)(m_TokenList[StartIndex+1]))->Name;
				Species = ((TokenStructure *)(m_TokenList[StartIndex+2]))->Name;
				Event = ((TokenStructure *)(m_TokenList[StartIndex+3]))->Name;
				m_FGSEValues[0] = strtol(Family, &Dummy, 10);
				m_FGSEValues[1] = strtol(Genus, &Dummy, 10);
				m_FGSEValues[2] = strtol(Species, &Dummy, 10);
				m_FGSEValues[3] = strtol(Event, &Dummy, 10);
				for (i=0; i<4; i++) 
					FGSEValues[i] = m_FGSEValues[i];
				// Enter a new script block
				BeginBlock(SCRIPT_BLOCK);
			}
			m_EventScriptCount++;
			return PARSE_OK;
		}

		// Check for tokens outside of install, event and remove scripts
		// Note that comments and blank lines are allowed outside
		if (m_ScriptClass == OUTSIDE_SCRIPT) {
			strcpy(m_ErrorString, "Token is not inside a script");
			Keyword->TokenClass = ERROR_TOKEN;
			return PARSE_FAIL_GENERAL;
		}

		switch (Keyword->Value) {
			
		// Deal with block-structure commands

		case ENCODE('e','n','d','m'):
			if (HandleBlocks) {
				// Allow an "endm" at the end of the install script
				// (It doesn't have to be there though)
				if ((m_IndentLevel == 0) && 
					(m_ScriptClass == INSTALL_SCRIPT)) {
					m_ScriptClass = OUTSIDE_SCRIPT;
					ScriptClass = INSTALL_SCRIPT;
					m_FGSEValues[0] = -1;
					m_FGSEValues[1] = -1;
					m_FGSEValues[2] = -1;
					m_FGSEValues[3] = -1;
					FGSEValues[0] = -1;
					FGSEValues[1] = -1;
					FGSEValues[2] = -1;
					FGSEValues[3] = -1;
					return PARSE_OK;
				}

				// Allow an "endm" at the end of the remove script
				// (It doesn't have to be there though)
				if ((m_IndentLevel == 0) && 
					(m_ScriptClass == REMOVE_SCRIPT)) {
					m_ScriptClass = OUTSIDE_SCRIPT;
					ScriptClass = REMOVE_SCRIPT;
					m_FGSEValues[0] = -1;
					m_FGSEValues[1] = -1;
					m_FGSEValues[2] = -1;
					m_FGSEValues[3] = -1;
					FGSEValues[0] = -1;
					FGSEValues[1] = -1;
					FGSEValues[2] = -1;
					FGSEValues[3] = -1;
					return PARSE_OK;
				}

				ParseStatus = EndBlock(ENCODE('e','n','d','m'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
			}
			m_ScriptClass = OUTSIDE_SCRIPT;
			m_FGSEValues[0] = -1;
			m_FGSEValues[1] = -1;
			m_FGSEValues[2] = -1;
			m_FGSEValues[3] = -1;
			return PARSE_OK;

		case ENCODE('r','e','p','s'):
			ParseStatus = ParseDecimalRV();
			if (ParseStatus != PARSE_OK)
				return ParseStatus;
			if (HandleBlocks)
				BeginBlock(REPEAT_BLOCK);
			return PARSE_OK;

		case ENCODE('r','e','p','e'):
			if (HandleBlocks) {		
				ParseStatus = EndBlock(ENCODE('r','e','p','e'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}			
			}
			return PARSE_OK;

		case ENCODE('s','u','b','r'):
			ParseStatus = ParseLabel();
			if (ParseStatus != PARSE_OK)
				return ParseStatus;
			if (HandleBlocks)
				BeginBlock(SUBROUTINE_BLOCK);
			return PARSE_OK;

		case ENCODE('r','e','t','n'):
			if (HandleBlocks) {
				ParseStatus = EndBlock(ENCODE('r','e','t','n'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
			}
			return PARSE_OK;

		case ENCODE('d','o','i','f'):
			ParseStatus = ParseCondition();
			if (ParseStatus != PARSE_OK)
				return ParseStatus;
			if (HandleBlocks)
				BeginBlock(IF_BLOCK);
			return PARSE_OK;

		case ENCODE('e','l','i','f'):
			ParseStatus = ParseCondition();
			if (ParseStatus != PARSE_OK)
				return ParseStatus;
			if (HandleBlocks) {
				ParseStatus = EndBlock(ENCODE('e','l','i','f'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
				BeginBlock(ELIF_BLOCK);
			}
			return PARSE_OK;

		case ENCODE('e','l','s','e'):
			if (HandleBlocks) {
				ParseStatus = EndBlock(ENCODE('e','l','s','e'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
				BeginBlock(ELSE_BLOCK);
			}
			return PARSE_OK;

		case ENCODE('e','n','d','i'):
			if (HandleBlocks) {
				ParseStatus = EndBlock(ENCODE('e','n','d','i'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
			}
			return PARSE_OK;

		case ENCODE('l','o','o','p'):
			if (HandleBlocks)
				BeginBlock(LOOP_BLOCK);
			return PARSE_OK;

		case ENCODE('u','n','t','l'):
			ParseStatus = ParseCondition();
			if (ParseStatus != PARSE_OK)
				return ParseStatus;
			if (HandleBlocks) {
				ParseStatus = EndBlock(ENCODE('u','n','t','l'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
			}
			return PARSE_OK;

		case ENCODE('e','v','e','r'):
			if (HandleBlocks) {
				ParseStatus = EndBlock(ENCODE('e','v','e','r'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
			}
			return PARSE_OK;

		case ENCODE('e','n','u','m'): 
		case ENCODE('e','t','c','h'): 
		case ENCODE('e','s','e','e'):
		case ENCODE('e','p','a','s'):
			for (i=0; i<3; i++) {
				ParseStatus = ParseDecimalRV();
				if (ParseStatus != PARSE_OK)
					return ParseStatus;
			}
			if (HandleBlocks)
				BeginBlock(ENUMERATOR_BLOCK);
			return PARSE_OK;

		case ENCODE('e','c','o','n'):
  		    ParseStatus = ParseAgentRV();
			if (ParseStatus != PARSE_OK)
					return ParseStatus;
			if (HandleBlocks)
				BeginBlock(ENUMERATOR_BLOCK);
			return PARSE_OK;
		case ENCODE('n','e','x','t'):
			if (HandleBlocks) {
				ParseStatus = EndBlock(ENCODE('n','e','x','t'));
				IndentLevel = m_IndentLevel;
				if (ParseStatus != PARSE_OK) {
					Keyword->TokenClass = ERROR_TOKEN;
					return ParseStatus;
				}
			}
			return PARSE_OK;
		} // switch

		const OpSpec* op = theCAOSDescription.FindCommand(Keyword->Name);
		ParseStatus = ParseOpSpec(Keyword, NORMAL_COMMAND_TOKEN, op, "Command expected");
		return ParseStatus;
	}

	// Check for being outside of a script
	if (m_ScriptClass == OUTSIDE_SCRIPT) {
		strcpy(m_ErrorString, "Token is not inside a script");
		Keyword->TokenClass = ERROR_TOKEN;
		return PARSE_FAIL_GENERAL;
	}
	strcpy(m_ErrorString, "Command expected");
	Keyword->TokenClass = ERROR_TOKEN;
	return PARSE_FAIL_GENERAL;
}



void CMacroParse::Parse(char * MacroCode)
{   
	int CommandBufferSize;
    int ErrorBufferSize;
    int StartIndex;
    int EndIndex;
	int TempTokenIndex;
    int IndentLevel;
    BOOL ScriptClass;
    int ParseStatus;
    int i;
	int Tab;
	int TokenClass;
	CommandStructure *Command;
	TokenStructure *Token;
	TokenStructure *EndToken;
	TokenStructure *NextToken;
	int Len, HeaderLen, TrailerLen;
	int PositionAfter;
	static char RTFLine[MAX_RTF_LINE];
	char *RTFTemp;
	int FGSEValues[4];
	int LineNumber;

    // Clean up any memory used by the last parse
	Cleanup();
	// Re-initialize member variables
	Initialize();

	LineNumber = 0;

    // Break the macro code into discrete tokens
    TokenizeString(MacroCode);

    if (m_TokenCount == 0)
        return;

	PositionAfter = 0;
    CommandBufferSize = DEFAULT_COMMAND_COUNT;
    m_CommandList.SetSize(CommandBufferSize);
    ErrorBufferSize = DEFAULT_ERROR_COUNT;
    m_ErrorIndexList.SetSize(ErrorBufferSize);
        
    do {
        StartIndex = m_TokenIndex;
        ParseStatus = ParseCommand(TRUE, IndentLevel, ScriptClass, FGSEValues);
        EndIndex = m_TokenIndex - 1;
                
        if (ParseStatus == PARSE_OK) {
			// Clear the error buffer
            m_ErrorString[0] = '\0';
            // For non-blank lines, skip past a following newline
            if (m_TokenIndex < m_TokenCount) {
				EndToken = (TokenStructure *)(m_TokenList[EndIndex]);
				NextToken = (TokenStructure *)(m_TokenList[m_TokenIndex]);
                if ((EndToken->Name[0] != '\r') && 
					(NextToken->Name[0] == '\r')) {
                    m_TokenIndex++;
				}
			}
		}
        else if (ParseStatus == PARSE_FAIL_GENERAL) {
			Token = (TokenStructure *)(m_TokenList[EndIndex]);
            if (Token->Name[0] != '\r') {
                sprintf(m_ErrorString, "%s, got '%s'", m_ErrorString, Token->Name);
				// Recover from the syntax error
				TempTokenIndex = m_TokenIndex;
				while (TempTokenIndex < m_TokenCount) {
					Token = (TokenStructure *)(m_TokenList[TempTokenIndex]);
					if (Token->Name[0] == '\r') {
						EndIndex = TempTokenIndex-1;
						m_TokenIndex = TempTokenIndex+1;
						break;
					}
					else if (Token->TokenClass == COMMENT_TOKEN) {
						EndIndex = TempTokenIndex-1;
						m_TokenIndex = TempTokenIndex;
						break;
					}
					else if (IsCommandHeader(Token)) {
						EndIndex = TempTokenIndex-1;
						m_TokenIndex = TempTokenIndex;
						break;
					}
					else {
						// Mark the token as being part of a bad command
						Token->TokenClass = BAD_TOKEN;
						EndIndex++;
						TempTokenIndex++;
						m_TokenIndex++;
					}
				}        
			}
			else {
				// A newline was the "bad" token in a command, but we
                // can't highlight a newline, so don't make it part of
                // the command
                strcat(m_ErrorString, ", got end-of-line");
                EndIndex--;
            }
        }
		else // PARSE_FAIL_END
			// Ran out of tokens
			sprintf(m_ErrorString, "%s, got end-of-source", m_ErrorString);
		
        if (m_CommandCount >= CommandBufferSize) {
            // Ran out of space, so double the size of the command buffer
            CommandBufferSize = CommandBufferSize * 2;
            m_CommandList.SetSize(CommandBufferSize);
        }

		Command = new CommandStructure;
		Command->StartIndex = StartIndex;
        Command->EndIndex = EndIndex;
        Command->ParseStatus = ParseStatus;
		if (ParseStatus == PARSE_OK)
			Command->ErrorString = NULL;
		else {
			Command->ErrorString = new char [strlen(m_ErrorString)+1];
			strcpy(Command->ErrorString, m_ErrorString);
		}
        Command->IndentLevel = IndentLevel;
        Command->ScriptClass = ScriptClass;
        for (i=0; i<4; i++)
            Command->FGSEValues[i] = FGSEValues[i];
		Command->LineNumber = m_CommandCount+1;

		//
		// Calculate the RTF representation of this command line
		//

		RTFTemp = RTFLine;		
		Token = (TokenStructure *)(m_TokenList[StartIndex]);
		memcpy(RTFTemp, "\\plain\\f0\\fs20\\cf5 ", 19); 
		RTFTemp += 19;
		
		if (Token->TokenClass == NEWLINE_TOKEN) {
			// Got a blank line
			memcpy(RTFTemp, "\\line", 5);
			RTFTemp += 5;
			Token->PositionAfter = PositionAfter;
			PositionAfter += 2;
		}
		else {
			// Write a tab for each indent level
			for (Tab=0; Tab<IndentLevel; Tab++) {
				memcpy(RTFTemp, "\\tab ", 5); 
				RTFTemp += 5;
			}
			PositionAfter += IndentLevel;
			for (i=StartIndex; i<=EndIndex; i++) {
				Token = (TokenStructure *)(m_TokenList[i]);
				memcpy(RTFTemp, "\\plain\\f0\\fs20\\cf", 17);
				RTFTemp += 17;
				TokenClass = Token->TokenClass;
				switch (TokenClass) {
				case STRING_CONSTANT_TOKEN:
				case BYTE_STRING_TOKEN:
				case DECIMAL_CONSTANT_TOKEN:
					if (ParseStatus != PARSE_OK) {
						// Red
						memcpy(RTFTemp, "1 ", 2);
						RTFTemp += 2;
					}
					else {
						// Brown
						memcpy(RTFTemp, "0 ", 2);
						RTFTemp += 2;
					}
					break;
				case COMMENT_TOKEN:
					if (ParseStatus != PARSE_OK) {
						// Red
						memcpy(RTFTemp, "1 ", 2);
						RTFTemp += 2;
					}
					else {
						// Green
						memcpy(RTFTemp, "4 ", 2);
						RTFTemp += 2;
					}
					break;
				case LVRV_TOKEN: 
				case LABEL_TOKEN:
				case RELATIVE_OPERATOR_TOKEN:
				case LOGICAL_OPERATOR_TOKEN:
					if (ParseStatus != PARSE_OK) {
						// Red
						memcpy(RTFTemp, "1 ", 2);
						RTFTemp += 2;
					}
					else {
						// Purple
						memcpy(RTFTemp, "2 ", 2);
						RTFTemp += 2;
					}
					break;
				case NORMAL_COMMAND_TOKEN:
					if (ParseStatus != PARSE_OK) {
						// Red
						memcpy(RTFTemp, "1 ", 2);
						RTFTemp += 2;
					}
					else {
						// Blue
						memcpy(RTFTemp, "3 ", 2);
						RTFTemp += 2;
					}
					break;
				case BAD_TOKEN:
					// Red
					memcpy(RTFTemp, "1 ", 2);
					RTFTemp += 2;
					break;
				case ERROR_TOKEN:
					// Bold red
					memcpy(RTFTemp, "1\\b ", 4);
					RTFTemp += 4;
					break;						
				} // switch
				
				if (TokenClass == STRING_CONSTANT_TOKEN)
				{
					Len = Token->Size;
					for (int i = 0; i < Len; ++i)
					{
						*RTFTemp = *(Token->Name + i);
						RTFTemp++;

						// escape \ characters for rich edit control
						if (*(Token->Name + i) == '\\') {
							*RTFTemp = *(Token->Name + i);
							RTFTemp++;
						}
					}
				}
				else
				{
					Len = Token->Size;
					memcpy(RTFTemp, Token->Name, Len);
					RTFTemp += Len;
				}

				if (i < EndIndex) {
					*RTFTemp = ' ';
					RTFTemp++;
					Token->PositionAfter = PositionAfter;
					PositionAfter += Len + 1;
				}
				else {
					memcpy(RTFTemp, "\\line", 5);
					RTFTemp += 5;
					Token->PositionAfter = PositionAfter;
					PositionAfter += Len + 2;
				}
			}
		}
		*RTFTemp = '\0';

		// Deal with newlines
		if (m_TokenIndex > EndIndex+1) {
			Token = (TokenStructure *)(m_TokenList[m_TokenIndex-1]);
			Token->PositionAfter = PositionAfter-2;
		}

		Len = (int)(RTFTemp - RTFLine);
		Command->RTFString = new char[Len+1];
		memcpy(Command->RTFString, RTFLine, Len+1);
		Command->RTFStringSize = Len;
		m_RTFStringSizeTotal += Len;	
		m_CommandList[m_CommandCount] = Command;

        if (ParseStatus != PARSE_OK) {
            if (m_ErrorCount >= ErrorBufferSize) {
                // Ran out of space, so double the size of the error index buffer
                ErrorBufferSize = ErrorBufferSize * 2;
                m_ErrorIndexList.SetSize(ErrorBufferSize);
			}
            // Store the index of the bad command
            m_ErrorIndexList[m_ErrorCount] = m_CommandCount;
			m_ErrorCount++;
        }
		m_CommandCount++;
	} while (m_TokenIndex < m_TokenCount);
    
	LineNumber = m_CommandCount;

    if (m_IndentLevel > 0) {
        // We've reached the end of the macro code but are still within
        // nested blocks, so discover which ones
        do {        
            if (m_CommandCount >= CommandBufferSize) {
                CommandBufferSize = CommandBufferSize * 2;
                m_CommandList.SetSize(CommandBufferSize);
            }
			Command = new CommandStructure;
			Command->ParseStatus = EndBlock(0);
			strcat(m_ErrorString, ", got end-of-source");
			Command->ErrorString = new char [strlen(m_ErrorString)+1];
			strcpy(Command->ErrorString, m_ErrorString);
			Command->StartIndex = -1;
			Command->EndIndex = -1;
			Command->IndentLevel = -1;
			Command->ScriptClass = OUTSIDE_SCRIPT;
			for (i=0; i<4; i++) 
				Command->FGSEValues[i] = -1;
			Command->LineNumber = LineNumber;
			Command->RTFString = NULL;
			Command->RTFStringSize = 0;
			m_CommandList[m_CommandCount] = Command;
            if (m_ErrorCount >= ErrorBufferSize) {
                ErrorBufferSize = ErrorBufferSize * 2;
                m_ErrorIndexList.SetSize(ErrorBufferSize);
			}
            m_ErrorIndexList[m_ErrorCount] = m_CommandCount;
			m_ErrorCount++;
			m_CommandCount++;

		    // Move out to the next nested block
            m_IndentLevel--;
		} while (m_IndentLevel > 0);
	}
    // Shrink the arrays to fit
    m_CommandList.SetSize(m_CommandCount);
    m_ErrorIndexList.SetSize(m_ErrorCount);

	// Create an RTF string for all the commands
	HeaderLen = strlen(RTF_HEADER);
	TrailerLen = strlen(RTF_TRAILER);
	Len = HeaderLen + m_RTFStringSizeTotal + TrailerLen;
	m_RTF = new char[Len+1];
	RTFTemp = m_RTF;
	memcpy(RTFTemp, RTF_HEADER, HeaderLen);
	RTFTemp += HeaderLen;
	m_RTFBody = m_RTF + HeaderLen;
	for (i=0; i<m_CommandCount; i++) {
		Command = (CommandStructure *)m_CommandList[i];
		if (Command->RTFString != NULL) {
			memcpy(RTFTemp, Command->RTFString, Command->RTFStringSize);
			RTFTemp += Command->RTFStringSize;
		}
	}
	// This is a bit of a trick to allow the extraction of the RTF body
	// from the full RTF string.
	// For the extraction of the full RTF, the trailer can be set to the
	// trailer character (ie '}') and for the extraction of the RTF body
	// it can be set to the string terminator
	*RTFTemp = '\0';
	*(RTFTemp+1) = '\0';
	m_RTFTrailer = RTFTemp;
}



void CMacroParse::GetScriptInformation
	(CPtrArray  & ScriptList,
	 int        & ScriptCount)
{
	ScriptStructure *Script;
	CommandStructure *Command;
	CommandStructure *NextCommand;
	TokenStructure *StartToken;
	TokenStructure *Token;
	int TokenIndex;
	int StartCommand, EndCommand;
	int StartIndex, EndIndex;
	int i, j;
	int NextScriptClass;
	int CurrentScriptClass;
	BOOL InjectBufferComplete;
	char *InjectPtr;
	static char InjectBuffer[MAX_INJECT_SIZE];
	int InjectLength;
	int ScriptNumber;
	int EventScriptNumber;

	if (m_ErrorCount != 0)
		// No script information is returned if there are syntax errors
		return;

	// Clean up any previous script list memory
	for (i=0; i<m_ScriptCount; i++) {
		Script = (ScriptStructure *)m_ScriptList[i];
		if (Script->InjectString != NULL)
			delete [](Script->InjectString);
		delete Script;
	}

	// The script list has two entries minimum: Entry 0 is for the
	// install script, entry 1 is for the remove script. All other
	// entries are for event scripts - if there are any.
	// For install and remove scripts, an InjectString of NULL flags
	// that the script is not present. For all scripts, an InjectString
	// containing the empty string flags that the script was present 
	// but had no commands in it
	EventScriptNumber = 2;
	m_ScriptCount = m_EventScriptCount + 2;
	m_ScriptList.SetSize(m_ScriptCount);
	for (i=0; i<m_ScriptCount; i++) {
		Script = new ScriptStructure;
		Script->InjectString = NULL;
		m_ScriptList[i] = Script;
	}

	InjectPtr = InjectBuffer;	
	StartCommand = 0;
	for (i=0; i<m_CommandCount; i++) {
		Command = (CommandStructure *)m_CommandList[i];
		if (Command->ScriptClass == OUTSIDE_SCRIPT)
			// Ignore comments and blank lines outside of scripts
			continue;

		StartIndex = Command->StartIndex;
		EndIndex = Command->EndIndex;
		StartToken = (TokenStructure *)m_TokenList[Command->StartIndex];
		if ((StartToken->TokenClass != NEWLINE_TOKEN) &&
			(StartToken->TokenClass != COMMENT_TOKEN)) {
			for (TokenIndex=StartIndex; TokenIndex<=EndIndex; TokenIndex++) {
				Token = (TokenStructure *)m_TokenList[TokenIndex];
				if (((Token->Value != ENCODE('e', 'n', 'd', 'm')) &&
					 (Token->Value != ENCODE('i', 's', 'c', 'r')) &&
					 (Token->Value != ENCODE('r', 's', 'c', 'r')))) {
					if (Token->Value != ENCODE('s', 'c', 'r', 'p')) {
						// Add the command's tokens to the current inject 
						// buffer, Note that "iscr" and "rscr" are COS file 
						// delimiters and are not injected
						memcpy(InjectPtr, Token->Name, Token->Size);
						InjectPtr += Token->Size;
						*InjectPtr = ' ';
						InjectPtr++;
					}
					else {
						// Move past the family, genus, species and event IDs
						TokenIndex+=5;
					}
				}
			}
		}

		CurrentScriptClass = Command->ScriptClass;
		if (StartToken->Value == ENCODE('e','n','d','m'))
			// We're at the end of a script, the buffer is complete
			InjectBufferComplete = TRUE;
		else if (i+1 == m_CommandCount)
			// We're on the last command, the buffer is complete
			InjectBufferComplete = TRUE;
		else {
			// Look ahead at the script class of the following command
			NextCommand = (CommandStructure *)m_CommandList[i+1];
			NextScriptClass = NextCommand->ScriptClass;
			if (NextScriptClass == OUTSIDE_SCRIPT)
				InjectBufferComplete = TRUE;
			else if (CurrentScriptClass == NextScriptClass) 
				InjectBufferComplete = FALSE;
			else 
				InjectBufferComplete = TRUE;
		}

		if (InjectBufferComplete) {	
			InjectLength = InjectPtr - InjectBuffer;
			if (InjectLength > 0) {
				// Remove final space
				InjectPtr--;
			}
			*InjectPtr = '\0';
			if (CurrentScriptClass == INSTALL_SCRIPT)
				ScriptNumber = 0;
			else if (CurrentScriptClass == REMOVE_SCRIPT)
				ScriptNumber = 1;
			else {
				ScriptNumber = EventScriptNumber;
				EventScriptNumber++;
			}

			Script = (ScriptStructure *)m_ScriptList[ScriptNumber]; 
			Script->InjectString = new char[InjectLength+1];
			memcpy(Script->InjectString, InjectBuffer, InjectLength+1);
			for (j=0; j<4; j++) {
				Script->FGSEValues[j] = Command->FGSEValues[j];
			}

			// Get ready to start another inject buffer
			InjectPtr = InjectBuffer;
			EndCommand = i;
			Script->StartCommandIndex = StartCommand;
			Script->EndCommandIndex = EndCommand;
			StartCommand = i+1;
		}
	}
	ScriptList.Copy(m_ScriptList);
	ScriptCount = m_ScriptCount;
}



void CMacroParse::GetParseResults
	(CPtrArray  & TokenList,
	 int		& TokenCount,
	 CPtrArray	& CommandList,
	 int		& CommandCount,
	 CUIntArray & ErrorIndexList,
	 int		& ErrorCount)
{
	TokenList.Copy(m_TokenList);
	TokenCount = m_TokenCount;
	CommandList.Copy(m_CommandList);
	CommandCount = m_CommandCount;
	ErrorIndexList.Copy(m_ErrorIndexList);
	ErrorCount = m_ErrorCount;
}



void CMacroParse::ParseLine(char * MacroCode)
{   
	int Dummy1;
	BOOL Dummy2;
	int Dummy3[4];
	int EndIndex;
	int i;
	TokenStructure * Token;

	// Clean up any heap memory used by the last parse
	Cleanup();
	// Re-initialize member variables
	Initialize();

    // Break the macro code into discrete tokens
    TokenizeString(MacroCode);

    if (m_TokenCount == 0) {
		// Empty line
		m_ParseLineStatus = PARSE_OK;
        return;
	}
   
    m_TokenIndex = 0;
    // Parse the command without block handling
    m_ParseLineStatus = ParseCommand(FALSE, Dummy1, Dummy2, Dummy3);
    EndIndex = m_TokenIndex - 1;
            
    if (m_ParseLineStatus == PARSE_OK) {
		// Check for superfluous tokens
		if (m_TokenCount > m_TokenIndex) {
			// Set the first superfluous token to be the error token
			// and all following tokens to be "bad" tokens
			m_ParseLineStatus = PARSE_FAIL_GENERAL;
			Token = (TokenStructure *)(m_TokenList[m_TokenIndex]);
			Token->TokenClass = ERROR_TOKEN;
			sprintf(m_ErrorString, "Superfluous tokens starting at '%s'",
				Token->Name);
			for (i=m_TokenIndex+1; i<m_TokenCount; i++) {
				Token = (TokenStructure *)(m_TokenList[i]);
				Token->TokenClass = BAD_TOKEN;
			}
		}
		else 
			// Clear the error string for completeness
			m_ErrorString[0] = '\0';
	}
    else {
		// Command failed
        if (m_ParseLineStatus == PARSE_FAIL_GENERAL) {
			Token = (TokenStructure *)(m_TokenList[EndIndex]);
            sprintf(m_ErrorString, "%s, got '%s'", m_ErrorString, 
				Token->Name);
        }
		else {
			// Ran out of tokens (must have hit the end of the line)
            strcat(m_ErrorString, ", got end-of-line");
        }
		// Mark all the command's tokens as being "bad"
		for (i=0; i<m_TokenCount; i++) {
			Token = (TokenStructure *)(m_TokenList[i]);
			Token->TokenClass = BAD_TOKEN;
		}
		// If the command failed on a specific token, tben mark that 
		// token as the error token. If it failed because it ran out of
		// tokens (PARSE_FAIL_END) then there is no token to mark so do
		// nowt.
		if (m_ParseLineStatus == PARSE_FAIL_GENERAL) {
			Token = (TokenStructure *)(m_TokenList[EndIndex]);
			Token->TokenClass = ERROR_TOKEN;
		}
	}
}



void CMacroParse::GetParseLineResults
	(CPtrArray & TokenList,
	 int	   & TokenCount,
	 int	   & ParseStatus,
	 char *    & ErrorString)
{
	TokenList.Copy(m_TokenList);
	TokenCount = m_TokenCount;
    ParseStatus = m_ParseLineStatus;
	ErrorString = m_ErrorString;
}



char * CMacroParse::GetRTFHeader() 
{
	return RTF_HEADER;
}


char * CMacroParse::GetRTFTrailer() 
{
	return RTF_TRAILER;
}


char * CMacroParse::GetRTFBody()
{
	if (m_TokenCount != 0) {
		*m_RTFTrailer = '\0';
		return m_RTFBody;
	}
	else
		return "";
}


char * CMacroParse::GetRTF()
{
	if (m_TokenCount != 0) {
		*m_RTFTrailer = '}';
		return m_RTF;
	}
	else
		return RTF_EMPTY;
}


void CMacroParse::Nullify() 
{
	Cleanup();
	Initialize();
}

// static
bool CMacroParse::LoadSyntax(const std::string& filename)
{
	CAOSDescription backup = theCAOSDescription;
	bool result = theCAOSDescription.LoadSyntax(filename);
	if (!result)
		theCAOSDescription = backup;
	return result;
}

