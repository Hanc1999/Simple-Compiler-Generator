//
//  table2parser.hpp
//  mini_c_parser
//
//  Created by 施熠民 on 2021/4/11.
//

#ifndef table2parser_hpp
#define table2parser_hpp

#include <stdio.h>
#include "grammar2table.hpp"

extern bool now_debug;

// ParseTreeNode, class,
// to store the information of a parse tree node, including the node index, the symbol index, the children nodes.
class ParseTreeNode{
public:
    // data field
    symbol PTN_symbol;
    bool PTN_is_terminal;
    int PTN_index;
    int PTN_production_index;
    std::string PTN_content;
    std::vector<ParseTreeNode*> PTN_children;
    // methods
    // constructor
    // ParseTreeNode(), constructor of class::ParseTreeNode,
    // to declare the necessary information like the node index, symbol index and whether the symbol is a termimal.
    ParseTreeNode(int PTN_index, symbol PTN_symbol, bool is_terminal);
};

// ParseTree, class,
// to store the information of a parse tree, and provides the method to read the token from the input file strea-
// m, and the method to build the tree in a recursive way.
class ParseTree{
public:
    // data field
    ParseTreeNode* root;
    Grammar* G;
    FILE* fp;
    FILE* mvfp;
    // methods
    // makeTree(), method function of class::ParseTree,
    // to make the whole tree in a recursive way. (1) identify its own symbol index; (2) if it's terminal node, then
    // to match the look ahead token; (3) if it's not the terminal, then read the current look ahead token and then
    // look up in the given look up table to identify which production rule should be used to build up the children-
    // nodes list: that is to make the children nodes in the order of the production RHS symbols.
    void makeTree(ParseTreeNode* ptn, std::string dummy);
    void PT_debug();
    // next_token(), method function of class::ParseTree,
    // to fetch the look ahead token, if the last look ahead token has been already matched by some terminal node, t-
    // hen fetch a new token from the input file character stream.
    std::string next_token();
    // match_token(), method function of class::ParseTree,
    // to match the current look ahead token.
    std::string match_token();
    std::string the_next_token;
    std::string the_next_token_content;
    bool still_looking_up;
    // constructor
    // class ParseTree
    // ParseTree(), constructor of class::ParseTree,
    // give the parse tree(the main function of parser driver) the input file pointer, to read the token stream, and
    // then build up a tree node as the root, then apply the function makeTree(0 to make the whole tree.
    ParseTree(FILE* fp, Grammar* G);
};

#endif /* table2parser_hpp */
