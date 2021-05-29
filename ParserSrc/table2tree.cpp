//
//  table2parser.cpp
//  mini_c_parser
//
//  Created by 施熠民 on 2021/4/11.
//

#include "table2tree.hpp"

extern int PTN_counter;

// class ParseTreeNode
// ParseTreeNode(), constructor of class::ParseTreeNode,
// to declare the necessary information like the node index, symbol index and whether the symbol is a termimal.
ParseTreeNode::ParseTreeNode(int PTN_index, symbol PTN_symbol, bool is_terminal){
    this->PTN_index = PTN_index;
    this->PTN_symbol = PTN_symbol;
    this->PTN_is_terminal = is_terminal;
}

// class ParseTree
// ParseTree(), constructor of class::ParseTree,
// give the parse tree(the main function of parser driver) the input file pointer, to read the token stream, and
// then build up a tree node as the root, then apply the function makeTree(0 to make the whole tree.
ParseTree::ParseTree(FILE* fp, Grammar* G){
    // feed data field
    this->G = G;
    this->fp = fp;
    this->mvfp = fp;
    this->still_looking_up = false;
    // make root node
    symbol root_symbol = 0;
    bool root_is_terminal = false;
    ParseTreeNode* root = new ParseTreeNode(PTN_counter++, root_symbol, root_is_terminal);
    this->root = root;
    makeTree(root, "");
}

// next_token(), method function of class::ParseTree,
// to fetch the look ahead token, if the last look ahead token has been already matched by some terminal node, t-
// hen fetch a new token from the input file character stream.
std::string ParseTree::next_token(){
    if (this->still_looking_up) {
        return this->the_next_token;
    }
    this->still_looking_up = true;
    std::vector<char> token_buffer;
    std::vector<char> content_buffer;
    char next_char = '\0';
    bool in_content = false;
    while (!feof(this->mvfp)) {
        next_char = fgetc(this->mvfp);
        if (next_char == ' ' || next_char == '\t' || next_char == '\n') {
            break; // break the loop and actually drop the useless next_char
        }
        else if(next_char == ':'){
            in_content = true;
            continue;
        }
        // if not blank
        if (!in_content) {
            token_buffer.push_back(next_char);
        }
        else{
            content_buffer.push_back(next_char);
        }
        
    }
    std::string res(token_buffer.begin(), token_buffer.end());
    std::string content_res(content_buffer.begin(), content_buffer.end());
    this->the_next_token.assign(res);
    this->the_next_token_content.assign(content_res);
    return res;
}

// match_token(), method function of class::ParseTree,
// to match the current look ahead token.
std::string ParseTree::match_token(){
    std::string res = next_token();
    this->still_looking_up = false;
    next_token();
    return res;
}

// makeTree(), method function of class::ParseTree,
// to make the whole tree in a recursive way. (1) identify its own symbol index; (2) if it's terminal node, then
// to match the look ahead token; (3) if it's not the terminal, then read the current look ahead token and then
// look up in the given look up table to identify which production rule should be used to build up the children-
// nodes list: that is to make the children nodes in the order of the production RHS symbols.
void ParseTree::makeTree(ParseTreeNode* ptn, std::string dummy){
    // fetch node information
    bool this_node_is_terminal = ptn->PTN_is_terminal;
    symbol this_node_symbol = ptn->PTN_symbol;
    // decide whether terminal
    if (this_node_is_terminal) {
        if (this_node_symbol != this->G->G_string2term[eps]->symbol_index) {
            ptn->PTN_content.assign(this->the_next_token_content);
            std::string matched_token = this->match_token();
        }
        // demo
        if (now_debug) {
            std::cout<<"[TRE]: "<<dummy<<"-> "<<this->G->G_symbol2term[this_node_symbol]->term_name<<" \t(terminal, content=\""<<ptn->PTN_content<<"\")"<<std::endl;
        }
        
        return;
    }
    // decide which production to use
    std::string next_token = this->next_token();
    symbol token_symbol = this->G->G_string2term[next_token]->symbol_index;
    std::pair<symbol, symbol> lookup_pair = {this_node_symbol, token_symbol};
    if (G->G_Table.count(lookup_pair) == 0) { // what if count >1?
        std::cout<<"[TRE]: error input"<<std::endl;
        std::cout<<"[TRE]: nonterminal: "<<this_node_symbol<<"'"<<this->G->G_symbol2nont[this_node_symbol]->nont_name<<"'"<<" terminal: "<<token_symbol<<"'"<<this->G->G_symbol2term[token_symbol]->term_name<<"'"<<std::endl;
        return;
    }
    else{
        std::set<production*>::iterator pit = this->G->G_Table[lookup_pair].begin();
        if ( this->G->G_Table[lookup_pair].size()>1 && (*pit)->prod_RHS.front() == this->G->G_string2term[eps]->symbol_index) {
//            std::cout<<"[TRE]: Trick Applied"<<std::endl;
            pit++;
        }
        production* p_to_apply = *pit;
        ptn->PTN_production_index = p_to_apply->prod_index;
        // demo
        if (now_debug) {
            std::cout<<"[TRE]: "<<dummy<<"-> "<<this->G->G_symbol2nont[this_node_symbol]->nont_name<<"\t(pid:"<<ptn->PTN_production_index<<")"<<std::endl;
        }
        for (symbol s: p_to_apply->prod_RHS) {
            bool new_node_is_terminal = !this->G->is_nonterminal(s);
            ParseTreeNode* new_node = new ParseTreeNode(PTN_counter++, s, new_node_is_terminal);
            ptn->PTN_children.push_back(new_node);
        }
        for (ParseTreeNode* next_ptn: ptn->PTN_children) {
            this->makeTree(next_ptn,dummy+"-");
        }
    }
}
