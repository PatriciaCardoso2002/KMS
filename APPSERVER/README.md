Por algum motivo a length do vetor dos output signals continua a 0, temos que tentar partir o netXPTO em partes e ver o que realmente precisamos, acho que será mais facil fazer um ficheiro menor á nossa medida.
O console.cpp é um ficheiro que apenas corrige uns erros de compilação, define funções que não estão definidas no netxpto mas que são chamadas a dada altura.
User
(gdb) print outputSignals.size()
$1 = 0
(gdb) print outputSignals[0]
Cannot access memory at address 0x0
(gdb) print *outputSignals[0]
Cannot access memory at address 0x0
(gdb) 

User
Program received signal SIGSEGV, Segmentation fault.
LoadAscii::initialize (this=0x7fffffffdd90) at load_ascii_20200819.cpp:10
10		outputSignals[0]->setSymbolPeriod(symbolPeriod);


Breakpoint 1, LoadAscii::initialize (this=0x7fffffffdd90) at load_ascii_20200819.cpp:10
10		outputSignals[0]->setSymbolPeriod(symbolPeriod);
(gdb) print outputSignals
$1 = std::vector of length 0, capacity 0
(gdb) print symbolPeriod
$2 = 1
(gdb) print samplingPeriod
$3 = 1


g++ -g -o main main_app.cpp netxpto_20200819.cpp load_ascii_20200819.cpp console.cpp
