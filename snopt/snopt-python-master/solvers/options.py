from  snopt.solvers import snopt7_python as fsnopt

#-------------------------------------------------------------------------------#

def copyOpts(verbose,usropts,work):
    # Copy options to workspace
    info = 0

    keywords = sorted(usropts.options.keys())
    for key in keywords:
        if key not in ['Verbose',
                       'Start type',
                       'Specs filename',
                       'Print filename',
                       'Summary',
                       'Max memory attempts']:
            if usropts.options[key][0] is not None:
                if usropts.options[key][0] is bool:
                    optstr = key + ' ' + 'yes' if usropts.options[key][0] is True else 'no'
                else:
                    optstr = key + ' ' + str(usropts.options[key][0])
                if verbose:
                    print('  Setting option: ' + optstr)

                if type(usropts) is SNOPT_options:
                    info = fsnopt.copyoptions(optstr,work.cw,work.iw,work.rw)
                elif type(usropts) is DNOPT_options:
                    info = fdnopt.copyoptions(optstr,work.cw,work.iw,work.rw)
    return info

#-------------------------------------------------------------------------------#

class OptionsClass(object):
    '''
    Options Class
    '''
    def __init__ (self):
        self.setup()
        self.solverName = 'None'

    def setup(self):
        self.options = None

    def __str__(self):
        text = '\n' + self.solverName + ' options \n'
        keywords = self.options.keys()
        keywords.sort()
        for key in keywords:
            value = self.options[key][0]
            if value is None:
                text += ' ' + str(key) + ': undefined\n'
            else:
                text += ' ' + str(key) + ': ' + str(self.options[key][0]) + '\n'
        text += '\n' + self.solverName + \
                ' will set any parameters that are undefined to defaults.\n' + \
                'Please refer to ' + self.solverName + ' documentation for details.\n'
        return text

    def printOptions(self):
        print(self.__repr__())

    def setOption(self,name,value):
        try:
            if type(value) is self.options[name][2]:
                self.options[name][0] = value
        except:
            raise RuntimeError('Incorrect option keyword or type: ' + name)

    def getOption(self,name):
        try:
            value = self.options[name][0]
            return value
        except:
            raise RuntimeError('Incorrect option keyword: ' + name)

    def resetOption(self,name):
        try:
            self.options[name] = self.options[name][1]
        except:
            raise RuntimeError('Incorrect option keyword: ' + name)

#-------------------------------------------------------------------------------#

class SQIC_options(OptionsClass):
    '''
    SQIC_options class:
    '''
    def __init__ (self):
        self.setup()
        self.solverName = 'SQIC'

    def setup(self):
        self.options = {
            # [Current value, default value, type]
            'Start type'            : ['Cold','Cold',str],

            'Specs filename'        : [None,None,str],

            'Print filename'        : ['SQIC.out','SQIC.out',str],
            'Print frequency'       : [None,None,int],
            'Print level'           : [None,None,int], # minor print level

            'Summary'               : ['yes','yes',str],
            'Summary frequency'     : [None,None,int],

            'Solution file'         : [None,None,int],
            'Solution print'        : [None,None,bool],
            'Minor print level'     : [None,None,int],

            'Sticky parameters'     : [None,None,int],
            'Suppress'              : [None,None,int],
            'Time limit'            : [None,None,float],
            'Timing level'          : [None,None,int],
            'System information'    : [None,None,int],
            'Verify level'          : [None,None,int],

            #'Problem minmax'        : ['Minimize','Minimize',str],
            'Proximal point'        : [None,None,int],
            #'QP solver'             : [None,None,str],    # Cholesky/CG/QN

            'Minor phase1'          : [None,None,float],  #tolOptFP
            'Feasibility tolerance' : [None,None,float],  #tolx
            'Optimality tolerance'  : [None,None,float],  #tolOptQP

            'Iteration limit'       : [None,None,int],  #itnlim

            'CG tolerance'          : [None,None,float],
            'CG preconditioning'    : [None,None,int],
            'CG iterations'         : [None,None,int],

            'Crash option'          : [None,None,int],
            'Crash tolerance'       : [None,None,float],

            'Debug level'           : [None,None,int],

            'Derivative level'      : [None,None,int],
            'Derivative linesearch' : [None,None,int],
            'Derivative option'     : [None,None,int],

            'Elastic objective'     : [None,None,int],
            'Elastic mode'          : [None,None,int],
            'Elastic weight'        : [None,None,float],
            'Elastic weightmax'     : [None,None,float],

            'Hessian frequency'     : [None,None,int],
            'Hessian flush'         : [None,None,int],
            'Hessian type'          : [None,None,int],
            'Hessian updates'       : [None,None,int],

            'Infinite bound'        : [1.0e+20,1.0e+20,float],
            'Major step limit'      : [None,None,float],
            'Unbounded objective'   : [None,None,float],
            'Unbounded step'        : [None,None,float],

            #'LU type'               : [None,None,str],   #partial/complete/rook
            'LU swap'               : [None,None,float],
            'LU factor tolerance'   : [None,None,float],
            'LU update tolerance'   : [None,None,float],
            'LU density'            : [None,None,float],
            'LU singularity'        : [None,None,float],

            'New superbasics'       : [None,None,int],
            'Partial pricing'       : [None,None,int],
            'Penalty parameter'     : [None,None,float],
            'Pivot tolerance'       : [None,None,float],
            'Reduced Hessian limit' : [None,None,int],
            'Superbasics limit'     : [None,None,int],

            'Scale option'          : [None,None,int],
            'Scale tolerance'       : [None,None,float],
            'Scale print'           : [None,None,int],

            'Verbose'               : [False,False,bool]  # python verbose
        }

#-------------------------------------------------------------------------------#

class SNOPT_options(OptionsClass):
    '''
    SNOPT_options class:
    '''

    def __init__ (self):
        self.setup()
        self.solverName = 'SNOPT'

    def setup(self):
        self.options = {
            # [Current value, default value, type]
            'Start type'            : ['Cold','Cold',str],  ##

            'Specs filename'        : [None,None,str],  ##

            'Print filename'        : ['SNOPT.out','SNOPT.out',str],  ##
            'Print frequency'       : [None,None,int],
            'Print level'           : [None,None,int], # minor print level

            'Summary'               : ['yes','yes',str],
            'Summary frequency'     : [None,None,int],

            'Solution file'         : [None,None,int],
            'Solution print'        : [None,None,bool],
            'Major print level'     : [None,None,int],
            'Minor print level'     : [None,None,int],

            'Sticky parameters'     : [None,None,int],
            'Suppress'              : [None,None,int],
            'Time limit'            : [None,None,float],
            'Timing level'          : [None,None,int],
            'System information'    : [None,None,int],
            'Verify level'          : [None,None,int],

            'Max memory attempts'   : [10,10,int],

            'Total character workspace' : [None,None,int],
            'Total integer workspace'   : [None,None,int],
            'Total real workspace'      : [None,None,int],

            #'Problem minmax'        : ['Minimize','Minimize',str],
            'Proximal point'        : [None,None,int],
            #'QP solver'             : [None,None,str],    # Cholesky/CG/QN

            'Major feasibility'     : [None,None,float],  #tolCon
            'Major optimality'      : [None,None,float],  #tolOptNP
            'Minor feasibility'     : [None,None,float],  #tolx
            'Minor optimality'      : [None,None,float],  #tolOptQP
            'Minor phase1'          : [None,None,float],  #tolOptFP
            'Feasibility tolerance' : [None,None,float],  #tolx
            'Optimality tolerance'  : [None,None,float],  #tolOptQP

            'Iteration limit'       : [None,None,int],  #itnlim
            'Major iterations'      : [None,None,int],  #mMajor
            'Minor iterations'      : [None,None,int],  #mMinor

            'CG tolerance'          : [None,None,float],
            'CG preconditioning'    : [None,None,int],
            'CG iterations'         : [None,None,int],

            'Crash option'          : [None,None,int],
            'Crash tolerance'       : [None,None,float],

            'Debug level'           : [None,None,int],

            'Derivative level'      : [None,None,int],
            'Derivative linesearch' : [None,None,int],
            'Derivative option'     : [None,None,int],

            'Elastic objective'     : [None,None,int],
            'Elastic mode'          : [None,None,int],
            'Elastic weight'        : [None,None,float],
            'Elastic weightmax'     : [None,None,float],

            'Hessian frequency'     : [None,None,int],
            'Hessian flush'         : [None,None,int],
            'Hessian type'          : [None,None,int],
            'Hessian updates'       : [None,None,int],

            'Infinite bound'        : [1.0e+20,1.0e+20,float],
            'Major step limit'      : [None,None,float],
            'Unbounded objective'   : [None,None,float],
            'Unbounded step'        : [None,None,float],

            'Linesearch tolerance'  : [None,None,float],
            'Linesearch debug'      : [None,None,int],

            #'LU type'               : [None,None,str],   #partial/complete/rook
            'LU swap'               : [None,None,float],
            'LU factor tolerance'   : [None,None,float],
            'LU update tolerance'   : [None,None,float],
            'LU density'            : [None,None,float],
            'LU singularity'        : [None,None,float],

            'New superbasics'       : [None,None,int],
            'Partial pricing'       : [None,None,int],
            'Penalty parameter'     : [None,None,float],
            'Pivot tolerance'       : [None,None,float],
            'Reduced Hessian limit' : [None,None,int],
            'Superbasics limit'     : [None,None,int],

            'Scale option'          : [None,None,int],
            'Scale tolerance'       : [None,None,float],
            'Scale print'           : [None,None,int],

            'Verbose'               : [False,False,bool]  ##
        }

#-------------------------------------------------------------------------------#

class DNOPT_options(OptionsClass):
    '''
    DNOPT_options class:
    '''

    def __init__ (self):
        self.setup()
        self.solverName = 'DNOPT'

    def setup(self):
        self.options = {
            # [Current value, default value, type]
            'Start type'            : ['Cold','Cold',str],  ##

            'Specs filename'        : [None,None,str],  ##

            'Print filename'        : ['SNOPT.out','SNOPT.out',str],  ##
            'Print frequency'       : [None,None,int],
            'Print level'           : [None,None,int], # minor print level

            'Summary'               : ['yes','yes',str],
            'Summary frequency'     : [None,None,int],

            'Solution file'         : [None,None,int],
            'Solution print'        : [None,None,bool],
            'Major print level'     : [None,None,int],
            'Minor print level'     : [None,None,int],

            'Sticky parameters'     : [None,None,int],
            'Suppress'              : [None,None,int],
            'Time limit'            : [None,None,float],
            'Timing level'          : [None,None,int],
            'System information'    : [None,None,int],
            'Verify level'          : [None,None,int],

            'Max memory attempts'   : [10,10,int],

            'Total character workspace' : [None,None,int],
            'Total integer workspace'   : [None,None,int],
            'Total real workspace'      : [None,None,int],

            #'Problem minmax'        : ['Minimize','Minimize',str],
            'Proximal point'        : [None,None,int],
            #'QP solver'             : [None,None,str],    # Cholesky/CG/QN

            'Major feasibility'     : [None,None,float],  #tolCon
            'Major optimality'      : [None,None,float],  #tolOptNP
            'Minor feasibility'     : [None,None,float],  #tolx
            'Minor optimality'      : [None,None,float],  #tolOptQP
            'Minor phase1'          : [None,None,float],  #tolOptFP
            'Feasibility tolerance' : [None,None,float],  #tolx
            'Optimality tolerance'  : [None,None,float],  #tolOptQP

            'Iteration limit'       : [None,None,int],  #itnlim
            'Major iterations'      : [None,None,int],  #mMajor
            'Minor iterations'      : [None,None,int],  #mMinor

            'CG tolerance'          : [None,None,float],
            'CG preconditioning'    : [None,None,int],
            'CG iterations'         : [None,None,int],

            'Crash option'          : [None,None,int],
            'Crash tolerance'       : [None,None,float],

            'Debug level'           : [None,None,int],

            'Derivative level'      : [None,None,int],
            'Derivative linesearch' : [None,None,int],
            'Derivative option'     : [None,None,int],

            'Elastic objective'     : [None,None,int],
            'Elastic mode'          : [None,None,int],
            'Elastic weight'        : [None,None,float],
            'Elastic weightmax'     : [None,None,float],

            'Hessian frequency'     : [None,None,int],
            'Hessian flush'         : [None,None,int],
            'Hessian type'          : [None,None,int],
            'Hessian updates'       : [None,None,int],

            'Infinite bound'        : [1.0e+20,1.0e+20,float],
            'Major step limit'      : [None,None,float],
            'Unbounded objective'   : [None,None,float],
            'Unbounded step'        : [None,None,float],

            'Linesearch tolerance'  : [None,None,float],
            'Linesearch debug'      : [None,None,int],

            #'LU type'               : [None,None,str],   #partial/complete/rook
            'LU swap'               : [None,None,float],
            'LU factor tolerance'   : [None,None,float],
            'LU update tolerance'   : [None,None,float],
            'LU density'            : [None,None,float],
            'LU singularity'        : [None,None,float],

            'Partial pricing'       : [None,None,int],
            'Penalty parameter'     : [None,None,float],
            'Pivot tolerance'       : [None,None,float],
            'Reduced Hessian limit' : [None,None,int],

            'Scale option'          : [None,None,int],
            'Scale tolerance'       : [None,None,float],
            'Scale print'           : [None,None,int],

            'Verbose'               : [False,False,bool]  ##
        }
