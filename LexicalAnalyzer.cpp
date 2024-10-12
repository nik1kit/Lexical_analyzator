#include "LexicalAnalyzer.h"
#include "Token.h"
#include <iostream>

LexicalAnalyzer::LexicalAnalyzer(const std::string& inputFileName, const std::string& outputFileName) {
    inputFile.open(inputFileName);
    outputFile.open(outputFileName);
}

bool LexicalAnalyzer::isValidIdentifier(const std::string& word) const {
    // Проверяем, состоит ли слово только из заглавных и строчных букв латинского алфавита
    for (char c : word) {
        if (!isalpha(c)) { // Если символ не является буквой
            return false; 
        }
    }
    return true; 
}


LexicalAnalyzer::~LexicalAnalyzer() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

TokenType LexicalAnalyzer::getKeywordOrIdentifier(const std::string& lexeme) {
    if (lexeme == "PROGRAM") return TokenType::PROGRAM;
    if (lexeme == "begin") return TokenType::BEGIN;
    if (lexeme == "end") return TokenType::END;
    if (lexeme == "INTEGER") return TokenType::TYPE;
    if (lexeme == "to") return TokenType::EXPR;
    if (lexeme == "do") return TokenType::BEGIN;
    if (lexeme == "if") return TokenType::IF;  
    if (lexeme == "then") return TokenType::THEN;  
    return TokenType::ID_NAME;
}

TokenType LexicalAnalyzer::getNumber(const std::string& lexeme) {
    // Проверяем, является ли строка целым числом
    for (char c : lexeme) {
        if (!isdigit(c)) {
            return TokenType::ERROR; 
        }
    }
    return TokenType::INT_NUM;
}

bool LexicalAnalyzer::isDelimiter(char c) const {
    return (c == '(' || c == ')' || c == ',' || c == ';' || c == '\'' || c == '\"' || c == '.');
}

void LexicalAnalyzer::handleDelimiters(char c) {
    std::string delim(1, c);
    tokenList.addToken(Token(TokenType::DELIMITER, delim, -1));
}

Token LexicalAnalyzer::getNextLexeme() {
    std::string word;
    char c;
    int index = 1;

    while (inputFile.get(c)) {
        if (isspace(c)) {
            index++;
            continue;
        }

        if (isalpha(c)) { // Идентификатор или ключевое слово
            word.clear();
            word += c;
            while (inputFile.get(c) && (isalnum(c) || c == '_')) {
                word += c;
            }
            inputFile.unget(); // Возвращаем последний символ в поток

            // Проверка на ключевое слово PROGRAM
            if (word == "PROGRAM") {
                std::string id;
                if (inputFile >> id && isValidIdentifier(id)) {
                    // Сначала возвращаем токен PROGRAM
                    tokenList.addToken(Token(TokenType::PROGRAM, "PROGRAM", index));
                    // Следующий токен будет BEGIN с идентификатором
                    Token nextToken = Token(TokenType::BEGIN, id, index + 1);
                    return nextToken;
                }
                else {
                    return Token(TokenType::ERROR, id, index); // Ошибка, если идентификатор невалидный
                }
            }

            // Проверка на конструкцию END PROGRAM
            if (word == "END") {
                std::string nextWord;
                if (inputFile >> nextWord && nextWord == "PROGRAM") {
                    std::string id;
                    if (inputFile >> id && isValidIdentifier(id)) {
                        tokenList.addToken(Token(TokenType::END, "END", index));
                        tokenList.addToken(Token(TokenType::PROGRAM, "PROGRAM", index + 1));
                        Token thirdToken = Token(TokenType::END, id, index + 2);
                        return thirdToken;
                    }
                    else {
                        return Token(TokenType::ERROR, id, index); // Ошибка, если идентификатор невалидный
                    }
                }
            }

            // Проверка на идентификатор
            if (isValidIdentifier(word)) {
                TokenType type = getKeywordOrIdentifier(word);
                return Token(type, word, index);
            }
            else {
                return Token(TokenType::ERROR, word, index);
            }
        }

        else if (isdigit(c)) { // Константа целого числа
            word.clear();
            word += c;
            while (inputFile.get(c) && isdigit(c)) {
                word += c;
            }
            inputFile.unget(); // Возвращаем последний символ в поток
            return Token(TokenType::INT_NUM, word, index);
        }

        else if (c == ':' && inputFile.peek() == ':') { // Оператор ::
            inputFile.get(c); // Пропускаем второй двоеточие
            return Token(TokenType::OPERATOR, "::", index);
        }

        else if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/') { // Операторы
            word = c;
            return Token(TokenType::OPERATOR, word, index);
        }

        else if (isDelimiter(c)) { // Разделители
            return Token(TokenType::DELIMITER, std::string(1, c), index);
        }

        else {
            // Если встретился ошибочный символ, начинаем собирать ошибочную лексему
            word.clear();
            word += c;
            while (inputFile.get(c) && !isspace(c) && !isDelimiter(c)) {
                word += c;
            }
            inputFile.unget(); // Вернуть последний корректный символ
            return Token(TokenType::ERROR, word, index);
        }
    }

    return Token(TokenType::UNKNOWN, "", index); // UNKNOWN токен в конце файла
}





void LexicalAnalyzer::analyze() {
    Token token;
    while ((token = getNextLexeme()).type != TokenType::UNKNOWN) {
        tokenList.addToken(token);
    }

    // Печатаем токены, с учетом того, что токены с ошибками будут в конце
    tokenList.printTokens(outputFile);
}
