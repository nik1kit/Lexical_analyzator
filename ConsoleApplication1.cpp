#include <iostream>
#include "LexicalAnalyzer.h"
#include "Token.h"


int main() {
    LexicalAnalyzer lexer("input.txt", "output.txt");
    lexer.analyze();
    return 0;
}
