residues = 0
with open('out2.txt', 'r') as infile:
    while 1:
        line = infile.readline()
        if line == '': break
        if line.split(':') == 'Reverse':
            residues += 1
        
print residues
