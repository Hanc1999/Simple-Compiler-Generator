//
//  nfa2dfa.hpp
//  simple_c_scanner
//
//  Created by 施熠民 on 2021/3/26.
//

#ifndef nfa2dfa_hpp
#define nfa2dfa_hpp

#include <stdio.h>
#include "regex2nfa.h"
#include <set>

// declare a constant global state_index_counter:
extern int DFA_state_index_counter;

// DFS_eps(), helper function,
// to do the deep first search in the NFA graph (trans only if the action is eps).
vector<int>* eps_closure(NFA_SubGraph* the_graph, int query_state_index);

// eps_closure(NFA_SubGraph* the_graph, int query_state_index), function,
// which can find the eps-closure of the whole input NFA state object.
vector<int>* eps_closure(NFA_SubGraph* the_graph, vector<int>* query_state_indices);

// DFA_state, class,
// to store a DFA state variables like contained NFA states, possible actions and provides methods.
class DFA_state{
public:
    // whether has been visited
    bool done;
    // whether contains a final NFA state
    bool is_final;
    vector<string> final_regex_names;
    
    // DFA_state(), constructor function of class::DFA_state,
    // to set the state index from the global counter, and set the bool status.
    DFA_state(int DFA_state_index);
    // itself index
    int DFA_state_index;
    // keep the list of states
    vector<int>* NFA_states_inside;
    // actions
    vector<char> DFA_actions;
};

// DFA_Graph, class,
// to store the information of a full DFA graph, which contains like the NFA graph pointer on which
// the DFA graph bases, and the starting point, the transition look-up table.
class DFA_Graph{
public:
    // the NFA (sub)graph access
    NFA_SubGraph* NFA_Graph;
    
    // DFA_Graph(), constructor function of class::DFA_Graph,
    // to set its local NFA graph as the input, and execute the this->BUILD() function.
    DFA_Graph(NFA_SubGraph* NFA_Graph);
    // start DFA state
    DFA_state* DFA_Start;
    // transition map, key is pair
    map<pair<DFA_state*, char>, DFA_state*> Dtran;
    
    // movve(), method function of class::DFA_Graph,
    // to find the set of the NFA states which can be arrived/transferred from any state s in the
    // given NFA state set T by applying the action char a (and the eps).
    vector<int>* movve(DFA_state* current_DFA_state, char action);
    // states
    vector<DFA_state*> Dstates;
    
    // make_DFA_state, method function of class::DFA_Graph,
    // to make a new object of DFA_state in heap, which will generate the vector of possible acti-
    // ons of the state by searching the original NFA graph, and check whether the DFA state cont-
    // ains the NFA state which is a final state of any regular expression.
    DFA_state* make_DFA_state(vector<int>* NFA_states_inside);
    
    // DFA_BUILD(), method function of class::DFA_Graph,
    // build the overall DFA graph from (1) the starting eps-closure of the start point of the NFA gra-
    // ph and set its done flag as false, (2) then each time to build a new DFA state if there is a un-
    // done DFA state, search its transfer next DFA state with all possible action chars, (4) add it i-
    // nto the state list if never the same has been, (5) update the transition table of the DFA graph.
    void DFA_BUILD();
    
    // show(), helper debug function,
    // to show the DFA graph.
    void show();
};

#endif /* nfa2dfa_hpp */
