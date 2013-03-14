/*
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
		for(int i=0; i<(sizeof(keys) / sizeof(keys[0])); ++i)
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
*/

/*
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
		for(int i=0; i<(sizeof(keys) / sizeof(keys[0])); ++i)
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
		//back up the scopestack
		ScopeStack<element> tempStack = p_Repos->scopeStack();
		element elem = p_Repos->scopeStack().pop();
		while ((elem.type != "class") && (p_Repos->scopeStack().size() > 0))
		{
			elem = p_Repos->scopeStack().pop();
		}
		if(elem.type == "class")
		{
			ITokCollection& tc = *pTc;
			size_t keyPosUsing = tc.find("*") > tc.find("&") ? tc.find("&") : tc.find("*");
			std::cout << "\n  Relationship: " << elem.name << " uses " << tc[keyPosUsing - 1];

			GraphSingleton *s;
			s = GraphSingleton::getInstance();

			if (elem.name != tc[keyPosUsing - 1]) // In case of preventing compiler assignment, copy, or destr.
				s->addRelationshipToGraph(elem.name, tc[keyPosUsing - 1], "uses");

		}
		p_Repos->scopeStack() = tempStack;

	}

};
*/

/*
///////////////////////////////////////////////////////////////
// rule to detect aggregation relationship

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
*/