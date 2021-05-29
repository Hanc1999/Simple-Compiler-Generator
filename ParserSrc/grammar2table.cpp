//
//  grammar2table.cpp
//  mini_c_parser
//
//  Created by 施熠民 on 2021/4/8.
//

#include "grammar2table.hpp"
extern symbol symbol_counter;
extern int production_counter;

// class production
// production(), constructor of class::production,
// to construct a new production object by declaring its production index, its lhs symbol(which nonterminal
// owns it) and its rhs(symbol list)
production::production(int prod_index, symbol lhs, std::vector<symbol>rhs){
    this->prod_index = prod_index;
    this->prod_LHS = lhs;
    this->prod_RHS = rhs;
}

// class nonterminal
// nonterminal(), constructor of class::nonterminal,
// to construct a new production object by declaring its symbol_index and its name(string).
nonterminal::nonterminal(int symbol_index, std::string name){
    this->symbol_index = symbol_index;
    this->nont_name = name;
}

// nont_add_production(), method function of class::nonterminal,
// used to add new production whose lhs must be this nonterminal symbol.
void nonterminal::nont_add_production(production *new_production){
    this->nont_prodections.push_back(new_production);
}

// class terminal
// terminal(), constructor of class::terminal,
// to construct a new terminal object by declaring its symbol_index and its name(string).
terminal::terminal(int symbol_index, std::string name){
    this->symbol_index = symbol_index;
    this->term_name = name;
}

// class Grammar
// identify_nonterminals(), helper function,
// to build up all the nonterminal objects when searching the Grammar in the first pass.
std::string identify_nonterminals(std::string* raw_product, Grammar* G){
    std::vector<char> buffer;
    for (char k: *raw_product) {
        if (k==':') {
            break;
        }
        buffer.push_back(k);
    }
    std::string name(buffer.begin(), buffer.end());
    nonterminal* new_nont = new nonterminal(symbol_counter, name);
    G->G_nonterminals.push_back(new_nont); // for ordering nonterminal list
    (G->G_symbol2nont)[symbol_counter] = new_nont; // for symbol->nonterminal look up
    (G->G_string2nont)[name] = new_nont; // for string->nonterminal look up
    symbol_counter++;
    return name;
}

// helpers
// check_and_add(), helper function,
// to check whether a string name belongs to a current existing termianl or nontermianl, if not, it will try
// to declare a new terminal object with this string name in the memory heap.
void check_and_add(Grammar* G, std::string name){
    bool exists = false;
    // whether existing terminal?
    for(terminal* t: G->G_terminals){
        if (t->term_name == name) {
            exists = true;
        }
    }
    // or whether existing nonterminal?
    for(nonterminal* nt: G->G_nonterminals){
        if(nt->nont_name == name){
            exists = true;
        }
    }
    // if neither we add a new terminal
    if (exists) {
        return;
    }
    terminal* new_terminal = new terminal(symbol_counter, name);
    G->G_symbol2term[symbol_counter] = new_terminal;
    G->G_string2term[name] = new_terminal;
    G->G_terminals.push_back(new_terminal);
    symbol_counter++;
}

// identify_productions(), helper function,
// to check each production in the raw Grammar string list, to split each production out and declare differ-
// ent (1)nontermianl object, (2)terminal object, (3)production object for this Grammar.
void identify_productions(std::string* raw_product, Grammar* G){
    std::vector<char> buffer;
    std::string lhs;
    std::vector<std::string> rhs;
    std::vector<std::vector<std::string>> productions;
    bool in_rhs = false;
    for (int i = 0; i < raw_product->length(); i++) {
        char k = (*raw_product)[i];
        bool isfinal = false;
        if (i == raw_product->length()-1) {
            isfinal = true;
        }
        
        if (!in_rhs && k != ':') {
            buffer.push_back(k);
        }
        else if (!in_rhs && k == ':') {
            in_rhs = true;
            lhs = std::string(buffer.begin(), buffer.end());
            buffer.clear();
        }
        else if (in_rhs && k == ' ' ) {
            std::string new_symbol = std::string(buffer.begin(), buffer.end());
            //
            check_and_add(G, new_symbol);
            rhs.push_back(new_symbol);
            buffer.clear();
        }
        else if (in_rhs && (k == '|' || isfinal) ) {
            if (isfinal) {
                buffer.push_back(k);
            }
            std::string new_symbol = std::string(buffer.begin(), buffer.end());
            //
            check_and_add(G, new_symbol);
            rhs.push_back(new_symbol);
            buffer.clear();
            productions.push_back(rhs);
            rhs.clear();
        }
        else if (in_rhs && k!= ' ') {
            buffer.push_back(k);
        }
        
    }
    check_and_add(G, eps);
    check_and_add(G, EndOfFile);
    // debug
    if (now_debug) {
        for(std::vector<std::string> rhs: productions){
            std::cout<<"[GRM]:"<<lhs<<"-->";
            for (std::string sym: rhs) {
                std::cout<<sym<<",";
            }
            std::cout<<std::endl;
        }
    }
    for(std::vector<std::string> rhs: productions){
        nonterminal* lhs_nonterminal = G->G_string2nont[lhs];
        symbol lhs_nont_symbol = lhs_nonterminal->symbol_index;
        std::vector<symbol> rhs_of_new_production;
        for (std::string sym: rhs) {
            if (G->is_nonterminal(sym)) {
                rhs_of_new_production.push_back(G->G_string2nont[sym]->symbol_index);
            }
            else{
                rhs_of_new_production.push_back(G->G_string2term[sym]->symbol_index);
            }
        }
        production* new_production = new production(production_counter++, lhs_nont_symbol, rhs_of_new_production);
        lhs_nonterminal->nont_add_production(new_production);
    }
}

// is_nonterminal(), method function of class::Grammar,
// to check whether a given symbol is a nonterminal symbol by searching all the existing nonterminals.
bool Grammar::is_nonterminal(symbol symbol_index){
    for(nonterminal* nt: this->G_nonterminals){
        if (nt->symbol_index == symbol_index) {
            return true;
        }
    }
    return false;
}
bool Grammar::is_nonterminal(std::string symbol_name){
    for(nonterminal* nt: this->G_nonterminals){
        if (nt->nont_name == symbol_name) {
            return true;
        }
    }
    return false;
}

// Grammar(), constructor of class::Grammar,
// which will read the input raw Grammar string list, then generate the nontermianl, terminal and producti-
// on by each production string in the form of "A:B b C|D E f|A b" => { "A:B b D", "A:D E f", "A:A b"}.
Grammar::Grammar(std::vector<std::string>* raw_grammar){
    // first pass(to find out all the nonterminals)
//    std::cout<<"[GRM]:---------------check nonterminals---------------"<<std::endl;
    for (std::string raw_product: *raw_grammar ) {
        std::string nont_name = identify_nonterminals(&raw_product, this);
//        std::cout<<"[GRM]: "<< nont_name<<" "<<this->G_string2nont[nont_name]->symbol_index <<std::endl;
    }
    
    // second pass(to generate all the production objects)
    for (std::string raw_product: *raw_grammar ) {
        identify_productions(&raw_product, this);
    }
    // debug
    if (now_debug) {
        std::cout<<"[GRM]:---------------check productions---------------"<<std::endl;
        this->G_debug();
        // debug 2
        std::cout<<"[GRM]:---------------check terminals---------------"<<std::endl;
        for(terminal* t: this->G_terminals){
            std::cout<<"[GRM]: "<<t->term_name<<" "<<t->symbol_index<<std::endl;
        }
    }
    
}

// helper
// eliminate_the_direct_left_recs(), function,
// implemented the algorithm which can eliminate the direct left recursion of the production, described in
// Gragon Book Algorithm 4.19.
void eliminate_the_direct_left_recs(nonterminal* this_nont, Grammar* G){
    bool if_direct_rec = false;
    std::vector<production*> subset_1;
    std::vector<production*> subset_2;
    // spliting the productions into 2 sets: with/without direct left recursion
    for(production* p: this_nont->nont_prodections){
        // if is left-self-recursive
        if (p->prod_RHS[0] == this_nont->symbol_index) {
            if_direct_rec = true;
            subset_1.push_back(p);
        }
        else{
            subset_2.push_back(p);
        }
    }
    // if there exists left recursion (sub_set1 is not empty)
    if (if_direct_rec) {
        //generate new nonterminal A', declare it and add it into the G
        std::string new_nonterminal_name = this_nont->nont_name + "'";
        while (G->G_string2nont.count(new_nonterminal_name)==1 || G->G_string2term.count(new_nonterminal_name)==1) {
            new_nonterminal_name+="'";
        }
        nonterminal* new_nonterminal = new nonterminal(symbol_counter, new_nonterminal_name);
        G->G_nonterminals.push_back(new_nonterminal);
        G->G_string2nont[new_nonterminal_name] = new_nonterminal;
        G->G_symbol2nont[symbol_counter] = new_nonterminal;
        symbol_counter++;
        // delete all direct-left-rec
        std::vector<production*>::iterator it = this_nont->nont_prodections.begin();
        while (it != this_nont->nont_prodections.end()) {
            if ((*it)->prod_RHS[0] == this_nont->symbol_index) {
                this_nont->nont_prodections.erase(it);
                it = this_nont->nont_prodections.begin();
            }
            else{
                it++;
            }
        }
        // then add new production to the old A
        for(production* p_2: this_nont->nont_prodections){
            if (p_2->prod_RHS[0] == G->G_string2term[eps]->symbol_index) { // if production is eps
                p_2->prod_RHS = {new_nonterminal->symbol_index};
            }
            else{
                p_2->prod_RHS.push_back(new_nonterminal->symbol_index); // else
            }
        }
        // then build up the new A'
        for (production* p_1: subset_1) {
            symbol new_lhs = new_nonterminal->symbol_index;
            std::vector<symbol> new_rhs;
            new_rhs.assign(p_1->prod_RHS.begin()+1, p_1->prod_RHS.end()); // omit the first nonterminal, which is the original symbol A
            new_rhs.push_back(new_lhs);
            production* new_prod = new production(production_counter++, new_lhs, new_rhs);
            new_nonterminal->nont_add_production(new_prod);
        }
        production* new_eps_prod = new production(production_counter++, new_nonterminal->symbol_index, {G->G_string2term[eps]->symbol_index});
        new_nonterminal->nont_add_production(new_eps_prod);
    }
}

// G_eliminate_left_rec(), method function of class::Grammar,
// to eliminate both direct and non-direct left recursions of each production rule stored in the Grammar ob-
// jecti, described in detail in Gragon Book Algorithm 4.19. Main steps: (1)replace all the production rule
// in the form of "Ai: Aj, lembda" where j<i and Aj has productions like "Aj:alpha|beta" into "Ai: alpha, l-
// embda| beta, lembda"; (2) apply the elimination of the direct left recursion: trasfering "A:A, alpha|bet-
// a" into "Ai:beta, A'" and "A':alpha, A'", where the A' is a new nonterminal object.
void Grammar::G_eliminate_left_rec(){
    for (int i = 0; i<this->G_nonterminals.size(); i++) {
        nonterminal* this_nonterminal = this->G_nonterminals[i]; // fetch Ai
        for (int j = 0; j<i; j++) {
            nonterminal* that_nonterminal = this->G_nonterminals[j]; // fetch Aj
            // to check each production in Ai to see whether starts with Aj
            int original_length = (int)this_nonterminal->nont_prodections.size();
            std::vector<production*> new_productions;
            for(int this_prod_index = 0; this_prod_index<original_length; this_prod_index++)
            {
                // if yes,
                if (this_nonterminal->nont_prodections[this_prod_index]->prod_RHS[0] == that_nonterminal->symbol_index){
                    // then use a loop to generate the new productions
                    std::vector<symbol> a_new_rhs;
                    for(production* that_p: that_nonterminal->nont_prodections){
                        a_new_rhs.assign(this_nonterminal->nont_prodections[this_prod_index]->prod_RHS.begin()+1, this_nonterminal->nont_prodections[this_prod_index]->prod_RHS.end());
                        a_new_rhs.insert(a_new_rhs.begin(), that_p->prod_RHS.begin(), that_p->prod_RHS.end());
                        production* a_new_production = new production(production_counter++, this_nonterminal->symbol_index, a_new_rhs);
                        new_productions.push_back(a_new_production);
                        a_new_rhs.clear();
                    }
                    // and also erase the current production because it is already replaced
                }
                else{
                    new_productions.push_back(this_nonterminal->nont_prodections[this_prod_index]);
                }

            }
            this_nonterminal->nont_prodections = new_productions;
        }
        eliminate_the_direct_left_recs(this_nonterminal, this);
    }
    // debug
    if (now_debug) {
        std::cout<<"[GRM]:---------------check productions (2)---------------"<<std::endl;
        this->G_debug();
    }
    
}

// helpers for G_eliminate_common_factor
// find_current_longest_commonf(), helper function,
// to find the longest common factor in the current productions of a same nonterminal object.
std::vector<symbol> find_current_longest_commonf(nonterminal* nt, bool* finished){
    std::vector<symbol> buffer;
    bool cf_found = false;
    for (int i = 0; i<nt->nont_prodections.size(); i++) {
        production* pi = nt->nont_prodections[i];
        for (int j = i+1; j<nt->nont_prodections.size(); j++) {
            production* pj = nt->nont_prodections[j];
            int k = 0;
            while (pi->prod_RHS[k] == pj->prod_RHS[k] && k < pi->prod_RHS.size() && k < pj->prod_RHS.size()) { // avoid the buffer overflow
                *finished = false;
                cf_found = true;
                buffer.push_back(pi->prod_RHS[k]);
                k++;
            }
            if (cf_found) {
                return buffer;
            }
        }
    }
    return buffer;
}

// eliminate_a_cf(), function,
// to eliminate a single common factor from a set of productions of a same nonterminal algorithm described i-
// n Dragon Book Algorithm 4.21.
void eliminate_a_cf(nonterminal* nt, std::vector<symbol> cf, Grammar* G){
    std::vector<production*> p_subset_cf;
    std::vector<production*> p_subset_noncf;
    std::vector<production*>::iterator it = nt->nont_prodections.begin();
    while(it!=nt->nont_prodections.end()){
        production* p = *it;
        int k = 0;
        while (cf[k] == p->prod_RHS[k] && k < cf.size() && k < p->prod_RHS.size()) {
            k++;
        }
        if (k == cf.size()) {
            p_subset_cf.push_back(p);
            it = nt->nont_prodections.erase(it);
        }
        else{
            p_subset_noncf.push_back(p);
            it++;
        }
    }
    // generate new nonterminal A', declare it and add it into the G
    std::string new_nont_name = nt->nont_name+"'";
    while (G->G_string2nont.count(new_nont_name)==1 || G->G_string2term.count(new_nont_name)==1) {
        new_nont_name+="'";
    }
    nonterminal* new_nont = new nonterminal(symbol_counter, new_nont_name);
    G->G_nonterminals.push_back(new_nont);
    G->G_string2nont[new_nont_name] = new_nont;
    G->G_symbol2nont[symbol_counter] = new_nont;
    symbol_counter++;
    // update old A
    std::vector<symbol> new_nt_prod_rhs;
    new_nt_prod_rhs.assign(cf.begin(), cf.end());
    new_nt_prod_rhs.push_back(new_nont->symbol_index);
    production* new_nt_prod = new production(production_counter++, nt->symbol_index, new_nt_prod_rhs);
    nt->nont_add_production(new_nt_prod);
    // fill up the new nonterminal A'
    for (production* p: p_subset_cf) {
        std::vector<symbol> new_new_nont_prod_rhs;
        if (p->prod_RHS.size() == cf.size()) {
            new_new_nont_prod_rhs.push_back(G->G_string2term[eps]->symbol_index);
        }
        else{
            new_new_nont_prod_rhs.assign(p->prod_RHS.begin()+cf.size(), p->prod_RHS.end());
        }
        production* new_new_nont_prod = new production(production_counter++, new_nont->symbol_index, new_new_nont_prod_rhs);
        new_nont->nont_add_production(new_new_nont_prod);
    }
}

// G_eliminate_common_factor(), method function of class::Grammar,
// to eliminate all the common factors of all the notnerminal's productions, steps:(1) for a given nontermin-
// al to find a current longest common factor, (2) if there is not such a cf, go to next nonterminal, (3) if
// yes, then apply the algorithm to remove it by establishing new nonterminal obejcts.
void Grammar::G_eliminate_common_factor(){
    bool finished = false;
    while (!finished) {
        finished=true;
        for (int i = 0; i < this->G_nonterminals.size(); i++) {
            nonterminal* nt = this->G_nonterminals[i];
            std::vector<symbol> next_cf = find_current_longest_commonf(nt, &finished);
            if (next_cf.size() != 0) {
                eliminate_a_cf(nt, next_cf, this);
            }
        }
    }
    // debug
    if (now_debug) {
        std::cout<<"[GRM]:---------------check productions (3)---------------"<<std::endl;
        this->G_debug();
        std::cout<<"[GRM]:---------------check nonterminals (2)---------------"<<std::endl;
        for(nonterminal* nt: this->G_nonterminals){
            std::cout<<"[GRM]: "<<nt->nont_name<<" "<<nt->symbol_index<<std::endl;
        }
    }
}

// G_debug(), method function of class::Grammar,
// debug tool, to show all the productions in current Grammar obejct.
void Grammar::G_debug(){
    for(nonterminal* nt: this->G_nonterminals){
        for(production* p: nt->nont_prodections){
            std::cout<<"[GRM]: pid:"<<p->prod_index<<" \t";
            std::cout<<this->G_symbol2nont[p->prod_LHS]->nont_name<<"-->";
            for(symbol s: p->prod_RHS){
                if (this->is_nonterminal(s)) {
                    std::cout<<this->G_symbol2nont[s]->nont_name<<",";
                }
                else{
                    std::cout<<this->G_symbol2term[s]->term_name<<",";
                }
            }
            std::cout<<std::endl;
        }
    }
}

// helpers
// First(), function,
// recursive function used to find all elements belongs to a nonterminal's first set, details described in G-
// ragon Book Session 4.4.2.
std::set<symbol> First(nonterminal* nt, Grammar* G){
    std::set<symbol> First_set;
    symbol symbol_eps = G->G_string2term[eps]->symbol_index;
    for(production* p: nt->nont_prodections){
        for (int i = 0; i<p->prod_RHS.size(); i++) {
            if (!G->is_nonterminal(p->prod_RHS[i])) {
                // if the following one is terminal or eps(which only occurs when i = 0)
                First_set.insert(p->prod_RHS[i]);
                break; // no more need to continue the search
            }
            // else if the following one is nonterminal:
            std::set<symbol> new_first_set;
            new_first_set = First(G->G_symbol2nont[p->prod_RHS[i]], G);
            // put the things in except the eps
            std::set<symbol>::iterator it = new_first_set.begin();
            while (it!=new_first_set.end()) {
                if (*it != symbol_eps || i == (p->prod_RHS.size()-1)) {
                    // only if the current symbol is the last rhs symbol we will add the eps
                    First_set.insert(*it);
                }
                it++;
            }
            if (new_first_set.count(symbol_eps)==0) {
                // if new_first_set contains no eps, no need to continue the search
                break;
            }
        }
    }
    return First_set;
}

// G_makeFirsts(), method function of class::Grammar,
// to make all the first sets for each nonterminal.
void Grammar::G_makeFirsts(){
//    std::cout<<"[GRM]:-----------------check First set-----------------"<<std::endl;
    for(nonterminal* nt: this->G_nonterminals){
        nt->nont_First = First(nt, this);
        // debug
//        std::cout<<"[FIR]: "<<nt->nont_name<<" has first set:\t";
//        std::set<symbol>::iterator sit = nt->nont_First.begin();
//        while (sit!=nt->nont_First.end()) {
//            std::cout<<*sit<<"'"<<this->G_symbol2term[*sit]->term_name<<"'"<<"\t";
//            sit++;
//        }
//        std::cout<<std::endl;
    }
}

// helpers

// First_of_list(), helper function,
// to find the first set of a symbol list, can be used to find the first set of a sub-rhs list.
std::set<symbol> First_of_list(production* p, int start_index, Grammar* G){
    std::set<symbol> First_set_of_list;
    symbol symbol_eps = G->G_string2term[eps]->symbol_index;
    for (int i = start_index; i<p->prod_RHS.size(); i++) {
        symbol current_symbol = p->prod_RHS[i];
        if (!G->is_nonterminal(current_symbol)) {
            // if is terminal or eps
            First_set_of_list.insert(current_symbol);
            break;
        }
        else{
            // if is nonterminal
            std::set<symbol> new_first_set = G->G_symbol2nont[current_symbol]->nont_First;
            std::set<symbol>::iterator it = new_first_set.begin();
            while (it != new_first_set.end()) {
                if (*it != symbol_eps || i == (p->prod_RHS.size()-1)) {
                    First_set_of_list.insert(*it);
                }
                it++;
            }
            if(new_first_set.count(symbol_eps) == 0){
                break;
            }
        }
    }
    return First_set_of_list;
}

// copy_all_follow_to(), helper fucntion,
// to copy the elements of one nonterminal's follow set to another nonterminal's follow set.
void copy_all_follow_to(nonterminal* A, nonterminal* B, bool* all_done){
    // copy all A's follow to B
    int original_B_follow_len = (int)B->nont_Follow.size();
    for(symbol s: A->nont_Follow){
        B->nont_Follow.insert(s);
    }
    int after_B_follow_len = (int)B->nont_Follow.size();
    if (after_B_follow_len > original_B_follow_len) {
        *all_done = false;
    }
}

// G_makeFollows(), method function of class::Grammar,
// to find the follow sets for all the nonterminals by the following phases: (0) put $ into the starting non-
// terminal; (2) for any "A:a B b", put all symbols in First(b) into Follow(B), except the eps; (3) for any
// form "A:a B b", if First(b) contains eps, put all symbols in FOLLOW(A) into FOLLOW(B).
void Grammar::G_makeFollows(){
    bool all_done = false;
    symbol symbol_eps = this->G_string2term[eps]->symbol_index;
    // phase 0, add "$" to S
    this->G_symbol2nont[0]->nont_Follow.insert(this->G_string2term[EndOfFile]->symbol_index);
    // phase 1
    for(nonterminal* nt: this->G_nonterminals){
        for(production* p: nt->nont_prodections){
            // for each production:
            for (int i = 0; i<p->prod_RHS.size()-1; i++) {
                // for each A->aBb, this is B, that is b
                symbol this_symbol = p->prod_RHS[i];
                if (this->is_nonterminal(this_symbol)) {
                    // if B is nonterminal symbol:
                    nonterminal* B = this->G_symbol2nont[this_symbol];
                    std::set<symbol> First_of_b = First_of_list(p, i+1, this);
                    std::set<symbol>::iterator b_it = First_of_b.begin();
                    while (b_it!=First_of_b.end()) {
                        if (*b_it != symbol_eps) {
                            B->nont_Follow.insert(*b_it);
                        }
                        b_it++;
                    }
                }
            }
        }
    }
    // phase 2
    while (!all_done) {
        all_done = true;
        for(nonterminal* nt: this->G_nonterminals){
            for(production* p: nt->nont_prodections){
                // for each production (1)A->aB or (2)A->aBb
                nonterminal* A = this->G_symbol2nont[p->prod_LHS];
                symbol tail_symbol = p->prod_RHS.back();
                // case (1)
                if (this->is_nonterminal(tail_symbol)) {
                    nonterminal* B = this->G_symbol2nont[tail_symbol];
                    copy_all_follow_to(A, B, &all_done);
                }
                // case (2)
                for (int i = 0; i<p->prod_RHS.size()-1; i++) {
                    // this is B, that is b
                    symbol this_symbol = p->prod_RHS[i];
                    if (this->is_nonterminal(this_symbol)) {
                        nonterminal* B = this->G_symbol2nont[this_symbol];
                        std::set<symbol> First_of_b = First_of_list(p, i+1, this);
                        if(First_of_b.count(symbol_eps) == 1){
                            copy_all_follow_to(A, B, &all_done);
                        }
                    }
                }
            }
        }
    }
    // debug phase
    if (now_debug) {
        std::cout<<"[GRM]:-----------------check Follow set-----------------"<<std::endl;
        for(nonterminal* nt: this->G_nonterminals){
            // debug
            std::cout<<"[FOL]: "<<nt->nont_name<<" has follow set:\t";
            std::set<symbol>::iterator sit = nt->nont_Follow.begin();
            while (sit!=nt->nont_Follow.end()) {
                std::cout<<*sit<<"'"<<this->G_symbol2term[*sit]->term_name<<"'"<<"\t";
                sit++;
            }
            std::cout<<std::endl;
        }
    }
    
}

// G_build_table(), method function of class::Grammar,
// to build up the look ahead table of this (already LL(1)) Grammar. Following phases: (1) for production in
// the form of "A:a", put it in all map[A,x] where x belongs to First(a); (2) if eps in First(a), put all F-
// ollow(a) element y into table with map[A,y] = this production.
void Grammar::G_build_table(){
    for(nonterminal* nt: this->G_nonterminals){
        for(production* p: nt->nont_prodections){
            symbol lhs_sym = p->prod_LHS;
            symbol symbol_eps = this->G_string2term[eps]->symbol_index;
            // phase1: build the first set of the rhs of this production
            std::set<symbol> rhs_First_set = First_of_list(p, 0, this);
            // phase2: make each entry of the table
            nonterminal* A = this->G_symbol2nont[lhs_sym];
            // if eps in the rhs First set:
            for(symbol a: rhs_First_set){
                if (a == symbol_eps) {
                    for(symbol b: A->nont_Follow){
                        this->G_Table[{lhs_sym, b}].insert(p);
                    }
                }
                else{
                    this->G_Table[{lhs_sym, a}].insert(p);
                }
            }
        }
    }
    // debug
    if (now_debug) {
        std::cout<<"[TAB]:-----------------check the Table-----------------"<<std::endl;
        std::map<std::pair<symbol, symbol>, std::set<production*>>::iterator it = this->G_Table.begin();
        while (it!=this->G_Table.end()) {
            std::cout<<"[TAB]: "<<this->G_symbol2nont[it->first.first]->symbol_index<<"'"<<this->G_symbol2nont[it->first.first]->nont_name<<"'"<<"\t"<<this->G_symbol2term[it->first.second]->symbol_index<<"'"<<this->G_symbol2term[it->first.second]->term_name<<"'"<<":\t";
            for(production* p: it->second){
                std::cout<<p->prod_index<<"\t";
            }
            if (it->second.size()>1) {
                std::cout<<"[!AMBIGUOUS!]";
            }
            std::cout<<std::endl;
            it++;
        }
    }
    
}
