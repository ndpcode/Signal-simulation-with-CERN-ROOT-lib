path=C:\Program Files\Microsoft Visual Studio 10.0\VC\bin;C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;$path$

del MyFancyClassDict.cxx

del MyFancyClassDict.h

C:\SoftwareRepository\root_v6.24.00\bin\rootcint.exe MyFancyClassDict.cxx -c -p MyFancyClass.h MyFancyClassLinkDef.h

pause