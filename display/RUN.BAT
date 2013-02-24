:run.bat

cd debug
echo off
echo Displaying usage
cppAnaly.exe
cls

echo Testing cppAnaly.exe nonrecursively, with cpp files.
cppAnaly.exe test cpp 
cls

echo Testing cppAnaly.exe recursively, with cpp and h files.
cppAnaly.exe -R test cpp h
cls

echo Testing cppAnaly.exe with load graph mode.  Loads a graph out of a .xml file.  
cppAnaly.exe -G out.xml
cls

echo Please open the .xml file using Internet Explorer to verify that the graph was written in .xml format.  