#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <iostream>
#include <fstream>
#include <string>
#include "TokenList.h"
#include "Token.h"

class LexicalAnalyzer {
public:
    LexicalAnalyzer(const std::string& inputFileName, const std::string& outputFileName);
    ~LexicalAnalyzer();
    void analyze();

private:
    std::ifstream inputFile;
    std::ofstream outputFile;
    TokenList tokenList;

    // ћетоды дл€ конечного автомата
    TokenType getKeywordOrIdentifier(const std::string& lexeme);
    TokenType getNumber(const std::string& lexeme);
    bool isOperator(char c) const;
    bool isDelimiter(char c) const;
    void handleOperators(char c);
    void handleDelimiters(char c);
    Token getNextLexeme(); // Ќовый метод дл€ получени€ следующей лексемы
};

#endif
