#ifndef TOKENLIST_H
#define TOKENLIST_H

#include "Token.h"
#include <fstream>

class TokenList {
public:
    TokenList();
    ~TokenList();

    void addToken(const Token& token);
    int hashFunction(const std::string& lexeme) const;

    void printTokens(std::ofstream& outputFile);

private:
    static const int HASH_TABLE_SIZE = 1000;
    Token* hashTable[HASH_TABLE_SIZE];

    static const int MAX_TOKENS = 10000; // Максимальное количество токенов
    Token* tokenSequence[MAX_TOKENS]; // Для вывода токенов по порядку
    int tokenCount; // Счетчик количества добавленных токенов
};

#endif 
