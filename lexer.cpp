#include "lexer.hpp"

Token Lexer::get_next_token() {
	char c = file_content[index++];
	switch (c) {
		case '(': return Token(Token::Type::OPEN_PAREN, c);
		case ')': return Token(Token::Type::CLOSE_PAREN, c);
		case '[': return Token(Token::Type::OPEN_BRACKET, c);
		case ']': return Token(Token::Type::CLOSE_BRACKET, c);
		case '{': return Token(Token::Type::OPEN_CURLY, c);
		case '}': return Token(Token::Type::CLOSE_CURLY, c);
		case '>': return Token(Token::Type::GREATER_THAN, c);
		case '<': return Token(Token::Type::LOWER_THAN, c);
		case ';': return Token(Token::Type::SEMICOLON, c);
		case ':': return Token(Token::Type::COLON, c);
		case ',': return Token(Token::Type::COMMA, c);
		case '=': return form_equals(c);
		case '-': return Token(Token::Type::SUB, c);
		case '+': return Token(Token::Type::ADD, c);
		case '/': return Token(Token::Type::DIV, c);
		case '*': return Token(Token::Type::MUL, c);
		case '%': return Token(Token::Type::MOD, c);
		case '"': return form_string(c);
		case ' ':
		case '\n':
		case '\t': return get_next_token();
	}

	if (is_number(c)) {
		return form_number(c);
	}

	if (is_letter(c)) {
		return form_name(c);
	}

	Utils::error("Unexpected token: " + c);
}

bool Lexer::has_more_tokens() {
	return this->index < file_content.size();
}

void Lexer::tokenize() {
	while (has_more_tokens()) {
		tokens.push_back(get_next_token());
	}

	this->index = 0;
}

bool Lexer::is_number(char c) {
	return c >= '0' && c <= '9';
}

bool Lexer::is_letter(char c) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_';
}

bool Lexer::is_num(char c) {
	return c >= '0' && c <= '9';
}

Token Lexer::form_name(char c) {
	std::string value;
	while (is_letter(c)) {
		value += c;
		c = file_content[index++];
	}
	index--;

	if (value == "fnc") {
		return Token(Token::Type::FUNCTION, value);
	} else if (value == "return") {
		return Token(Token::Type::RETURN, value);
	} else if (value == "var") {
		return Token(Token::Type::VAR, value);
	} else if (value == "if") {
		return Token(Token::Type::IF, value);
	} else if (value == "for") {
		return Token(Token::Type::FOR, value);
	} else if (value == "include") {
		return Token(Token::Type::INCLUDE_DIRECTIVE, value);
	} else if (value == "else") {
		return Token(Token::Type::ELSE, value);
	} else if (value == "while") {
		return Token(Token::Type::WHILE, value);
	}

	return Token(Token::Type::NAME, value);
}

Token Lexer::form_number(char c) {
	std::string value;
	while (is_number(c)) {
		value += c;
		c = file_content[index++];
	}
	index--;

	return Token(Token::Type::LITERAL_NUMBER, value);
}

Token Lexer::form_equals(char c) {
	char nchar = file_content[index];
	if (nchar == '=') {
		index++;
		return Token(Token::Type::EQUALS_COMPARE, std::string(1, c) + nchar);
	}

	return Token(Token::Type::EQUALS, c);
}

Token Lexer::form_string(char c) {
	std::string value;

	c = file_content[index++]; // skip open quotes
	do {
		value += c;
		c = file_content[index++];
	} while (c != '"');

	return Token(Token::Type::LITERAL_STRING, value);
}

Token Lexer::expect_next_token(Token::Type token_type, std::string error_msg) {
	if (this->tokens[index].get_type() == token_type) {
		return this->tokens[index++];
	} else {
		Utils::error(error_msg);
	}
}

Token Lexer::next_token() {
	return this->tokens[index++];
}

Token Lexer::explore_next_token() {
	return this->tokens[index];
}

Token Lexer::explore_last_token() {
	return this->tokens[index - 1];
}

bool Lexer::is_parsed() {
	return this->tokens.size() <= index;
}

void Lexer::set_file_content(std::string file_content) { this->file_content = file_content; }
std::string Lexer::get_file_content() { return this->file_content; }
void Lexer::set_tokens(std::vector<Token> tokens) { this->tokens = tokens; }
std::vector<Token> Lexer::get_tokens() { return this->tokens; }

Lexer::Lexer() {}
Lexer::Lexer(std::string filepath) : file_content(Utils::read_file(filepath)) {
	this->index = 0;
	tokenize();
}

Lexer Lexer::from_content(std::string content) {
	Lexer lexer = Lexer();
	lexer.set_file_content(content);
	lexer.index = 0;
	lexer.tokenize();
	return lexer;
}

static_assert(Token::Type::TOKEN_COUNTER == 30, "Unhandled TOKEN_COUNTER on lexer.cpp");