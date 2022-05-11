import numpy as np
import scipy.sparse as sp

#-------------------------------------------------------------------------------#

class SNOPT_work(object):
    def __init__(self,lencw=500,leniw=500,lenrw=500):
        self.setup(lencw,leniw,lenrw)

    def setup(self,lencw=500,leniw=500,lenrw=500):
        self.lencw   = lencw
        self.leniw   = leniw
        self.lenrw   = lenrw
        self.cw      = np.empty((lencw,8),dtype='|S1')
        self.iw      = np.zeros(leniw,'i',order='F')
        self.rw      = np.zeros(lenrw,float,order='F')

    def work_resize(self,mincw,miniw,minrw):
        if mincw > self.lencw:
            tmp = np.copy(self.cw)
            self.cw = np.empty((mincw,8),dtype='|S1')
            self.cw[0:self.lencw] = tmp[0:self.lencw]
            del tmp

            self.lencw = mincw
            self.iw[6] = mincw

        if miniw > self.leniw:
            tmp = self.iw
            self.iw = np.zeros(miniw,'i',order='F')
            self.iw[0:self.leniw] = tmp[0:self.leniw]
            del tmp

            self.leniw = miniw
            self.iw[4] = miniw

        if minrw > self.lenrw:
            tmp = self.rw
            self.rw = np.zeros(minrw,float,order='F')
            self.rw[0:self.lenrw] = tmp[0:self.lenrw]
            del tmp

            self.lenrw = minrw
            self.iw[2] = minrw


