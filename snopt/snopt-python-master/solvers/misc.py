import numpy as np

date = '''Jan 2019'''

def printInfo(solver,name,m,n,nName,Names,hs,x,bl,bu,rc,header=True):
    if header:
        print(''.join('-' for j in range(82)))
        print(' {} python interface   ({})'.format(solver,date))
        print('   Problem: {}'.format(name))
        print('   # variables = {:d}; # constraints = {:d} \n'.format(n,m))

        form =  '{:>8s}{:>10s}{:>16s}{:>16s}{:>16s}{:>16s}'
        print(form.format('Name','state(j)','low(j)','x0(j)','upp(j)','mul(j)'))

    form = '{0:>8s}{1:10d}{2:16.6e}{3:16.6e}{4:16.6e}{5:16.6e}'

    if nName == 1:
        arrays = zip([repr(j) for j in range(n+m)],hs,bl,x,bu,rc)
    else:
        arrays = zip(np.char.decode(Names),hs,bl,x,bu,rc)

    for namej,hsj,xlj,xj,xuj,xmj in arrays:
        print(form.format(namej,hsj,xlj,xj,xuj,xmj))

    print(''.join('-' for j in range(82)))
