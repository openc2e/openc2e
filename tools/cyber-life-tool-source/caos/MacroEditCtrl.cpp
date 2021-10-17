////////////////////////////////////////////////////////////////////////////////
// MacroEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Caos.h"

//
// Private Member Functions
//

// Static function
DWORD CALLBACK CMacroEditCtrl::InEditStreamCallback
	(DWORD SourceBuffer, 
	 LPBYTE DestinationBuffer, 
	 LONG BytesToBeCopied, 
	 LONG FAR *AmountCopied)
{
	int Copied;
	char c;
	char **ptr;

	ptr = (char **)SourceBuffer;
	Copied = 0;
	do {
		if (Copied == BytesToBeCopied) {
			*AmountCopied = Copied;
			return 0;
		}
		c = **ptr;
		if (c == '\0') {
			// Reached the end of the source buffer
			*AmountCopied = Copied;
			return 0;
		}
		// Copy a byte from the source (the RTF string) to the destination 
		// (the Rich Edit control)
		(*ptr)++;
		DestinationBuffer[Copied] = c;
		Copied++;
	} while (TRUE);
}


// Static function
DWORD CALLBACK CMacroEditCtrl::OutEditStreamCallback
	(DWORD DestinationBuffer, 
	 LPBYTE SourceBuffer, 
	 LONG BytesToBeCopied, 
	 LONG FAR *AmountCopied)
{
	char **ptr = (char **)DestinationBuffer;

	if (BytesToBeCopied == 0) {
		*AmountCopied = 0;
		return 0;
	}
	memcpy(*ptr, SourceBuffer, BytesToBeCopied);
	*ptr += BytesToBeCopied;
	*AmountCopied = BytesToBeCopied;
	return 0;
}




void CMacroEditCtrl::NotifyParentAboutSelectionChange()
{
	if (m_ReadOnly)
		return;

	int StartLineNumber, EndLineNumber;
	CWnd *Parent;

	GetLineNumbers(StartLineNumber, EndLineNumber);
	Parent = GetParent();
	if (Parent != NULL) {
		Parent->SendMessage(WM_SELECTION_CHANGED, StartLineNumber, 
			EndLineNumber);
	}
}


void CMacroEditCtrl::NotifyParentAboutContentChange()
{
	CWnd *Parent = GetParent();
	if ((Parent != NULL) && !m_ContentsChanged) {
		Parent->SendMessage(WM_CONTENTS_EDITED, 0, 0);
	}	
	m_ContentsChanged = TRUE;
}


void CMacroEditCtrl::NotifyParentAboutLineError(int LineNumber)
{
	CWnd *Parent;

	Parent = GetParent();
	if (Parent != NULL) 
		Parent->SendMessage(WM_LINE_ERROR, LineNumber+1, 0);		
}



void CMacroEditCtrl::ParseLine(int LineNumber) 
{
	char LineContents[MAX_LINE_SIZE];
	long Start, End;
	CPtrArray TokenList;
	int TokenCount;
	int ParseStatus;
	char * ErrorString;
	TokenStructure *Token;
	char *Name;
	int Size, Pos, CharIndex, Len;
	int i;

	// Save the current selection
	GetSel(Start, End);

	// Get the length of the given line
	CharIndex = LineIndex(LineNumber);
	Len = LineLength(CharIndex);

	if (Len == 0)
		// A completely blank line, do nothing
		return;

	// Extract the contents of the given line
	GetLine(LineNumber, LineContents, MAX_LINE_SIZE - 4);
	LineContents[Len] = '\0';

	// Parse the given line and get the parse results
	m_Parser.ParseLine(LineContents);
	m_Parser.GetParseLineResults(TokenList, TokenCount, ParseStatus, 
		ErrorString);

	// Colour the tokens based on the parsing results 
	for (i=0; i<TokenCount; i++) {
		Token = (TokenStructure *)(TokenList[i]);
		Size = Token->Size;
		Name = Token->Name;
		Pos = Token->PositionBefore;
		switch (Token->TokenClass) {
		case STRING_CONSTANT_TOKEN:
		case BYTE_STRING_TOKEN:
		case DECIMAL_CONSTANT_TOKEN: 
			// Brown
			m_CharFormat.crTextColor = RGB(128, 64, 64);
			m_CharFormat.dwEffects = 0;
			break;
		case COMMENT_TOKEN:  
			// Green
			m_CharFormat.crTextColor = RGB(0, 128, 0);
			m_CharFormat.dwEffects = 0;
			break;
		case LVRV_TOKEN:    
		case LABEL_TOKEN:
		case RELATIVE_OPERATOR_TOKEN:
		case LOGICAL_OPERATOR_TOKEN:
			// Purple
			m_CharFormat.crTextColor = RGB(200, 0, 200);
			m_CharFormat.dwEffects = 0;
			break;
		case NORMAL_COMMAND_TOKEN:
			// Blue
			m_CharFormat.crTextColor = RGB(0, 0, 255);
			m_CharFormat.dwEffects = 0;
			break;
		case BAD_TOKEN:
			// Red
			m_CharFormat.crTextColor = RGB(255, 0, 0);
			m_CharFormat.dwEffects = 0;
			break;						
		case ERROR_TOKEN: 
			// Bold red
			m_CharFormat.crTextColor = RGB(255, 0, 0);
			m_CharFormat.dwEffects = CFM_BOLD;
			break;
		default:
			ASSERT(0);
		} // switch
		// Colour the token
		SetSel(CharIndex+Pos, CharIndex+Pos+Size);
		SetSelectionCharFormat(m_CharFormat);
	}

	if (ParseStatus == PARSE_OK)
		// Command line is OK
		m_ErrorString = NULL;
	else {
		// Command line failed
		m_ErrorString = ErrorString;
		NotifyParentAboutLineError(LineNumber);
	}

	// Restore the selection
	SetSel(Start, End);
}



void CMacroEditCtrl::ParseChangedLine(int LineNumber) 
{
	// If no line has actually changed then do nothing
	if (m_ChangedLineNumber == -1) 
		return;

	// Don't parse the line until the cursor has moved off it
	if (LineNumber == m_ChangedLineNumber)
		return;

	// Parse the changed line
	ParseLine(m_ChangedLineNumber);		
	// Flag that no line is changed
	m_ChangedLineNumber = -1;
}



//
// Public Member Functions
//


// Constructor
CMacroEditCtrl::CMacroEditCtrl()
{
	m_CharFormat.cbSize = sizeof(CHARFORMAT);
	m_CharFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE |
		CFM_STRIKEOUT | CFM_PROTECTED | CFM_SIZE |
		CFM_COLOR | CFM_FACE | CFM_OFFSET | CFM_CHARSET;
	m_CharFormat.dwEffects = 0;
	m_CharFormat.yHeight = 200;
	m_CharFormat.yOffset = 0;
	m_CharFormat.bCharSet = ANSI_CHARSET;
	m_CharFormat.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
	strcpy(m_CharFormat.szFaceName, "Courier New");
	m_CharFormat.crTextColor = RGB(0,0,0);

	m_DefaultCharFormat.cbSize = sizeof(CHARFORMAT);
	m_DefaultCharFormat.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE |
		CFM_STRIKEOUT | CFM_PROTECTED | CFM_SIZE |
		CFM_COLOR | CFM_FACE | CFM_OFFSET | CFM_CHARSET;
	m_DefaultCharFormat.dwEffects = 0;
	m_DefaultCharFormat.yHeight = 200;
	m_DefaultCharFormat.yOffset = 0;
	m_DefaultCharFormat.bCharSet = ANSI_CHARSET;
	m_DefaultCharFormat.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
	strcpy(m_DefaultCharFormat.szFaceName, "Courier New");
	m_DefaultCharFormat.crTextColor = RGB(0,0,0);

	// A changed line number of -1 means that there is no changed line
	m_ChangedLineNumber = -1;
	m_ErrorString = NULL;	
	m_ContentsChanged = FALSE;
	m_DoingBlockIndent = FALSE;
}


// Destructor
CMacroEditCtrl::~CMacroEditCtrl()
{	
	// Nothing extra to do
}

BOOL CMacroEditCtrl::CreateVersion10(const RECT& rect, CWnd* pParentWnd, UINT nID, bool bReadOnly)
{
	m_ReadOnly = bReadOnly;
	BOOL ok = CRichEditCtrl::Create(WS_HSCROLL | WS_VSCROLL | ES_WANTRETURN | 
		ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
		WS_CHILD | WS_VISIBLE | (bReadOnly ? ES_READONLY : 0), rect, pParentWnd, nID);
	ASSERT(ok);
	SetEventMask(ENM_CHANGE);
	LimitText(500000);
	SetDefaultCharFormat(m_DefaultCharFormat);
	NotifyParentAboutSelectionChange();
	CCaosApp::DoBackgroundColour(*this);
	return ok;
}

BOOL CMacroEditCtrl::CreateVersion20(const RECT& rect, CWnd* pParentWnd, UINT nID, bool bReadOnly)
{
	m_ReadOnly = bReadOnly;
	BOOL ok = CreateRichEdit20(WS_HSCROLL | WS_VSCROLL | ES_WANTRETURN | 
		ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
		WS_CHILD | WS_VISIBLE | (bReadOnly ? ES_READONLY : 0), rect, pParentWnd, nID);
	ASSERT(ok);
	SetEventMask(ENM_CHANGE);
	LimitText(500000);
	SetDefaultCharFormat(m_DefaultCharFormat);
	int val = SendMessage(EM_SETUNDOLIMIT, 2048, 0);
	SendMessage(EM_AUTOURLDETECT, TRUE, 0);
	NotifyParentAboutSelectionChange();
	CCaosApp::DoBackgroundColour(*this);
	return ok;
}

BOOL CMacroEditCtrl::CreateRichEdit20(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!AfxInitRichEdit())
		return FALSE;

	CWnd* pWnd = this;
	BOOL bOK = pWnd->Create(RICHEDIT_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
	ASSERT(bOK);

	return bOK;
}

void CMacroEditCtrl::SetContentsFromMacroCode
	(char *       MacroCode,
	 CPtrArray  & TokenList,
	 int        & TokenCount,
	 CPtrArray  & CommandList,
	 int        & CommandCount,
	 CUIntArray & ErrorIndexList,
	 int        & ErrorCount)
{
	int BadCommand;
	int NewPosition;
	CommandStructure *Command;
	TokenStructure *Token;
	char *RTF;
	EDITSTREAM EditStream;

	// Parse the macro code and get the parsing results
	m_Parser.Parse(MacroCode);
	m_Parser.GetParseResults(TokenList, TokenCount, CommandList, CommandCount,
		ErrorIndexList, ErrorCount);

	// Get the RTF string for the macro code
	RTF = m_Parser.GetRTF();
	// Stream the RTF string into the control
 	EditStream.dwCookie = (DWORD)(&RTF);
	EditStream.pfnCallback = InEditStreamCallback;
	StreamIn(SF_RTF, EditStream);
	ShowScrollBar(SB_VERT, TRUE);

	if (ErrorCount > 0) {
		// Get the first bad command
		BadCommand = ErrorIndexList[0];
		Command = (CommandStructure *)CommandList[BadCommand];
		if (Command->IndentLevel >= 0) {
			// Move the cursor to the first token in the first bad command
			Token = (TokenStructure *)(TokenList[Command->StartIndex]);
			NewPosition = Token->PositionAfter;
		}
		else {
			// Got an end-of-source nesting error, move to the end of the text
			NewPosition = GetTextLength();
		}
		// Move the cursor
		SetSel(NewPosition, NewPosition);
	}
	else { 
		// No syntax errors, so set the cursor to the beginning of the text
		SetSel(0, 0);
	}

	NotifyParentAboutSelectionChange();
	// There is no changed line
	m_ChangedLineNumber = -1;
	// The contents of the control are brand new 
	m_ContentsChanged = FALSE;
}


void CMacroEditCtrl::SetContentsFromRTF(char *RTF)
{
	EDITSTREAM EditStream;
	char *RTFLocal = RTF;
	EditStream.dwCookie = (DWORD)(&RTFLocal);
	EditStream.pfnCallback = InEditStreamCallback;
	StreamIn(SF_RTF, EditStream);
	SetSel(0,0);
	ShowScrollBar(SB_VERT, TRUE);
	NotifyParentAboutSelectionChange();
	// There is no changed line
	m_ChangedLineNumber = -1;
	// The contents of the control are brand new 
	m_ContentsChanged = FALSE;
}



void CMacroEditCtrl::CheckContents
	(CPtrArray  & TokenList,
	 int        & TokenCount,
	 CPtrArray  & CommandList,
	 int        & CommandCount,
	 CUIntArray & ErrorIndexList,
	 int        & ErrorCount)
{
	EDITSTREAM EditStream;
	char *MacroCode;
	char *Buffer;
	char *RTF;
	int BadCommand;
	int NewPosition;
	CommandStructure *Command;
	TokenStructure *Token;
	int Len;
	int LineLen;
	int CurrentLine;
	long Start, End;
	int CharIndex;
	char LineContents[MAX_LINE_SIZE];
	int PositionBefore;
	int Displacement;
	int i;	
	int TokenEndPosition;
	int TokenStartPosition;
	int Position;
	BOOL Found;
	int Dummy1;
	char *Dummy2;
	
	// Get the control contents
	Len = GetTextLength();
	if (Len == 0) {
		// The control is empty
		TokenCount = 0;
		CommandCount = 0;
		ErrorCount = 0;
		return;
	}

	MacroCode = new char[Len+1];
	Buffer = MacroCode;
	EditStream.dwCookie = (DWORD)(&Buffer);
	EditStream.pfnCallback = OutEditStreamCallback;
	StreamOut(SF_TEXT, EditStream);
	MacroCode[Len] = '\0';

	// Get the current selection
	GetSel(Start, End);
	// Get the contents of the current line
	CurrentLine = LineFromChar(Start);
	int firstvis = GetFirstVisibleLine();
	int posv = GetScrollPos(SB_VERT);
	int posh = GetScrollPos(SB_HORZ);
	CharIndex = LineIndex(CurrentLine);
	LineLen = LineLength(CharIndex);
	GetLine(CurrentLine, LineContents, MAX_LINE_SIZE - 4);
	LineContents[LineLen] = '\0';

	// Parse the current line in order to find out the position
	// of the cursor relative to the command's tokens 
	m_Parser.ParseLine(LineContents);
	m_Parser.GetParseLineResults(TokenList, TokenCount, Dummy1, Dummy2);
	if (TokenCount == 0) {
		// Got a blank line, so point at the newline at the end of the line
		PositionBefore = CharIndex + LineLen;
		Displacement = 0;
	}
	else {
		Token = (TokenStructure *)TokenList[0];
		if (Token->TokenClass == COMMENT_TOKEN) {
			PositionBefore = CharIndex + Token->PositionBefore;
			Displacement = Start - PositionBefore;
		}
		else {
			// Scan the token list for a token near the cursor 
			Found = FALSE;
			Position = Start - CharIndex;
			for (i=0; i<TokenCount; i++) {
				Token = (TokenStructure *)TokenList[i];
				TokenStartPosition = Token->PositionBefore;
				TokenEndPosition = TokenStartPosition + Token->Size - 1;
				if (Position < TokenStartPosition) {
					// The cursor is in white space before the token
					PositionBefore = CharIndex + TokenStartPosition;
					Displacement = 0;
					Found = TRUE;
					break;
				}
				else if ((Position >= TokenStartPosition) && 
					     (Position <= TokenEndPosition)) {
					// The cursor is inside the token
					PositionBefore = CharIndex + TokenStartPosition;
					Displacement = Position - TokenStartPosition;
					Found = TRUE;
					break;
				}
			}
			if (!Found) {
				// The cursor was after the end of the last token, so use the 
				// end of the line
				PositionBefore = CharIndex + LineLen; 
				Displacement = 0;
			}
		}
	}
	// With RichEdit 2.0 internally, "paragraph" marks
	// are now stored as just a /n rather than a /n/r
	// When you stream in and out, this is mapped by Richedit to
	// a /n/r.  However, the consequence is that the
	// index into the streamed memory is different
	// from the char pos, by the value of the current line:
	PositionBefore += CurrentLine;
	
	m_Parser.Parse(MacroCode);
	m_Parser.GetParseResults(TokenList, TokenCount, CommandList, CommandCount,
		ErrorIndexList, ErrorCount);
	delete []MacroCode;
	
	RTF = m_Parser.GetRTF();
	EditStream.dwCookie = (DWORD)(&RTF);
	EditStream.pfnCallback = InEditStreamCallback;
	StreamIn(SF_RTF, EditStream);
	ShowScrollBar(SB_VERT, TRUE);

	if (ErrorCount > 0) {
		BadCommand = ErrorIndexList[0];
		Command = (CommandStructure *)CommandList[BadCommand];
		if (Command->IndentLevel >= 0) {
			// Move the cursor to the first token in the first bad command
			Token = (TokenStructure *)(TokenList[Command->StartIndex]);
			NewPosition = Token->PositionAfter;

			// With RichEdit 2.0 internally, "paragraph" marks
			// are now stored as just a /n rather than a /n/r
			// When you stream in and out, this is mapped by Richedit to
			// a /n/r.  However, the consequence is that the
			// index into the streamed memory is different
			// from the char pos, by the value of the current line:
			NewPosition -= Command->LineNumber;
			NewPosition++;
		}
		else {
			// Got an end-of-source nesting error, move to the end
			NewPosition = GetTextLength();
		}
	}
	else {
		Found = FALSE;
		// Find the new location of the token that the cursor was near
		for (i=0; i<TokenCount; i++) {
			Token = (TokenStructure *)TokenList[i];
			if (Token->PositionBefore == PositionBefore) {
				Found = TRUE;
				break;
			}
		}
		if (Found)
			NewPosition = Token->PositionAfter + Displacement;
		else
			// Move to the end
			NewPosition = GetTextLength();

		// With RichEdit 2.0 internally, "paragraph" marks
		// are now stored as just a /n rather than a /n/r
		// When you stream in and out, this is mapped by Richedit to
		// a /n/r.  However, the consequence is that the
		// index into the streamed memory is different
		// from the char pos, by the value of the current line:
		NewPosition -= CurrentLine;
	}	

	if (ErrorCount == 0)
	{
		// Move our viewport
		LineScroll(firstvis);
		SetScrollPos(SB_VERT, posv, TRUE);
		SetScrollPos(SB_HORZ, posh, TRUE);
	}

	// Move the cursor
	SetSel(NewPosition, NewPosition);
	
	NotifyParentAboutSelectionChange();
	m_ChangedLineNumber = -1;
}




BOOL CMacroEditCtrl::Save(const char *Filename, bool setModified)
{
	char *Contents;
	CFile f;
	BOOL ok; 
	char *Buffer;
	int Len;
	EDITSTREAM EditStream;

	// Try to open the file for writing
	ok = f.Open(Filename, CFile::modeCreate|CFile::modeWrite);
	if (!ok) {
		return FALSE;
	}

	Len = GetTextLength();
	Contents = new char[Len+1];
	Buffer = Contents;
	EditStream.dwCookie = (DWORD)(&Buffer);
	EditStream.pfnCallback = OutEditStreamCallback;
	StreamOut(SF_TEXT, EditStream);
	Contents[Len] = '\0';

	f.Write(Contents, Len);
	f.Close();
	delete []Contents;
	if (setModified)
		m_ContentsChanged = FALSE;
	return TRUE;
}




void CMacroEditCtrl::GetScriptInformation
		(CPtrArray  & ScriptList,
		 int        & ScriptCount)
{	
	m_Parser.GetScriptInformation(ScriptList, ScriptCount);
}


void CMacroEditCtrl::Nullify()
{
	m_Parser.Nullify();
}


BOOL CMacroEditCtrl::IsModified()
{
	return m_ContentsChanged;
}



BOOL CMacroEditCtrl::HasSelection() 
{
	long Start, End;
	GetSel(Start, End);
	if (Start == End) 
		return FALSE;
	else
		return TRUE;
}


void CMacroEditCtrl::SetLineNumber(int LineNumber)
{
	int CurrentLine;
	long CharIndex;

	// Move the cursor to the beginning of the line
	CharIndex = LineIndex(LineNumber-1);
	SetSel(CharIndex, CharIndex);
	NotifyParentAboutSelectionChange();
	CurrentLine = LineFromChar(CharIndex);
	ParseChangedLine(CurrentLine);
}


void CMacroEditCtrl::GetLineNumbers(int & StartLineNumber, int & EndLineNumber) 
{
	long Start, End;
	CString SelText;
	
	GetSel(Start, End);
	if (End > Start) {
		SelText = GetSelText();
		// New rich edit control 2.0 makes these '\v' - very strange!
		if ((SelText.Right(1) == '\v') || (SelText.Right(1) == '\r'))
			// The selection was over a newline. so move it back
			End -= 1;
	}
	StartLineNumber = LineFromChar(Start) + 1;
	EndLineNumber = LineFromChar(End) + 1;
}



char *CMacroEditCtrl::GetLineErrorString()
{
	return m_ErrorString;
}


void CMacroEditCtrl::BlockComment()
{
	long Start, End;
	int StartLine, EndLine;
	int Line;
	int CharIndex;
	char LineContents[MAX_LINE_SIZE];
	int Len;
	CString Temp;
	int EndIndex;
	CString SelText;
	
	GetSel(Start, End);
	SelText = GetSelText();
	if ((SelText.Right(1) == '\v') || (SelText.Right(1) == '\r'))
		// The selection was over a newline. so move it back
		End -= 2;
	if (Start >= End)
		// There is no selection, do nothing
		return;
	
	StartLine = LineFromChar(Start);
	EndLine = LineFromChar(End);

	for (Line=StartLine; Line<=EndLine; Line++) {
		// Get the contents of each line
		CharIndex = LineIndex(Line);
		Len = LineLength(CharIndex);
		GetLine(Line, LineContents, MAX_LINE_SIZE - 4);
		LineContents[Len] = '\0';
		// Select the entire line
		SetSel(CharIndex, CharIndex+Len);
		// Add a comment character to the line contents
		Temp = CString("*") + CString(LineContents);
		// Replace the whole line with the starred line 
		ReplaceSel(Temp);
		// Colour the newly formed line
		ParseLine(Line);
	}

	EndIndex = LineIndex(EndLine);
	EndIndex += LineLength(EndIndex);
	SetSel(EndIndex, EndIndex);
	NotifyParentAboutSelectionChange();
	NotifyParentAboutContentChange();
	m_ChangedLineNumber = -1;
}


void CMacroEditCtrl::BlockIndent()
{
	long Start, End;
	int StartLine, EndLine;
	int Line;
	int CharIndex;
	char LineContents[MAX_LINE_SIZE];
	int Len;
	CString Temp;
	int EndIndex;
	CString SelText;
	
	GetSel(Start, End);
	SelText = GetSelText();
	if ((SelText.Right(1) == '\v') || (SelText.Right(1) == '\r'))
		// The selection was over a newline. so move it back
		End -= 1;
	if (Start >= End)
		// There is no selection, do nothing
		return;

	StartLine = LineFromChar(Start);
	EndLine = LineFromChar(End);

	for (Line=StartLine; Line<=EndLine; Line++) {
		// Get the contents of each line
		CharIndex = LineIndex(Line);
		Len = LineLength(CharIndex);
		GetLine(Line, LineContents, MAX_LINE_SIZE - 4);
		LineContents[Len] = '\0';
		// Select the entire line
		SetSel(CharIndex, CharIndex+Len);
		// Add a comment character to the line contents
		Temp = CString("\t") + CString(LineContents);
		// Replace the whole line with the tabbed line 
		ReplaceSel(Temp);
		// Colour the newly formed line
		ParseLine(Line);
	}

	EndIndex = LineIndex(EndLine);
	EndIndex += LineLength(EndIndex);
	SetSel(EndIndex, EndIndex);
	NotifyParentAboutSelectionChange();
	NotifyParentAboutContentChange();
	m_ChangedLineNumber = -1;
}


void CMacroEditCtrl::BlockUncomment()
{
	long Start, End;
	int StartLine, EndLine;
	int Line;
	int CharIndex;
	char LineContents[MAX_LINE_SIZE];
	int Len;
	CString Temp;
	int EndIndex;
	CString SelText;
	BOOL Found;
	char *p;
	char c;

	GetSel(Start, End);
	SelText = GetSelText();
	if ((SelText.Right(1) == '\v') || (SelText.Right(1) == '\r'))
		// The selection was over a newline. so move it back
		End -= 1;
	if (Start >= End)
		// There is no selection, do nothing
		return;

	StartLine = LineFromChar(Start);
	EndLine = LineFromChar(End);

	for (Line=StartLine; Line<=EndLine; Line++) {
		// Get the contents of each line
		CharIndex = LineIndex(Line);
		Len = LineLength(CharIndex);
		if (Len == 0)
			// Empty line, move on to next line
			continue;
		GetLine(Line, LineContents, MAX_LINE_SIZE - 4);
		LineContents[Len] = '\0';

		// Scan the line for an initial star
		p = LineContents;
		Found = FALSE;
		while ((c = *p) != '\0') {
			if ((c == ' ') || (c == '\t')) 
				p++;
			else if (c == '*') {	
				Found = TRUE;
				break;
			}
			else 
				break;
		}

		if (!Found)
			// The line did not have an initial star. move on to next line
			continue;

		// Move the line contents down a character, overwriting the star
		memmove(p, p+1, Len);
		// Replace the commented line with the uncommented one
		SetSel(CharIndex, CharIndex+Len);
		ReplaceSel(LineContents);
		// The new uncommented line may have errors now, so parse it
		ParseLine(Line);
	}

	EndIndex = LineIndex(EndLine);
	EndIndex += LineLength(EndIndex);
	SetSel(EndIndex, EndIndex);
	NotifyParentAboutSelectionChange();
	NotifyParentAboutContentChange();
	m_ChangedLineNumber = -1;
}



void CMacroEditCtrl::BlockOutdent()
{
	long Start, End;
	int StartLine, EndLine;
	int Line;
	int CharIndex;
	char LineContents[MAX_LINE_SIZE];
	int Len;
	CString Temp;
	int EndIndex;
	CString SelText;
	char *p;
	char c;	
	char *Destination;
	char *Source;
	int SpaceCount;
	int TabSize = 6;
	
	GetSel(Start, End);
	SelText = GetSelText();
	if ((SelText.Right(1) == '\v') || (SelText.Right(1) == '\r'))
		// The selection was over a newline. so move it back
		End -= 1;
	if (Start >= End)
		// There is no selection, do nothing
		return;

	StartLine = LineFromChar(Start);
	EndLine = LineFromChar(End);

	for (Line=StartLine; Line<=EndLine; Line++) {
		// Get the contents of each line
		CharIndex = LineIndex(Line);
		Len = LineLength(CharIndex);
		if (Len == 0)
			// Empty line, move on to next line
			continue;
		GetLine(Line, LineContents, MAX_LINE_SIZE - 4);
		LineContents[Len] = '\0';

		Destination = LineContents;
		p = LineContents;
		SpaceCount = 0;
		while ((c = *p) != '\0') {
			if (c == ' ') {
				p++;
				SpaceCount++;
				Source = p;
				if (SpaceCount == TabSize) {
					break;
				}
			}
			else if (c == '\t') {	
				Source = p+1;
				break;
			}
			else {
				Source = p;
				break;
			}
		}

		// Overwrite the indentation characters, if any
		memmove(Destination, Source, LineContents + Len - Source + 1);
		// Replace the indented line with the unindented one
		SetSel(CharIndex, CharIndex+Len);
		ReplaceSel(LineContents);
		// Colour the newly formed line
		ParseLine(Line);
	}

	EndIndex = LineIndex(EndLine);
	EndIndex += LineLength(EndIndex);
	SetSel(EndIndex, EndIndex);
	NotifyParentAboutSelectionChange();
	NotifyParentAboutContentChange();
	m_ChangedLineNumber = -1;
}




void CMacroEditCtrl::SelectAll()
{
	// Cursor is about to move to line zero
	ParseChangedLine(0);
	SetSel(0, -1);
	NotifyParentAboutSelectionChange();
}



void CMacroEditCtrl::Cut()
{
	long Start, End;
	int StartLine;

	// Perform the default cut operation
	CRichEditCtrl::Cut();
	// Notify parent about changes
	NotifyParentAboutSelectionChange();
	NotifyParentAboutContentChange();

	// Mark the line where the cursor ends up as being changed
	GetSel(Start, End);
	StartLine = LineFromChar(Start);
	m_ChangedLineNumber = StartLine;
}



void CMacroEditCtrl::Paste()
{
	int i;
	long Start, End;
	int StartLine, EndLine;

	// Which line are we on before the paste?
	GetSel(Start, End);
	StartLine = LineFromChar(Start);
	// Perform the default paste operation
	CRichEditCtrl::Paste();
	// Which line are we on after the paste?
	GetSel(Start, End);
	EndLine = LineFromChar(Start);
	// Parse all lines of the pasted block, except the last
	for (i=StartLine; i<EndLine; i++) {
		ParseLine(i);
	}
	// Don't parse the final line - just mark it as changed
	m_ChangedLineNumber = EndLine;
	NotifyParentAboutSelectionChange();
	NotifyParentAboutContentChange();
}




BOOL CMacroEditCtrl::Find
	(char *SearchText, 
	 DWORD SearchFlags, 
	 BOOL DirectionDown)
{
	long Start, End;
	FINDTEXTEX Finder;
	int Total;
	CHARRANGE CharacterRange;
	CHARRANGE FoundCharacterRange;
	long Position;
	CString Message;
	BOOL Found;
	long Min;
	int CurrentLine;
	int Result;

	Total = GetTextLength();

	// Get the current cursor position
	GetSel(Start, End);
	// Assume the search text will not be found
	Found = FALSE;
	Finder.lpstrText = SearchText;

	if (DirectionDown) {
		// Search from the end of the selection to the end of the document
		CharacterRange.cpMin = End;
		CharacterRange.cpMax = Total;
		Finder.chrg = CharacterRange;
		Position = FindText(SearchFlags, &Finder);
		if (Position == -1) {
			Message.Format("Reached the end of the document. Do you want to continue searching from the beginning?");
			::Beep(450, 100);
			Result = ::AfxMessageBox(Message, MB_YESNO | MB_ICONEXCLAMATION);
			if (Result == IDYES) {
				// Search from the beginning to the end of the document
				CharacterRange.cpMin = 0;
				CharacterRange.cpMax = Total;
				Finder.chrg = CharacterRange;
				Position = FindText(SearchFlags, &Finder);
			}
			else {
				// User doesn't want to continue - text not found
				return FALSE;
			}
		}
		if (Position != -1) {
			// We found an instance of the search text
			Found = TRUE;
			FoundCharacterRange = Finder.chrgText;
		}
	}
	else {
		// Upwards search. This is a bit of a crap way of doing it but
		// there doesn't seem to be a way of setting the character range
		// for upwards searching. As ever, thanks a bundle Microsoft.
		Min = 0;
		CharacterRange.cpMax = Start;
		// Search from the start of the selection to the beginning of 
		// the document
		do {
			CharacterRange.cpMin = Min;
			Finder.chrg = CharacterRange;		
			Position = FindText(SearchFlags, &Finder);
			if (Position == -1) 
				break;
			FoundCharacterRange = Finder.chrgText;
			Found = TRUE;
			Min = Finder.chrgText.cpMax;
		} while (TRUE);
		if (!Found) {
			Message.Format("Reached the beginning of the document. Do you want to continue searching from the end?");
			::Beep(450, 100);
			Result = ::AfxMessageBox(Message, MB_YESNO | MB_ICONEXCLAMATION);
			if (Result == IDYES) {
				// Search from the end to the beginning of the document
				Min = 0;
				CharacterRange.cpMax = Total;
				do {
					CharacterRange.cpMin = Min;
					Finder.chrg = CharacterRange;		
					Position = FindText(SearchFlags, &Finder);
					if (Position == -1) 
						break;
					FoundCharacterRange = Finder.chrgText;
					Found = TRUE;
					Min = Finder.chrgText.cpMax;
				} while (TRUE);
			}
			else {
				// User doesn't want to continue - text not found
				return FALSE;
			}
		}
	}

	if (!Found) {
		// Search text was not found 
		Message.Format("Unable to find '%s'", SearchText);
		::Beep(450, 100);
		::AfxMessageBox(Message, MB_ICONEXCLAMATION);
		return FALSE;
	}

	// Highlight the found search text
	SetSel(FoundCharacterRange.cpMin, FoundCharacterRange.cpMax);
	NotifyParentAboutSelectionChange();
	CurrentLine = LineFromChar(FoundCharacterRange.cpMin);
	ParseChangedLine(CurrentLine);

	return TRUE;
}



BOOL CMacroEditCtrl::Replace
	(char *SearchText, 
	 char *ReplaceText,
	 DWORD SearchFlags)
{
	long Start, End;
	FINDTEXTEX Finder;
	int Total;
	CHARRANGE CharacterRange;
	long Position;
	CString Message;
	int ReplaceLen;
	int Result;
	int CurrentLine;

	ReplaceLen = strlen(ReplaceText);
	Total = GetTextLength();
	// Search from the current cursor position until the end of the document
	GetSel(Start, End);
	Finder.lpstrText = SearchText;
	CharacterRange.cpMin = Start;
	CharacterRange.cpMax = Total;
	Finder.chrg = CharacterRange;
	Position = FindText(SearchFlags, &Finder);
	if (Position == -1) {
		Message.Format("Reached the end of the document. Do you want to continue searching from the beginning?");
		::Beep(450, 100);
		Result = ::AfxMessageBox(Message, MB_YESNO | MB_ICONEXCLAMATION);
		if (Result == IDYES) {
			// Search from the beginning of the document until the end
			CharacterRange.cpMin = 0;
			CharacterRange.cpMax = Total;
			Finder.chrg = CharacterRange;
			Position = FindText(SearchFlags, &Finder);
			if (Position == -1) {
				Message.Format("Unable to find '%s'", SearchText);
				::Beep(450, 100);
				::AfxMessageBox(Message, MB_ICONEXCLAMATION);
				return FALSE;
			}
		}
		else {
			// User doesn't want to continue - text not found
			return FALSE;
		}
	}

	if (Finder.chrgText.cpMin == Start && Finder.chrgText.cpMax == End) {
		// The current selection covers an instance of the search text, so
		// replace it with the replacement text
		ReplaceSel(ReplaceText);
		NotifyParentAboutContentChange();
		// Highlight the replacement text
		SetSel(Start, Start + ReplaceLen);
		NotifyParentAboutSelectionChange();
		CurrentLine = LineFromChar(Start);
		ParseChangedLine(CurrentLine);
		m_ChangedLineNumber = CurrentLine;
		// Document length will very probably have changed
		Total = GetTextLength();
		// Search from the end of the replaced text til the end of the document
		CharacterRange.cpMin = Start + ReplaceLen;
		CharacterRange.cpMax = Total;
		Finder.chrg = CharacterRange;
		Position = FindText(SearchFlags, &Finder);
		if (Position == -1) {
			Message.Format("Reached the end of the document. Do you want to continue searching from the beginning?");
			::Beep(450, 100);
			Result = ::AfxMessageBox(Message, MB_YESNO | MB_ICONEXCLAMATION);
			if (Result == IDYES) {
				CharacterRange.cpMin = 0;
				CharacterRange.cpMax = Total;
				Finder.chrg = CharacterRange;
				Position = FindText(SearchFlags, &Finder);
				if (Position == -1) {
					Message.Format("Unable to find '%s'", SearchText);
					::Beep(450, 100);
					::AfxMessageBox(Message, MB_ICONEXCLAMATION);
					return FALSE;
				}
			}
			else {
				// User doesn't want to continue - text not found
				return FALSE;
			}
		}
	}	
	// Highlight the found search text
	SetSel(Finder.chrgText.cpMin, Finder.chrgText.cpMax);
	NotifyParentAboutSelectionChange();
	CurrentLine = LineFromChar(Finder.chrgText.cpMin);
	ParseChangedLine(CurrentLine);
	return TRUE;
}





int CMacroEditCtrl::ReplaceAll
	(char *SearchText, 
	 char *ReplaceText,
	 DWORD SearchFlags, 
	 BOOL WholeFile)
{
	FINDTEXTEX Finder;
	int Total;
	CHARRANGE CharacterRange;
	long Position;
	CString Message;
	int ReplaceLen;
	int SearchLen;
	int Max;
	int Min;
	BOOL Found;
	int CurrentLine;
	int ReplaceCount;
	long Start, End;
	long EndPosition;
	int LengthDifference;

	SearchLen = strlen(SearchText);
	ReplaceLen = strlen(ReplaceText);
	LengthDifference = ReplaceLen - SearchLen;

	Total = GetTextLength();

	if (WholeFile) {
		//
		// Replace all instances of the search text within the document
		//
		ReplaceCount = 0;
		Finder.lpstrText = SearchText;
		Min = 0;
		Found = FALSE;
		do {
			// Locate the next instance
			CharacterRange.cpMin = Min;
			CharacterRange.cpMax = Total;
			Finder.chrg = CharacterRange;
			Position = FindText(SearchFlags, &Finder);
			if (Position == -1)
				break;
			Found = TRUE;
			// Highlight the search text instance
			SetSel(Finder.chrgText.cpMin, Finder.chrgText.cpMax);
			// Replace it with the replacement text
			ReplaceSel(ReplaceText);
			EndPosition = Finder.chrgText.cpMin + ReplaceLen;
			// Move the cursor to just after the end of the replacement text
			SetSel(EndPosition, EndPosition);
			NotifyParentAboutSelectionChange();	
			CurrentLine = LineFromChar(EndPosition);	
			ParseChangedLine(CurrentLine);
			m_ChangedLineNumber = CurrentLine;
			ReplaceCount++;
			// Start searching after the end of the replacement text
			Min = Finder.chrgText.cpMin + ReplaceLen;
			// Re-calculate the length of the document
			Total += LengthDifference;
		} while (TRUE);
		if (Found) {
			NotifyParentAboutContentChange();
			Message.Format("Finished searching for '%s' - %d replacement(s) made", 
				SearchText, ReplaceCount);
			::Beep(450, 100);
			::AfxMessageBox(Message, MB_ICONINFORMATION);
			return ReplaceCount;
		}
		else {
			Message.Format("Unable to find '%s'. No replacements were made", SearchText);
			::Beep(450, 100);
			::AfxMessageBox(Message, MB_ICONEXCLAMATION);
			return 0;
		}
	}

	else {
		//
		// Replace all instances of the search text within the current selection
		//
		GetSel(Start, End);
		ReplaceCount = 0;
		Finder.lpstrText = SearchText;
		Min = Start;
		Max = End;
		Found = FALSE;
		CharacterRange.cpMin = Min;
		do {
			// Search for the next instance of the search text
			CharacterRange.cpMax = Max;
			Finder.chrg = CharacterRange;
			Position = FindText(SearchFlags, &Finder);
			if (Position == -1)
				// Not found
				break;
			if (Finder.chrgText.cpMax - Finder.chrgText.cpMin < SearchLen) {
				// FindText() can find a part of the search text if the search
				// text straddles the end of the selection boundary. When this 
				// happens, we are at the end of the search process.
				break;
			}
			Found = TRUE;
			// Highlight the search text instance
			SetSel(Finder.chrgText.cpMin, Finder.chrgText.cpMax);
			// Replace it with the replacement text
			ReplaceSel(ReplaceText);
			// Adjust the end of the selection
			Max += LengthDifference;
			// Re-establish the selection
			SetSel(Min, Max);
			NotifyParentAboutSelectionChange();	
			CurrentLine = LineFromChar(Finder.chrgText.cpMin);	
			ParseChangedLine(CurrentLine);
			m_ChangedLineNumber = CurrentLine;
			ReplaceCount++;
			// Continue searching from the end of the replaced text
			CharacterRange.cpMin = Finder.chrgText.cpMax + LengthDifference;
		} while (TRUE);

		if (Found) {
			NotifyParentAboutContentChange();
			Message.Format("Finished searching for '%s' - %d replacement(s) made", SearchText, ReplaceCount);
			::Beep(450, 100);
			::AfxMessageBox(Message, MB_ICONINFORMATION);
			return ReplaceCount;
		}
		else {
			Message.Format("Unable to find '%s'. No replacements were made ", 
				SearchText);
			::Beep(450, 100);
			::AfxMessageBox(Message, MB_ICONEXCLAMATION);
			return 0;
		}
	}
}



//
// Message Handlers
//


BEGIN_MESSAGE_MAP(CMacroEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CMacroEditCtrl)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





void CMacroEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	if (m_ReadOnly)
		return;

	long Start, End;
	unsigned short KeyState;
	int CurrentLine;

	// Determine if the Control key is being pressed
	KeyState = ::GetKeyState(VK_CONTROL);
	m_ControlKeyDown = ((KeyState & 0x0080) == 0x0080);
	if (m_ControlKeyDown) {
		// Deal with Control sequences
		if (nChar == 'C') {
			Copy();
			return;
		}
		else if (nChar == 'V') {
			Paste();
			return;
		}
		else if (nChar == 'X') {
			Cut();
			return;
		}
		else if (nChar == 'A') {
			SelectAll();
			return;
		}
		else if ((nChar != VK_LEFT) && 
			(nChar != VK_RIGHT) &&	
			(nChar != VK_UP) &&
			(nChar != VK_DOWN) &&
			(nChar != VK_HOME) &&
			(nChar != VK_END) &&
			(nChar != VK_PRIOR) &&
			(nChar != VK_NEXT))
			// Allow control + movement key but block all other control 
			// sequences
			return;
	}

	if (nChar == VK_TAB) {
		// Tab is used for block indent if there is a selection and for 
		// inserting a single tab character if there isn't. Single tab
		// insertion will be handled in the OnChar() function. The block indent
		// flag is used to stop OnChar() from processing the Tab character. 
		GetSel(Start, End);
		if (Start != End) {
			m_DoingBlockIndent = TRUE;	
			BlockIndent();
			return;
		}
	}

	m_DoingBlockIndent = FALSE;

	// Call the default key down handler
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	// Get the new location of the selection
	GetSel(Start, End);
	// Get the starting line of the selection
	CurrentLine = LineFromChar(Start);

	if (nChar == VK_DELETE || nChar == VK_INSERT) {
		// The Delete key does not produce a WM_CHAR message, so I have to 
		// deal with it here
		// This also cover Shift+Insert (Paste)
		NotifyParentAboutSelectionChange();
		NotifyParentAboutContentChange();
		m_ChangedLineNumber = CurrentLine;
		return;
	}

	if ((nChar != VK_LEFT) && 
		(nChar != VK_RIGHT) &&	
		(nChar != VK_UP) &&
		(nChar != VK_DOWN) &&
		(nChar != VK_HOME) &&
		(nChar != VK_END) &&
		(nChar != VK_PRIOR) &&
		(nChar != VK_NEXT))
		// Only interested in cursor movement keys
		return;

	NotifyParentAboutSelectionChange();

	if (Start == End) 
		// Update a changed line only if there is no selection
		ParseChangedLine(CurrentLine);
}



void CMacroEditCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_ReadOnly)
		return;

	long Start, End;
	int CurrentLine;

	// Call the default left button down handler. Note that this call returns
	// only after the user has released the left mouse button, ie the user can
	// drag the mouse around to make a selection before returning
	CRichEditCtrl::OnLButtonDown(nFlags, point);

	NotifyParentAboutSelectionChange();

	// Get the starting line of the selection
	GetSel(Start, End);
	CurrentLine = LineFromChar(Start);

	int posv = GetScrollPos(SB_VERT);
	int posh = GetScrollPos(SB_HORZ);
	ParseChangedLine(CurrentLine);
	SetScrollPos(SB_VERT, posv, TRUE);
	SetScrollPos(SB_HORZ, posh, TRUE);
}



void CMacroEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_ReadOnly)
		return;

	CHARFORMAT cfCurrent;
	long Start, End;
	int CurrentLine;
	char LineContents[MAX_LINE_SIZE];
	int CharIndex;
	int Len;
	char c;
	char *p;

	// Control-key combinations have already been dealt with. This is needed 
	// because a control-key combination will produce a WM_CHAR 
	if (m_ControlKeyDown)
		return;

	if (m_DoingBlockIndent)
		// Needed to stop a single tab insertion during block indent
		return;

	if ((nChar != VK_BACK) && (nChar != VK_RETURN))	{
		// Change the colour of newly typed characters to be black
		GetSelectionCharFormat(cfCurrent);
		if (cfCurrent.crTextColor != RGB(0, 0, 0))
			SetSelectionCharFormat(m_DefaultCharFormat);
	}
	// Call the default character handler
	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);

	GetSel(Start, End);
	CurrentLine = LineFromChar(Start);
	NotifyParentAboutSelectionChange();
	NotifyParentAboutContentChange();

	if (nChar != VK_RETURN) {
		// Flag the current line as being changed
		m_ChangedLineNumber = CurrentLine;
		return;
	}

	//
	// Deal with Return key	
	//

	// Parse the previous line
	ParseLine(CurrentLine-1);
	// Add the same indentation to the new line as was present on the previous
	// line
	CharIndex = LineIndex(CurrentLine-1);
	Len = LineLength(CharIndex);
	GetLine(CurrentLine-1, LineContents, MAX_LINE_SIZE-4);
	LineContents[Len] = '\0';

	p = LineContents;
	while ((c = *p) != '\0') {
		if ((c == ' ') || (c == '\t')) 
			p++;
		else {
			*p = '\0';
			break;
		}
	}
	if (strlen(LineContents) != 0)
		// The Return key will have forced there to be no selection, so replace
		// the cursor with the indentation string
		ReplaceSel(LineContents);
	// Mark this newly created line as being changed
	m_ChangedLineNumber = CurrentLine;
}

// Takes a position before parsing, and returns a position
// after parsing (with indentation characters etc. now added)
// Uses parse results from last run.  Accurate to resolution
// of a token.
int CMacroEditCtrl::PositionBeforeToPositionAfter(int pos_before)
{
	CPtrArray  TokenList;
	int        TokenCount;
	CPtrArray  CommandList;
	int        CommandCount;
	CUIntArray ErrorIndexList;
	int        ErrorCount;

	m_Parser.GetParseResults(TokenList, TokenCount, CommandList, CommandCount,
		ErrorIndexList, ErrorCount);

	int pos_after = -1;
	for (int i = 0; i < TokenCount; ++i)
	{
		TokenStructure *Token = (TokenStructure *)(TokenList[i]);
		if (pos_before < Token->PositionBefore)
			break;
		pos_after = Token->PositionAfter;
	}
	ASSERT(pos_after >= 0);

	return pos_after;
}

