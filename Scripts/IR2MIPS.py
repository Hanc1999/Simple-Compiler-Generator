#!/usr/bin/env python
# coding: utf-8

# In[24]:


import os
import re
import sys


# In[25]:


fixedhead = '''
.data
_hint: .asciiz "Input:"
_ret: .asciiz "\\n"
.globl main
.text
read:
    li $v0,4
    la $a0,_hint
    syscall
    li $v0,5
    syscall
    jr $ra
print:
    li $v0,1
    syscall
    li $v0,4
    la $a0,_ret
    syscall
    move $v0,$0
    jr $ra
'''


# In[32]:


# define some frequently used registers
sp = "$sp"
fp = "$fp"
t0 = "$t0"
t1 = "$t1"
t2 = "$t2"
t3 = "$t3"
t4 = "$t4"
t5 = "$t5"
zero = "$zero"
a0 = "$a0"
v0 = "$v0"
HI = "$HI"
LO = "$LO"


# In[72]:


# declare a symbol table
symboltable = []

# define helper routines
def st_checkid(name):
    if len(symboltable) == 0: 
        if name.startswith("Temp"):
            Case_Declare([None,name])
            return std_checkid(name)
        return -1
    for i in range(len(symboltable)):
        if symboltable[i] == name:
            return i
    if name.startswith("Temp"):
        Case_Declare([None,name])
        return st_checkid(name)
    return -1

def st_addid(name):
    symboltable.append(name)
    return st_checkid(name)


# In[73]:


# MIPS ops
def MIPSinit():
    print("move $sp $fp")

def add(d, s1, s2):
    print("add",d, s1, s2)
    
def addi(d, s, i):
    print("addi",d, s, str(i))

# sub $rd, $rs, $rt	R[$rd] ← R[$rs] - R[$rt]
def sub(d, s1, s2):
    print("sub", d, s1, s2)

def mult(s1, s2):
    print("mult", s1, s2)

# Numerator and denominator
def div(num, den):
    print("div", num, den)

# sllv $rd, $rt, $rs	R[$rd] ← R[$rt] << R[$rs]
def sllv(des, s1, s2):
    print("sllv", des, s1, s2)
    
# srav $rd, $rt, $rs	R[$rd] ← R[$rt] >> R[$rs]
def srav(des, s1, s2):
    print("srav", des, s1, s2)

# slt $rd, $rs, $rt	R[$rd] ← R[$rs] < R[$rt]
def slt(des, s1, s2):
    print("slt", des, s1, s2)

# or $rd, $rs, $rt	R[$rd] ← R[$rs] | R[$rt]
def or_(des, s1, s2):
    print("or", des, s1, s2)

# and $rd, $rs, $rt	R[$rd] ← R[$rs] & R[$rt]
def and_(des, s1, s2):
    print("and", des, s1, s2)

# nor $rd, $rs, $rt	R[$rd] ← !(R[$rs] | R[$rt])	
def nor(des, s1, s2):
    print("nor", des, s1, s2)

def mfhi(t):
    print("mfhi",t)

def mflo(t):
    print("mflo",t)
    
# sw: source_register, offload(int), target_register
def sw(s, off, d):
    if off<0: 
        print("ERROR: using not defined variable")
#         exit()
    print("sw", s, str(off)+"("+d+")")

# lw: target_register, offload(int), source_register
def lw(d, off, s):
    if off<0: 
        print("ERROR: using not defined variable")
#         exit()
    print("lw", d, str(off)+"("+s+")")

def li(d, i):
    print("li", d, str(i))

def move(d, s):
    print("move", d, s)
    
def syscall():
    print("syscall")

def j(label):
    print("j",label)
    
#  bne: $t0,$t1,target
#  branch to target if  $t0 <> $t1
def bne(s1, s2, label):
    print("bne",s1,s2,label)


# In[95]:


def Case_LABEL(line_list):
    print(line_list[1],':')
    
def Case_GOTO(line_list):
    j(line_list[1])
    
def Case_READ(line_list):
    name = line_list[1]
    #
    li(v0, 5)
    syscall()
    sw(v0, st_checkid(name)*4, fp)
    
def Case_WRITE(line_list):
    name = line_list[1]
    #
    lw(t0, st_checkid(name)*4, fp)
    li(v0, 1)
    move(a0, t0)
    syscall()
    
def Case_Declare(line_list):
    name = line_list[1]
    #
    temp = st_addid(name)
    addi(sp, sp, 4)
    sw(zero, temp*4, fp)
    
def Case_IF(line_list):
    label = line_split[5]
    s1 = line_list[1]
    #
    lw(t0, st_checkid(s1)*4, fp)
    bne(t0, zero, label)

def Case_ASSIGN(line_list):
    if line_list[2][0] == '#':
        i = int(line_list[2][1:])
        des = line_list[0]
        #
        li(t0, i)
        sw(t0, st_checkid(des)*4, fp)
    else:
        src = line_list[2]
        des = line_list[0]
        #
        lw(t0, st_checkid(src)*4, fp)
        sw(t0, st_checkid(des)*4, fp)
        
def Math_PLUS(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    add(t0, t1, t0)
    sw(t0, st_checkid(des)*4, fp)
    
def Math_MINUS(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    sub(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

def Math_MULT(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    mult(t0, t1)
    mflo(t2)
    sw(t2, st_checkid(des)*4, fp)

def Math_DIVID(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    div(t0,t1)
    mflo(t2)
    sw(t2, st_checkid(des)*4, fp)

def Math_SHIFT_L(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    sllv(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

def Math_SHIFT_R(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    srav(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

def Logic_LESS(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    slt(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

def Logic_LARGER(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    slt(t0, t1, t0)
    sw(t0, st_checkid(des)*4, fp)
    
def Math_OR(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    or_(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

def Math_AND(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    and_(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

# def Logic_OR():
    
def Logic_UNEQUAL(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    # < or >
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    sub(t0, t0, t1)
    slt(t1, t0, zero)
    slt(t0, zero, t0)
    or_(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

def Logic_EQUAL(line_list):
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    # not (< or >)
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    sub(t0, t0, t1)
    slt(t1, t0, zero)
    slt(t0, zero, t0)
    nor(t0, t0, t1)
    sw(t0, st_checkid(des)*4, fp)

def Logic_LEQ(line_list):# less or equal
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    sub(t0, t0, t1)
    slt(t0, zero, t0)
    nor(t0, t0, zero)
    sw(t0, st_checkid(des)*4, fp)

def Logic_GEQ(line_list):# greater or equal
    s1 = line_list[2]
    s2 = line_list[4]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    lw(t1, st_checkid(s2)*4, fp)
    sub(t0, t0, t1)
    slt(t0, t0, zero)
    nor(t0, t0, zero)
    sw(t0, st_checkid(des)*4, fp)

def Math_UNARY_NOT(line_list):
    s1 = line_list[3]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    nor(t0, t0, zero)
    sw(t0, st_checkid(des)*4, fp)

def Math_UNARY_MINUS(line_list):
    s1 = line_list[3]
    des = line_list[0]
    #
    lw(t0, st_checkid(s1)*4, fp)
    sub(t0, zero, t0)
    sw(t0, st_checkid(des)*4, fp)


# In[96]:


def Translate_Line(line_list):
    if line_list[0] == "Declare": 
#         print("Declare")
        Case_Declare(line_list)
    elif line_list[0] == "IF": 
#         print("IF")
        Case_IF(line_list)
    elif line_list[0] == "GOTO":
#         print("GOTO")
        Case_GOTO(line_list)
    elif line_list[0] == "LABEL": 
#         print("Label")
        Case_LABEL(line_list)
    elif line_list[0] == "scanf": 
#         print("scanf")
        Case_READ(line_list)
    elif line_list[0] == "printf": 
#         print("printf")
        Case_WRITE(line_list)
    elif len(line_list) == 3: 
#         print("ASS")
        Case_ASSIGN(line_list)
    elif len(line_list) == 4:
#         print("UNARY MATH")
        OP = line_list[2]
        if OP == "-":
            Math_UNARY_MINUS(line_list)
        elif OP == "!":
            Math_UNARY_NOT(line_list)
        else:
            print("ERROR: unknown operator")
    elif len(line_list) == 5: 
#         print("BINARY MATH")
        OP = line_list[3]
        if OP == "||": 
            Math_OR(line_list)#to update
        elif OP == "&&":
            Math_AND(line_list)#to update
        elif OP == "|": 
            Math_OR(line_list)
        elif OP == "&":
            Math_AND(line_list)
        elif OP == "==":
            Logic_EQUAL(line_list)
        elif OP == "!=":
            Logic_UNEQUAL(line_list)
        elif OP == "<": 
            Logic_LESS(line_list)
        elif OP == ">":
            Logic_LARGER(line_list)
        elif OP == "<=":
            Logic_LEQ(line_list)
        elif OP == ">=":
            Logic_GEQ(line_list)
        elif OP == ">>":
            Math_SHIFT_R(line_list)
        elif OP == "<<":
            Math_SHIFT_L(line_list)
        elif OP == "+":
            Math_PLUS(line_list)
        elif OP == "-":
            Math_MINUS(line_list)
        elif OP == "*":
            Math_MULT(line_list)
        elif OP == "/":
            Math_DIVID(line_list)
        else:
            print("ERROR: unknown operator")
    else:
        print("ERROR: unknown instruction")


# In[99]:


# read and trans
# filepointer = open("./IR01.txt")
filepointer = open(sys.argv[1])
giegie = ""
# MIPSinit(giegie)
MIPSinit()
for line in filepointer:
    line_split = re.split('[, \n]',line.rstrip())
    line_split = [ele for ele in line_split if ele != '']
    Translate_Line(line_split)

