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
    if (lexeme == "program" || lexeme == "var" || lexeme == "begin" || lexeme == "end" || lexeme == "for" ||
        lexeme == "if" || lexeme == "then" || lexeme == "do" || lexeme == "to" || lexeme == "write" ||
        lexeme == "integer" || lexeme == "array" || lexeme == "of" || lexeme == "randomize" || lexeme == "halt") {
        return TokenType::KEYWORD;
    }
    return TokenType::ID_NAME;
}

TokenType LexicalAnalyzer::getNumber(const std::string& lexeme) {
    return TokenType::INT_NUM;
}

bool LexicalAnalyzer::isOperator(char c) const {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == ':' || c == '=' || c == '<' || c == '>');
}

bool LexicalAnalyzer::isDelimiter(char c) const {
    return (c == '(' || c == ')' || c == '[' || c == ']' || c == ',' || c == ';' || c == '.' || c == '\'' || c == '\"');
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
            index++; // Увеличиваем индекс при пробеле
            continue; // Игнорируем пробелы
        }

        if (isalpha(c) || c == '_') { // Начало идентификатора или ключевого слова
            word.clear();
            word += c;
            while (inputFile.get(c) && (isalnum(c) || c == '_')) {
                word += c;
            }
            inputFile.unget(); // Возвращаем последний символ обратно в поток
            TokenType type = getKeywordOrIdentifier(word);
            return Token(type, word, index);
        }
        else if (isdigit(c)) { // Начало числа
            word.clear();
            word += c;
            while (inputFile.get(c) && isdigit(c)) {
                word += c;
            }
            inputFile.unget(); // Возвращаем последний символ обратно в поток
            return Token(TokenType::INT_NUM, word, index);
        }
        else if (isOperator(c)) { // Оператор
            handleOperators(c);
            return Token(TokenType::OPERATOR, std::string(1, c), index);
        }
        else if (isDelimiter(c)) { // Разделитель
            handleDelimiters(c);
            return Token(TokenType::DELIMITER, std::string(1, c), index);
        }
        else {
            return Token(TokenType::ERROR, std::string(1, c), index);
        }
    }
    return Token(TokenType::UNKNOWN, "", index); // Возвращаем UNKNOWN, если конец файла
}


void LexicalAnalyzer::analyze() {
    Token token;
    while ((token = getNextLexeme()).type != TokenType::UNKNOWN) {
        tokenList.addToken(token);
    }

    // Печатаем токены, с учетом того, что токены с ошибками будут в конце
    tokenList.printTokens(outputFile);
}
