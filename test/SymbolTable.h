#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <unordered_map>
#include <string>
#include <list>
#include <iostream>
#include "Utility.h"

typedef std::string Namespace;
typedef std::string SourceFile;
typedef std::pair<Namespace, SourceFile> NamespaceInfo;
typedef std::list<NamespaceInfo> Namespaces;
typedef std::string TypeName;
typedef std::unordered_map<TypeName, Namespaces> Symbols;


class SymbolTable {
public:
  void Add(TypeName t, Namespace ns, SourceFile f)
  {
    NamespaceInfo info;
    info.first = ns;
    info.second = f;
    Symbols::iterator iter = symbols.find(t);
    if(iter != symbols.end())
    {
      iter->second.push_back(info);
    }
    else
    {
      Namespaces nsp;
      nsp.push_back(info);
      symbols[t] = nsp;
    }
  }
  bool containsType(const TypeName& type)
  {
    return symbols.find(type) != symbols.end();
  }
  SourceFile lookUpFile(const TypeName& type, const Namespace& ns)
  {
    Symbols::iterator iter = symbols.find(type);
    if(iter == symbols.end())
      return "";
    for(auto info : iter->second)
      if(info.first == ns)
        return info.second;
    return "";
  }
private:
  std::unordered_map<TypeName, Namespaces> symbols;
};

#endif // !SYMBOLTABLE_H