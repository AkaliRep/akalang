#include <iostream>
#include <cstring>
#include "parser.hpp"

Parser::Parser(Lexer* lexer) : tokens(lexer->get_tokens()), lexer(lexer) {}
std::vector<Statement*> Parser::parse_code() {
	std::vector<Statement*> fnc_vector;
	while (!lexer->is_parsed()) {
		lexer->next_token();
		fnc_vector.push_back(parse_function());
	}

	return fnc_vector;
}

Statement* Parser::parse_function() {
	Statement* stmt = new Statement(); 
	stmt->fnc = new Func_Def();
	stmt->type = STMT_TYPE_FUNCTION_DECLARATION;

	Token token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after fnc keyword");
	stmt->fnc->name = token.get_value();

	lexer->expect_next_token(Token::Type::OPEN_PAREN, "Parsing error: expected open paren after function declaration");

	token = lexer->next_token();
	if (token.get_type() != Token::Type::CLOSE_PAREN && token.get_type() != Token::Type::NAME){
		Utils::error("Parsing error: expected name or close parent on function arguments");
	}

	if (token.get_type() == Token::Type::NAME) {
		stmt->fnc->arguments = parse_fnc_arguments(token);
	} 

	lexer->expect_next_token(Token::Type::GREATER_THAN, "Parsing error: expected type after function arguments");
	token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: untyped functions are not allowed");
	stmt->fnc->return_type = get_type_from_string(token.get_value());

	lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected block after function declaration");
	stmt->fnc->body = parse_block();

	return stmt;
}

std::vector<Func_Arg*> Parser::parse_fnc_arguments(Token name_token) {
	std::vector<Func_Arg*> function_arguments;
	while (true) {
		Func_Arg* argument = new Func_Arg();
		argument->name = name_token.get_value();
		lexer->expect_next_token(Token::Type::COLON, "Parsing error: untyped function params are not allowed");
		Token token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: untyped function params are not allowed");
		argument->type = get_type_from_string(token.get_value());
		function_arguments.push_back(argument);

		name_token = lexer->next_token();
		if (name_token.get_type() == Token::Type::CLOSE_PAREN) {
			break;
		}
		if (name_token.get_type() != Token::Type::COMMA) {
			Utils::error("Parsing error: expected COMMA as argument separator on token: " + name_token.get_value());
		}
		name_token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after COMMA separator");
	}

	return function_arguments;
}

VarType Parser::get_type_from_string(std::string val) {
	static_assert(VAR_TYPE_COUNTER == 4, "Unhandled VAR_TYPE_COUNTER on get_type_from_string");
	if (val == "int") {
		return VAR_TYPE_INT;
	} else if (val == "bool") {
		return VAR_TYPE_BOOL;
	} else if (val == "long") {
		return VAR_TYPE_LONG;
	} else if (val == "str") {
		return VAR_TYPE_STR;
	} else {
		Utils::error("Parsing error: unknown type (types allowed: int, bool, long, str)");
	}
}

std::vector<Statement*> Parser::parse_block() {
	static_assert(STMT_TYPE_COUNTER == 7, "Unhandled STMT_TYPE_COUNTER on parse_block() at parser.cpp");
	bool unfinished_block = true;
	std::vector<Statement*> block;

	while (unfinished_block) {
		Token token = lexer->next_token();
		switch (token.get_type()) {
			case Token::Type::NAME: 
				block.push_back(parse_name()); // This can be FUNC_CALL or VAR_REASIGNATION 
				break;
			case Token::Type::RETURN:
				block.push_back(parse_return());
				break;
			case Token::Type::VAR:
				block.push_back(parse_var());
				break;
			case Token::Type::IF:
				block.push_back(parse_if());
				continue; 
			case Token::Type::WHILE:
				block.push_back(parse_while());
				continue;
			case Token::Type::CLOSE_CURLY: 
				unfinished_block = false;
				break;
			default: Utils::error("Parsing error: couldn't parse expression " + token.get_value());
		}

		if (unfinished_block) {
			token = lexer->explore_next_token();
			if (token.get_type() != Token::Type::CLOSE_CURLY 
				&& token.get_type() != Token::Type::SEMICOLON) {
				Utils::error("Parsing error: expected semicolon at the end of statement, but got " + token.get_value());
			}
			if (token.get_type() == Token::Type::SEMICOLON) {
				lexer->next_token();
			}
		}
	}

	return block;
}

Statement* Parser::parse_while() {
	Statement* stmt = new Statement();
	stmt->type = STMT_TYPE_WHILE;
	stmt->whilee = new While();
	stmt->whilee->condition = parse_expr(lexer->next_token());
	lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected open curly after if condition, but got " + lexer->explore_last_token().get_value());
	stmt->whilee->block = parse_block();
	return stmt;
}

Statement* Parser::parse_if() {
	Statement* stmt = new Statement();
	stmt->type = STMT_TYPE_IF;
	stmt->iif = new If();
	stmt->iif->condition = parse_expr(lexer->next_token());
	lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected open curly after if condition, but got " + lexer->explore_last_token().get_value());
	stmt->iif->then = parse_block();
	Token token = lexer->explore_next_token();
	if (token.get_type() == Token::Type::ELSE) {
		lexer->next_token();
		lexer->expect_next_token(Token::Type::OPEN_CURLY, "Parsing error: expected open curly after else statement");
		stmt->iif->elsse = parse_block();
	}

	return stmt;
}

Statement* Parser::parse_return() {
	Statement* ret = new Statement();
	ret->type = STMT_TYPE_RETURN;
	ret->expr = parse_expr(lexer->next_token());
	return ret;
}

Statement* Parser::parse_var() {
	Statement* var = new Statement();
	var->var = new Var_Asign();
	var->type = STMT_TYPE_VAR_DECLARATION;
	Token token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: expected name after var keyword");
	var->var->name = token.get_value();
	lexer->expect_next_token(Token::Type::COLON, "Parsing error: untyped variables are not allowed");
	token = lexer->expect_next_token(Token::Type::NAME, "Parsing error: untyped variables are not allowed");
	var->var->type = get_type_from_string(token.get_value());
	lexer->expect_next_token(Token::Type::EQUALS, "Parsing error: expected expresion after variable declaration");
	var->var->value = parse_expr(lexer->next_token());
	return var;
}

Statement* Parser::parse_name() {
	Token token = lexer->explore_last_token();
	Token ntoken = lexer->next_token();
	switch (ntoken.get_type()) {
		case Token::Type::EQUALS: return parse_var_reasignation(token);
		case Token::Type::OPEN_PAREN: {
			Statement* stmt = new Statement();
			stmt->expr = new Expr();
			stmt->type = STMT_TYPE_EXPR;
			stmt->expr->type = EXPR_TYPE_FUNC_CALL;
			stmt->expr->func_call = parse_func_call(token);
			return stmt;
		}
		default: Utils::error("Parsing error: couldn't parse expression");
	}
}

Statement* Parser::parse_var_reasignation(Token name) {
	Statement* stmt = new Statement();
	stmt->var = new Var_Asign();
	stmt->type = STMT_TYPE_VAR_REASIGNATION;
	stmt->var->name = name.get_value();
	stmt->var->value = parse_expr(lexer->next_token());
	return stmt;
}

Func_Call* Parser::parse_func_call(Token name) {
	Func_Call* func_call = new Func_Call();
	func_call->name = name.get_value();

	Token token = lexer->next_token();
	if (token.get_type() != Token::Type::CLOSE_PAREN 
		&& token.get_type() == Token::Type::NAME
		|| token.get_type() == Token::Type::LITERAL_NUMBER
		|| token.get_type() == Token::Type::LITERAL_STRING) {
		func_call->expr = parse_func_call_args(token);
	}

	return func_call;
}

std::vector<Expr*> Parser::parse_func_call_args(Token token) {
	std::vector<Expr*> func_call_args;
	func_call_args.push_back(parse_expr(token));
	while (true) {
		token = lexer->next_token();

		if (token.get_type() == Token::Type::CLOSE_PAREN) {
			break;
		}
		if (token.get_type() != Token::Type::COMMA) {
			Utils::error("Expected COMMA function as argument separator, got " + token.get_value());
		}
		token = lexer->next_token();
		func_call_args.push_back(parse_expr(token));
	}

	return func_call_args;
}

Expr* Parser::parse_expr(Token token) {
	return parse_expr_with_precedence(token, OP_PREC_0);
}

Expr* Parser::parse_primary_expr(Token token) {
	static_assert(EXPR_TYPE_COUNTER == 6, "Unhandled EXPR_TYPE_FUNC_COUNT on parse_expr() on file parser.cpp");
	Expr* expr = new Expr();
	switch (token.get_type()) {
		case Token::Type::NAME:
			if (token.get_value() == "true") {
				expr->type = EXPR_TYPE_LITERAL_BOOL;
				expr->boolean = true;
				return expr;
			} else if (token.get_value() == "false") {
				expr->type = EXPR_TYPE_LITERAL_BOOL;
				expr->boolean = false;
				return expr;
			} else {
				Token ntoken = lexer->explore_next_token();
				if (ntoken.get_type() == Token::Type::OPEN_PAREN) {
					lexer->next_token(); // skip OPEN_PAREN
					expr->type = EXPR_TYPE_FUNC_CALL;
					expr->func_call = parse_func_call(token);
					return expr;
				} else {
					expr->type = EXPR_TYPE_VAR_READ;
					expr->var_read = token.get_value();
					return expr;
				}
			}
		case Token::Type::LITERAL_NUMBER: {
			expr->type = EXPR_TYPE_LITERAL_NUMBER;
			expr->number = std::atoi(token.get_value().c_str());
			return expr;
		}

		case Token::Type::LITERAL_STRING: {
			expr->type = EXPR_TYPE_LITERAL_STRING;
			expr->string = token.get_value(); // TODO add string parse
			return expr;
		}
	}

	Utils::error("Unexpected parsing expression: " + token.get_value());
}

Expr* Parser::parse_expr_with_precedence(Token token, OpPrec prec) {
	if (prec >= OP_PREC_COUNT) {
		return parse_primary_expr(token);
	}

	Expr* expr = new Expr();
	Expr* lhs = parse_expr_with_precedence(token, (OpPrec) (prec + 1));

	token = lexer->explore_next_token();
	if (is_op(token) && get_prec_by_op_type(get_op_type_by_token_type(token)) == prec) {
		lexer->next_token();
		expr->type = EXPR_TYPE_OP;
		Op* op = new Op();
		op->type = get_op_type_by_token_type(token);
		op->lhs = lhs;
		op->rhs = parse_expr_with_precedence(lexer->next_token(), prec);

		expr->op = op;
	} else {
		expr = lhs;
	}

	return expr;
}

OpType Parser::get_op_type_by_token_type(Token token) {
	static_assert(OP_TYPE_COUNT == 8, "Unhandled OP_TYPE_COUNT on get_op_type_by_token_type at parser.cpp");
	switch (token.get_type()) {
		case Token::Type::ADD: return OP_TYPE_ADD;
		case Token::Type::SUB: return OP_TYPE_SUB;
		case Token::Type::MUL: return OP_TYPE_MUL;
		case Token::Type::DIV: return OP_TYPE_DIV;
		case Token::Type::MOD: return OP_TYPE_MOD;
		case Token::Type::LOWER_THAN: return OP_TYPE_LT;
		case Token::Type::GREATER_THAN: return OP_TYPE_GT;
		case Token::Type::EQUALS_COMPARE: return OP_TYPE_EQ;
		default: Utils::error("Unknown token type: " + token.get_value());
	}
}

OpPrec Parser::get_prec_by_op_type(OpType op_type) {
	static_assert(OP_TYPE_COUNT == 8, "Unhandled OP_TYPE_COUNT on get_prec_by_op_type at parser.cpp");
	switch (op_type) {
		case OP_TYPE_LT:
		case OP_TYPE_GT:
		case OP_TYPE_EQ:
			return OP_PREC_0;
		case OP_TYPE_ADD: 
		case OP_TYPE_SUB:
			return OP_PREC_1;
		case OP_TYPE_MUL: 
		case OP_TYPE_DIV: 
		case OP_TYPE_MOD:
			return OP_PREC_2;
		default: Utils::error("Unknown operation type");
	}
}

bool Parser::is_op(Token token) {
	static_assert(OP_TYPE_COUNT == 8, "Unhandled OP_TYPE_COUNT on get_op_type_by_token_type at parser.cpp");
	switch (token.get_type()) {
		case Token::Type::LOWER_THAN:
		case Token::Type::GREATER_THAN:
		case Token::Type::EQUALS_COMPARE:
		case Token::Type::ADD:
		case Token::Type::SUB:
		case Token::Type::MUL:
		case Token::Type::DIV:
		case Token::Type::MOD:
			return true;

		default: return false;
	}
}
