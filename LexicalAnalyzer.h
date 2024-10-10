#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <fstream>
#include <string>
#include "TokenList.h"

class LexicalAnalyzer {
public:
    LexicalAnalyzer(const std::string& inputFileName, const std::string& outputFileName);
    ~LexicalAnalyzer();
    void analyze();

private:
    std::ifstream inputFile;
    std::ofstream outputFile;
    TokenList tokenList;

    Token getNextLexeme();
    TokenType getKeywordOrIdentifier(const std::string& lexeme);
    TokenType getNumber(const std::string& lexeme);
    bool isOperator(const std::string& str) const;
    bool isDelimiter(char c) const;
    void handleOperators(char c);
    void handleDelimiters(char c);
    bool isValidIdentifier(const std::string& word) const; // Объявление функции
};

#endif // LEXICALANALYZER_H
