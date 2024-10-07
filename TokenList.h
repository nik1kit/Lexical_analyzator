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

    // Объявление функции вывода токенов
    void printTokens(std::ofstream& outputFile);

private:
    static const int HASH_TABLE_SIZE = 100;
    Token* hashTable[HASH_TABLE_SIZE];
};

#endif // TOKENLIST_H
