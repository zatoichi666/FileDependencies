///////////////////////////////////////////////////////////////
// ConfigureParserRelat.cpp - builds and configures parsers  //
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
// This package configures the parser for the second pass    //
// in the source files that will find type relationships and //
// add them to the graph.                                    //
///////////////////////////////////////////////////////////////

#include "Parser.h"
#include "SemiExpression.h"
#include "Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParserRelat.h"

extern Repository* pRepo;

//----< destructor releases all parts >------------------------------

ConfigParseToConsoleRelat::~ConfigParseToConsoleRelat()
{
	// when Builder goes out of scope, everything must be deallocated
	delete pFR;
	//delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;

	delete pHandlePush;
	delete pBeginningOfScope;
	delete pHandlePop;
	delete pEndOfScope;

	//delete pCompositionOpportunity;
	//delete pUsingOpportunity;
	//delete pAggregationOpportunity;
	delete pInheritanceOpportunity;

	delete pClassDefinition;
	delete pClassScope;
	delete pEnumStatement;
	delete pEnumScope;

	delete pFunctionDefinition;
	delete pPushFunction;
	delete pStructDefinition;
	delete pStructScope;
	delete pVarDeclaration;
	delete pPushVarDecl;
	delete pGlobalVarDeclaration;
	delete pPushGlobalVarDecl;
}
//----< attach toker to a file stream or stringstream >------------



bool ConfigParseToConsoleRelat::Attach(const std::string& name, bool isFile)
{
	if(pToker == 0)
		return false;
	return pToker->attach(name, isFile);
}
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToConsoleRelat::Build()
{
	try
	{   // add Parser's main parts
		pToker = new Toker;
		pToker->returnComments(false);
		pSemi = new SemiExp(pToker);
		pSemi->returnNewLines(false);
		pParser = new Parser(pSemi);
		//pRepo = new Repository(pToker);

		// add code folding rules
		pFR = new codeFoldingRules;
		pParser->addFoldingRules(pFR);

		// configure to manage scope
		// these must come first - they return true on match
		// so rule checking continues
		pBeginningOfScope = new BeginningOfScope();	pHandlePush = new HandlePush(pRepo);
		pBeginningOfScope->addAction(pHandlePush);	pParser->addRule(pBeginningOfScope);
		pEndOfScope = new EndOfScope();	pHandlePop = new HandlePop(pRepo);
		pEndOfScope->addAction(pHandlePop);	pParser->addRule(pEndOfScope);

		// Rules and actions to push types on scope stack	
		pClassDefinition = new ClassDefinition;	pClassScope = new ClassScope(pRepo);  
		pClassDefinition->addAction(pClassScope);	pParser->addRule(pClassDefinition);
		pEnumStatement = new EnumStatement();	pEnumScope = new EnumScope(pRepo);
		pEnumStatement->addAction(pEnumScope);	pParser->addRule(pEnumStatement);
		pFunctionDefinition = new FunctionDefinition;	pPushFunction = new PushFunction(pRepo);  
		pFunctionDefinition->addAction(pPushFunction);	pParser->addRule(pFunctionDefinition);
		pStructDefinition = new StructDefinition;	pStructScope = new StructScope(pRepo);  
		pStructDefinition->addAction(pStructScope);	pParser->addRule(pStructDefinition);

		// configure to detect and act on function definitions
		// these will stop further rule checking by returning false
		//pCompositionOpportunity = new CompositionOpportunity();	pPrintComposition = new PrintComposition(pRepo);
		//pCompositionOpportunity->addAction(pPrintComposition);	pParser->addRule(pCompositionOpportunity);
		//pUsingOpportunity = new UsingOpportunity();	pPrintUsing = new PrintUsing(pRepo);
		//pUsingOpportunity->addAction(pPrintUsing);	pParser->addRule(pUsingOpportunity);
		pInheritanceOpportunity = new InheritanceOpportunity();	pPrintInheritance = new PrintInheritance(pRepo);
		pInheritanceOpportunity->addAction(pPrintInheritance);	pParser->addRule(pInheritanceOpportunity);
		//pAggregationOpportunity = new AggregationOpportunity();	pPrintAggregation = new PrintAggregation(pRepo);
		//pAggregationOpportunity->addAction(pPrintAggregation);	pParser->addRule(pAggregationOpportunity);

		//Project 2 configurations
		pVarDeclaration = new VarDeclaration(pRepo);		pPushVarDecl = new PushVarDecl(pRepo);  
		pVarDeclaration->addAction(pPushVarDecl);		pParser->addRule(pVarDeclaration);
		pGlobalVarDeclaration = new GlobalVarDeclaration(pRepo);		pPushGlobalVarDecl = new PushGlobalVarDecl(pRepo);  
		pGlobalVarDeclaration->addAction(pPushGlobalVarDecl);		pParser->addRule(pGlobalVarDeclaration);
		return pParser;
	}
	catch(std::exception& ex)
	{	std::cout << "\n\n  " << ex.what() << "\n\n";
	return 0;
	}
}

#ifdef TEST_CONFIGUREPARSERRELAT

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ConfigureParserRelat module\n "
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

		ConfigParseToConsoleRelat configure;
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
