import numpy as np
import scipy.sparse as sp

from   optimize.solvers          import dnopt_python as fdnopt
from   optimize.solvers.options  import DNOPT_options, copyOpts
from   optimize.solvers.solution import DNOPT_solution
from   optimize.solvers.misc     import printInfo
from   optimize.solvers.work     import SNOPT_work

#-------------------------------------------------------------------------------#

def dnopt(funobj,funcon,nnObj,nnJac,x0,H,A=None,J=None,**kwargs):
    """
    dnopt calls the solver DNOPT to solve the optimization
    problem:
              min       f_0(x)
              s.t.       [  x ]
                   bl <= [    ] <= bu
                         [c(x)]

    where f(x) is a vector of smooth nonlinear constraint functions, f_0(x)
    is a smooth scalar objective function, c(x) are the nonlinear and linear
    constraints, defined such that nonlinear constraints come first.

    --------------------------------------------
    Input arguments:
    --------------------------------------------
    Keyword    Description
    ----------------------
    funobj     is a user-defined function that computes the objective and its
               gradient (required)
    funcon     is a user-defined function that computes the constraints and the
               Jacobian (required; set to None if no constraints)
    funhes     is a user-defined function that computes the Hessian of the
               Lagrangian (optional; only provide this if you want to use
                           the 2nd derivative version of DNOPT)

    nnObj      is the number of nonlinear objective variables (nnObj >= 0)
    nnJac      is the number of nonlinear Jacobian variables

    H          is the Hessian of the Lagrangian
               - H can be a numpy 2-dimensional array

    A          is the linear constraint Jacobian
               - If the problem is unconstrained orbound-constrained, A = None
               - A can be a numpy 2-dimensional array

    J          is the nonlinear constraint Jacobian
               - If the problem is unconstrained orbound-constrained, J = None
               - J can be a numpy 2-dimensional array

    iObj       indicates which row of A is the free row containing the linear
               objective vector (default 0)
    ObjAdd     is the constant term of the objective (default 0.0)

    states     are the initial states of the variables (default 0)
    x0         are the initial values of x (default 0.0)
    bl         are the lower bounds of the problem (default -infinity)
    bu         are the upper bounds of the problem (default +infinity)
    y          are the initial multipliers of the variables and constraints (default 0.0)
    names      is an array of names for the variables and constraints (default '')

    name       is the name of the problem (default '')

    options    contains the user options for the SNOPT solver

    dnopt will try to compute the number of linear and nonlinear constraints,
    and the number of  variables from the given data.

    If it doesn't do it correctly, provide dnopt with the following info:
      mLcon    is the number of linear constraints  (mLcon >= 0)
      mNcon    is the number of nonlinear constraints  (mNcon >= 0)
      n        is the number of variables  (n > 0)

    """

    name    = kwargs.get('name','')
    usropts = kwargs.get('options',DNOPT_options())

    verbose  = usropts.getOption('Verbose')
    maxTries = usropts.getOption('Max memory attempts')
    inf      = usropts.getOption('Infinite bound')

    mNcon    = kwargs.get('mNcon',0)
    mLcon    = kwargs.get('mLcon',0)
    n        = kwargs.get('n',0)

    if A is not None:
        if type(A) is np.ndarray and A.ndim == 2:
            mLcon = A.shape[0] if mLcon is None else mLcon
        else:
            raise TypeError('Type of A is unsupported')
        assert A.shape == (mLcon,n)

    if J is not None:
        if type(J) is np.ndarray and J.ndim == 2:
            mNcon = J.shape[0] if mNcon is None else mNcon
        else:
            raise TypeError('Type of A is unsupported')
        assert J.shape == (mNcon,n)

    assert mLcon >= 0
    assert mNcon >= 0
    m = mLcon + mNcon

    if type(H) is np.ndarray and H.ndim == 2:
        print(H.shape)
        n = H.shape[0] if n == 0 else n
    else:
        raise TypeError('Type of A is unsupported')

    assert n > 0
    assert H.shape == (n,n)

    if verbose:
        print('There are {} linear constraints, {} nonlinear constraints, and {} variables'.format(mLcon,mNcon,n))


    iObj   = kwargs.get('iObj',0)
    ObjAdd = kwargs.get('ObjAdd',0.0)

    states = kwargs.get('states', np.zeros(n+m,int))
    bl     = kwargs.get('bl',-inf*np.ones(n+m,float))
    bu     = kwargs.get('bu', inf*np.ones(n+m,float))
    y      = kwargs.get('y',np.zeros(n+m,float))
    Names  = kwargs.get('names',np.empty((1,8),dtype='|S1'))

    assert states.shape == (n+m,)
    assert bl.shape == (n+m,)
    assert bu.shape == (n+m,)
    assert x0.shape == (n+m,)
    assert y.shape  == (n+m,)


    # Deal with Names:
    nName = len(Names)
    assert Names.dtype.char == 'S'
    assert nName == 1 or nName == n+m
    assert Names.shape == (nName,) or Names.shape == (nName,8)


    if verbose:
        printInfo('DNOPT',name,mLcon+mNcon,n,nName,Names,states,x0,bl,bu,y)


    # Set up workspace
    dnwork  = SNOPT_work(505,5000,5000)
    usrwork = SNOPT_work(1,1,1)

    # Initialize DNOPT workspace
    # parameters are set to undefined values
    if name == '':
        prtfile = usropts.getOption('Print filename')
    else:
        prtfile = name.strip() + '.out'
    prtlen  = len(prtfile)
    summOn  = 1 if (usropts.getOption('Summary')).lower() == "yes" else 0
    fdnopt.dninit_wrap(prtfile,prtlen,summOn,dnwork.cw,dnwork.iw,dnwork.rw)


    # Copy options to DNOPT workspace
    info = copyOpts(verbose,usropts,dnwork)


    # Read specs file if one was given
    info = 0
    spcfile = usropts.getOption('Specs filename')
    if spcfile is not None:
        info = fdnopt.dnspec_wrap(spcfile,dnwork.cw,dnwork.iw,dnwork.rw)
        if info != 101 and info != 104:
            print('Specs read failed: INFO = {:d}'.format(info))
            return info

    # Check memory
    info,mincw,miniw,minrw = fdnopt.dnmem_wrap(mLcon,mNcon,n,nnJac,nnObj,iObj,
                                               dnwork.cw,dnwork.iw,dnwork.rw)
    dnwork.work_resize(mincw,miniw,minrw)


    # Solve problem
    Start = usropts.getOption('Start type')
    iStart = 0
    if   Start == 'Warm':
        iStart = 1
    elif Start == 'Hot':
        iStart = 2

    if Names.shape != (1,8):
        dnNames = Names.view('S1').reshape((Names.size,-1))
    else:
        dnNames = Names


    result = DNOPT_solution(name,Names)
    count = 1
    while True:
        if funcon is None:
            result.states, \
            result.f, \
            result.gObj, \
            result.H, \
            result.objective, \
            result.num_inf, \
            result.sum_inf, \
            result.x, \
            result.y, \
            result.info, \
            result.iterations, \
            result.major_itns, \
            mincw, \
            miniw, \
            minrw = fdnopt.dnopt_uncon_wrap(iStart, nnJac, nnObj,
                                            name, dnNames, iObj, ObjAdd,
                                            funobj, states, bl, bu, H, x0, y,
                                            usrwork.cw, usrwork.iw, usrwork.rw,
                                            dnwork.cw, dnwork.iw, dnwork.rw)
        else:
            result.states, \
            result.f, \
            result.gObj, \
            result.fCon, \
            result.J, \
            result.H, \
            result.objective, \
            result.num_inf, \
            result.sum_inf, \
            result.x, \
            result.y, \
            result.info, \
            result.iterations, \
            result.major_itns, \
            mincw, \
            miniw, \
            minrw = fdnopt.dnopt_wrap(iStart, mLcon, mNcon, nnJac, nnObj,
                                      name, dnNames, iObj, ObjAdd,
                                      funcon, funobj,
                                      states, A, bl, bu, J, H, x0, y,
                                      usrwork.cw, usrwork.iw, usrwork.rw,
                                      dnwork.cw, dnwork.iw, dnwork.rw)
        if  result.info/10 == 8:
            count += 1
            if count > maxTries:
                print(' Could not allocate memory for DNOPT')
                return info
            dnwork.work_resize(mincw, miniw, minrw)
        else:
            break

    # Finish up
    fdnopt.dnend_wrap(dnwork.iw)

    # Print solution?
    if verbose:
        print(result)

    return result


#-------------------------------------------------------------------------------#

def qpHx(H,x,state):
    return np.dot(H,x)


def dqopt(H,x0,**kwargs):
    """ dqopt solves the quadratic optimization problem:
          min    f + c'x + half*x'Hx
          s.t.   xl <=   x  <= xu
          s.t.   al <=  Ax  <= au

    --------------------------------------------
    Input arguments:
    --------------------------------------------
    Keyword    Description
    ----------------------
    H          is the Hessian of the quadratic objective and should be a
               numpy 2-dimensional matrix

    x0         are the intial values of x (required)

    name       is the name of the problem (default '')

    xl, xu     are the lower and upper bounds of x (default -/+ infinity)

    A          is the linear constraint matrix (if constraints exist)
               - A can be a tuple (valA,indA,locA) containing the sparse structure
                 of the matrix

               - A can be a scipy csc_matrix

               - A can be a numpy 2-dimensional array

    al, au     are the lower and upper bounds of the linear constraints
                (default -/+ infinity)

    f          is the constant term of the quadratic objective (default 0.0)
    c          is the linear term of the quadratic objective (default 0.0)

    states     is a (n+m)-vector denoting the initial states for the problem
               (default 0)

    eType      is a (n+m)-vector that defines which variables are to be treated
               as being elastic in elastic mode (default 0)

    options    contains the user options for the SQOPT solver

    """

    name     = kwargs.get('name','')
    usropts  = kwargs.get('options',SNOPT_options())

    verbose  = usropts.getOption('Verbose')
    inf      = usropts.getOption('Infinite bound')

    m        = kwargs.get('m',0)
    n        = kwargs.get('n',0)
    A        = kwargs.get('A',None)

    # Linear constraint matrix
    if A is None:
        m    = 0
        n    = x0.shape[0]
        A    = np.zeros(1,n)

    else:
        if type(A) is np.ndarray and A.ndim == 2:
            m, n = A.shape
        else:
            raise TypeError('Type of A is unsupported')


    # Hessian matrix
    if type(H) is np.ndarray and H.ndim == 2:
        nnH = H.shape[0]
    else:
        raise TypeError('Error with callable H')


    f      = kwargs.get('f',0.0)
    c      = kwargs.get('c',np.zeros(1))
    xl     = kwargs.get('xl',-inf*np.ones(n,float))
    xu     = kwargs.get('xu', inf*np.ones(n,float))
    if m > 0:
        al = kwargs.get('al',-inf*np.ones(m,float))
        au = kwargs.get('au', inf*np.ones(m,float))
        try:
            bl = np.concatenate([xl,al])
            bu = np.concatenate([xu,au])
        except:
            raise InputError('Check the bounds of the problem')
    else:
        bl = xl
        bu = xu
    states = kwargs.get('states',np.zeros(n+m,'i'))
    eType  = kwargs.get('eType',np.zeros(n+m,'i'))
    Names  = kwargs.get('names',np.empty((1,8),dtype='|S1'))
    y      = np.zeros(n+m,float)

    if m > 0:
        x = np.concatenate([x0,np.zeros(m)])
    else:
        x = x0

    assert c.size   <= n
    assert bl.shape == (n+m,)
    assert bu.shape == (n+m,)
    assert states.shape == (n+m,)


    # Deal with names
    nName = len(Names)
    assert Names.dtype.char == 'S'
    assert nName == 1 or nName == n+m
    assert Names.shape == (nName,) or Names.shape == (nName,8)

    if verbose:
        printInfo('DQOPT',name,m,n,nName,Names,states,x,bl,bu,y)


    # Set up workspace
    snwork  = SNOPT_work(505,5000,5000)
    usrwork = SNOPT_work(1,1,1)


    # Initialize SQOPT
    if name == '':
        prtfile = usropts.getOption('Print filename')
    else:
        prtfile = name.strip() + '.out'
    prtlen  = len(prtfile)
    summOn  = 1 if (usropts.getOption('Summary')).lower() == "yes" else 0
    fdnopt.dqinit_wrap(prtfile,prtlen,summOn,snwork.cw,snwork.iw,snwork.rw)


    # Copy options to SQIC
    info = copyOpts(verbose,usropts,snwork)


    # Read specs file if one was given
    info = 0
    spcfile = usropts.getOption('Specs filename')
    if spcfile is not None:
        info = fdnopt.dqspec_wrap(spcfile,snwork.cw,snwork.iw,snwork.rw)
        if info != 101 and info != 104:
            print('Specs read failed: INFO = {:d}'.format(info))
            return info


    # Solve the QP
    Start  = usropts.getOption('Start type')
    iObj   = 0

    if Names.shape != (1,8):
        dnNames = Names.view('S1').reshape((Names.size,-1))
    else:
        dnNames = Names

    result = DNOPT_solution(name)

    result.states,
    result.x,
    result.y,
    result.info,
    result.iterations,
    result.objective,
    result.num_inf,
    result.sum_inf = fdnopt.dqopt_wrap(Start, n, m, nnH, dnNames, nName, iObj, f,
                                       name, A, bl, bu, c, H, qpHx,
                                       eType, state, x, y,
                                       usrwork.cw,usrwork.iw,usrwork.rw,
                                       snwork.cw,snwork.iw,snwork.rw)

    # Finish up
    fsnopt.dnend_wrap(snwork.iw)

    # Print solution?
    if verbose:
        print(result)

    # Return solution
    return result
