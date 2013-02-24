#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
///////////////////////////////////////////////////////////////
// ActionsAndRules.h - Graph Library                          //
// Ver 1.0                                                   //
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
This module defines several action classes.  Its classes provide 
specialized services needed for specific applications.  The modules
Parser, SemiExpression, and Tokenizer, are intended to be reusable
without change.  This module provides a place to put extensions of
these facilities and is not expected to be reusable. 

Public Interface:
=================
Toker t(someFile);              // create tokenizer instance
SemiExp se(&t);                 // create a SemiExp attached to tokenizer
Parser parser(se);              // now we have a parser
Rule1 r1;                       // create instance of a derived Rule class
Action1 a1;                     // create a derived action
r1.addAction(&a1);              // register action with the rule
parser.addRule(&r1);            // register rule with parser
while(se.getSemiExp())          // get semi-expression
parser.parse();               //   and parse it

Build Process:
==============
Required files
- Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
Build commands (either one)
- devenv CodeAnalysis.sln
- cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
semiexpression.cpp tokenizer.cpp /link setargv.obj

Maintenance History:
====================
ver 2.0 : 01 Jun 11
- added processing on way to building strong code analyzer
ver 1.1 : 17 Jan 09
- changed to accept a pointer to interfaced ITokCollection instead
of a SemiExpression
ver 1.0 : 12 Jan 06
- first release

*/
//

/*
TODO: 
1. Figure out what scope I'm in when I hit a type of the below rules.  Print it out 
2. Resolve specific indexing, rules should check for presence of tokens and order of appearance.
3. Inheritance logic complete
4. Using logic improvement
*/

#include <queue>
#include <string>
#include <sstream>
#include <stack>
#include <iterator>

#include "Parser.h"
#include "ITokCollection.h"
#include "ScopeStack.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

#include "GraphSingleton.h"

using namespace GraphLib;

typedef GraphLib::Graph<std::string, std::string> graph;
typedef GraphLib::Vertex<std::string, std::string> vertex;

///////////////////////////////////////////////////////////////
// ScopeStack element is application specific

struct element
{
	std::string type;
	std::string name;
	size_t lineCount;
	std::string show()
	{
		std::ostringstream temp;
		temp << "(";
		temp << type;
		temp << ", ";
		temp << name;
		temp << ", ";
		temp << lineCount;
		temp << ")"; 
		return temp.str();
	}
};

///////////////////////////////////////////////////////////////
// Repository instance is used to share resources
// among all actions.

class Repository  // application specific
{
	ScopeStack<element> stack;
	Toker* p_Toker;
public:
	Repository(Toker* pToker)
	{
		p_Toker = pToker;
	}
	ScopeStack<element>& scopeStack()
	{
		return stack;
	}
	Toker* Toker()
	{
		return p_Toker;
	}
	size_t lineCount()
	{
		return (size_t)(p_Toker->lines());
	}
};

///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--BeginningOfScope rule";
		if(pTc->find("{") < pTc->length())
		{
			doActions(pTc);
			return false;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction
{
	Repository* p_Repos;
public:
	HandlePush(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{

		element elem;
		elem.type = "unknown";
		elem.name = "anonymous";
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--EndOfScope rule";
		if(pTc->find("}") < pTc->length())
		{
			doActions(pTc);
			return false;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePop : public IAction
{
	Repository* p_Repos;
public:
	HandlePop(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		if(p_Repos->scopeStack().size() == 0)
			return;
		element elem = p_Repos->scopeStack().pop();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect Composition relationships

class CompositionOpportunity : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]	= { "#", "(", ")", "{", "}", "return", "+", "-", "*", "&", 
			"ios", "ios_base", "istream", "iostream", "ostream","streambuf", "ifstream", "fstream", 
			"ofstream", "filebuf", "bool", "char", "int", "float", "double", "void", "wchar_t", 
			"long", "<<", ">>", "enum", "struct", "class", "=", "unordered_map", "vector", "size_t",
			"string", "list"
		};
		for (int i=0; i<39; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}
	bool containsSpecialKeyword(ITokCollection& tc)
	{
		for (int i=0;i<(int)tc.length();i++)
			if (isSpecialKeyWord(tc[i]))
				return true;
		return false;
	}

	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if ( !containsSpecialKeyword(tc) && (tc.length() > 2) )
		{
			//std::cout << "\n--Composition relationship rule";
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print Composition relationship to console
///////////////////////////////////////////////////////////////
// If there are scope resolution operators, the last one precedes the datatype
class PrintComposition : public IAction
{
	Repository* p_Repos;
public:
	PrintComposition(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{

		if(p_Repos->scopeStack().size() == 0)
			return;

		element elem = p_Repos->scopeStack().pop(); // reverts the scopeStack, leaving a local copy
		p_Repos->scopeStack().push(elem);           // called elem, in lieu of a peek() function

		if((elem.type == "class") || (elem.type == "struct") )
		{
			ITokCollection& tc = *pTc;
			ITokCollection& tc_copy = *pTc;
			size_t lastScopeRes = tc_copy.find("::");
			while (tc_copy.find("::") != tc_copy.length())
			{
				lastScopeRes = tc_copy.find("::");
				tc_copy[lastScopeRes] = "-";
			}
			size_t keyPosComposes;
			if (lastScopeRes < tc.length())
			{
				keyPosComposes = lastScopeRes + 1;
			}
			else
			{
				keyPosComposes = tc.find(",") - 2;
				if (tc.find(",") == tc.length())
					keyPosComposes = tc.find(";") - 2;
			}
			//std::cout << "\n  Relationship: " << elem.name << " composes " << tc[keyPosComposes] << " len:" << tc.length();
			GraphSingleton *s;
			s = GraphSingleton::getInstance();

			s->addRelationshipToGraph(elem.name, tc[keyPosComposes], "composes");

		}
	}
};

///////////////////////////////////////////////////////////////
// rule to detect Using relationships

class UsingOpportunity : public IRule
{
public:
	bool isStdDatatype(const std::string& tok)
	{
		const static std::string keys[]
		= {  "ios", "ios_base", "istream", "iostream", "ostream","streambuf", "ifstream", "fstream", 
			"ofstream", "filebuf", "bool", "char", "int", "float", "double", "void", "wchar_t", 
			"long", "string"
		};
		for(int i=0; i<19; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}

	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		size_t keyPosClosedParen = tc.find(")");
		size_t keyPosOpenParen = tc.find("(");	
		size_t keyPosUsing = tc.find("*") > tc.find("&") ? tc.find("&") : tc.find("*");

		if ((keyPosClosedParen < tc.length()) && (keyPosUsing < tc.length()) && 
			(keyPosOpenParen < tc.length()) && (keyPosUsing > keyPosOpenParen) && 
			(keyPosClosedParen > keyPosUsing) && !isStdDatatype(tc[keyPosUsing-1])) 
		{
			//std::cout << "\n--Using relationship rule";
			doActions(pTc);
			return false;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print Using relationship to console

class PrintUsing : public IAction
{
	Repository* p_Repos;

public:
	PrintUsing(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{

		if(p_Repos->scopeStack().size() == 0)
			return;
		element elem = p_Repos->scopeStack().pop();
		p_Repos->scopeStack().push(elem);  // leaves the scopestack uneffected
		
		if(elem.type == "class")
		{
			ITokCollection& tc = *pTc;
			size_t keyPosUsing = tc.find("*") > tc.find("&") ? tc.find("&") : tc.find("*");
			//std::cout << "\n  Relationship: " << elem.name << " uses " << tc[keyPosUsing - 1];

			GraphSingleton *s;
			s = GraphSingleton::getInstance();

			if (elem.name != tc[keyPosUsing - 1]) // In case of preventing compiler assignment, copy, or destr.
				s->addRelationshipToGraph(elem.name, tc[keyPosUsing - 1], "uses");

		}
	}

};

///////////////////////////////////////////////////////////////
// rule to detect Inheritance relationships

class InheritanceOpportunity : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			size_t keyPos1 = tc.find("{");
			size_t keyPos2 = tc.find("class");
			size_t keyPos3 = tc.find(":");
			if ((keyPos1 < tc.length()) && (keyPos2 < tc.length()) && (keyPos3 < tc.length()))
			{
				//std::cout << "\n--Inheritance Relationship rule";
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print Inheritance relationship to console

class PrintInheritance : public IAction
{
	Repository* p_Repos;



public:
	PrintInheritance(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		if(p_Repos->scopeStack().size() == 0)
			return;
		element elem = p_Repos->scopeStack().pop();
		p_Repos->scopeStack().push(elem);  // leaves the scopeStack uneffected

		ITokCollection& tc = *pTc;
		size_t posInheritor = tc.find("class") > tc.find("struct") ? tc.find("struct") + 1 : tc.find("class") + 1;
		size_t posInherits = tc.find("{") - 1; 
		if (tc[posInherits] == "\n")
			if (tc[posInherits+1] == "}")
				posInherits++;
			else
				posInherits--;

		//std::cout << "\n  Relationship: " << tc[posInheritor] << " inherits " << tc[posInherits];

		GraphSingleton *s;
		s = GraphSingleton::getInstance();

		s->addRelationshipToGraph(tc[posInherits], tc[posInheritor], "inherits");
	}

};

///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements

class AggregationOpportunity : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if(pTc->find("new") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print aggregation relationship to console

class PrintAggregation : public IAction
{
	Repository* p_Repos;
public:
	PrintAggregation(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		if(p_Repos->scopeStack().size() == 0)
			return;
		element elem = p_Repos->scopeStack().pop();
		p_Repos->scopeStack().push(elem);  // leaves the scopestack uneffected

		if((elem.type == "class") || (elem.type == "struct"))
		{
			ITokCollection& tc = *pTc;
			size_t len = tc.find("new");
			//std::cout << "\n  Relationship: " + elem.name + " aggregates " + tc[len + 1];
			// pop anonymous scope


			GraphSingleton *s;
			s = GraphSingleton::getInstance();

			s->addRelationshipToGraph(elem.name,tc[len + 1],"aggregates");

			//Do addEdge
		}
	}
};

///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements

class PreprocStatement : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{

		if(pTc->find("#") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console

class PrintPreproc : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		//std::cout << "\n\n  Type detected: Preprocessor: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect typedef statements

class EnumStatement : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{

		if(pTc->find("enum") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console

class PrintEnum : public IAction
{

public:

	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		size_t len = tc.find("enum");
		std::string enumName;
		GraphSingleton *s;
		s = GraphSingleton::getInstance();

		if (tc[len+1] == "{")
			enumName = "noName";
		else
			enumName = tc[len + 1];

		std::cout << "\n  Type detected: Enum: " << enumName << "\n";
		s->addTypeToGraph(enumName);

	}
};

///////////////////////////////////////////////////////////////
// action to push enum on scopestack 

class EnumScope : public IAction
{
	Repository* p_Repos;

public:
	EnumScope(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		if (p_Repos->scopeStack().size() > 0)
		{
			element cacheElement = p_Repos->scopeStack().pop();
			//

			p_Repos->scopeStack().push(cacheElement);
		}


		ITokCollection& tc = *pTc;
		size_t len = tc.find("enum");
		std::string enumName;

		if (tc[len+1] == "{")
			enumName = "noName";
		else
			enumName = tc[len + 1];

		// push enum on scopestack
		element elem;
		elem.type = "enum";
		elem.name = enumName;
		elem.lineCount = p_Repos->lineCount();

		p_Repos->scopeStack().push(elem);
		std::cout << ".";
	}
};


///////////////////////////////////////////////////////////////
// rule to detect typedef statements

class TypedefStatement : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{

		if(pTc->find("typedef") < pTc->length() && !(pTc->find("enum")))
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console

class PrintTypedef : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		size_t len = tc.find("typedef");
		std::cout << "\n  Type detected: Typedef: " << tc[len + 2];

	}
};

///////////////////////////////////////////////////////////////
// rule to detect function definitions

class FunctionDefinition : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
		= { "for", "while", "switch", "if", "catch" };
		for(int i=0; i<5; ++i)
			if(tok == keys[i])
				return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			size_t len = tc.find("(");
			if(len < tc.length() && !isSpecialKeyWord(tc[len-1]))
			{
				//std::cout << "\n--FunctionDefinition rule";
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class PushFunction : public IAction
{
	Repository* p_Repos;
public:
	PushFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push function scope
		std::string name = (*pTc)[pTc->find("(") - 1];
		element elem;
		elem.type = "function";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect class definitions

class ClassDefinition : public IRule
{
public:

	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			size_t len = tc.find("class");
			if(len < tc.length() /*&& (tc.find(":") == tc.length() )*/)
			{
				//std::cout << "\n--ClassDefinition rule";
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push class name onto ScopeStack and add vertex to graph

class PushClass : public IAction
{
	Repository* p_Repos;

public:
	PushClass(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push class/struct scope
		std::string name = (*pTc)[pTc->find("class") + 1];
		element elem;
		elem.type = "class";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);

		GraphSingleton *s;
		s = GraphSingleton::getInstance();
		s->addTypeToGraph(name);
	}

};

///////////////////////////////////////////////////////////////
// action to push class name onto ScopeStack without adding a vertex to the graph

class ClassScope : public IAction
{
	Repository* p_Repos;

public:
	ClassScope(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push class/struct scope
		std::string name = (*pTc)[pTc->find("class") + 1];
		element elem;
		elem.type = "class";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);

	}

};


///////////////////////////////////////////////////////////////
// rule to detect struct definitions

class StructDefinition : public IRule
{
public:

	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			size_t len = tc.find("struct");
			if(len < tc.length() )
			{
				//std::cout << "\n--StructDefinition rule";
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push struct name onto ScopeStack

class PushStruct : public IAction
{
	Repository* p_Repos;


public:
	PushStruct(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push class/struct scope
		std::string name = (*pTc)[pTc->find("struct") + 1];
		element elem;
		elem.type = "struct";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);

		GraphSingleton *s;
		s = GraphSingleton::getInstance();
		s->addTypeToGraph(name);
	}
};

///////////////////////////////////////////////////////////////
// action to push struct name onto ScopeStack

class StructScope : public IAction
{
	Repository* p_Repos;

public:
	StructScope(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push class/struct scope
		std::string name = (*pTc)[pTc->find("struct") + 1];
		element elem;
		elem.type = "struct";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
		std::cout << "Here\n";
	}
};


///////////////////////////////////////////////////////////////
// rule to detect struct definitions

class UnionDefinition : public IRule
{
public:

	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if(tc[tc.length()-1] == "{")
		{
			size_t len = tc.find("union");
			if(len < tc.length() )
			{
				//std::cout << "\n--UnionDefinition rule";
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push union name onto ScopeStack

class PushUnion : public IAction
{
	Repository* p_Repos;


public:
	PushUnion(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push class/struct scope
		std::string name = (*pTc)[pTc->find("union") + 1];
		element elem;
		elem.type = "union";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);

		GraphSingleton *s;
		s = GraphSingleton::getInstance();
		s->addTypeToGraph(name);

	}
};

///////////////////////////////////////////////////////////////
// action to push union name onto ScopeStack

class UnionScope : public IAction
{
	Repository* p_Repos;

public:
	UnionScope(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push class/struct scope
		std::string name = (*pTc)[pTc->find("union") + 1];
		element elem;
		elem.type = "union";
		elem.name = name;
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);

	}
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console

class PrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		std::cout << "\n\n  FuncDef Stmt: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// action to send signature of a function def to console

class PrettyPrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		pTc->remove("public");
		pTc->remove(":");
		pTc->trimFront();
		int len = pTc->find(")");
		std::cout << "\n\n  Pretty Stmt:    ";
		for(int i=0; i<len+1; ++i)
			std::cout << (*pTc)[i] << " ";
	}
};

#endif
