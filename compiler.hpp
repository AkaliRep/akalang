#pragma once
#include <vector>
#include <map>
#include "parser.hpp"
#include "lexer.hpp"

// Registers order for function parameters
const std::vector<std::string> x64regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
const std::vector<std::string> x32regs = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
const std::vector<std::string> x16regs = {"di", "si", "dx", "cx", "r8w", "r9w"};
const std::vector<std::string> x8regs = {"dil", "sil", "dh", "ch", "r8b", "r9b"};
const std::string BUILTIN_PATH = "./builtin/";

class Compiler {
private:
	std::vector<Statement> instructions;
	// Global function register
	std::map<std::string, std::vector<VarType>> global_function_register; 

	// Hardcoded strings
	std::vector<std::string> string_data_segment;

	void inc_rbp_offset(int& rbp_offset, VarType data_type);
	std::string get_reg_by_data_type_and_counter(int& counter, VarType data_type);
	std::string get_data_size_by_data_type(VarType data_type);
	std::string get_return_reg_by_data_type(VarType data_type);

public:
	Compiler(std::vector<Statement> instructions);
	std::string compile_statement(Statement stmt);
	std::string compile_function(Statement function);
	std::string compile_return(Statement stmt);
	std::string compile_expr(Expr expr);
	std::string compile_func_call(Expr expr);
	std::string compile_boolean(Expr expr);
	std::string compile_number(Expr expr);
	std::string compile_string(Expr expr);
	std::string compile_program();
	std::string build_data_segment();
	std::string compile_builtin();
};