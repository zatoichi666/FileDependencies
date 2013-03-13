/////////////////////////////////////////////////////////////////////
// SymbolTable.cpp - SymbolTable demo storing qualified types      //
//                   Helper for Project #2                         //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2013       //
/////////////////////////////////////////////////////////////////////


#include "SymbolTable.h"
#include <iostream>

int main()
{
  Title("MT3Q5 - Symbol Table");
  SymbolTable symbols;
  symbols.Add("X", "::", "X.h");
  symbols.Add("Y", "A::", "W.h");
  symbols.Add("X", "B::", "V.cpp");

  if(symbols.containsType("X"))
    std::cout << "\n  ::X is defined in " << symbols.lookUpFile("X", "::");
  if(symbols.containsType("Y"))
    std::cout << "\n  B::X is defined in " << symbols.lookUpFile("X", "B::");
  if(symbols.containsType("X"))
    std::cout << "\n  A::Y is defined in " << symbols.lookUpFile("Y", "A::");
  if(symbols.containsType("Foobar"))
    std::cout << "\n  ::Foobar is defined in " << symbols.lookUpFile("Foobar", "::");
  else
    std::cout << "\n  ::Foobar not defined";
  std::cout << "\n\n";
}