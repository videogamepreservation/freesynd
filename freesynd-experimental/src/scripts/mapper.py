
from time import sleep
from struct import unpack, pack, calcsize
from array  import *

s = ', .`!"#$%&\'()*+-/0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~¨£½¾ø'

f = file('maps/map01.dat')
xs,ys,zs = unpack('<LLL', f.read(12))

m = f.read(xs*ys*calcsize('<L'))

offtbl = [ unpack('<L', m[(i)*4:(i+1)*4])[0] for i in range(xs*ys) ]
m = ''

for k in range(xs):
    for j in range(ys):
        f.seek(int(offtbl[j*xs+k]))
        m = m + f.read(int(zs))


m = array('B', m)

f=[0]*256
for i in range(len(m)): f[m[i]] += 1

f = [ i[0] for i in list(enumerate(f)) if i[1] ]
f.sort()

s = ', .`!"#$%&\'()*+-/0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~'+30*'F'
c = dict(zip(f, s))

l = [ [''.join(map(lambda x: c[x],
                   [ m[zs*ys*i+zs*j+k] for i in range(xs) ] ))
       for j in range(ys) ]
      for k in range(zs) ]
del m

print len(l),len(l[0]),len(l[0][0])

def lvl(j):
    for i in l[j]: print i

def show():
    for j in range(len(l)):
        for i in l[j]: print i
        sleep(0.5)

show()

def tofile(f, m):
    xs, ys, zs = len(m), len(m[0]), len(m[0][0])
    d, o = {}, xs*ys
    for y in range(ys):
        for x in range(xs):
            d[m[x][y]]
    f.write(pack('<LLL', xs, ys, zs))
            
