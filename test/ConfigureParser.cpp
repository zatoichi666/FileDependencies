///////////////////////////////////////////////////////////////
// ConfigureParser.cpp - builds and configures parsers       //
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
// This package configures the parser for the first pass     //
// in the source files that will find user defined types and //
// add them to the graph.                                    //
///////////////////////////////////////////////////////////////

#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
	// when Builder goes out of scope, everything must be deallocated

	delete pHandlePush;
	delete pBeginningOfScope;
	delete pHandlePop;
	delete pEndOfScope;
	delete pPushFunction;
	delete pFunctionDefinition;
	delete pClassDefinition;
	delete pPushClass;
	delete pPrintPreproc;
	delete pPreprocStatement;
	delete pEnumStatement;
	delete pPrintEnum;
	delete pTypedefStatement;
	delete pPrintTypedef;
	delete pUnionDefinition;
	delete pPushUnion;
	delete pFR;
	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;

}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
	if(pToker == 0)
		return false;
	return pToker->attach(name, isFile);
}
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToConsole::Build()
{	try
{	// add Parser's main parts
	pToker = new Toker;
	pToker->returnComments(false);
	pSemi = new SemiExp(pToker);
	pSemi->returnNewLines(false);
	pParser = new Parser(pSemi);
	pRepo = new Repository(pToker);
	// add code folding rules
	pFR = new codeFoldingRules;
	pParser->addFoldingRules(pFR);
	// configure to manage scope
	// these must come first - they return true on match
	// so rule checking continues
	pBeginningOfScope = new BeginningOfScope();		pHandlePush = new HandlePush(pRepo);
	pBeginningOfScope->addAction(pHandlePush);		pParser->addRule(pBeginningOfScope);
	pEndOfScope = new EndOfScope();		pHandlePop = new HandlePop(pRepo);
	pEndOfScope->addAction(pHandlePop);		pParser->addRule(pEndOfScope);
	pPreprocStatement = new PreprocStatement();		pPrintPreproc = new PrintPreproc();
	pPreprocStatement->addAction(pPrintPreproc);		pParser->addRule(pPreprocStatement);

	// configure to detect and act on user-defined type definitions
	// these will stop further rule checking by returning false
	pClassDefinition = new ClassDefinition;		pPushClass = new PushClass(pRepo); 
	pClassDefinition->addAction(pPushClass);		pParser->addRule(pClassDefinition);
	pEnumStatement = new EnumStatement();		pPrintEnum = new PrintEnum(pRepo);		
	pEnumStatement->addAction(pPrintEnum);		pParser->addRule(pEnumStatement);
	pFunctionDefinition = new FunctionDefinition;		pPushFunction = new PushFunction(pRepo);  
	pFunctionDefinition->addAction(pPushFunction);		pParser->addRule(pFunctionDefinition);
	pStructDefinition = new StructDefinition;		pPushStruct = new PushStruct(pRepo);  
	pStructDefinition->addAction(pPushStruct);		pParser->addRule(pStructDefinition);
	pTypedefStatement = new TypedefStatement();		pPrintTypedef = new PrintTypedef(pRepo);
	pTypedefStatement->addAction(pPrintTypedef);		pParser->addRule(pTypedefStatement);
	pUnionDefinition = new UnionDefinition;		pPushUnion = new PushUnion(pRepo);  
	pUnionDefinition->addAction(pPushUnion);		pParser->addRule(pUnionDefinition);


	
	return pParser;
}
catch(std::exception& ex)
{
	std::cout << "\n\n  " << ex.what() << "\n\n";
	return 0;
}
}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ConfigureParser module\n "
		<< std::string(32,'=') << std::endl;

	// collecting tokens from files, named on the command line

	if(argc < 2)
	{
		std::cout 
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	for(int i=1; i<argc; ++i)
	{
		std::cout << "\n  Processing file " << argv[i];
		std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

		ConfigParseToConsoleTypes configure;
		Parser* pParser = configure.Build();
		try
		{
			if(pParser)
			{
				if(!configure.Attach(argv[i]))
				{
					std::cout << "\n  could not open file " << argv[i] << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			// now that parser is built, use it

			while(pParser->next())
				pParser->parse();
			std::cout << "\n\n";
		}
		catch(std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
		std::cout << "\n\n";
	}
}

#endif
