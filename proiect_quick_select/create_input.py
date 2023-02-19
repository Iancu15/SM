import random

f = open("input", "w")
size = int(input('Write size: '))
s = str(size // 2) + ' ' + str(size) + ' '
list_of_numbers = list(range(size))
random.shuffle(list_of_numbers)
for j in list_of_numbers:
    s += str(j) + ' '

f.write(s)