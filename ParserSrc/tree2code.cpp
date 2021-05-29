//
//  tree2code.cpp
//  mini_C_compiler
//
//  Created by 施熠民 on 2021/5/1.
//

#include "tree2code.hpp"

// class SymbolTable
int SymbolTable::ST_check_and_set(std::string var_name, int current_scope){
    if (this->ST_vars.count(var_name)==0) {
        // case 1: no such var
        std::stack<int> scope_stk;
        scope_stk.push(current_scope);
        this->ST_vars[var_name] = scope_stk; // it should be a hard copy
    }
    else{
        if (this->ST_vars[var_name].top() != current_scope) {
            // case 2: we have a var out the scope with same name
            ST_vars[var_name].push(current_scope);
        }
        else{
            // case 3: multiple declaration, error!
            return -1;
        }
    }
    return 1;
}

void SymbolTable::operator=(SymbolTable* rhs){
    this->ST_scope = rhs->ST_scope;
    this->ST_temp_counter = rhs->ST_temp_counter;
    this->ST_vars = rhs->ST_vars; // should be hard
}

SymbolTable::SymbolTable(){
    this->ST_scope = 0;
    this->ST_temp_counter = 0;
}

SymbolTable::SymbolTable(SymbolTable* rhs){
    this->ST_scope = rhs->ST_scope;
    this->ST_temp_counter = rhs->ST_temp_counter;
    this->ST_vars = rhs->ST_vars; // should be hard
}

// class IRGenerator
IRGenerator::IRGenerator(ParseTreeNode* root){
    this->IRG_root_node = root;
    this->IRG_label_counter = 0;
    // diff production rule sets
    this->IRG_IF_cases_set = {125, 134};
    this->IRG_WHILE_cases_set = {28, 113};
    this->IRG_DOWHILE_cases_set = {29, 114};
    this->IRG_EXP_cases_set = {33, 35, 37, 39, 41, 44, 49, 52, 55};
    this->IRG_EXP10_cases_set = {59, 60};
    this->IRG_EXP11_cases_set = {61, 128};
    this->IRG_EXPPIE_cases_set = {71, 73, 75, 77, 79, 80, 82, 83, 84, 85, 87, 88, 90, 91, 93, 94};
    this->IRG_DECLARE_cases_set = {118};
    this->IRG_ASSIGN_cases_set = {122, 131};
    this->IRG_TRIVIAL_cases_set = {2, 13, 14, 15, 17, 18, 19, 20, 21, 22, 58, 63, 135};
    this->IRG_NULL_cases_set = {66, 68, 70, 72, 74, 76, 78, 81, 86, 89, 92, 95, 121, 126, 129};
    this->IRG_BLOCK_cases_set = {9, 10};
    this->IRG_FUNCTION_cases_set = {31, 32, 116, 117};
    
}

void IRGenerator::IRGstart(){
    if(now_debug){
        std::cout<<"[IRC]:------------------IR codes------------------"<<std::endl;
    }
    SymbolTable* init_symbol_table = new SymbolTable();
    this->Translate(this->IRG_root_node, init_symbol_table);
}


// helpers of Translate: each case

bool belongs_to(int e, std::vector<int>* v){
    for(int a: *v){
        if (e == a) {
            return true;
        }
    }
    return false;
}

std::string Trans_Declare_Case(ParseTreeNode *this_node, SymbolTable *this_stable){
    std::string IDname = this_node->PTN_children[0]->PTN_content;
    int iferror = this_stable->ST_check_and_set(IDname, this_stable->ST_scope);
    if (iferror < 0) {
        std::cout<<"[IRC]: ERROR-MULTIPLE-DECLARATION-IN-SAME-SCOPE!"<<std::endl;
        return "ERROR";
    }
    // pid of declaration'
    int pid_of_decpie = this_node->PTN_children[1]->PTN_production_index;
    // case by case
    if (pid_of_decpie == 119) {
        std::string INT_NUM = this_node->PTN_children[1]->PTN_children[1]->PTN_content;
        std::cout<<"Declare "<<IDname<<", INT"<<std::endl;
        std::cout<<IDname<<" = #"<<INT_NUM<<std::endl;
    }
    else if(pid_of_decpie == 120){
        std::string INT_NUM = this_node->PTN_children[1]->PTN_children[1]->PTN_content;
        std::cout<<"Declare "<<IDname<<"[#"<<INT_NUM<<"], INT"<<std::endl;
    }
    else{
        // case 121
        std::cout<<"Declare "<<IDname<<", INT"<<std::endl;
    }
    return "";
}

std::string Trans_EXP_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    std::string TempAname = IRG->Translate(this_node->PTN_children[0], this_stable);
    std::string TempBname = IRG->Translate(this_node->PTN_children[1], this_stable);
    if (TempBname == "") {
        // if it's the end directly return
        return TempAname;
    }
    // else we generate a new Temp
    std::string res =  "Temp"+std::to_string(this_stable->ST_temp_counter);
    this_stable->ST_temp_counter++;
    std::string OP = this_node->PTN_children[1]->PTN_children[0]->PTN_content;
    std::cout<<res<<" = "<<TempAname<<" "<<OP<<" "<<TempBname<<std::endl;
    return res;
}

std::string Trans_EXP10_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    std::string OP = this_node->PTN_children[0]->PTN_content;
    std::string TempAname = IRG->Translate(this_node->PTN_children[1], this_stable);
    std::string res =  "Temp"+std::to_string(this_stable->ST_temp_counter);
    this_stable->ST_temp_counter++;
    std::cout<<res<<" = "<<OP<<" "<<TempAname;
    return res;
}

std::string Trans_EXP11_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
//    std::cout<<"EXP11 callled"<<std::endl;
    if (this_node->PTN_production_index == 128) {
        std::string IDname = this_node->PTN_children[0]->PTN_content;
        if (this_node->PTN_children[1]->PTN_production_index == 130) {
            IDname = IDname + "[" + IRG->Translate(this_node->PTN_children[1]->PTN_children[1], this_stable) + "]";
        }
        return IDname;
    }
    else{
        // pid of this node = 61
        std::string res = "Temp"+std::to_string(this_stable->ST_temp_counter);
        this_stable->ST_temp_counter++;
        std::cout<<res<<" = "<<("#" + this_node->PTN_children[0]->PTN_content)<<std::endl;
        return res;
    }
}

std::string Trans_EXPPIE_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    if (this_node->PTN_children.size() == 1) {
        return "";
    }
    std::string TempAname = IRG->Translate(this_node->PTN_children[1], this_stable);
    std::string TempBname = IRG->Translate(this_node->PTN_children[2], this_stable);
    if (TempBname == "") {
        return TempAname;
    }
    std::string res =  "Temp"+std::to_string(this_stable->ST_temp_counter);
    this_stable->ST_temp_counter++;
    std::string OP = this_node->PTN_children[2]->PTN_children[0]->PTN_content;
    std::cout<<res<<" = "<<TempAname<<" "<<OP<<" "<<TempBname<<std::endl;
    return res;
}

std::string Trans_ASSIGN_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    std::string IDname = this_node->PTN_children[0]->PTN_content;
    int pid_of_rchild = this_node->PTN_children[1]->PTN_production_index;
    if (pid_of_rchild == 123) {
        std::string TempAname = IRG->Translate(this_node->PTN_children[1]->PTN_children[1], this_stable);
        std::string TempBname = IRG->Translate(this_node->PTN_children[1]->PTN_children[4], this_stable);
        std::cout<<IDname<<"["<<TempAname<<"]"<<" = "<<TempBname<<std::endl;
    }
    else if (pid_of_rchild == 124){
        std::string TempAname = IRG->Translate(this_node->PTN_children[1]->PTN_children[1], this_stable);
        std::cout<<IDname<<" = "<<TempAname<<std::endl;
    }
    else if (pid_of_rchild == 132){
        std::string TempAname = IRG->Translate(this_node->PTN_children[1]->PTN_children[1], this_stable);
        std::string TempBname = IRG->Translate(this_node->PTN_children[1]->PTN_children[4], this_stable);
        std::cout<<IDname<<"["<<TempAname<<"]"<<" = "<<TempBname<<std::endl;
        // continue to work
        IRG->Translate(this_node->PTN_children[1]->PTN_children[6], this_stable);
    }
    else{
        // if pid_of_rchild is 133
        std::string TempAname = IRG->Translate(this_node->PTN_children[1]->PTN_children[1], this_stable);
        std::cout<<IDname<<" = "<<TempAname<<std::endl;
        // continue to work
        IRG->Translate(this_node->PTN_children[1]->PTN_children[3], this_stable);
    }
    return "";
}

std::string Trans_IF_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    // make common labels
    std::string LabelTrue = "label_" + std::to_string(IRG->IRG_label_counter);
    IRG->IRG_label_counter++;
    std::string LabelNext;
    std::string LebelFalse;
    // by cases
    int this_pid = this_node->PTN_production_index;
    if (this_pid == 125) {
        // find the condition
        std::string Condition = IRG->Translate(this_node->PTN_children[0]->PTN_children[2], this_stable);
        // see cases
        int rhs_pid = this_node->PTN_children[1]->PTN_production_index;
        if (rhs_pid == 126) {
            // then the only-if case
            LabelNext = "label_" + std::to_string(IRG->IRG_label_counter);
            IRG->IRG_label_counter++;
            std::cout<<"IF "<<Condition<<" != 0 GOTO "<<LabelTrue<<std::endl;
            std::cout<<"GOTO "<<LabelNext<<std::endl;
            std::cout<<"LABEL "<<LabelTrue<<" :"<<std::endl;
            IRG->Translate(this_node->PTN_children[0]->PTN_children[4], this_stable);
            std::cout<<"LABEL "<<LabelNext<<" :"<<std::endl;
        }
        else{
            // rhs_pid = 127, the if-else case
            LebelFalse = "label_" + std::to_string(IRG->IRG_label_counter);
            IRG->IRG_label_counter++;
            LabelNext = "label_" + std::to_string(IRG->IRG_label_counter);
            IRG->IRG_label_counter++;
            std::cout<<"IF "<<Condition<<" != 0 GOTO "<<LabelTrue<<std::endl;
            std::cout<<"GOTO "<<LebelFalse<<std::endl;
            std::cout<<"LABEL "<<LabelTrue<<" :"<<std::endl;
            IRG->Translate(this_node->PTN_children[0]->PTN_children[4], this_stable);
            std::cout<<"GOTO "<<LabelNext<<std::endl;
            std::cout<<"LABEL "<<LebelFalse<<" :"<<std::endl;
            IRG->Translate(this_node->PTN_children[1]->PTN_children[1], this_stable);
            std::cout<<"LABEL "<<LabelNext<<" :"<<std::endl;
        }
        
    }
    else{
        // this_pid = 134
        // find the condition
        std::string Condition = IRG->Translate(this_node->PTN_children[2], this_stable);
        // by cases
        int rhs_pid = this_node->PTN_children[5]->PTN_production_index;
        if (rhs_pid == 135) {
            // then the only-if case
            LabelNext = "label_" + std::to_string(IRG->IRG_label_counter);
            IRG->IRG_label_counter++;
            std::cout<<"IF "<<Condition<<" != 0 GOTO "<<LabelTrue<<std::endl;
            std::cout<<"GOTO "<<LabelNext<<std::endl;
            std::cout<<"LABEL "<<LabelTrue<<" :"<<std::endl;
            IRG->Translate(this_node->PTN_children[4], this_stable);
            std::cout<<"LABEL "<<LabelNext<<" :"<<std::endl;
            // continue work
            IRG->Translate(this_node->PTN_children[5]->PTN_children[0], this_stable);
        }
        else{
            // rhs_pid = 136, the if-else case
            LebelFalse = "label_" + std::to_string(IRG->IRG_label_counter);
            IRG->IRG_label_counter++;
            LabelNext = "label_" + std::to_string(IRG->IRG_label_counter);
            IRG->IRG_label_counter++;
            std::cout<<"IF "<<Condition<<" != 0 GOTO "<<LabelTrue<<std::endl;
            std::cout<<"GOTO "<<LebelFalse<<std::endl;
            std::cout<<"LABEL "<<LabelTrue<<" :"<<std::endl;
            IRG->Translate(this_node->PTN_children[4], this_stable);
            std::cout<<"GOTO "<<LabelNext<<std::endl;
            std::cout<<"LABEL "<<LebelFalse<<" :"<<std::endl;
            IRG->Translate(this_node->PTN_children[5]->PTN_children[1], this_stable);
            std::cout<<"LABEL "<<LabelNext<<" :"<<std::endl;
            // continue works
            IRG->Translate(this_node->PTN_children[5]->PTN_children[2], this_stable);
        }
    }
    return "";
}

std::string Trans_WHILE_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    // make labels
    std::string LabelWhile = "label_" + std::to_string(IRG->IRG_label_counter);
    IRG->IRG_label_counter++;
    std::string LabelStart = "label_" + std::to_string(IRG->IRG_label_counter);
    IRG->IRG_label_counter++;
    std::string LabelEnd = "label_" + std::to_string(IRG->IRG_label_counter);
    IRG->IRG_label_counter++;
    // make code
    std::cout<<"LABEL "<<LabelWhile<<" :"<<std::endl;
    std::string Condition = IRG->Translate(this_node->PTN_children[2], this_stable);
    std::cout<<"IF "<<Condition<<" != 0 GOTO "<<LabelStart<<std::endl;
    std::cout<<"GOTO "<<LabelEnd<<std::endl;
    std::cout<<"LABEL "<<LabelStart<<" :"<<std::endl;
    IRG->Translate(this_node->PTN_children[4], this_stable);
    std::cout<<"GOTO "<<LabelWhile<<std::endl;
    std::cout<<"LABEL "<<LabelEnd<<" :"<<std::endl;
    // by cases
    int pid = this_node->PTN_production_index;
    if (pid == 113) {
        // continue work
        IRG->Translate(this_node->PTN_children[5], this_stable);
    }
    return "";
}

std::string Trans_DOWHILE_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    // make label
    std::string LabelBegin = "label_" + std::to_string(IRG->IRG_label_counter);
    IRG->IRG_label_counter++;
    // make code
    std::cout<<"LABEL "<<LabelBegin<<" :"<<std::endl;
    IRG->Translate(this_node->PTN_children[1], this_stable);
    std::string Condition = IRG->Translate(this_node->PTN_children[4], this_stable);
    std::cout<<"IF "<<Condition<<" != 0 GOTO "<<LabelBegin<<std::endl;
    // by cases
    int pid = this_node->PTN_production_index;
    if (pid == 114) {
        // continue work
        IRG->Translate(this_node->PTN_children[7], this_stable);
    }
    return "";
}

std::string Trans_FUNCTION_Case(ParseTreeNode *this_node, SymbolTable *this_stable, IRGenerator* IRG){
    int pid = this_node->PTN_production_index;
    std::string FUNC = this_node->PTN_children[0]->PTN_content;
    if (pid == 31 || pid == 116) {
        std::string IDname = this_node->PTN_children[2]->PTN_content;
        std::cout<<FUNC<<" "<<IDname<<std::endl;
        if (pid == 116) {
            // continue work
            IRG->Translate(this_node->PTN_children[5], this_stable);
        }
    }
    else{
        // pid == 32 or 117
        std::string TempAname = IRG->Translate(this_node->PTN_children[2], this_stable);
        std::cout<<FUNC<<" "<<TempAname<<std::endl;
        if (pid == 117) {
            // continue work
            IRG->Translate(this_node->PTN_children[5], this_stable);
        }
    }
    return "";
}

std::string IRGenerator::Translate(ParseTreeNode *this_node, SymbolTable *this_stable){
    std::string res = "";
    
    int pid = this_node->PTN_production_index;
    
    if (belongs_to(pid, &this->IRG_IF_cases_set)) {
        std::string midres = Trans_IF_Case(this_node, this_stable, this);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_WHILE_cases_set)){
        std::string midres = Trans_WHILE_Case(this_node, this_stable, this);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_DOWHILE_cases_set)){
        std::string midres = Trans_DOWHILE_Case(this_node, this_stable, this);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_EXP_cases_set)){
        std::string midres = Trans_EXP_Case(this_node, this_stable, this);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_EXP10_cases_set)){
        std::string midres = Trans_EXP10_Case(this_node, this_stable, this);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_EXP11_cases_set)){
        std::string midres = Trans_EXP11_Case(this_node, this_stable, this);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_EXPPIE_cases_set)){
        std::string midres = Trans_EXPPIE_Case(this_node, this_stable, this);
        return midres;
        
    }
    else if (belongs_to(pid, &this->IRG_DECLARE_cases_set)){
        std::string midres =  Trans_Declare_Case(this_node, this_stable);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_ASSIGN_cases_set)){
        std::string midres = Trans_ASSIGN_Case(this_node, this_stable, this);
        return midres;
    }
    else if (belongs_to(pid, &this->IRG_TRIVIAL_cases_set)){
        for(ParseTreeNode* next: this_node->PTN_children){
            if (!next->PTN_is_terminal) {
                return this->Translate(next, this_stable);
            }
        }
    }
    else if (belongs_to(pid, &this->IRG_NULL_cases_set)){
        return "";
    }
    else if (belongs_to(pid, &this->IRG_BLOCK_cases_set)){
        for (ParseTreeNode* next: this_node->PTN_children) {
            if (!next->PTN_is_terminal) {
                SymbolTable* new_stable = new SymbolTable(this_stable);
                return this->Translate(next, new_stable);
            }
        }
    }
    else if (belongs_to(pid, &this->IRG_FUNCTION_cases_set)){
        std::string midres = Trans_FUNCTION_Case(this_node, this_stable, this);
        return midres;
    }
    
    // if not returned yet we keep go through each nonterminal nodes
    for(ParseTreeNode* next: this_node->PTN_children){
        if (!next->PTN_is_terminal) {
//            SymbolTable* new_stable = new SymbolTable(this_stable);
//            new_stable->ST_scope++;
//            this->Translate(next, new_stable);
            this->Translate(next, this_stable);
        }
    }
//    std::cout<<"I hope this place not reached!"<<std::endl;
    return res;
}
