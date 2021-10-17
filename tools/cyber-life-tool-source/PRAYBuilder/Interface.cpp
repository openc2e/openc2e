#include "../../common/PRAYFiles/PrayManager.h"
#include "../../common/PRAYFiles/PrayChunk.h"
#include "../../common/PRAYFiles/StringIntGroup.h"
#include "../../common/PRAYFiles/PrayException.h"

#include "../../common/SimpleLexer.h"
#include "../../common/FileFuncs.h"

#include <stdio.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <strstream>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

static bool ourCompressionFlag = false;

void decodeType(int thisType,std::string& into)
{
	switch(thisType)
	{
	case SimpleLexer::typeFinished:
		into = "End of File";
		break;
	case SimpleLexer::typeError:
		into = "***LEXER ERROR***";
		break;
	case SimpleLexer::typeString:
		into = "Quoted String";
		break;
	case SimpleLexer::typeNumber:
		into = "Numerical Value";
		break;
	case SimpleLexer::typeSymbol:
		into = "Symbol or keyword";
		break;
	default:
		into = "Unknown type";
	}
}

bool expect(int thisType, std::string& thisToken, int expectedType, int linenumber)
{
	if (thisType == expectedType)
		return true;
	std::string wantType, gotType;

	decodeType(thisType,gotType);
	decodeType(expectedType,wantType);
	printf("Error on line %d. Expecting %s, got %s (%s)\n",linenumber,wantType.c_str(),gotType.c_str(),thisToken.c_str());
	return false;
}

void munchComment(int& currentTokenType, std::string& currentToken, SimpleLexer& inputLexer)
{
	bool eat;
	while (currentTokenType == SimpleLexer::typeSymbol && currentToken == "(-")
	{
		// Let's eat comments :)
		eat = true;
		while (eat)
		{
			currentTokenType = inputLexer.GetToken(currentToken);
			if (currentTokenType == SimpleLexer::typeFinished)
				eat = false;
			if (currentTokenType == SimpleLexer::typeSymbol)
				if (currentToken == "-)")
					eat = false;
		}
		// Eaten comment - ready to carry on :)
		if (currentTokenType == SimpleLexer::typeFinished)
		{
			printf("Unexpected end of file processing comment :(\n");
			exit(1);
		}
		currentTokenType = inputLexer.GetToken(currentToken);
	}
}

#define EXPECT(a) expect(currentTokenType,currentToken,SimpleLexer::a,inputLexer.GetLineNum())

int main(int argc, char* argv[])
{
	printf("Welcome to the PRAYBuilder 0.102, please wait, processing...\n");

	std::string src;
	if (argc == 2)
	{
		src = argv[1];
		printf("No compression\n");
	}
	else if (argc == 3)
	{
		if (argv[1] != std::string("--compress"))
		{
			printf("The only option allowed before the filename is --compress.\n");
			return 1;
		}
		printf("Compression enabled\n");
		ourCompressionFlag = true;
		src = argv[2];
	}
	else 
	{
		printf("PRAYBuilder takes parameters as follows:\nPRAYBuilder [--compress] template_filename\n");
		return 1;
	}

	std::string dst = src;
	if (dst.size() > 4 && (dst.substr(dst.size() - 4, 4) == ".txt"))
		dst = dst.substr(0, dst.size() - 4);
	dst += ".agents";

	if (FileExists(dst.c_str()))
		DeleteFile(dst.c_str());
	
	printf("Please wait, processing %s into %s\n",src.c_str(),dst.c_str());

	std::ifstream srcStream;

	try
	{
		srcStream.open(src.c_str());
	}
	catch (...)
	{
		printf("Oh dear, couldn't open input stream\n");
		return 2;
	}

	SimpleLexer inputLexer(srcStream);

	printf("Please wait, compiling...\n");

	std::string currentChunkName;
	std::string currentChunkType;

	std::string currentToken;
	StringIntGroup sg;

	enum
	{
		lookingChunk,
		lookingTag
	} myState;

	int currentTokenType = inputLexer.GetToken(currentToken);

	myState = lookingChunk;

	munchComment(currentTokenType,currentToken,inputLexer);

	if (!EXPECT(typeString))
		return 6;
	PrayManager pm;
	pm.SetLanguage(currentToken);

	currentTokenType = inputLexer.GetToken(currentToken);

	std::string chunkName,chunkType;

	while (currentTokenType != SimpleLexer::typeFinished)
	{
		munchComment(currentTokenType,currentToken,inputLexer);
		if (myState == lookingChunk)
		{
			// We are looking for a chunk identifier (Symbol/Token)
			if (!EXPECT(typeSymbol))
				return 4;
			// Right then, let's think... If "inline" do file chunk, if "chunk" do prep for stringintgroup.

			if (currentToken == "inline")
			{
				printf("Parsing inline chunk...\n");
				// Deal with inline chunk...
				currentTokenType = inputLexer.GetToken(currentToken);
				munchComment(currentTokenType,currentToken,inputLexer);
				// Expecting a nice Symbol...
				if (!EXPECT(typeSymbol))
					return 5;
				// The symbol should be 4 chars exactly
				if (currentToken.size() != 4)
				{
					printf("Expecting chunk symbol type. It was not four chars. PRAYBuilder aborting.\nOffending token was %s on line %d",currentToken.c_str(),inputLexer.GetLineNum());
					return 7;
				}
				// The symbol is indeed 4 chars.
				chunkType = currentToken;
				currentTokenType = inputLexer.GetToken(currentToken);
				munchComment(currentTokenType,currentToken,inputLexer);
				if (!EXPECT(typeString))
					return 6;
				// The string is the name of the chunk.
				chunkName = currentToken;
				printf("Chunk is of type: %s and is called %s\n",chunkType.c_str(),chunkName.c_str());
				currentTokenType = inputLexer.GetToken(currentToken);
				munchComment(currentTokenType,currentToken,inputLexer);
				if (!EXPECT(typeString))
					return 6;
				// The string is the name of the file.
				printf("The data for the inline chunk comes from %s\n",currentToken.c_str());
				FILE* inlineFile;
				inlineFile = fopen(currentToken.c_str(),"rb");
				if (inlineFile == NULL)
				{
					printf("Oh dear, couldn't open the file :(\n");
					return 10;
				}
				// File opened, let's munch in the data...
				fseek(inlineFile,0,SEEK_END);
				int bytes = ftell(inlineFile);
				fseek(inlineFile,0,SEEK_SET);
				uint8* filedata = new uint8[bytes];
				fread(filedata,bytes,1,inlineFile);
				fclose(inlineFile);
				// Let's dump that good old chunk into the file (nice and compressed too :)
				try
				{
					pm.AddChunkToFile(chunkName,chunkType,dst,bytes,filedata,ourCompressionFlag);
				}
				catch (PrayException& pe)
				{
					printf("Erkleroo, excepted during chunk add :( (%s)\n",pe.GetMessage().c_str());
					return 20;
				}
				catch (...)
				{
					printf("Erkleroo, excepted during chunk add :( (Some day I'll put some better error routines in :)\n");
					return 20;
				}
				delete [] filedata;
				currentTokenType = inputLexer.GetToken(currentToken);
				printf("Done parsing inline chunk...\n");
				continue;
			} // inline
			if (currentToken == "group")
			{
				printf("Please wait, preparing StringIntGroup chunk...\n");
				currentTokenType = inputLexer.GetToken(currentToken);
				munchComment(currentTokenType,currentToken,inputLexer);
				// Expecting a nice Symbol...
				if (!EXPECT(typeSymbol))
					return 5;
				// The symbol should be 4 chars exactly
				if (currentToken.size() != 4)
				{
					printf("Expecting chunk symbol type. It was not four chars. PRAYBuilder aborting.\nOffending token was %s on line %d",currentToken.c_str(),inputLexer.GetLineNum());
					return 7;
				}
				chunkType = currentToken;
				currentTokenType = inputLexer.GetToken(currentToken);
				munchComment(currentTokenType,currentToken,inputLexer);
				if (!EXPECT(typeString))
					return 6;
				// The string is the name of the chunk.
				chunkName = currentToken;
				printf("Chunk is of type: %s and is called %s\n",chunkType.c_str(),chunkName.c_str());
				currentTokenType = inputLexer.GetToken(currentToken);
				printf("Prepared, looking.....\n");
				myState = lookingTag;
				continue;
			}

		}
		else if (myState == lookingTag)
		{
			if (currentTokenType == SimpleLexer::typeSymbol)
			{
				// Hit end of chunk, so write it out, and reset state
				printf("Storing Chunk...\n");
				std::ostrstream os;
				sg.SaveToStream(os);
				char* str = os.str();
				int leng = os.pcount();
				try
				{
					pm.AddChunkToFile(chunkName,chunkType,dst,leng,(unsigned char *)str,ourCompressionFlag);
				}
				catch (PrayException& pe)
				{
					printf("Erkleroo, excepted during chunk add :( (%s)\n",pe.GetMessage().c_str());
					return 21;
				}
				catch (...)
				{
					printf("Erkleroo, excepted during group add :( (Some day I'll put some better error routines in :)\n");
					return 21;
				}
				sg.Clear();
				myState = lookingChunk;
				printf("Done.\n");
				continue;
			}
			if (currentTokenType == SimpleLexer::typeString)
			{
				// Right then, we have a tag. Format is...
				// "tag" <value>
				// <value> is either a number, a string, or a symbol.
				std::string thisTag = currentToken;
				currentTokenType = inputLexer.GetToken(currentToken);
				munchComment(currentTokenType,currentToken,inputLexer);

				std::string tempString;
				int bytes;
				char* filedata;

				switch(currentTokenType)
				{
				case SimpleLexer::typeString:
				{
					// We have a string->string mapping
					std::string t;
					if (sg.FindString(thisTag,t))
						printf("Warning, Tag %s redefined from %s to %s\n",thisTag.c_str(),t.c_str(),currentToken.c_str());
					sg.AddString(thisTag,currentToken);
					break;
				}
				case SimpleLexer::typeNumber:
				{
					// We have a string->int mapping
					int ti;
					if (sg.FindInt(thisTag,ti))
						printf("Warning, Tag %s redefined from %d to %s\n",thisTag.c_str(),ti,currentToken.c_str());
					sg.AddInt(thisTag,atoi(currentToken.c_str()));
					break;
				}
				case SimpleLexer::typeSymbol:
				{
					// We have a string->file (I.E. string) mapping
					if (currentToken != "@")
					{
						printf("The only valid symbol in this context is \"@\". Error on line %d",inputLexer.GetLineNum());
						return 22;
					}
					currentTokenType = inputLexer.GetToken(currentToken);
					munchComment(currentTokenType,currentToken,inputLexer);
					if (!EXPECT(typeString))
						return 23;
					printf("Trying to add a file mapping to %s\n",currentToken.c_str());
					FILE* inlineFile;
					inlineFile = fopen(currentToken.c_str(),"rb");
					if (inlineFile == NULL)
					{
						printf("Oh dear, couldn't open the file :(\n");
						return 10;
					}
					// File opened, let's munch in the data...
					fseek(inlineFile,0,SEEK_END);
					bytes = ftell(inlineFile);
					fseek(inlineFile,0,SEEK_SET);
					filedata = new char[bytes];
					fread(filedata,bytes,1,inlineFile);
					fclose(inlineFile);

					tempString.assign(filedata,bytes);
					delete [] filedata;
					std::string ts;
					if (sg.FindString(thisTag,ts))
						printf("Warning, Redefining tag %s with a file.\n",thisTag.c_str());
					sg.AddString(thisTag,tempString);
					break;
				}
				default:
				{
					printf("Expecting number,symbol or string. Error on line %d\n",inputLexer.GetLineNum());
					return 24;
				}
				}
				currentTokenType = inputLexer.GetToken(currentToken);
				continue;
			}
		}
		else
		{
			printf("Unknown lookahead state :(\n");
			return 3;
		}
	}

	if (myState == lookingTag)
	{
		printf("Storing Chunk...\n");
		std::ostrstream os;
		sg.SaveToStream(os);
		char* str = os.str();
		int leng = os.pcount();
		try
		{
			pm.AddChunkToFile(chunkName,chunkType,dst,leng,(unsigned char *)str,ourCompressionFlag);
		}
		catch (PrayException& pe)
		{
			printf("Erkleroo, excepted during chunk add :( (%s)\n",pe.GetMessage().c_str());
			return 21;
		}
		catch (...)
		{
			printf("Erkleroo, excepted during group add :( (Some day I'll put some better error routines in :)\n");
			return 21;
		}
		sg.Clear();
		myState = lookingChunk;
		printf("Done.\n");
	}
	return 0;
}

