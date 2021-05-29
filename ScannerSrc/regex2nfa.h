//
//  regex2nfa.h
//  simple_c_scanner
//
//  Created by 施熠民 on 2021/3/21.
//

#ifndef regex2nfa_h
#define regex2nfa_h

// include std
#include <iostream>
#include <cstdlib>
#include <stdio.h>
// include tools
#include <vector>
#include <map>
#include <stack>
// define macro
#define eps '@'

using namespace std;

// declare a constant global state_index_counter:
extern int state_index_counter;

// for each NFA state:
class NFA_state{
public:
    vector<NFA_state*>nexts;
    vector<char>actions;
    int state_index;
    // if state is final state; which final state
    bool is_final;
    string regName;
    
    // add_transition(), method function of class::NFA_state,
    // to add transition (action, next NFA state) tuple to the NFA state object.
    void add_transition(NFA_state* next, char action);
    
    // NFA_state(), constructor function of class::NFA_state,
    // to assign the index of NFA state ans set the final flag as default.
    NFA_state(int state_index);
};

// for each NFA Node:
class NFA_Node{
public:
    // assign_En/Ex(), method function of class::NFA_Node,
    // to assign the enter/exit NFA state for a NFA node object.
    NFA_state* En;
    NFA_state* Ex;
 
    // assign enter/exit state
    void assign_En(NFA_state* en);
    void assign_Ex(NFA_state* ex);
};

class NFA_SubGraph{
public:
    // NFA_SubGraph(), constructor function of class::NFA_SubGraph,
    // to build a new NFA_SubGraph object in the heap, (1) parse the regular expression, (2) make
    // NFA sbugraph in each node, (3) merge together, assign starting/final state points, (4) bui-
    // ld up the state index->state pointer look-up table through DFS.
    NFA_SubGraph(string regularExpression, string regName);
    
    // NFA_SubGraph(), overload constructor function of class::NFA_SubGraph,
    // to merge different NFA sub-graphs, which represents multiple different regular expressions
    // into an overall NFA graph.
    NFA_SubGraph(vector<NFA_SubGraph*>* subGraphs);
    
    NFA_state* Start;
    NFA_state* Final;
    
    // assign_Start/Final(), helper function of class::NFA_SubGraph,
    // to assign the starting/ending point of the NFA (sub)graph.
    void assign_Start(NFA_state* en);
    void assign_Final(NFA_state* ex);
    
    // show(), helper debug function of class::NFA_SubGraph,
    // to show the NFA graph.
    void show(NFA_state* this_state);
    
    // DFS(), method function of class::NFA_SubGraph,
    // to DFS the NFA graph in a recursiv way.
    void DFS(NFA_state* this_state, map<NFA_state*, bool>* visited);
    
    // makeFinalState(), method function of class::NFA_SubGraph,
    // to assign for the final state of the NFA graph, to record the token name of the regular ex-
    // press and make the bool is_final flag of the state be true.
    void makeFinalState(string regName);
    
    // mapping state index to NFA_state*
    map<int, NFA_state*> index2state;
    
protected:
    map<NFA_state*, bool> visited_i2s; // dirty after once use
    
    // DFS_index2state(), method function of class::NFA_SubGraph,
    // building up the state index->state pointer look-up table for the easy random access.
    void DFS_index2state(NFA_state* this_state);
};

NFA_Node* makeNode(char op, NFA_Node* left, NFA_Node* right, int if_escape = 0);

#endif /* regex2nfa_h */
