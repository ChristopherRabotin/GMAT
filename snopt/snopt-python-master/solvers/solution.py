import os, sys
import numpy as np

#-------------------------------------------------------------------------------#
class SQIC_solution(object):
    '''
    SQIC_solution class:
    '''
    def __init__ (self,name=''):
        self.setup(name)

    def setup(self,name):
        self.name       = name
        self.x          = None
        self.basis      = None
        self.z          = None
        self.bl         = None
        self.bu         = None
        self.info       = 0
        self.iterations = 0
        self.numinf     = 0
        self.suminf     = 0.
        self.objective  = 0.

    def __str__(self):
        text = ''.join('-' for j in range(82)) + '\n'
        text+= ' SQIC Results for problem '  + self.name + '\n'
        text+= '   EXIT code          = ' + repr(self.info) + '\n'
        text+= '   Final objective    = ' + repr(self.objective) + '\n'
        text+= '   Total # iterations = ' + repr(self.iterations) + '\n'

        try:
            soln = '     state(j)      low(j)           x(j)           upp(j)        mul(j)\n'
            form = ' {0:8d}{1:16.6e}{2:16.6e}{3:16.6e}{4:16.6e}'
            for hsj,blj,xj,buj,zj in np.nditer([self.basis,self.bl,self.x,self.bu,self.z]):
                soln+= form.format(hsj,blj,xj,buj,zj) + '\n'
            soln+= ''.join('-' for j in range(82))
            text+= soln
        except:
            pass
        return text

#-------------------------------------------------------------------------------#
class SNOPTA_solution(object):
    '''
    SNOPTA_solution class:
    '''
    def __init__ (self,name='',xnames=None,Fnames=None):
        self.setup(name,xnames,Fnames)

    def setup(self,name,xnames,Fnames):
        self.name    = name
        self.xnames  = xnames
        self.Fnames  = Fnames

        self.x       = None
        self.xstates = None
        self.xmul    = None
        self.F       = None
        self.Fstates = None
        self.Fmul    = None
        self.info    = 0

        self.iterations = 0
        self.major_itns = 0
        self.nS         = 0
        self.num_inf    = 0
        self.sum_inf    = 0.
        self.objective  = 0.

    def __str__(self):
        text = ''.join('-' for j in range(82)) + '\n'
        text+= ' SNOPTA Results for problem '  + self.name + '\n'
        text+= '   EXIT code          = ' + repr(self.info) + '\n'
        text+= '   Final objective    = ' + repr(self.objective) + '\n'
        text+= '   Total # iterations = ' + repr(self.iterations) + '\n'

        form = '{:>8s}{:>10s}{:>16s}{:>16s}'
        text+= form.format('Name','xstate(j)','x(j)','xmul(j)') + '\n'

        form = '{0:>8s}{1:10d}{2:16.6e}{3:16.6e}'

        n = self.x.size
        if len(self.xnames) == 1:
            arrays = zip([repr(j) for j in range(n)],self.xstates,self.x,self.xmul)
        else:
            arrays = zip(np.char.decode(self.xnames),self.xstates,self.x,self.xmul)

        for namej,hsj,xj,xmj in arrays:
            text+= form.format(namej,hsj,xj,xmj) + '\n'


        form = '{:>8s}{:>10s}{:>16s}{:>16s}'
        text+= form.format('Name','Fstate(j)','F(j)','Fmul(j)') + '\n'

        form = '{0:>8s}{1:10d}{2:16.6e}{3:16.6e}'

        n = self.F.size
        if len(self.Fnames) == 1:
            arrays = zip([repr(j) for j in range(n)],self.Fstates,self.F,self.Fmul)
        else:
            arrays = zip(np.char.decode(self.Fnames),self.Fstates,self.F,self.Fmul)

        for namej,hsj,xj,xmj in arrays:
            text+= form.format(namej,hsj,xj,xmj) + '\n'

        return text

#-------------------------------------------------------------------------------#
class SNOPT_solution(object):
    '''
    SNOPT_solution class:
    '''
    def __init__ (self,name='',Names=None):
        self.setup(name,Names)

    def setup(self,name,Names):
        self.name   = name
        self.Names  = Names
        self.states = None
        self.x      = None
        self.pi     = None
        self.rc     = None
        self.info   = 0

        self.iterations = 0
        self.major_itns = 0
        self.nS         = 0
        self.num_inf    = 0
        self.sum_inf    = 0.
        self.objective  = 0.

    def __str__(self):
        text = ''.join('-' for j in range(82)) + '\n'
        text+= ' SNOPT Results for problem '  + self.name + '\n'
        text+= '   EXIT code          = ' + repr(self.info) + '\n'
        text+= '   Final objective    = ' + repr(self.objective) + '\n'
        text+= '   Total # iterations = ' + repr(self.iterations) + '\n'

        form = '{:>8s}{:>10s}{:>16s}{:>16s}'
        text+= form.format('Name','state(j)','x(j)','mul(j)') + '\n'

        form = '{0:>8s}{1:10d}{2:16.6e}{3:16.6e}'
        if len(self.Names) == 1:
            n = self.x.size
            arrays = zip([repr(j) for j in range(n)],self.states,self.x,self.rc)
        else:
            arrays = zip(np.char.decode(self.Names),self.states,self.x,self.rc)

        for namej,hsj,xj,xmj in arrays:
            text+= form.format(namej,hsj,xj,xmj) + '\n'

        return text

#-------------------------------------------------------------------------------#
class DNOPT_solution(object):
    '''
    DNOPT_solution class:
    '''
    def __init__ (self,name='',Names=None):
        self.setup(name,Names)

    def setup(self,name,Names):
        self.name   = name
        self.Names  = Names
        self.states = None
        self.x      = None
        self.y      = None
        self.info   = 0

        self.iterations = 0
        self.major_itns = 0
        self.num_inf    = 0
        self.sum_inf    = 0.
        self.objective  = 0.

        self.f    = None
        self.gObj = None
        self.fCon = None
        self.J    = None
        self.H    = None

    def __str__(self):
        text = ''.join('-' for j in range(82)) + '\n'
        text+= ' DNOPT Results for problem '  + self.name + '\n'
        text+= '   EXIT code          = ' + repr(self.info) + '\n'
        text+= '   Final objective    = ' + repr(self.objective) + '\n'
        text+= '   Total # iterations = ' + repr(self.iterations) + '\n'

        form = '{:>8s}{:>10s}{:>16s}{:>16s}'
        text+= form.format('Name','state(j)','x(j)','mul(j)') + '\n'

        form = '{0:>8s}{1:10d}{2:16.6e}{3:16.6e}'
        if len(self.Names) == 1:
            n = self.x.size
            arrays = zip([repr(j) for j in range(n)],self.states,self.x,self.y)
        else:
            arrays = zip(np.char.decode(self.Names),self.states,self.x,self.y)

        for namej,hsj,xj,xmj in arrays:
            text+= form.format(namej,hsj,xj,xmj) + '\n'

        return text
