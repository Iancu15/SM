from random import randint

f = open("input", "w")
size = int(input('Write size: '))
s = str(size) + '\n'
for _ in range(size):
    for _ in range(size):
        s += str(randint(-100, 100)) + ' '

    s += '\n'

f.write(s)