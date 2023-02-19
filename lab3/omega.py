import sys

k = int(input("Please enter k: "))
m = int(input("Please enter m: "))
N = pow(2, k)
pairs = []
for i in range(0, m):
    print('Enter pair number', i + 1, end ='')
    pair = input(": ")
    pair = tuple(int(x) for x in pair.split(' '))
    pairs.append(pair)

def shuffle(x):
    return (2 * x + abs((2 * x) // N)) % N

def decimal_to_binary(x):
    binary = []
    while x >= 1:
        binary.append(x % 2)
        x = x // 2
    
    binary += (k - len(binary)) * [0]
    
    return list(reversed(binary))

for pair in pairs:
    print(pair)
    (src, dest) = pair
    curr_pos = src
    binary_src = decimal_to_binary(src)
    binary_dest = decimal_to_binary(dest)
    binary_index = 0
    for floor in reversed(range(0, k)):
        curr_pos = shuffle(curr_pos)
        print('Etaj', floor, ':', curr_pos, '-> bloc', curr_pos // 2, '-> ', end = '')
        if binary_src[binary_index] == binary_dest[binary_index]:
            print('conexiune directa -> ', curr_pos)
        else:
            if curr_pos % 2 == 0:
                curr_pos += 1
            else:
                curr_pos -= 1

            print('conexiune inversa -> ', curr_pos)
        
        binary_index += 1