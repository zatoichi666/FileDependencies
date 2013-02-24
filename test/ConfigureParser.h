#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
///////////////////////////////////////////////////////////////
// ConfigureParser.h - builds and configures parsers         //
// Ver 2.0                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Dell E6510, Windows 7                        //
// Application: CSE687 - OOD, Pr#1, Spring 2013              //
// Author:      Matt Synborski                               //
//              matthewsynborski@gmail.com                   //
///////////////////////////////////////////////////////////////
//                                                           //
// Originally written by James Fawcett for CSE-687 adapted   // 
//                         by Matt Synborski                 //
//                                                           //
///////////////////////////////////////////////////////////////
/*
Module Operations: 
==================
This module builds and configures parsers.  It builds the parser
parts and configures them with application specific rules and actions.

Public Interface:
=================
ConfigParseToConsole conConfig;
conConfig.Build();
conConfig.Attach(someFileName);

Build Process:
==============
Required files
- ConfigureParser.h, ConfigureParser.cpp, Parser.h, Parser.cpp,
ActionsAndRules.h, ActionsAndRules.cpp,
SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
Build commands (either one)
- devenv Project1HelpS06.sln
- cl /EHsc /DTEST_PARSER ConfigureParser.cpp parser.cpp \
ActionsAndRules.cpp \
semiexpression.cpp tokenizer.cpp /link setargv.obj

Maintenance History:
====================
ver 2.0 : 01 Jun 11
- Major revisions to begin building a strong code analyzer
ver 1.1 : 01 Feb 06
- cosmetic changes to ConfigureParser.cpp
ver 1.0 : 12 Jan 06
- first release

*/

#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"
#include "FoldingRules.h"

///////////////////////////////////////////////////////////////
// build parser that writes its output to console

class ConfigParseToConsole : IBuilder
{
public:
	ConfigParseToConsole() {};
	~ConfigParseToConsole();
	bool Attach(const std::string& name, bool isFile=true);
	Parser* Build();


private:
	// Builder must hold onto all the pieces

	Toker* pToker;
	SemiExp* pSemi;
	Parser* pParser;
	Repository* pRepo;

	// add folding rules

	FoldingRules* pFR;

	// add Rules and Actions

	BeginningOfScope* pBeginningOfScope;
	HandlePush* pHandlePush;

	EndOfScope* pEndOfScope;
	HandlePop* pHandlePop;

	PreprocStatement* pPreprocStatement;
	PrintPreproc* pPrintPreproc;

	// User defined types

	ClassDefinition* pClassDefinition;
	PushClass* pPushClass;

	EnumStatement* pEnumStatement;
	PrintEnum* pPrintEnum;

	FunctionDefinition* pFunctionDefinition;
	PushFunction* pPushFunction;

	StructDefinition* pStructDefinition;
	PushStruct* pPushStruct;

	TypedefStatement* pTypedefStatement;
	PrintTypedef* pPrintTypedef;

	UnionDefinition* pUnionDefinition;
	PushUnion* pPushUnion;

	// prohibit copies and assignments

	ConfigParseToConsole(const ConfigParseToConsole&);
	ConfigParseToConsole& operator=(const ConfigParseToConsole&);
};


#endif
