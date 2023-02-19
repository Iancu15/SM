import re

# numar de procesoare
p = 3
nr_ops = int(input("Please enter number of operations: "))

# de la tastatura se citesc operatiile de citire ca {nr_procesor}{operatie}
# P1Rd se citeste ca 1r, iar P3Wr ca 3w
# exemplu: 1r 2r 3r 1r 1w 1r 2w 3r
ops_str = re.split(' ', input('Enter the operations sequence separated by spaces: '))
ops = []
for i in range(nr_ops):
    op = ops_str[i]
    ops.append((op[1], int(op[0])))

formatting = "{:<3} {:<18} {:<10} {:<10} {:<10} {:<40} {:<10}"
print(formatting.format('t', 'Actiune procesor', 'Stare P1', 'Stare P2', 'Stare P3', 'Actiuni pe magistrala', 'Sursa date'))
print(formatting.format('t0', 'initial', 'I', 'I', 'I', '-', '-'))

def proc_has_block(proc_state):
    if proc_state != 'I':
        return True
    
    return False

def get_action(proc_state, op_type):
    if proc_state == 'I':
        if op_type == 'r':
            return 'BusRd'
        else:
            return 'BusRdx'
    
    if proc_state == 'S':
        if op_type == 'r':
            return '-'
        else:
            return 'BusRdx'
    
    if proc_state == 'E' or proc_state == 'M':
        return '-'


def update_state(index_proc, proc_states, op_type, op_proc, bus_action, all_actions):
    proc_state = proc_states[index_proc]
    if index_proc == op_proc:
        all_actions.append(bus_action)
        if op_type == 'w':
            return 'M'

        if proc_state == 'I':
            if any(map(proc_has_block, proc_states)):
                return 'S'
            else:
                return 'E'
        
        return proc_state

    if bus_action == '-' or proc_state == 'I':
        return proc_state
    
    if proc_state == 'S':
        all_actions.append('Flush*')
    else:
        all_actions.append('Flush')

    if bus_action == 'BusRd':
        return 'S'
    
    # pt bus_action == 'BusRdx'
    return 'I'

def is_valid(action):
    if action == '-':
        return False
    
    return True

# scuze, dar n-am idee cum sa trimit pointer de int in python
# ma folosesc de faptul ca listele sunt pointeri
def persist_curr_source(curr_source, proc):
    if len(curr_source) == 0:
        curr_source.append(str(proc))
    else:
        curr_source[0] = str(proc)

def is_flush_star(action):
    if action == 'Flush*':
        return True

    return False

# adaug (1) ca oricum nu o sa existe copii ale blocului ca
# toate blocurile pe care le aveau intra fie in starea I sau M
def get_flush_type(all_actions):
    if any(map(is_flush_star, all_actions)):
        return 'Flush* (1)'
    
    # flush sigur a fost printre actiuni daca s-a modificat blocul
    return 'Flush (1)'

def get_data_source(all_actions, proc_state, proc, curr_source, op_type):
    if len(all_actions) == 1 and all_actions[0] == '-':
        return '-'

    if proc_state == 'E':
        persist_curr_source(curr_source, proc)
        return 'Mem'
    
    if proc_state == 'S' and op_type == 'r':
        return 'Cache' + curr_source[0]
    
    if proc_state == 'M' and op_type == 'w':
        persist_curr_source(curr_source, proc)
        return get_flush_type(all_actions)

    return 'X'

# '-' ca sa se indexeze de la 1
proc_states = ['I', 'I', 'I', 'I']
curr_source = []
for i in range(len(ops)):
    (type, proc) = ops[i]
    type_str = 'Rd'
    if type == 'w':
        type_str = 'Wr'

    bus_action = get_action(proc_states[proc], type)
    all_actions = ['-']
    proc_states = ['I'] + [update_state(index_proc, proc_states, type, proc, bus_action, all_actions) for index_proc in range(1, 4)]
    all_actions = list(filter(is_valid, all_actions))
    if len(all_actions) == 0:
        all_actions = ['-']

    data_source = get_data_source(all_actions, proc_states[proc], proc, curr_source, type)
    print (formatting.format('t' + str(i + 1), 'P' + str(proc) + type_str, proc_states[1], proc_states[2], proc_states[3], ' '.join(all_actions), data_source))