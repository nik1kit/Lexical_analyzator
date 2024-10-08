#include "TokenList.h"
#include <iostream>

TokenList::TokenList() : tokenCount(0) {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        hashTable[i] = nullptr;
    }
    for (int i = 0; i < MAX_TOKENS; ++i) {
        tokenSequence[i] = nullptr;
    }
}

TokenList::~TokenList() {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        delete hashTable[i];
    }
    for (int i = 0; i < MAX_TOKENS; ++i) {
        delete tokenSequence[i];
    }
}

int TokenList::hashFunction(const std::string& lexeme) const {
    int hash = 0;
    for (char c : lexeme) {
        hash = (hash * 31 + c) % (HASH_TABLE_SIZE / 2); // Хеш-функция только для основной части таблицы
    }
    return hash;
}

void TokenList::addToken(const Token& token) {
    if (tokenCount < MAX_TOKENS) {
        tokenSequence[tokenCount] = new Token(token.type, token.lexeme, token.index);
        tokenCount++;
    }

    // Добавляем в хеш-таблицу
    int index = hashFunction(token.lexeme);
    while (hashTable[index] != nullptr && hashTable[index]->lexeme != token.lexeme) {
        index = (index + 1) % HASH_TABLE_SIZE; // Пробирование при коллизии
    }

    // Если ячейка пуста, добавляем новый токен
    if (hashTable[index] == nullptr) {
        hashTable[index] = new Token(token.type, token.lexeme, token.index);
    }
}

void TokenList::printTokens(std::ofstream& outputFile) {
    for (int i = 0; i < tokenCount; ++i) {
        Token* token = tokenSequence[i];
        if (token != nullptr) {
            std::string tokenType;
            switch (token->type) {
            case TokenType::PROGRAM:
                tokenType = "PROGRAM";
                break;
            case TokenType::BEGIN:
                tokenType = "BEGIN";
                break;
            case TokenType::END:
                tokenType = "END";
                break;
            case TokenType::DESCRIPTIONS:
                tokenType = "DESCRIPTIONS";
                break;
            case TokenType::DESCR:
                tokenType = "DESCR";
                break;
            case TokenType::TYPE:
                tokenType = "TYPE";
                break;
            case TokenType::VARLIST:
                tokenType = "VARLIST";
                break;
            case TokenType::OPERATORS:
                tokenType = "OPERATORS";
                break;
            case TokenType::OP:
                tokenType = "OP";
                break;
            case TokenType::EXPR:
                tokenType = "EXPR";
                break;
            case TokenType::SIMPLEEXPR:
                tokenType = "SIMPLEEXPR";
                break;
            case TokenType::ID_NAME:
                tokenType = "ID_NAME";
                break;
            case TokenType::INT_NUM:
                tokenType = "INT_NUM";
                break;
            case TokenType::OPERATOR:
                tokenType = "OPERATOR";
                break;
            case TokenType::DELIMITER:
                tokenType = "DELIMITER";
                break;
            case TokenType::ERROR:
                tokenType = "ERROR";
                continue;  // Skip adding this token to the hash table
            default:
                tokenType = "UNKNOWN";
                break;
            }
            outputFile << tokenType << " | " << token->lexeme << " | " << i + 1 << std::endl;
        }
    }
    outputFile << "\n";

    // Print tokens with errors from the second half of the hash table
    for (int i = HASH_TABLE_SIZE / 2; i < HASH_TABLE_SIZE; ++i) {
        Token* token = tokenSequence[i];
        if (tokenSequence[i] != nullptr && token->type == TokenType::ERROR) {
            outputFile << "ERROR" << " | " << token->lexeme << " | " << i + 1 << std::endl;
        }
    }
}

