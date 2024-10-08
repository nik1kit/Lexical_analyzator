#include "LexicalAnalyzer.h"
#include "Token.h"

LexicalAnalyzer::LexicalAnalyzer(const std::string& inputFileName, const std::string& outputFileName) {
    inputFile.open(inputFileName);
    outputFile.open(outputFileName);
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
    if (lexeme == "program") return TokenType::PROGRAM;
    if (lexeme == "begin") return TokenType::BEGIN;
    if (lexeme == "end") return TokenType::END;
    if (lexeme == "integer") return TokenType::TYPE;
    if (lexeme == "for") return TokenType::OP;
    if (lexeme == "to") return TokenType::EXPR;
    if (lexeme == "do") return TokenType::OPERATORS;
    return TokenType::ID_NAME;
}

TokenType LexicalAnalyzer::getNumber(const std::string& lexeme) {
    return TokenType::INT_NUM;
}

bool LexicalAnalyzer::isOperator(const std::string& str) const {
    return (str == "+" || str == "-" || str == "*" || str == "/" ||
        str == ":" || str == "=" || str == "<" || str == ">" ||
        str == "..");
}

bool LexicalAnalyzer::isDelimiter(char c) const {
    return (c == '(' || c == ')' || c == '[' || c == ']' || c == ',' || c == ';' ||  c == '\'' || c == '\"' || c == '.');
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
    int index = 1; // Переменная для отслеживания индекса

    while (inputFile.get(c)) {
        if (isspace(c)) {
            index++; // Увеличиваем индекс для пробелов
            continue; // Игнорируем пробелы
        }

        // Проверка на двухсимвольный оператор
        if (c == '.') {
            char nextChar;
            if (inputFile.get(nextChar) && nextChar == '.') {
                // Обнаружен оператор '..'
                return Token(TokenType::OPERATOR, "..", index);
            }
            else {
                inputFile.unget(); // Если не '..', вернуть символ обратно
            }
        }

        // Проверка на одиночные операторы
        if (isOperator(std::string(1, c))) { // Оператор
            return Token(TokenType::OPERATOR, std::string(1, c), index);
        }

        // Добавляем проверку на комментарий
        if (c == '/') {
            char nextChar;
            if (inputFile.get(nextChar) && nextChar == '/') { // Нашли начало комментария "//"
                while (inputFile.get(c) && c != '\n') {
                    word += c; // Читаем комментарий до конца строки
                }
                return Token(TokenType::DESCRIPTIONS, word, index);
            }
            else {
                inputFile.unget(); // Если не комментарий, возвращаем символ обратно
            }
        }

        if (isalpha(c) || c == '_') { // Идентификатор или ключевое слово
            word.clear();
            word += c;
            while (inputFile.get(c) && (isalnum(c) || c == '_')) {
                word += c;
            }
            inputFile.unget(); // Возвращаем последний символ в поток
            TokenType type = getKeywordOrIdentifier(word);
            return Token(type, word, index);
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
        else if (isDelimiter(c)) { // Разделитель
            return Token(TokenType::DELIMITER, std::string(1, c), index);
        }
        else {
            return Token(TokenType::ERROR, std::string(1, c), index);
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
