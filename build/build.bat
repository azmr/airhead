cl /nologo /W4 /Z7 ..\airhead_test.cpp
cl /nologo /EP ..\airhead_test.cpp > expanded.cpp
cl /nologo /W4 /Z7 expanded.cpp
