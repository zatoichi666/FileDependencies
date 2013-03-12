///////////////////////////////////////////////////////////////
// Executive.cpp - Top level executable for SP13 Project 1   //
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
Package Operations: 
===================
This package serves as the top level executable {int main(...)}.
It receives command line parameters from the OS, and parses C++ 
source code and headers for type relationship analysis for the 
Spring 2012 CSE-687 project number 1.

This executive takes two passes, if the user specifies parsing at
the command line.  The first pass to find all user-defined types
and add them into the graph (vertices).  The second pass is to 
find all the relationships between the types and add them to the 
graph.  

Build Process:
==============
Required files
- Executive, Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
ActionsAndRules.h, ActionsAndRules.cpp, 
ConfigureParser.h, ConfigureParser.cpp,
ItokCollection.h, SemiExpression.h, SemiExpression.cpp, 
tokenizer.h, tokenizer.cpp
Build commands (either one)
- devenv display.sln
- cl /EHsc Executive.cpp parser.cpp ActionsAndRules.cpp \
ConfigureParser.cpp ConfigureParserRelat.cpp FoldingRules.cpp 
ScopeStack.cpp semiexpression.cpp tokenizer.cpp /link setargv.obj

Maintenance History:
====================
ver 2.0 : 12 Feb 13
- Adapted by Msynborski

*/

#include <string>
#include <iostream>
#include "Tokenizer.h"
#include "SemiExpression.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "ConfigureParserRelat.h"
#include "FoldingRules.h"
#include "filefind.h"
#include "FileSystem.h"
#include "GraphXml.h"

typedef Display<node, std::string> display;
typedef GraphXml<node, std::string> graphXml;

#ifdef TEST_EXEC

void readGraphXmlFile(std::string filename)
{
	graph g;
	std::string line;
	std::ifstream xmlFile (filename);
	std::string infile;

	if (xmlFile.is_open())
	{
		while ( xmlFile.good() )
		{
			std::getline(xmlFile,line);
			infile += line;

		}
		xmlFile.close();
	}
	XmlReader rdr(infile);
	g = graphXml::readXml(rdr);
	display::show(g);
}


void printOutGraph()
{
	GraphSingleton *s;
	s = GraphSingleton::getInstance();
	graph g;
	g = s->getGraph();
	
	graphXml::printPrettyGraph(g);
}

void writeGraphXmlFile()
{
	GraphSingleton *s;
	s = GraphSingleton::getInstance();
	graph g;
	g = s->getGraph();
	graphXml::writeXml("out.xml",g);
}

void printBanner()
{
	std::cout << "cppAnaly.exe by Matt Synborski\n";
	std::cout << "______________________________\n\n";
}

void printUsage()
{
	std::cout << "Usage:\n______\n";
	std::cout << "1. \"cppAnaly -h\" or \"cppAnaly /?\" or \"parser\" Prints this usage information\n";
	std::cout << "2. \"cppAnaly {folder}\" will execute on a specific folder without recursing into subfolders.\n";
	std::cout << "3. \"cppAnaly -R {folder}\" will execute on a specific folder recursing into subfolders.\n";
	std::cout << "4. \"cppAnaly -G {file}.xml\" will display a graphXml file without parsing C++ files.\n";
}

void pressAKeyPrompt()
{	
	std::cout << "\nPress a key to continue\n";
	std::getchar();
}

std::vector<std::string> getExtListFromCommandArgs(int argc, char* argv[])
{
	std::vector<std::string> extList;
	std::string cmdArguments = argv[1];
	if (cmdArguments == "-R")
		for (int i=3;i<argc; i++)
			extList.push_back(argv[i]);
	else
		for (int i=2;i<argc; i++)
			extList.push_back(argv[i]);

	return extList;
}

std::vector<std::string> getFileListToParse(int argc, char* argv[], bool isRecursive)
{
	std::string cmdArguments = argv[1];
	std::vector<std::string> extList;
	std::string path;
	std::vector<std::string> vecTxtFiles;
	path = argv[1];
	if (cmdArguments == "-R")
		path = argv[2];
	Directory dir;
	dir.setCurrentDirectory(path);
	findFile_r( vecTxtFiles, dir, isRecursive, getExtListFromCommandArgs( argc, argv ) );

	return vecTxtFiles;
}

void demonstrateGraphSearch()
{
	GraphSingleton *s;
	s = GraphSingleton::getInstance();
	graph g;
	g = s->getGraph();
	std::cout << "\nDemonstrating search functions for vertices and edges\n";
	std::cout << "_____________________________________________________\n";
	s->graphSearchVertex("CPolygon");
	std::cout << "\n";
	s->graphSearchEdge("inherits");
	std::cout << "\n";
}

void processAFolderPass1(int argc, char* argv[], bool isRecursive, std::vector<std::string> fileList)
{
	ConfigParseToConsole configure;
	Parser* pParser;
	pParser = configure.Build();

	for(std::vector<std::string>::iterator iterTxt = fileList.begin();
		iterTxt != fileList.end();
		++iterTxt)
	{
		try
		{
			if(pParser)
			{
				if(!configure.Attach(*iterTxt))
				{
					std::cout << "\n  could not open file " << *iterTxt << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";

			}
			std::cout << "\n Parsing file: " << *iterTxt << "\n";
			while(pParser->next())
				pParser->parse();
		}
		catch(std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
	}
}

void processAFolderPass2(int argc, char* argv[], bool isRecursive, std::vector<std::string> fileList)
{
	ConfigParseToConsoleRelat configure;
	Parser* pParser;
	pParser = configure.Build();
	
	for(std::vector<std::string>::iterator iterTxt = fileList.begin();
		iterTxt != fileList.end();
		++iterTxt)
	{
		
		try
		{
			if(pParser)
			{
				if(!configure.Attach(*iterTxt))
				{
					std::cout << "\n  could not open file " << *iterTxt << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
			}
			//std::cout << "\n Parsing file: " << *iterTxt << "\n";
			while(pParser->next())
				pParser->parse();
		}
		catch(std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
	}
}

int main(int argc, char* argv[])
{
	printBanner();
	std::string cmdArg;
	std::string direct = Directory::getCurrentDirectory();
	if (argc == 1)
		printUsage();
	else if ((argc == 2))
	{	cmdArg = argv[1];
		if  ((cmdArg == "-h") || (cmdArg == "/?"))
			printUsage();
		else
		{	if (Directory::exists(argv[1]))
				std::cout << "No file extensions were provided to analyze in \\" << direct << "\n";
			else
				std::cout << "The specified directory doesn't exist.  \nPlease ensure the correct path is specified.\n\n";
		}
	}
	if (argc >= 3)
	{		cmdArg = argv[1];
		if (cmdArg == "-R") {
			std::vector<std::string> fileList = getFileListToParse( argc, argv, true);
			processAFolderPass1(argc, argv, true, fileList); processAFolderPass2(argc, argv, true, fileList);
			printOutGraph();demonstrateGraphSearch();
			Directory::setCurrentDirectory(direct);
			writeGraphXmlFile();
		}
		else if (cmdArg == "-G")
		{	cmdArg = argv[2];
			if (cmdArg.find(".xml") != std::string::npos)
				readGraphXmlFile(argv[2]);
			else
				std::cout << "Invalid graph file specified.  Did you specify a .xml file?\n";
		}
		else {			
			std::vector<std::string> fileList = getFileListToParse( argc, argv, false);
			
			processAFolderPass1(argc, argv, false, fileList); 
			
			processAFolderPass2(argc, argv, false, fileList);
			
			printOutGraph();demonstrateGraphSearch();
			Directory::setCurrentDirectory(direct);
			writeGraphXmlFile();
		}
	}
	pressAKeyPrompt();
	return 0;
}

#endif