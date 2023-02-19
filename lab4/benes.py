from os import P_ALL
import sys
import re

k = 3
N = pow(2, k)
# exemplu: 3 0 7 1 4 6 2 5
perm_str = re.split(' ', input('Enter permutation separated by spaces: '))
pairs = []
for i in range(0, N):
    pairs.append((i, int(perm_str[i])))

def get_dual(number):
    if number % 2 == 0:
        return number + 1
    else:
        return number - 1

def get_next_pair(P, C1, C2):
    for pair in P:
        if pair not in C1 and pair not in C2:
            return pair

def separate_perm(P):
    C1 = [P[0]]
    C2 = []
    curr_pair = P[0]
    curr_vec = 1
    for i in range(0, len(P) - 1):
        (left, right) = curr_pair
        if curr_vec == 1:
            dual = get_dual(right)
            for perm_pair in P:
                (perm_left, perm_right) = perm_pair
                if perm_right == dual:
                    if perm_pair not in C2:
                        curr_pair = perm_pair
                    else:
                        curr_pair = get_next_pair(P, C1, C2)

                    C2.append(curr_pair)
                    curr_vec = 2
                    break
        else:
            dual = get_dual(left)
            for perm_pair in P:
                (perm_left, perm_right) = perm_pair
                if perm_left == dual:
                    if perm_pair not in C1:
                        curr_pair = perm_pair
                    else:
                        curr_pair = get_next_pair(P, C1, C2)
            
                    C1.append(curr_pair)
                    curr_vec = 1
                    break
    
    return (C1, C2)

(C1, C2) = separate_perm(pairs)

def get_perm(C):
    P = []
    for pair in C:
        (left, right) = pair
        P.append((left // 2, right // 2))
    
    P.sort()
    return P

P1 = get_perm(C1)
P2 = get_perm(C2)

(C11, C21) = separate_perm(P1)
(C12, C22) = separate_perm(P2)

P11 = get_perm(C11)
P21 = get_perm(C21)
P12 = get_perm(C12)
P22 = get_perm(C22)

number_of_floors = 5
number_of_blocks = 4
benes = [[0 for _ in range(number_of_floors)] for _ in range(number_of_blocks)]
def print_benes(benes):
    for line in benes:
        print(line)

def get_connection(x):
    if x % 2 == 0:
        return 'directa'
    else:
        return 'inversa'

def get_connection_perm(P):
    if P == [(0, 0), (1, 1)]:
        return 'directa'
    else:
        return 'inversa'

for pair in C1:
    (left, right) = pair
    benes[left // 2][0] = get_connection(left)
    benes[right // 2][number_of_floors - 1] = get_connection(right)

for pair in C11:
    (left, right) = pair
    benes[left // 2][1] = get_connection(left)
    benes[right // 2][number_of_floors - 2] = get_connection(right)

for pair in C12:
    (left, right) = pair
    benes[left // 2 + number_of_blocks // 2][1] = get_connection(left)
    benes[right // 2 + number_of_blocks // 2][number_of_floors - 2] = get_connection(right)

benes[0][2] = get_connection_perm(P11)
benes[1][2] = get_connection_perm(P12)
benes[2][2] = get_connection_perm(P21)
benes[3][2] = get_connection_perm(P22)

print_benes(benes)