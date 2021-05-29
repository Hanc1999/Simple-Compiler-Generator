//
//  regex2nfa.cpp
//  simple_c_scanner
//
//  Created by 施熠民 on 2021/3/21.
//

#include "regex2nfa.h"


using namespace std;

// NFA_state(), constructor function of class::NFA_state,
// to assign the index of NFA state ans set the final flag as default.
NFA_state::NFA_state(int state_index){
    this->state_index = state_index;
    this->is_final = false;
}

// add_transition(), method function of class::NFA_state,
// to add transition (action, next NFA state) tuple to the NFA state object.
void NFA_state::add_transition(NFA_state *next, char action){
    this->nexts.push_back(next);
    this->actions.push_back(action);
}

// assign_En/Ex(), method function of class::NFA_Node,
// to assign the enter/exit NFA state for a NFA node object.
void NFA_Node::assign_En(NFA_state *en){
    this->En = en;
}
void NFA_Node::assign_Ex(NFA_state *ex){
    this->Ex = ex;
}

// makeNode(), function,
// according to the given operation flag op, to assembly the NFA node with 1 or 2 other NFA n-
// odes, an abstract object which contains a sub-graph of the overall NFA graph, mantaining t-
// he the head state and the tail state of the sub-graph. Ops: (1) 'or', (2) 'concat', (3) 'c-
// losure', (4) '+', (5) '?', (6) 'range', (7) (escaped) leaf node.
NFA_Node* makeNode(char op, NFA_Node* left, NFA_Node* right, int if_escape){
    if (if_escape == 1) {
        // declare 2 new states
        NFA_state* new_en = new NFA_state(state_index_counter++);
        NFA_state* new_ex = new NFA_state(state_index_counter++);
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        new_Node->assign_En(new_en);
        new_Node->assign_Ex(new_ex);
        // build up
        new_en->add_transition(new_ex, op);
        return new_Node;
    }
    if (op == '|') {
        // declare 2 new states
        NFA_state* new_en = new NFA_state(state_index_counter++);
        NFA_state* new_ex = new NFA_state(state_index_counter++);
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        new_Node->assign_En(new_en);
        new_Node->assign_Ex(new_ex);
        // build up
        new_Node->En->add_transition(left->En, eps);
        new_Node->En->add_transition(right->En, eps);
        left->Ex->add_transition(new_Node->Ex, eps);
        right->Ex->add_transition(new_Node->Ex, eps);
        return new_Node;
    }
    else if (op == '.') {
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        // build up
        new_Node->assign_En(left->En);
        new_Node->assign_Ex(right->Ex);
        left->Ex->add_transition(right->En, eps);
        return new_Node;
    }
    else if (op == '*') {
        // where the right child is empty
        // declare 2 new states
        NFA_state* new_en = new NFA_state(state_index_counter++);
        NFA_state* new_ex = new NFA_state(state_index_counter++);
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        new_Node->assign_En(new_en);
        new_Node->assign_Ex(new_ex);
        // build up
        new_Node->En->add_transition(left->En, eps);
        new_Node->En->add_transition(new_Node->Ex, eps);
        left->Ex->add_transition(new_Node->Ex, eps);
        left->Ex->add_transition(left->En, eps);
        return new_Node;
    }
    else if (op == '+') {
        // where the right child is empty
        // declare 2 new states
        NFA_state* new_en = new NFA_state(state_index_counter++);
        NFA_state* new_ex = new NFA_state(state_index_counter++);
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        new_Node->assign_En(new_en);
        new_Node->assign_Ex(new_ex);
        // build up
        new_Node->En->add_transition(left->En, eps);
        left->Ex->add_transition(new_Node->Ex, eps);
        left->Ex->add_transition(left->En, eps);
        return new_Node;
    }
    else if (op == '?'){
        // where the right child is empty
        // declare 2 new states
        NFA_state* new_en = new NFA_state(state_index_counter++);
        NFA_state* new_ex = new NFA_state(state_index_counter++);
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        new_Node->assign_En(new_en);
        new_Node->assign_Ex(new_ex);
        // build up
        new_Node->En->add_transition(left->En, eps);
        new_Node->En->add_transition(new_Node->Ex, eps);
        left->Ex->add_transition(new_Node->Ex, eps);
        return new_Node;
    }
    else if (op == '-'){
        char start_char = left->En->actions.front();
        char end_char = right->En->actions.front();
        char itr_char;
        // declare 2 new states
        NFA_state* new_en = new NFA_state(state_index_counter++);
        NFA_state* new_ex = new NFA_state(state_index_counter++);
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        new_Node->assign_En(new_en);
        new_Node->assign_Ex(new_ex);
        for (itr_char = start_char; itr_char <= end_char ; itr_char++) {
            new_en->add_transition(new_ex, itr_char);
        }
        return new_Node;
    }
    else{
        // declare 2 new states
        NFA_state* new_en = new NFA_state(state_index_counter++);
        NFA_state* new_ex = new NFA_state(state_index_counter++);
        // declare the new Node to return
        NFA_Node* new_Node = new NFA_Node();
        new_Node->assign_En(new_en);
        new_Node->assign_Ex(new_ex);
        // build up
        new_en->add_transition(new_ex, op);
        return new_Node;
    }
}

// show(), helper debug function of class::NFA_SubGraph,
// to show the NFA graph.
void NFA_SubGraph::show(NFA_state *this_state){
    map<NFA_state*, bool> visited;
    this->DFS(this_state, &visited);
}

// DFS(), method function of class::NFA_SubGraph,
// to DFS the NFA graph in a recursiv way.
void NFA_SubGraph::DFS(NFA_state* this_state, map<NFA_state*, bool>* visited){
    (*visited)[this_state] = true;
    int state_ind = this_state->state_index;
    // if final state
    if (this_state->is_final == true) {
        cout<<"[NFA]: state"<<state_ind<<" is final state for regex:"<<this_state->regName<<endl;
        return;
    }
    // else
    cout<<"[NFA]: state"<<state_ind<<" actions: ";
    for(const char& k : this_state->actions){
        cout << k << " ";
    }
    cout<<endl;

    for(NFA_state* k: this_state->nexts){
        if (!(*visited)[k]) {
            DFS(k, visited);
        }
    }
}

// makeFinalState(), method function of class::NFA_SubGraph,
// to assign for the final state of the NFA graph, to record the token name of the regular ex-
// press and make the bool is_final flag of the state be true.
void NFA_SubGraph::makeFinalState(string regName){
    this->Final->is_final = true;
    this->Final->regName = regName;
}

// regexParser(), function,
// to parse the string type regular expression into a syntax tree, where eac node is leaf or
// contains an OP like '|' or '.'.
NFA_Node* regexParser(string a){
    NFA_Node* new_Node = NULL;
//    cout<<"[SUB]: "<<a<<endl;
    int paren_counter = 0;
    int is_leaf = 0;
    if (a.length() == 1) {
        is_leaf = 1;
        new_Node = makeNode(a[0], NULL, NULL);
    }
    // escaping case special solution:
    if (a.length() == 2 && a[0] == '\\') {
        is_leaf = 1;
        new_Node = makeNode(a[1], NULL, NULL, 1);
    }
    for (int i = 0; i<a.length() && !is_leaf; i++) {
        char next_char = a[i];
        // if the coming character is '\', then the
        if (next_char == '\\') {
            i = i+2;
            next_char = a[i];
        }
        if (next_char=='(') {
            paren_counter ++;
        }
        else if (next_char == ')'){
            paren_counter --;
        }
//        where the @ means the +
        if((paren_counter == 0) && (next_char == '|' || next_char == '.' || next_char == '*' || next_char == '+' || next_char == '?' || next_char == '-' ) ){
            string sub_string_1 = a.substr(0,i);
            string sub_string_2 = a.substr(i+1, a.length() - i-1);
            NFA_Node* left = regexParser(sub_string_1);
            NFA_Node* right = NULL;
            if (sub_string_2.length()>0) {
                right = regexParser(sub_string_2);
            }
            
            new_Node = makeNode(next_char, left, right);
            break;
        }
        else if (i == a.length()-1 && paren_counter == 0 && next_char == ')'){
            string sub_string_1 = a.substr(1, a.length()-2);
            new_Node = regexParser(sub_string_1);
            break;
        }
    }
//    cout<<"[SUB-Back]: "<<a<<state_index_counter<<endl;
    return new_Node;
}

// assign_Start/Final(), helper function of class::NFA_SubGraph,
// to assign the starting/ending point of the NFA (sub)graph.
void NFA_SubGraph::assign_Start(NFA_state *en){
    this->Start = en;
}
void NFA_SubGraph::assign_Final(NFA_state *ex){
    this->Final = ex;
}

// NFA_SubGraph(), constructor function of class::NFA_SubGraph,
// to build a new NFA_SubGraph object in the heap, (1) parse the regular expression, (2) make
// NFA sbugraph in each node, (3) merge together, assign starting/final state points, (4) bui-
// ld up the state index->state pointer look-up table through DFS.
NFA_SubGraph::NFA_SubGraph(string regularExpression, string regName){
    NFA_Node* res;
    res = regexParser(regularExpression);
    this->assign_Start(res->En);
    this->assign_Final(res->Ex);
    this->makeFinalState(regName);
    this->DFS_index2state(this->Start);
}

// NFA_SubGraph(), overload constructor function of class::NFA_SubGraph,
// to merge different NFA sub-graphs, which represents multiple different regular expressions
// into an overall NFA graph.
NFA_SubGraph::NFA_SubGraph(vector<NFA_SubGraph*>* subGraphs){
    NFA_state* new_start_point = new NFA_state(state_index_counter++);
    this->index2state[new_start_point->state_index] = new_start_point;
    for (NFA_SubGraph* k : *subGraphs) {
        new_start_point->add_transition(k->Start, eps);
        map<int, NFA_state*>::iterator it = k->index2state.begin();
        while (it != k->index2state.end()) {
            this->index2state[it->first] = it->second;
            it++;
        }
    }
    this->Start = new_start_point;
    this->Final = NULL;
}

// DFS_index2state(), method function of class::NFA_SubGraph,
// building up the state index->state pointer look-up table for the easy random access.
void NFA_SubGraph::DFS_index2state(NFA_state *this_state){
    this->visited_i2s[this_state] = true;
    int state_id = this_state->state_index;
    this->index2state[state_id] = this_state;
    for(NFA_state* k: this_state->nexts){
        if (!this->visited_i2s[k]) {
            DFS_index2state(k);
        }
    }
}
