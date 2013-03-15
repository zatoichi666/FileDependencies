///////////////////////////////////////////////////////////////
// ActionsAndRules.cpp                                       //
// Ver 2.0                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Dell E6510, Windows 7                        //
// Application: CSE687 - OOD, Pr#2, Spring 2013              //
// Author:      Matt Synborski                               //
//              matthewsynborski@gmail.com                   //
///////////////////////////////////////////////////////////////
//                                                           //
// Originally written by James Fawcett for CSE-687 adapted   // 
//                         by Matt Synborski                 //
//                                                           //
///////////////////////////////////////////////////////////////

#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ActionsAndRules class\n "
            << std::string(30,'=') << std::endl;

  try
  {
    //std::queue<std::string> resultsQ;
    PrintPreproc pp;
    PreprocStatement pps;
    pps.addAction(&pp);

    FunctionDefinition fnd;
    PrintFunction prt;
    fnd.addAction(&prt);

    Toker toker("../ActionsAndRules.h");
    SemiExp se(&toker);
    Parser parser(&se);
    parser.addRule(&pps);
    parser.addRule(&fnd);
    while(se.get())
      parser.parse();
    std::cout << "\n\n";
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}
#endif
