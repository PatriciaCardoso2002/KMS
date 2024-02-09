Main_app.cpp corresponde ao app_server

main_app.o com o erro abaixo

g++ -std=c++17 -o main_app main_app.o
/usr/bin/ld: main_app.o: warning: relocation against `_ZTV5Block' in read-only section `.text._ZN5BlockD2Ev[_ZN5BlockD5Ev]'
/usr/bin/ld: main_app.o: in function `main':
main_app.cpp:(.text+0x101): undefined reference to `LoadAscii::initialize()'
/usr/bin/ld: main_app.cpp:(.text+0x110): undefined reference to `LoadAscii::runBlock()'
/usr/bin/ld: main_app.o: in function `Block::~Block()':
main_app.cpp:(.text._ZN5BlockD2Ev[_ZN5BlockD5Ev]+0xf): undefined reference to `vtable for Block'
/usr/bin/ld: main_app.o: in function `LoadAscii::LoadAscii(std::initializer_list<Signal*>, std::initializer_list<Signal*>)':
main_app.cpp:(.text._ZN9LoadAsciiC2ESt16initializer_listIP6SignalES3_[_ZN9LoadAsciiC5ESt16initializer_listIP6SignalES3_]+0x4a): undefined reference to `Block::Block(std::initializer_list<Signal*>, std::initializer_list<Signal*>)'
/usr/bin/ld: main_app.cpp:(.text._ZN9LoadAsciiC2ESt16initializer_listIP6SignalES3_[_ZN9LoadAsciiC5ESt16initializer_listIP6SignalES3_]+0x51): undefined reference to `vtable for LoadAscii'
/usr/bin/ld: main_app.o: in function `LoadAscii::~LoadAscii()':
main_app.cpp:(.text._ZN9LoadAsciiD2Ev[_ZN9LoadAsciiD5Ev]+0xf): undefined reference to `vtable for LoadAscii'
/usr/bin/ld: warning: creating DT_TEXTREL in a PIE
collect2: error: ld returned 1 exit status
