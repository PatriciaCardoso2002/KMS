#include <iostream>

namespace ConsoleOutputCommon {
    void clrLine() {
        // ANSI escape code para limpar a linha
        std::cout << "\033[2K"; // Limpa a linha inteira
        std::cout << "\r"; // Retorna o cursor para o início da linha
    }

    void moveCursorTopLeft() {
        // ANSI escape code para mover o cursor para o topo esquerdo do console
        std::cout << "\033[H";
    }
}
