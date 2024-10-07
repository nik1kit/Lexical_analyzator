#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <fstream>

enum class TokenType {
    KEYWORD,       // Программные ключевые слова, такие как PROGRAM, BEGIN
    ID_NAME,       // Идентификаторы
    INT_NUM,       // Целые числа
    FLOAT_NUM,     // Числа с плавающей точкой
    OPERATOR,      // Операторы, такие как +, -, *, /
    PUNCTUATOR,    // Знаки препинания, такие как ;, ,, (, )
    ERROR,         // Ошибки
    DELIMITER,     // Разделители
    UNKNOWN,       // Неизвестные токены

};

class Token {
public:
    TokenType type;
    std::string lexeme;
    int index;
    Token() : type(TokenType::UNKNOWN), lexeme(""), index(-1) {} // Конструктор по умолчанию

    Token(TokenType t, const std::string& lex, int idx) : type(t), lexeme(lex), index(idx) {}
};


#endif // TOKEN_H
