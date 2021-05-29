//
//  grammar2table.hpp
//  mini_c_parser
//
//  Created by 施熠民 on 2021/4/8.
//
#ifndef grammar2table_hpp
#define grammar2table_hpp

// include libs
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <stack>

// define macro
#define eps "@"
#define EndOfFile "$"

// define the data type of the symbol as [C_int]
typedef int symbol;

extern int symbol_counter;
extern int production_counter;
extern bool now_debug;

// production, class,
// to store the data of a production rule in the form of lhs->rhs1, rhs2..., including the lhs(symbol)
// and rhs(list of symbol). What's more, having the index for debug.
class production{
public:
    // data field
    int prod_index;
    std::string prod_string; // not necessary
    symbol prod_LHS;
    std::vector<symbol> prod_RHS;
    // methods
    // production(), constructor of class::production,
    // to construct a new production object by declaring its production index, its lhs symbol(which nonterminal
    // owns it) and its rhs(symbol list)
    production(int prod_index, symbol lhs, std::vector<symbol>rhs);
};

// nonterminal, class,
// to store the nonterminal symbol's informations, including the symbol index(int), the nonterminal's n-
// ame(string), the productions where it's the lhs, and also its follow set and its first set according
// to this (CFG) Grammar.
class nonterminal{
public:
    // data field
    symbol symbol_index;
    std::string nont_name;
    std::vector<production*> nont_prodections;
    std::set<symbol> nont_First;
    std::set<symbol> nont_Follow;
    // methods
    // nonterminal(), constructor of class::nonterminal,
    // to construct a new production object by declaring its symbol_index and its name(string).
    nonterminal(symbol symbol_index, std::string name);
    // nont_add_production(), method function of class::nonterminal,
    // used to add new production whose lhs must be this nonterminal symbol.
    void nont_add_production(production* new_production);
};

// terminal, class,
// to store the information of a terminal(token), including the symbol index and its name in string.
class terminal{
public:
    // data field
    symbol symbol_index;
    std::string term_name;
    // methods
    // terminal(), constructor of class::terminal,
    // to construct a new terminal object by declaring its symbol_index and its name(string).
    terminal(symbol symbol_index, std::string name);
};

// Grammar, class,
// to store the information of the Grammar and provides the functions to (1) converting the Grammar str-
// ing list to LL1 Grammar(by eliminating the left recursion and the left common factors), (2) building
// up the look ahead table for each nonterminal-terminal pair.
class Grammar{
public:
    // data field
    std::vector<nonterminal*> G_nonterminals;
    std::vector<terminal*> G_terminals;
    // look-ups for nonterminals
    std::map<symbol, nonterminal*> G_symbol2nont;
    std::map<std::string, nonterminal*> G_string2nont;
    // look-ups for terminals
    std::map<symbol, terminal*> G_symbol2term;
    std::map<std::string, terminal*> G_string2term;
    // the table
    std::map<std::pair<symbol, symbol>, std::set<production*>> G_Table;
    // check
    bool is_nonterminal(symbol symbol_index);
    bool is_nonterminal(std::string symbol_name);
    // methods
    // G_eliminate_left_rec(), method function of class::Grammar,
    // to eliminate both direct and non-direct left recursions of each production rule stored in the Grammar ob-
    // jecti, described in detail in Gragon Book Algorithm 4.19. Main steps: (1)replace all the production rule
    // in the form of "Ai: Aj, lembda" where j<i and Aj has productions like "Aj:alpha|beta" into "Ai: alpha, l-
    // embda| beta, lembda"; (2) apply the elimination of the direct left recursion: trasfering "A:A, alpha|bet-
    // a" into "Ai:beta, A'" and "A':alpha, A'", where the A' is a new nonterminal object.
    void G_eliminate_left_rec();
    // G_eliminate_common_factor(), method function of class::Grammar,
    // to eliminate all the common factors of all the notnerminal's productions, steps:(1) for a given nontermin-
    // al to find a current longest common factor, (2) if there is not such a cf, go to next nonterminal, (3) if
    // yes, then apply the algorithm to remove it by establishing new nonterminal obejcts.
    void G_eliminate_common_factor();
    // G_makeFirsts(), method function of class::Grammar,
    // to make all the first sets for each nonterminal.
    void G_makeFirsts();
    // G_makeFollows(), method function of class::Grammar,
    // to find the follow sets for all the nonterminals by the following phases: (0) put $ into the starting non-
    // terminal; (2) for any "A:a B b", put all symbols in First(b) into Follow(B), except the eps; (3) for any
    // form "A:a B b", if First(b) contains eps, put all symbols in FOLLOW(A) into FOLLOW(B).
    void G_makeFollows();
    // p debuger
    // G_debug(), method function of class::Grammar,
    // debug tool, to show all the productions in current Grammar obejct.
    void G_debug();
    // build the table
    // G_build_table(), method function of class::Grammar,
    // to build up the look ahead table of this (already LL(1)) Grammar. Following phases: (1) for production in
    // the form of "A:a", put it in all map[A,x] where x belongs to First(a); (2) if eps in First(a), put all F-
    // ollow(a) element y into table with map[A,y] = this production.
    void G_build_table();
    // Grammar constructor
    // Grammar(), constructor of class::Grammar,
    // which will read the input raw Grammar string list, then generate the nontermianl, terminal and producti-
    // on by each production string in the form of "A:B b C|D E f|A b" => { "A:B b D", "A:D E f", "A:A b"}.
    Grammar(std::vector<std::string>* raw_grammar);
};
//nishixiaozhuren
#endif /* grammar2table_hpp */
