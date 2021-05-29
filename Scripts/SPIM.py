#!/usr/bin/env python
# coding: utf-8

# In[2]:


import os
import re
import sys


# In[85]:


regs = [0 for i in range(32)]

    # register indice
reg_dict = {
    "$sp": 29,
    "$fp": 30,
    "$t0": 8,
    "$t1": 9,
    "$t2": 10,
    "$t3": 11,
    "$t4": 12,
    "$t5": 13,
    "$zero": 0,
    "$a0": 4,
    "$v0": 2,
    "$HI": 30,
    "$LO": 31
}

regs[reg_dict["$zero"]] = 0


# In[86]:


# memory list:
mems = [0 for i in range(1000)]


# In[87]:


# codes list, including all code lines:
codes = []
label_map = {}


# In[113]:


# helper
def is_digit(n):
    try:
        int(n)
        return True
    except ValueError:
        return  False


# In[198]:


# simulate different MIPS instructions by manipulating the registers with the memory addresses
def add(des, s1, s2):
    regs[reg_dict[des]] = regs[reg_dict[s1]] + regs[reg_dict[s2]]
    
def addi(des, s, i):
    regs[reg_dict[des]] = regs[reg_dict[s]] + i

# sub $rd, $rs, $rt	R[$rd] ← R[$rs] - R[$rt]
def sub(des, s1, s2):
    regs[reg_dict[des]] = regs[reg_dict[s1]] - regs[reg_dict[s2]]

def mult(s1, s2):
    regs[reg_dict["$HI"]] = ((regs[reg_dict[s1]] * regs[reg_dict[s2]]) % (2**64)) // (2**32)
    regs[reg_dict["$LO"]] = (regs[reg_dict[s1]] * regs[reg_dict[s2]]) % (2**32)

# Numerator and denominator
def div(num, den):
    regs[reg_dict["$HI"]] = (regs[reg_dict[num]] % regs[reg_dict[den]]) 
    regs[reg_dict["$LO"]] = (regs[reg_dict[num]] // regs[reg_dict[den]]) 

# sllv $rd, $rt, $rs	R[$rd] ← R[$rt] << R[$rs]
def sllv(des, s1, s2):
    regs[reg_dict[des]] = regs[reg_dict[s1]] << regs[reg_dict[s2]]
    
# srav $rd, $rt, $rs	R[$rd] ← R[$rt] >> R[$rs]
def srav(des, s1, s2):
    regs[reg_dict[des]] = regs[reg_dict[s1]] >> regs[reg_dict[s2]]

# slt $rd, $rs, $rt	R[$rd] ← R[$rs] < R[$rt]
def slt(des, s1, s2):
    if regs[reg_dict[s1]] < regs[reg_dict[s2]]:
        regs[reg_dict[des]] = 1
    else:
        regs[reg_dict[des]] = 0

# or $rd, $rs, $rt	R[$rd] ← R[$rs] | R[$rt]
def or_(des, s1, s2):
    regs[reg_dict[des]] = regs[reg_dict[s1]] | regs[reg_dict[s2]]

# and $rd, $rs, $rt	R[$rd] ← R[$rs] & R[$rt]
def and_(des, s1, s2):
    regs[reg_dict[des]] = regs[reg_dict[s1]] & regs[reg_dict[s2]]

# nor $rd, $rs, $rt	R[$rd] ← !(R[$rs] | R[$rt])	
def nor(des, s1, s2):
    regs[reg_dict[des]] = not (regs[reg_dict[s1]] | regs[reg_dict[s2]])

def mfhi(t):
    regs[reg_dict[t]] = regs[reg_dict["$HI"]]

def mflo(t):
    regs[reg_dict[t]] = regs[reg_dict["$LO"]]
    
# sw: source_register, offload(int), target_register
def sw(s, off, des):
    temp = regs[reg_dict[s]]
    addr = regs[reg_dict[des]] + off
    mems[addr] = temp

# lw: target_register, offload(int), source_register
def lw(des, off, s):
    addr = regs[reg_dict[s]] + off
    temp = mems[addr]
    regs[reg_dict[des]] = temp

def li(des, i):
    regs[reg_dict[des]] = i

def move(des, s):
    regs[reg_dict[des]] = regs[reg_dict[s]]
    
def syscall():
    v0 = regs[reg_dict["$v0"]]
    if v0 == 1:
        outbuffer = regs[reg_dict["$a0"]]
        print("stdout>>",str(outbuffer))
    elif v0 == 5:
        inbuffer = input("stdin<< ")
        print(inbuffer)
        if(not is_digit(inbuffer)): 
            print("[ERR] illegal input type")
            return -1
        regs[reg_dict["$v0"]] = int(inbuffer)
    return 0

def j(label):
    return label_map[label]
    
#  bne: $t0,$t1,target
#  branch to target if  $t0 <> $t1
def bne(s1, s2, label):
    if regs[reg_dict[s1]] != regs[reg_dict[s2]]:
        return label_map[label]
    else:
        return -1


# In[199]:


def ExecuteLine(ins, ins_index):
#     if len(line_split)>1 and line_split[1] == ":":
    if len(ins)>1 and ins[1] == ":":
        return ins_index+1
    OP = ins[0]
    if OP == "add":
        add(ins[1],ins[2],ins[3])
    elif OP == "addi":
        addi(ins[1], ins[2], int(ins[3]))
    elif OP == "sub":
        sub(ins[1],ins[2],ins[3])
    elif OP == "mult":
        mult(ins[1],ins[2])
    elif OP == "div":
        div(ins[1],ins[2])
    elif OP == "sllv":
        sllv(ins[1],ins[2],ins[3])
    elif OP == "srav":
        srav(ins[1],ins[2],ins[3])
    elif OP == "slt":
        slt(ins[1],ins[2],ins[3])
    elif OP == "or":
        or_(ins[1],ins[2],ins[3])
    elif OP == "and":
        and_(ins[1],ins[2],ins[3])
    elif OP == "nor":
        nor(ins[1],ins[2],ins[3])
    elif OP == "mfhi":
        mfhi(ins[1])
    elif OP == "mflo":
        mflo(ins[1])
    elif OP == "sw":
        sw(ins[1], int(ins[2]), ins[3])
    elif OP == "lw":
        lw(ins[1], int(ins[2]), ins[3])
    elif OP == "li":
        li(ins[1], int(ins[2]))
    elif OP == "move":
        move(ins[1],ins[2])
    elif OP == "syscall":
        ret = syscall()
        if ret < 0: return # error case
    elif OP == "j":
        jump_index = j(ins[1])
        return jump_index
    elif OP == "bne":
        jump_index = bne(ins[1], ins[2], ins[3])
        if jump_index != -1:
            return jump_index
    else:
        print("Error: wrong instruction") # error case
    return ins_index+1


# In[213]:


# pre process
# filepointer = open("./MIPS01.txt")
filepointer = open(sys.argv[1])
line_counter = 0
codes = []
for line in filepointer:
    line_split = re.split('[ ()]',line.rstrip())
    line_split = [ele for ele in line_split if ele != '']
    codes.append(line_split)
    if len(line_split)>1 and line_split[1] == ":":
        label_map[line_split[0]] = line_counter
    line_counter+=1


# In[214]:


codes_len = len(codes)
ins_index = 0
while(True):
    if ins_index >= codes_len: break
    ins = codes[ins_index]
    ins_index = ExecuteLine(ins, ins_index)

