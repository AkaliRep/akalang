#pragma once
#include <string>

class Token {
public:
	enum Type {
		TOKEN_TYPE_OPEN_PAREN,
		TOKEN_TYPE_CLOSE_PAREN,
		TOKEN_TYPE_OPEN_BRACKET,
		TOKEN_TYPE_CLOSE_BRACKET,
		TOKEN_TYPE_OPEN_CURLY,
		TOKEN_TYPE_CLOSE_CURLY,
		TOKEN_TYPE_GREATER_THAN,
		TOKEN_TYPE_LOWER_THAN,
		TOKEN_TYPE_SEMICOLON,
		TOKEN_TYPE_COLON,
		TOKEN_TYPE_COMMA,
		TOKEN_TYPE_EQUALS,
		TOKEN_TYPE_FUNCTION,
		TOKEN_TYPE_RETURN,
		TOKEN_TYPE_VAR,
		TOKEN_TYPE_FOR,
		TOKEN_TYPE_IF,
		TOKEN_TYPE_ELSE,
		TOKEN_TYPE_WHILE,
		TOKEN_TYPE_NAME,
		TOKEN_TYPE_LITERAL_NUMBER,
		TOKEN_TYPE_LITERAL_STRING,
		TOKEN_TYPE_ADD,
		TOKEN_TYPE_SUB,
		TOKEN_TYPE_MUL,
		TOKEN_TYPE_DIV,
		TOKEN_TYPE_INCLUDE_DIRECTIVE,
		TOKEN_COUNTER,
	};
	Token(Type type, char c);
	Token(Type type, std::string c);

private:
	Type type;
	std::string value;

public:
	void set_type(Type type);
	Type get_type();
	void set_value(std::string value);
	std::string get_value();
};
