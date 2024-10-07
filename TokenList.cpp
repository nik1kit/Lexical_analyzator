#include "TokenList.h"
#include <iostream>

TokenList::TokenList() {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        hashTable[i] = nullptr;
    }
}

TokenList::~TokenList() {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        delete hashTable[i];
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
    int index;

    if (token.type == TokenType::ERROR) {
        // Для токенов с ошибками используем вторую половину таблицы
        index = (HASH_TABLE_SIZE / 2) + (token.lexeme[0] % (HASH_TABLE_SIZE / 2));
    }
    else {
        // Основные токены обрабатываются обычной хеш-функцией
        index = hashFunction(token.lexeme);

        // Линейное пробирование при коллизии
        while (hashTable[index] != nullptr && hashTable[index]->lexeme != token.lexeme) {
            index = (index + 1) % (HASH_TABLE_SIZE / 2); // Пробирование только в пределах первой половины
        }
    }

    // Если ячейка пуста, добавляем новый токен
    if (hashTable[index] == nullptr) {
        hashTable[index] = new Token(token.type, token.lexeme, index);
    }
}

void TokenList::printTokens(std::ofstream& outputFile) {
    // Сначала выводим обычные токены из первой половины таблицы
    for (int i = 0; i < HASH_TABLE_SIZE / 2; ++i) {
        if (hashTable[i] != nullptr) {
            std::string tokenType;
            switch (hashTable[i]->type) {
            case TokenType::KEYWORD:
                tokenType = "KEYWORD";
                break;
            case TokenType::ID_NAME:
                tokenType = "ID_NAME";
                break;
            case TokenType::INT_NUM:
                tokenType = "INT_NUM";
                break;
            case TokenType::FLOAT_NUM:
                tokenType = "FLOAT_NUM";
                break;
            case TokenType::OPERATOR:
                tokenType = "OPERATOR";
                break;
            case TokenType::DELIMITER:
                tokenType = "DELIMITER";
                break;
            case TokenType::ERROR:
                tokenType = "ERROR";
                continue;  // Пропускаем добавление этого токена в хеш-таблицу
            default:
                tokenType = "UNKNOWN";
                break;
            }
            outputFile << tokenType << " | " << hashTable[i]->lexeme << " | " << hashTable[i]->index << std::endl;
        }
    }
    outputFile << "\n";
    // Затем выводим токены с ошибками из второй половины таблицы
    for (int i = HASH_TABLE_SIZE / 2; i < HASH_TABLE_SIZE; ++i) {
        if (hashTable[i] != nullptr) {
            outputFile << "ERROR" << " | " << hashTable[i]->lexeme << " | " << hashTable[i]->index << std::endl;
        }
    }
}
