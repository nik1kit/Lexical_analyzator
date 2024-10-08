#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <fstream>

enum class TokenType {
    PROGRAM,        // Program -> Begin Descriptions Operators End
    BEGIN,          // Begin -> PROGRAM Id
    END,            // End -> END Id
    DESCRIPTIONS,   // Descriptions -> Descr | Descr Descriptions
    DESCR,          // Descr -> Type VarList
    TYPE,           // Type -> INTEGER
    VARLIST,        // VarList -> Id | Id , VarList
    OPERATORS,      // Operators -> Op | Op Operators
    OP,             // Op -> Id = Expr | FOR Id = Expr TO Expr DO Operators
    EXPR,           // Expr -> SimpleExpr | SimpleExpr + Expr | SimpleExpr - Expr
    SIMPLEEXPR,     // SimpleExpr -> Id | Const | ( Expr )
    ID_NAME,        // Id -> id_name
    INT_NUM,        // Const -> int_num
    OPERATOR,       // Operators and assignment: +, -, *, =, etc.
    DELIMITER,      // Delimiters like parentheses, semicolons, etc.
    ERROR,          // Errors in lexical analysis
    UNKNOWN         // Unknown token
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    int index;
    Token() : type(TokenType::UNKNOWN), lexeme(""), index(-1) {} 

    Token(TokenType t, const std::string& lex, int idx) : type(t), lexeme(lex), index(idx) {}
};


#endif 
