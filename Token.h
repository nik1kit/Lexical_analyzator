#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <fstream>

enum class TokenType {
    KEYWORD,       // ����������� �������� �����, ����� ��� PROGRAM, BEGIN
    ID_NAME,       // ��������������
    INT_NUM,       // ����� �����
    FLOAT_NUM,     // ����� � ��������� ������
    OPERATOR,      // ���������, ����� ��� +, -, *, /
    PUNCTUATOR,    // ����� ����������, ����� ��� ;, ,, (, )
    ERROR,         // ������
    DELIMITER,     // �����������
    UNKNOWN,       // ����������� ������

};

class Token {
public:
    TokenType type;
    std::string lexeme;
    int index;
    Token() : type(TokenType::UNKNOWN), lexeme(""), index(-1) {} // ����������� �� ���������

    Token(TokenType t, const std::string& lex, int idx) : type(t), lexeme(lex), index(idx) {}
};


#endif // TOKEN_H
