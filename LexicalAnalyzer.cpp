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

void LexicalAnalyzer::handleOperators(char c) {
    std::string op(1, c);
    if (c == ':') {
        inputFile.get(c);
        if (c == '=') {
            op += c; // ':='
        }
        else {
            inputFile.unget(); // Если не '=', возвращаем назад
        }
    }
    tokenList.addToken(Token(TokenType::OPERATOR, op, -1));
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
            TokenType type = getKeywordOrIdentifier(word);
            if (type == TokenType::TYPE) {
                std::string description = word; 

                // Пропускаем пробелы после ключевого слова 
                while (inputFile.get(c) && isspace(c)) {}
                // Проверяем на наличие оператора присвоения '::'
                if (c == ':') {
                    if (inputFile.get(c) && c == ':') {
                        std::string varList; // список переменных

                        while (inputFile.get(c)) {
                            if (c == '\n') {
                                break;
                            }
                            if (isspace(c)) continue; // Пропускаем пробелы

                            if (isalpha(c)) { 
                                if (!varList.empty()) {
                                    varList += ','; // Добавляем запятую перед новой переменной, если varList не пуст
                                }
                                varList += c; 

                                // Считываем дополнительные символы в идентификатор
                                while (inputFile.get(c) && (isalnum(c) || c == '_')) {
                                    varList += c; 
                                }
                                inputFile.unget(); // Возвращаем последний символ в поток
                            }
                            else if (c == ',') { 
                                
                            }
                            else if (c == ';' || c == '\n' || c == '}') { 
                                break; 
                            }
                            else {
                                break; 
                            }
                        }

                        // Проверяем, не пуст ли varList
                        if (!varList.empty()) {
                            description += " " + varList;
                            return Token(TokenType::DESCRIPTIONS, description, index);
                        }
                    }
                }


            }

            // Читаем следующий символ после идентификатора
            if (inputFile.get(c)) { // Проверка на наличие следующего символа
                std::string variable = word; // Сохраняем переменную
                char nextChar;

                // Проверяем следующий символ
                if (inputFile.get(nextChar)) {
                    // Пропускаем пробелы
                    if (isspace(nextChar)) {
                        while (inputFile.get(nextChar) && isspace(nextChar));
                    }

                    // Проверяем на оператор присвоения
                    if (nextChar == '=') {
                        std::string expression = variable + " ="; // Начало выражения
                        char c;
                        bool containsArithmetic = false; // Флаг для проверки на наличие + или -

                        // Собираем выражение до конца строки
                        while (inputFile.get(c) && c != '\n') {
                            if (c == '+' || c == '-') {
                                containsArithmetic = true; // Нашли арифметический оператор
                                expression += std::string(1, c);
                            }
                            else {
                                expression += c;
                            }
                        }
                        // Проверяем, содержит ли выражение арифметические операторы
                        if (containsArithmetic) {
                            std::string new_expression;
                            for (int i = 4; i < expression.size(); i++) {
                                new_expression += expression[i];
                            }
                            return Token(TokenType::EXPR, new_expression, index); 
                        }
                        else {
                            return Token(TokenType::OP, expression, index); // Простое присвоение, возвращаем как OP
                        }
                    }
                    // Если не оператор присвоения, возвращаем символ обратно
                    inputFile.unget();
                }

            }

            // Проверка на конструкцию end do
            if (word == "end") {
                std::string nextWord;
                if (inputFile >> nextWord && nextWord == "do") {
                    return Token(TokenType::END, "end do", index);
                }
                else {
                    inputFile.unget(); // Возвращаем слово о, если не do
                    return Token(TokenType::END, word, index);
                }
            }

            // Проверка на ключевые слова PROGRAM и END
            if (word == "PROGRAM") {
                std::string id;
                if (inputFile >> id && isValidIdentifier(id)) {
                    return Token(TokenType::PROGRAM_ID, id, index);
                }
                else {
                    return Token(TokenType::UNKNOWN, id, index); // Ошибка, если id невалидный
                }
            }

            if (word == "END") {
                std::string nextWord;
                if (inputFile >> nextWord && nextWord == "PROGRAM") {
                    std::string id;
                    if (inputFile >> id && isValidIdentifier(id)) {
                        return Token(TokenType::END_ID, id, index);
                    }
                    else {
                        return Token(TokenType::UNKNOWN, id, index); // Ошибка, если id невалидный
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
            return Token(getNumber(word), word, index);
        }
        else if (isDelimiter(c)) { // Разделитель
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
