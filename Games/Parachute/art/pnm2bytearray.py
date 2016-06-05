import sys
filename = sys.argv[1];

with open(filename) as f:
    f.readline()
    f.readline()
    w,h = map(int, f.readline().split(" "))
    print(w,h)
    f.readline()
    byte = ['0']*8
    for i in range(w*h):
        line = f.readline().rstrip()
        if line == '0': byte[i%8] = '1'
        else: byte[i%8] = '0'
        if i%8 == 7:
            print 'B'+''.join(byte)+','
    r = w*h % 8 
    if r > 0:
        for j in range(8-r, 8): byte[j] = '0'
        print 'B'+''.join(byte)
    print w*h / 8.0
