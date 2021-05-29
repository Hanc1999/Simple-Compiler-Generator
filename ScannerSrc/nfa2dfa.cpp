//
//  nfa2dfa.cpp
//  simple_c_scanner
//
//  Created by 施熠民 on 2021/3/26.
//

#include "nfa2dfa.h"

// DFS_eps(), helper function,
// to do the deep first search in the NFA graph (trans only if the action is eps).
void DFS_eps(NFA_state* this_state, map<NFA_state*, bool>* visited, vector<int>* res){
    (*visited)[this_state] = true;
    for (int i = 0; i<this_state->nexts.size(); i++) {
        if (this_state->actions[i] == eps) {
            NFA_state* the_next = this_state->nexts[i];
            if ((*visited)[the_next]!=true) {
                res->push_back(the_next->state_index);
                DFS_eps(the_next, visited, res);
            }
        }
    }
}

// eps_closure(NFA_SubGraph* the_graph, int query_state_index), function,
// which can find the eps-closure of the whole input NFA state object.
vector<int>* eps_closure(NFA_SubGraph* the_graph, int query_state_index){
    vector<int>* res = new vector<int>;
    map<NFA_state*, bool> visited;
    NFA_state* start_point = the_graph->index2state[query_state_index];
    res->push_back(start_point->state_index);
    DFS_eps(start_point, &visited, res);
    return res;
}

// eps_closure(NFA_SubGraph* the_graph, vector<int>* query_state_indices), overload function,
// which can find the eps-closure of the whole input NFA state vector/array.
vector<int>* eps_closure(NFA_SubGraph* the_graph, vector<int>* query_state_indices){
    vector<int>* res = new vector<int>;
    map<NFA_state*, bool> visited;
    for (int i = 0; i < query_state_indices->size(); i++) {
        NFA_state* start_point = the_graph->index2state[(*query_state_indices)[i]];
        if (visited[start_point] != true) {
            res->push_back(start_point->state_index);
            DFS_eps(start_point, &visited, res);
        }
    }
    return res;
}

// DFA_state(), constructor function of class::DFA_state,
// to set the state index from the global counter, and set the bool status.
DFA_state::DFA_state(int DFA_state_index){
    this->DFA_state_index = DFA_state_index;
    this->done = false;
}

// DFA_Graph(), constructor function of class::DFA_Graph,
// to set its local NFA graph as the input, and execute the this->BUILD() function.
DFA_Graph::DFA_Graph(NFA_SubGraph* NFA_Graph){
    this->NFA_Graph = NFA_Graph;
    this->DFA_BUILD();
}

// movve(), method function of class::DFA_Graph,
// to find the set of the NFA states which can be arrived/transferred from any state s in the
// given NFA state set T by applying the action char a (and the eps).
vector<int>* DFA_Graph::movve(DFA_state* T, char a){
    map<int, bool> included;
    vector<int> states_will_be_touched;
    for (int i = 0; i<T->NFA_states_inside->size(); i++) {
        int current_NFA_index = (*T->NFA_states_inside)[i];
        NFA_state* current_NFA_state = this->NFA_Graph->index2state[current_NFA_index];
        vector<char> current_actions = current_NFA_state->actions;
        for (int j = 0; j<current_actions.size(); j++) {
            NFA_state* next_state = current_NFA_state->nexts[j];
            int next_state_index = next_state->state_index;
            if (current_actions[j] == a && current_actions[j] != eps && included[next_state_index] != true) {
                included[next_state_index] = true;
                states_will_be_touched.push_back(next_state_index);
            }
        }
    }
    return (eps_closure(this->NFA_Graph, &states_will_be_touched));
}

// make_DFA_state, method function of class::DFA_Graph,
// to make a new object of DFA_state in heap, which will generate the vector of possible acti-
// ons of the state by searching the original NFA graph, and check whether the DFA state cont-
// ains the NFA state which is a final state of any regular expression.
DFA_state* DFA_Graph::make_DFA_state(vector<int>* NFA_states_inside){
    DFA_state* this_DFA_state = new DFA_state(DFA_state_index_counter++);
    this_DFA_state->NFA_states_inside = NFA_states_inside;
    this_DFA_state->is_final = false;
    map<char, bool> included;
    for (int i = 0; i<NFA_states_inside->size(); i++) {
        int current_NFA_index = (*NFA_states_inside)[i];
        NFA_state* current_NFA_state = this->NFA_Graph->index2state[current_NFA_index];
        if (current_NFA_state->is_final) {
            this_DFA_state->is_final = true;
            this_DFA_state->final_regex_names.push_back(current_NFA_state->regName);
        }
        for(const char& k : current_NFA_state->actions){
            if (included[k] != true && k != eps) {
                included[k] = true;
                this_DFA_state->DFA_actions.push_back(k);
            }
        }
    }
    
    return this_DFA_state;
}

// Compare_DFA_states(), helper function,
// to compare 2 DFA states too see whether they contain same inner NFA states.
bool Compare_DFA_states(DFA_state* e1, DFA_state* e2){
    set<int> s1;
    set<int> s2;
    for(const int& k : *e1->NFA_states_inside){
        s1.insert(k);
    }
    for(const int& k : *e2->NFA_states_inside){
        s2.insert(k);
    }
    return s1==s2;
}

// Check_if_exist(), helper funciton,
// to check whether any of the state in the given DFA state vectors shares the same inner NFA
// state of the new DFA state.
bool Check_if_exist(vector<DFA_state*>* Dstates, DFA_state* new_state){
    bool if_exist = false;
    for (int i = 0; i<Dstates->size(); i++) {
        if (Compare_DFA_states((*Dstates)[i], new_state)) {
            if_exist = true;
            break;
        }
    }
    return if_exist;
}

// Ptr_of_same_state(), helper function,
// to find the pointer of the found same DFA state in the DFA graph's Dstates variable.
DFA_state* Ptr_of_same_state(vector<DFA_state*>* Dstates, DFA_state* new_state){
    for (int i = 0; i<Dstates->size(); i++) {
        if (Compare_DFA_states((*Dstates)[i], new_state)) {
            return (*Dstates)[i];
            break;
        }
    }
    return NULL;
}

// DFA_BUILD(), method function of class::DFA_Graph,
// build the overall DFA graph from (1) the starting eps-closure of the start point of the NFA gra-
// ph and set its done flag as false, (2) then each time to build a new DFA state if there is a un-
// done DFA state, search its transfer next DFA state with all possible action chars, (4) add it i-
// nto the state list if never the same has been, (5) update the transition table of the DFA graph.
void DFA_Graph::DFA_BUILD(){
    vector<int>* Start_state_closure = eps_closure(this->NFA_Graph, this->NFA_Graph->Start->state_index);
    this->DFA_Start = make_DFA_state(Start_state_closure);
//    for(const int k : *this->DFA_Start->NFA_states_inside){
//        cout << k << " ";
//    }
//    cout<<endl;
//    for(const char& k : this->DFA_Start->DFA_actions){
//        cout << k << " ";
//    }
//    cout<<endl;
    this->Dstates.push_back(this->DFA_Start);
    bool built_done = false;
    while (!built_done) {
        built_done = true;
        for (int i = 0; i<this->Dstates.size(); i++) {
            DFA_state* current_state = Dstates[i];
            if (!current_state->done) {
                built_done = false;
                current_state->done = true;
//                cout<<"touched1"<<endl;
                for(const char act : current_state->DFA_actions){
                    vector<int>* states_will_be_touched = movve(current_state, act);
                    // [debug]
//                    for (int idx: *states_will_be_touched) {
//                        cout<<idx<<" ";
//                    }
//                    cout<<endl;
                    
                    DFA_state* new_state = make_DFA_state(states_will_be_touched);
                    if (!Check_if_exist(&this->Dstates, new_state)) {
                        
                        this->Dstates.push_back(new_state);
                        this->Dtran[{current_state,act}] = new_state;
                    }
                    else{
                        
                        this->Dtran[{current_state,act}] = Ptr_of_same_state(&this->Dstates, new_state);
                    }
                }
            }
        }
    }
    
}

// show(), helper debug function,
// to show the DFA graph.
void DFA_Graph::show(){
    for( DFA_state* k : this->Dstates){
        cout<<"[SHO]: State"<<k->DFA_state_index<<":"<<endl;
        if (k->is_final) {
            cout<<"\tThis state is final state of regs:";
            for (string regname: k->final_regex_names) {
                cout<<" "<<regname;
            }
            cout<<endl;
        }
        cout<<"\tNFA states:";
        for (int idx: *k->NFA_states_inside) {
            cout<<idx<<" ";
        }
        cout<<endl;
        for ( char act: k->DFA_actions) {
            DFA_state* touching_state = this->Dtran[{k,act}];
            cout<<"\t"<<act<<"->State"<< touching_state->DFA_state_index<<endl;
        }
    }

}
