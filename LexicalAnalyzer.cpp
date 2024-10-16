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
    const char* keywords[] = { "INTEGER", "do", "to", "for"};
    TokenType keywordTokens[] = {
        TokenType::TYPE,
        TokenType::OP,
        TokenType::OP,
        TokenType::OP,
    };

    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
        const char* keyword = keywords[i];
        int len = strlen(keyword);

        // Проверка на длину: если длина не совпадает, это не ключевое слово
        if (lexeme.length() != len) {
            continue;
        }

        // Сравнение посимвольно
        bool match = true;
        for (int j = 0; j < len; ++j) {
            if (lexeme[j] != keyword[j]) {
                match = false;
                break;
            }
        }

        // Если ключевое слово найдено
        if (match) {
            return keywordTokens[i];
        }
    }

    // Если не найдено ключевое слово, это идентификатор
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
    bool lastWasOperator = false;

    while (inputFile.get(c)) {
        if (isspace(c)) {
            index++;
            continue;
        }

        if (isalpha(c)) { // Идентификатор или ключевое слово
            word.clear();
            if (c == 'P') {  // Возможное начало слова "PROGRAM"
                std::string programWord = "PROGRAM";
                bool isProgram = true;
                std::string potential_error;

                for (size_t i = 1; i < programWord.length(); i++) {
                    potential_error += c;
                    if (!inputFile.get(c) || c != programWord[i]) {
                        isProgram = false;
                        break;
                    }
                }

                if (isProgram) {  // Если слово "PROGRAM" было считано
                    inputFile.get(c);  // Считываем следующий символ после слова "PROGRAM"
                    if (isspace(c) || c == ':' || c == EOF) {  // Проверяем на пробел, разделитель или конец файла
                        std::string id;
                        if (inputFile >> id && isValidIdentifier(id)) {
                            tokenList.addToken(Token(TokenType::PROGRAM, "PROGRAM", index));
                            Token nextToken = Token(TokenType::BEGIN, id, index + 1);
                            return nextToken;
                        }
                        else {
                            return Token(TokenType::ERROR, id, index);  // Ошибка, если идентификатор невалидный
                        }
                    }
                    else {
                        std::string errorWord = "PROGRAM" + std::string(1, c);  // Добавляем первый некорректный символ

                        // Собираем оставшиеся некорректные символы до пробела или конца строки
                        while (inputFile.get(c) && !isspace(c) && c != EOF) {
                            errorWord += c;
                        }

                        return Token(TokenType::ERROR, errorWord, index);  // Возвращаем полное некорректное слово как ошибку
                    }

                }
                else {
                    potential_error += c;
                    // Считываем оставшиеся некорректные символы
                    while (inputFile.get(c) && !isspace(c) && c != EOF) {
                        potential_error += c;
                    }
                    if (isValidIdentifier(potential_error))
                        return Token(TokenType::ID_NAME, potential_error, index);
                    return Token(TokenType::ERROR, potential_error, index);  // Возвращаем полное некорректное слово как ошибку
                }
            }

            if (c == 'E') {  // Возможное начало конструкции "END PROGRAM"
                std::string endWord = "END";
                bool isEnd = true;
                std::string potential_error;
                for (size_t i = 1; i < endWord.length(); i++) {
                    potential_error += c;
                    if (!inputFile.get(c) || c != endWord[i]) {
                        isEnd = false;
                        break;
                    }
                }
                if (isEnd) {  // Если слово "END" было считано
                    inputFile.get(c);  // Считываем следующий символ
                    if (isspace(c) || c == ':' || c == EOF) {  // Проверяем на пробел, разделитель или конец файла
                        std::string programWord = "PROGRAM";
                        bool isProgram = true;
                        std::string potential_error;

                        for (size_t i = 0; i < programWord.length(); i++) {
                            potential_error += c;
                            if (!inputFile.get(c) || c != programWord[i]) {
                                isProgram = false;
                                break;
                            }
                        }

                        if (isProgram) {  // Если это действительно "END PROGRAM"
                            inputFile.get(c);  // Считываем следующий символ после "PROGRAM"
                            if (isspace(c) || c == ':' || c == EOF) {  // Проверяем на пробел, разделитель или конец файла
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
                            else {
                                tokenList.addToken(Token(TokenType::END, "END", index));
                                std::string errorWord = "PROGRAM" + std::string(1, c);  // Добавляем первый некорректный символ

                                // Собираем оставшиеся некорректные символы до пробела или конца строки
                                while (inputFile.get(c) && !isspace(c) && c != EOF) {
                                    errorWord += c;
                                }

                                return Token(TokenType::ERROR, errorWord, index);  // Возвращаем полное некорректное слово как ошибку
                            }
                        }
                        else {
                            tokenList.addToken(Token(TokenType::END, "END", index));
                            potential_error += c;
                            // Считываем оставшиеся некорректные символы
                            while (inputFile.get(c) && !isspace(c) && c != EOF) {
                                potential_error += c;
                            }
                            if (isValidIdentifier(potential_error))
                                return Token(TokenType::ID_NAME, potential_error, index);
                            return Token(TokenType::ERROR, potential_error, index);  // Возвращаем полное некорректное слово как ошибку
                        }
                    }
                    else {
                        std::string errorWord = "END" + std::string(1, c);  // Добавляем первый некорректный символ

                        // Собираем оставшиеся некорректные символы до пробела или конца строки
                        while (inputFile.get(c) && !isspace(c) && c != EOF) {
                            errorWord += c;
                        }

                        return Token(TokenType::ERROR, errorWord, index);  // Возвращаем полное некорректное слово как ошибку
                    }
                }
                else {
                    potential_error += c;
                    // Считываем оставшиеся некорректные символы
                    while (inputFile.get(c) && !isspace(c) && c != EOF) {
                        potential_error += c;
                    }
                    if (isValidIdentifier(potential_error))
                        return Token(TokenType::ID_NAME, potential_error, index);
                    return Token(TokenType::ERROR, potential_error, index);  // Возвращаем полное некорректное слово как ошибку
                }
            }

            word += c;
            while (inputFile.get(c) && (isalnum(c) || c == '_')) {
                word += c;
            }
            inputFile.unget(); // Возвращаем последний символ в поток



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



        else if (c == '=' || c == '+' || c == '-') {  // Операторы
            if (lastWasOperator) {
                return Token(TokenType::ERROR, "Consecutive operators", index);  // Если два оператора подряд, возвращаем ошибку
            }
            lastWasOperator = true;  // Устанавливаем флаг, что последний символ был оператором
            word = c;

            if (inputFile.peek() == c && (c == '+' || c == '-')) {  // Проверяем на ++ или --
                inputFile.get();  // Считываем второй оператор
                word += c;
                return Token(TokenType::ERROR, word, index);  // Возвращаем ошибку для ++ или --
            }

            return Token(TokenType::OPERATOR, word, index);  // Возвращаем оператор
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

    tokenList.printTokens(outputFile);
}
