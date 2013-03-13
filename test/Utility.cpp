// MTutility.cpp

#include <iostream>
#include <string>

void Title(const std::string& str, char underline='=', std::ostream& out=std::cout)
{
  out << "\n  " << str;
  out << "\n " << std::string(str.size() + 2, underline);
}