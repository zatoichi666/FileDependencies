:run.bat

cd debug
echo off

cls
echo Testing ActionsAndRules
pause
ActionsAndRules.exe 
pause

cls
echo Testing Graph
pause
Graph
pause

cls
echo Testing GraphSingleton
pause
GraphSingleton.exe


cls
echo Testing GraphXml
pause
GraphXml.exe
more test.xml
pause

cls
echo Testing Tokenizer
Tokenizer.exe ../graph.cpp
pause

cd ..