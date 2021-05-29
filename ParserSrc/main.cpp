//
//  main.cpp
//  mini_c_parser
//
//  Created by 施熠民 on 2021/4/8.
//

// include heads
#include "grammar2table.hpp"
#include "table2tree.hpp"
#include "tree2code.hpp"

// global variables
symbol symbol_counter = 0;
int production_counter = 0;
int PTN_counter = 0;
bool now_debug = false;

// main function
int main(int argc, const char * argv[]) {
    // the source CFG Grammar of MINI-C language
    std::vector<std::string> raw_grammar_MINIC_with_precedence = {
        "program:var_declarations statements",
        "var_declarations:var_declarations var_declaration|@",
        "var_declaration:INT declaration_list SEMI",
        "declaration_list:declaration_list COMMA declaration|declaration",
        "declaration:IDENTIFIER ASSIGN INT_NUM|IDENTIFIER LSQUARE INT_NUM RSQUARE|IDENTIFIER",
        "code_block:statement|LBRACE statements RBRACE",
        "statements:statements statement|statement",
        "statement:assign_statement SEMI|control_statement|read_write_statement SEMI|SEMI",
        "control_statement:if_statement|while_statement|do_while_statement SEMI|return_statement SEMI",
        "read_write_statement:read_statement|write_statement",
        "assign_statement:IDENTIFIER LSQUARE exp RSQUARE ASSIGN exp|IDENTIFIER ASSIGN exp",
        "if_statement:if_stmt|if_stmt ELSE code_block",
        "if_stmt:IF LPAR exp RPAR code_block",
        "while_statement:WHILE LPAR exp RPAR code_block",
        "do_while_statement:DO code_block WHILE LPAR exp RPAR",
        "return_statement:RETURN",
        "read_statement:READ LPAR IDENTIFIER RPAR",
        "write_statement:WRITE LPAR exp RPAR",
        "exp:exp2|exp OROR exp2",
        "exp2:exp3|exp2 ANDAND exp3",
        "exp3:exp4|exp3 OR_OP exp4",
        "exp4:exp5|exp4 AND_OP exp5",
        "exp5:exp6|exp5 EQ exp6|exp5 NOTEQ exp6",
        "exp6:exp7|exp6 LT exp7|exp6 GT exp7|exp6 LTEQ exp7|exp6 GTEQ exp7",
        "exp7:exp8|exp7 SHL_OP exp8|exp7 SHR_OP exp8",
        "exp8:exp9|exp8 PLUS exp9|exp8 MINUS exp9",
        "exp9:exp10|exp9 DIV_OP exp10|exp9 MUL_OP exp10",
        "exp10:exp11|NOT_OP exp11|MINUS exp11",
        "exp11:INT_NUM|IDENTIFIER|LPAR exp RPAR|IDENTIFIER LSQUARE exp RSQUARE"
    };
    
    // construct the LL(1) Grammar according to the raw CFG
    Grammar* GG = new Grammar(&raw_grammar_MINIC_with_precedence);
    // eliminate all the left recursion in the original Grammar
    GG->G_eliminate_left_rec();
    // eliminate all the left common factors
    GG->G_eliminate_common_factor();
    // make the first set of each nonterminals
    GG->G_makeFirsts();
    // make all the follow sets for each nonterminal
    GG->G_makeFollows();
    // build up the look ahead table according to the follow set and the first set
    GG->G_build_table();
    
    // declare file pointer to the output file of the parser
    FILE* fp;
    // read the file according to the user input argumentation
    if((fp = fopen(argv[1],"r")) == NULL){
        std::cout<<"File not exist"<<std::endl;
        exit(0);
    }

    // build up the AST tree when constructing the parser object
    ParseTree* parser = new ParseTree(fp, GG);
    // close the file, end
    fclose(fp);
    
    // generate IR code with stdio
    IRGenerator code_generator = IRGenerator(parser->root);
    code_generator.IRGstart();
    
    return 0;
}
