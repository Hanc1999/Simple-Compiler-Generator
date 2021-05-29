//
//  tree2code.hpp
//  mini_C_compiler
//
//  Created by 施熠民 on 2021/5/1.
//

#ifndef tree2code_hpp
#define tree2code_hpp

#include "grammar2table.hpp"
#include "table2tree.hpp"

extern bool now_debug;

class SymbolTable{
public:
    // data
    int ST_temp_counter;
    int ST_scope;
    std::map<std::string, std::stack<int>> ST_vars;
    // methods
    int ST_check_and_set(std::string var_name, int current_scope);
    void operator=(SymbolTable* rhs);
    SymbolTable();
    SymbolTable(SymbolTable* rhs);
};

class IRGenerator{
public:
    // data
    ParseTreeNode* IRG_root_node;
    int IRG_label_counter;
    // methods
    IRGenerator(ParseTreeNode* root);
    std::string Translate(ParseTreeNode* next_node, SymbolTable* next_stable);
    void IRGstart();
    // different pid sets:
    std::vector<int> IRG_IF_cases_set;
    std::vector<int> IRG_WHILE_cases_set;
    std::vector<int> IRG_DOWHILE_cases_set;
    std::vector<int> IRG_EXP_cases_set;
    std::vector<int> IRG_EXP10_cases_set;
    std::vector<int> IRG_EXP11_cases_set;
    std::vector<int> IRG_EXPPIE_cases_set;
    std::vector<int> IRG_DECLARE_cases_set;
    std::vector<int> IRG_ASSIGN_cases_set;
    std::vector<int> IRG_TRIVIAL_cases_set;
    std::vector<int> IRG_NULL_cases_set;
    std::vector<int> IRG_BLOCK_cases_set;
    std::vector<int> IRG_FUNCTION_cases_set;
};
#endif /* tree2code_hpp */
