import numpy as np
import scipy.sparse as sp

from   snopt.solvers          import snopt7_python as fsnopt
from   snopt.solvers.options  import SNOPT_options, copyOpts
from   snopt.solvers.solution import SNOPTA_solution, SNOPT_solution
from   snopt.solvers.misc     import printInfo
from   snopt.solvers.work     import SNOPT_work

#-------------------------------------------------------------------------------#

def snopta(usrfun,n,nF,**kwargs):
    """
    snopta() calls the SNOPTA solver to solve the optimization
    problem:
    min        F_objrow(x)
    s.t.   xlow <=   x  <= xupp
           Flow <= F(x) <= Fupp

    where F(x) is a vector of smooth linear and nonlinear constraint functions
    and F_objrow(x) is one of the components of F to be minimized, as specified by input
    argument ObjRow.

    The user should provide a function that returns the function F(x) and ideally
    their gradients.

    The problem is assumed to have n variables and nF constraint functions.

    --------------------------------------------
    Input arguments:
    --------------------------------------------
    Keyword    Description
    ----------------------
    usrfun     is the user-defined function that returns the objective, constraints
               and gradients (required)

    n          is the number of variables in the problem (n > 0) (required)

    nF         is the number of constraints in the problem (nF > 0) (required)

    name       is the name of the problem (default '')

    x0         are the intial values of x (default 0.0)
    xlow       are the lower bounds of x (default -infinity)
    xupp       are the upper bounds of x (default +infinity)
    xstates    contains the initial state of the x-variables (default 0)
    xmul       contains the initial multipliers of the x-variables (default 0.0)
    xnames     is an array of names for the x-variables (default '')

    F0         are the intial values of F (default 0.0)
    Flow       are the lower bounds of F (default -infinity)
    Fupp       are the upper bounds of F (default +infinity)
    Fstates    contains the initial state of the F-variables (default 0)
    Fmul       contains the initial multipliers of the F-variables (default 0.0)
    Fnames     is an array of names for the F-variables (default '')

    A          is the linear part of the constraint Jacobian of size nF by n.
               - A can be a tuple (A,iAfun,jAvar) containing the coordinates of the
                 linear constraint Jacobian, with iAfun containing the row indices and
                 jAvar containing the column indices.

               - A can be a scipy coo_matrix

               - A can be a numpy 2-dimensional array

    G          is the nonlinear part of the constraint Jacobian of size nF by n.
               - G can be a tuple (G,iGfun,jGvar) containing the coordinates of the
                 nonlinear constraint Jacobian, with iGfun containing the row indices and
                 jGvar containing the column indices.

               - G can be a scipy coo_matrix

               - G can be a numpy 2-dimensional array

               Here, the user only needs to provide the sparsity pattern of G.
               The values of G are provided in the user-defined subroutine.

    ObjRow     is an integer indicating the location of the objective in F(x) (default 0)
    ObjAdd     is a constant term of the objective (default 0.0)

    options    contains the user options for the SNOPT solver

    """

    name     = kwargs.get('name','')
    usropts  = kwargs.get('options',SNOPT_options())

    verbose  = usropts.getOption('Verbose')
    maxTries = usropts.getOption('Max memory attempts')
    inf      = usropts.getOption('Infinite bound')

    assert n  > 0, 'Error: n must be greater than 0'
    assert nF > 0, 'Error: nF must be greater than 0'

    neA = 0
    A   = kwargs.get('A',None)
    if A is not None:
        if   type(A) is tuple: # A = (A,iAfun,jAvar)
            try:
                (valA,iAfun,jAvar) = A
                neA = valA.size
            except:
                raise TypeError

        elif type(A) is np.ndarray and A.ndim == 2: # A = matrix
            spA   = sp.coo_matrix(A)

            iAfun = spA.row
            jAvar = spA.col
            valA  = spA.data
            neA   = spA.nnz

        elif type(A) is sp.coo_matrix: # A = scipy coo mtx
            iAfun = A.row
            jAvar = A.col
            valA  = A.data
            neA   = A.nnz

        else:
            raise TypeError

    neG = 0
    G   = kwargs.get('G',None)
    if G is not None:
        if   type(G) is tuple:  # G = (iGfun,jGvar)
            try:
                (iGfun,jGvar) = G
                neG = iGfun.size
            except:
                raise TypeError

        elif type(G) is np.ndarray and G.ndim == 2: # G = matrix
            spG    = sp.coo_matrix(G)
            iGfun  = spG.row
            jGvar  = spG.col
            neG    = spG.nnz

        elif type(G) is sp.coo_matrix: # G = scipy coo mtx
            iGfun  = G.row
            jGvar  = G.col
            neG    = G.nnz

        else:
            raise TypeError

    ObjRow  = kwargs.get('ObjRow',0)
    ObjAdd  = kwargs.get('ObjAdd',0.0)

    x0      = kwargs.get('x0',np.zeros(n,float))
    xlow    = kwargs.get('xlow',-inf*np.ones(n,float))
    xupp    = kwargs.get('xupp', inf*np.ones(n,float))
    xstates = kwargs.get('xstates',   np.zeros(n,int))
    xmul    = kwargs.get('xmul',   np.zeros(n,float))
    xnames  = kwargs.get('xnames', np.empty((1,8),dtype='|S1'))

    F0      = kwargs.get('F0',np.zeros(nF,float))
    Flow    = kwargs.get('Flow',-inf*np.ones(nF,float))
    Fupp    = kwargs.get('Fupp', inf*np.ones(nF,float))
    Fstates = kwargs.get('Fstates',  np.zeros(nF,int))
    Fmul    = kwargs.get('Fmul',    np.zeros(nF,float))
    Fnames  = kwargs.get('Fnames', np.empty((1,8),dtype='|S1'))

    assert xlow.shape == (n,)
    assert xupp.shape == (n,)
    assert xstates.shape == (n,)

    assert Flow.shape == (nF,)
    assert Fupp.shape == (nF,)
    assert Fstates.shape == (nF,)


    # Deal with xnames and Fnames
    nxname = len(xnames)
    assert xnames.dtype.char == 'S'
    assert nxname == 1 or nxname == n
    assert xnames.shape == (nxname,) or xnames.shape == (nxname,8)

    nFname = len(Fnames)
    assert Fnames.dtype.char == 'S'
    assert nFname == 1 or nFname == nF
    assert Fnames.shape == (nFname,) or Fnames.shape == (nFname,8)


    if verbose:
        printInfo('SNOPTA',name, 0,n,nxname,xnames,xstates,x0,xlow,xupp,xmul)
        printInfo('SNOPTA',name,nF,0,nFname,Fnames,Fstates,F0,Flow,Fupp,Fmul,header=False)


    # Set up workspace
    snwork  = SNOPT_work(505,5000,5000)
    usrwork = SNOPT_work(1,1,1)


    # Initialize SNOPT workspace
    # parameters are set to undefined values
    if name == '':
        prtfile = usropts.getOption('Print filename')
    else:
        prtfile = name.strip() + '.out'
    summOn  = 1 if (usropts.getOption('Summary')).lower() == "yes" else 0
    fsnopt.sninit_wrap(prtfile,summOn,snwork.cw,snwork.iw,snwork.rw)

    # Copy options to SNOPT workspace
    info = copyOpts(verbose,usropts,snwork)

    # Read specs file if one was given
    info = 0
    spcfile = usropts.getOption('Specs filename')
    if spcfile is not None:
        info = fsnopt.snspec_wrap(iSpecs,spcfile,snwork.cw,snwork.iw,snwork.rw)
        if info != 101 and info != 104:
            print('Specs read failed: INFO = {:d}'.format(info))
            return info


    # Get Jacobian structure if necessary:
    if A is None and G is None:
        print('  Could not determine Jacobian structure from user input')
        print('  Calling snJac...')

        lenA  = nF*n
        count = 1
        while True:
            snjac = fsnopt.snjac_wrap(nF,usrfun,lenA,lenA,x0,xlow,xupp,
                                      usrwork.cw,usrwork.iw,usrwork.rw,
                                      snwork.cw,snwork.iw,snwork.rw)
            info  = snjac[0]
            iAfun = snjac[1]
            jAvar = snjac[2]
            neA   = snjac[3]
            valA  = snjac[4]
            iGfun = snjac[5]
            jGvar = snjac[6]
            neG   = snjac[7]
            mincw = snjac[8]
            miniw = snjac[9]
            minrw = snjac[10]
            del snjac

            if info == 102:
                usropts.setOption('Derivative option',0)
                snwork.iw[103] = 0  # DerOpt
                break

            if info == 82 or info == 83 or info ==84:
                count+= 1
                if count > maxTries:
                    print(' Could not allocate memory for SNOPT')
                    return info
                snwork.work_resize(mincw,miniw,minrw)

    else:
        iAfun = np.array([1]) if neA == 0 else iAfun + 1
        jAvar = np.array([1]) if neA == 0 else jAvar + 1

        iGfun = np.array([1]) if neG == 0 else iGfun + 1
        jGvar = np.array([1]) if neG == 0 else jGvar + 1


    # Check memory
    info,mincw,miniw,minrw = fsnopt.snmema_wrap(nF,n,nxname,nFname,neA,neG,
                                                snwork.cw,snwork.iw,snwork.rw)
    snwork.work_resize(mincw,miniw,minrw)

    # Solve problem
    Start  = usropts.getOption('Start type')
    iStart = 0
    if   Start == 'Warm':
        iStart = 1
    elif Start == 'Hot':
        iStart = 2

    if xnames.shape != (1,8):
        snXnames = xnames.view('S1').reshape((xnames.size,-1))
    else:
        snXnames = xnames

    if Fnames.shape != (1,8):
        snFnames = Fnames.view('S1').reshape((Fnames.size,-1))
    else:
        snFnames = Fnames

    count = 1
    while True:
        res = fsnopt.snopta_wrap(iStart, nxname, nFname,
                                 ObjAdd, ObjRow, name, usrfun,
                                 iAfun, jAvar, neA, valA, iGfun, jGvar, neG,
                                 xlow, xupp, snXnames, Flow, Fupp, snFnames,
                                 x0, xstates, xmul, F0, Fstates, Fmul,
                                 usrwork.cw, usrwork.iw, usrwork.rw,
                                 snwork.cw, snwork.iw, snwork.rw)
        if  res[6]/10 == 8:
            count += 1
            if count > maxTries:
                print(' Could not allocate memory for SNOPT')
                return info
            snwork.work_resize(res[9],res[10],res[11])
        else:
            break

    # Results
    # res[0]  = x
    # res[1]  = xstates
    # res[2]  = xmul
    # res[3]  = f
    # res[4]  = Fstates
    # res[5]  = fmul
    # res[6]  = info
    # res[7]  = itn
    # res[8]  = mjritn
    # res[9]  = mincw
    # res[10] = miniw
    # res[11] = minrw
    # res[12] = nS
    # res[13] = nInf
    # res[14] = sInf
    # res[15] = Obj

    # Return solution
    result = SNOPTA_solution(name,xnames,Fnames)
    result.x          = res[0]
    result.xstates    = res[1]
    result.xmul       = res[2]
    result.F          = res[3]
    result.Fstates    = res[4]
    result.Fmul       = res[5]

    result.info       = res[6]

    result.iterations = res[7]
    result.major_itns = res[8]

    result.nS         = res[12]
    result.num_inf    = res[13]
    result.sum_inf    = res[14]
    result.objective  = res[15]

    # Finish up
    fsnopt.snend_wrap(snwork.cw,snwork.iw,snwork.rw)
    del res
    del snwork
    del usrwork

    # Print solution?
    if verbose:
        print(result)

    # Return result
    return result

#-------------------------------------------------------------------------------#

def snoptb(funobj,funcon,nnObj,nnCon,nnJac,x0,J,**kwargs):
    """
    snoptb calls the solver SNOPTB to solve the optimization
    problem:
              min       f_0(x)
              s.t.       [  x ]
                   bl <= [    ] <= bu
                         [c(x)]

    where f(x) is a vector of smooth nonlinear constraint functions, f_0(x)
    is a smooth scalar objective function, c(x) are the nonlinear and linear
    constraints, defined such that nonlinear constraints come first (see
    the SNOPT documentation for the SNOPTB interface).

    --------------------------------------------
    Input arguments:
    --------------------------------------------
    Keyword    Description
    ----------------------
    funobj     is a user-defined function that computes the objective and its
               gradient
    funcon     is a user-defined function that computes the constraints and the
               Jacobian

    nnObj      is the number of nonlinear objective variables (nnObj >= 0)
    nnCon      is the number of nonlinear constraints (nnCon >= 0)
    nnJac      is the number of nonlinear Jacobian variables
               (nnJac > 0 if nnCon > 0; nnJac = 0 if nnCon == 0)

    J          is the constraint Jacobian
               - J can be a tuple (valJ,indJ,locJ) containing the sparse structure
                 of the Jacobian

               - J can be a scipy csc_matrix

               - J can be a numpy 2-dimensional array

    iObj       indicates which row of J is the free row containing the linear
               objective vector (default 0)
    ObjAdd     is the constant term of the objective (default 0.0)

    states     are the initial states of the variables (default 0)
    x0         are the initial values of x (default 0.0)
    bl         are the lower bounds of the problem (default -infinity)
    bu         are the upper bounds of the problem (default +infinity)
    pi         are the initial multipliers of the constraints (default 0.0)
    names      is an array of names for the variables (default '')

    name       is the name of the problem (default '')

    options    contains the user options for the SNOPT solver

    snoptb will try to compute the number of constraints, variables,
    nonlinaer objective variables, nonlinear constraints and nonlinear
    Jacobian variables from the given data.

    If it doesn't do it correctly, provide snoptb with the following info:
      m        is the number of constraints
      n        is the number of variables

    """

    name    = kwargs.get('name','')
    usropts = kwargs.get('options',SNOPT_options())

    verbose  = usropts.getOption('Verbose')
    maxTries = usropts.getOption('Max memory attempts')
    inf      = usropts.getOption('Infinite bound')

    m        = kwargs.get('m',None)
    n        = kwargs.get('n',None)

    if   type(J) is tuple:
        try:
            (valJ,indJ,locJ) = J
            ne = valJ.size
        except:
            raise TypeError

        if m is None or n is None:
            raise ValueError('m and n need to be set when J is a tuple')

    elif type(J) is np.ndarray and J.ndim == 2:
        J0 = sp.csc_matrix(J)
        J0.sort_indices()

        indJ   = J0.indices
        locJ   = J0.indptr
        ne     = J0.nnz
        valJ   = J0.data

        m = J0.shape[0] if m is None else m
        n = J0.shape[1] if n is None else n

    elif type(J) is sp.csc_matrix:
        J.sort_indices()
        indJ   = J.indices
        locJ   = J.indptr
        ne     = J.nnz
        valJ   = J.data

        m = J.shape[0] if m is None else m
        n = J.shape[1] if n is None else n

    else:
        raise TypeError('Type of J is unsupported')

    if verbose:
        print('There are {} constraints and {} variables'.format(m,n))


    indJ = indJ + 1
    locJ = locJ + 1

    iObj   = kwargs.get('iObj',0)
    ObjAdd = kwargs.get('ObjAdd',0.0)

    hs     = kwargs.get('states', np.zeros(n+m,int))
    bl     = kwargs.get('bl',-inf*np.ones(n+m,float))
    bu     = kwargs.get('bu', inf*np.ones(n+m,float))
    pi     = kwargs.get('pi',np.zeros(m,float))
    rc     = np.zeros(n+m,float) #kwargs.get('rc',np.zeros(n+m,float))
    Names  = kwargs.get('names',np.empty((1,8),dtype='|S1'))

    rc[n+1:n+m] = pi[1:m]

    assert hs.shape == (n+m,)
    assert bl.shape == (n+m,)
    assert bu.shape == (n+m,)
    assert x0.shape == (n+m,)
    assert pi.shape == (m,)
    assert rc.shape == (n+m,)


    # Deal with Names:
    nName = len(Names)
    assert Names.dtype.char == 'S'
    assert nName == 1 or nName == n+m
    assert Names.shape == (nName,) or Names.shape == (nName,8)


    if verbose:
        printInfo('SNOPTB',name,m,n,nName,Names,hs,x0,bl,bu,rc)


    # Set up workspace
    snwork  = SNOPT_work(505,5000,5000)
    usrwork = SNOPT_work(1,1,1)


    # Initialize SNOPT workspace
    # parameters are set to undefined values
    if name == '':
        prtfile = usropts.getOption('Print filename')
    else:
        prtfile = name.strip() + '.out'
    summOn  = 1 if (usropts.getOption('Summary')).lower() == "yes" else 0
    fsnopt.sninit_wrap(prtfile,summOn,snwork.cw,snwork.iw,snwork.rw)


    # Copy options to SNOPT workspace
    info = copyOpts(verbose,usropts,snwork)


    # Read specs file if one was given
    info = 0
    spcfile = usropts.getOption('Specs filename')
    if spcfile is not None:
        info = fsnopt.snspec_wrap(spcfile,snwork.cw,snwork.iw,snwork.rw)
        if info != 101 and info != 104:
            print('Specs read failed: INFO = {:d}'.format(info))
            return info

    # Check memory
    neG = nnCon*nnJac
    info,mincw,miniw,minrw = fsnopt.snmem_wrap(m,n,ne,neG,nnCon,nnJac,nnObj,
                                               snwork.cw,snwork.iw,snwork.rw)
    snwork.work_resize(mincw,miniw,minrw)


    # Solve problem
    Start = usropts.getOption('Start type')

    if Names.shape != (1,8):
        snNames = Names.view('S1').reshape((Names.size,-1))
    else:
        snNames = Names

    count = 1
    while True:
        res = fsnopt.snoptb_wrap(Start, nName, nnCon, nnObj, nnJac,
                                 iObj, ObjAdd, name,
                                 funcon, funobj,
                                 valJ, indJ, locJ,
                                 bl, bu, snNames, hs, x0, pi,
                                 usrwork.cw, usrwork.iw, usrwork.rw,
                                 snwork.cw, snwork.iw, snwork.rw)
        if  res[4]/10 == 8:
            count += 1
            if count > maxTries:
                print(' Could not allocate memory for SNOPT')
                return info
            snwork.work_resize(res[7],res[8],res[9])
        else:
            break

    # Results
    # res[0]  = hs
    # res[1]  = x
    # res[2]  = pi
    # res[3]  = rc
    # res[4]  = info
    # res[5]  = itn
    # res[6]  = mjritn
    # res[7]  = mincw
    # res[8]  = miniw
    # res[9]  = minrw
    # res[10] = nS
    # res[11] = nInf
    # res[12] = sInf
    # res[13] = objective


    # Return solution
    result = SNOPT_solution(name,Names)
    result.states     = res[0]
    result.x          = res[1]
    result.pi         = res[2]
    result.rc         = res[3]
    result.info       = res[4]

    result.iterations = res[5]
    result.major_itns = res[6]

    result.nS         = res[10]
    result.num_inf    = res[11]
    result.sum_inf    = res[12]
    result.objective  = res[13]

    # Finish up
    fsnopt.snend_wrap(snwork.cw,snwork.iw,snwork.rw)

    # Print solution?
    if verbose:
        print(result)

    return result


#-------------------------------------------------------------------------------#

def snoptc(userfun,nnObj,nnCon,nnJac,x0,J,**kwargs):
    """
    snoptc calls the solver SNOPTC to solve the optimization
    problem:
              min       f_0(x)
              s.t.       [  x ]
                   bl <= [    ] <= bu
                         [c(x)]

    where f(x) is a vector of smooth nonlinear constraint functions, f_0(x)
    is a smooth scalar objective function, c(x) are the nonlinear and linear
    constraints, defined such that nonlinear constraints come first (see
    the SNOPT documentation for the SNOPTC interface).

    --------------------------------------------
    Input arguments:
    --------------------------------------------
    Keyword    Description
    ----------------------
    userfun    is a user-defined function that computes the objective and constraint
               functions and (if necessary) their derivatives

    nnObj      is the number of nonlinear objective variables (nnObj >= 0)
    nnCon      is the number of nonlinear constraints (nnCon >= 0)
    nnJac      is the number of nonlinear Jacobian variables
               (nnJac > 0 if nnCon > 0; nnJac = 0 if nnCon == 0)

    J          is the constraint Jacobian
               - J can be a tuple (valJ,indJ,locJ) containing the sparse structure
                 of the Jacobian.

               - J can be a scipy csc_matrix

               - J can be a numpy 2-dimensional array

    iObj       indicates which row of J is the free row containing the linear
               objective vector (default 0)
    ObjAdd     is the constant term of the objective (default 0.0)

    states     are the initial states of the variables (default 0)
    x          are the initial values of x (default 0.0)
    bl         are the lower bounds of the problem (default -infinity)
    bu         are the upper bounds of the problem (default +infinity)
    pi         are the initial multipliers of the constraints (default 0.0)
    names      is an array of names for the variables (default '')

    name       is the name of the problem (default '')

    options    contains the user options for the SNOPT solver

    snoptc will try to compute the number of constraints, variables,
    nonlinaer objective variables, nonlinear constraints and nonlinear
    Jacobian variables from the given data.

    If it doesn't do it correctly, provide snoptc with the following info:
      m        is the number of constraints
      n        is the number of variables

    """

    name    = kwargs.get('name','')
    usropts = kwargs.get('options',SNOPT_options())

    verbose  = usropts.getOption('Verbose')
    maxTries = usropts.getOption('Max memory attempts')
    inf      = usropts.getOption('Infinite bound')

    m       = kwargs.get('m',None)
    n       = kwargs.get('n',None)


    if   type(J) is tuple:
        try:
            (valJ,indJ,locJ) = J
            ne = valJ.size
        except:
            raise TypeError

    elif type(J) is np.ndarray and J.ndim == 2:
        J0 = sp.csc_matrix(J)
        J0.sort_indices()

        indJ   = J0.indices
        locJ   = J0.indptr
        ne     = J0.nnz
        valJ   = J0.data

        m = J0.shape[0] if m is None else m
        n = J0.shape[1] if n is None else n

    elif type(J) is sp.csc_matrix:
        J.sort_indices()
        indJ   = J.indices
        locJ   = J.indptr
        ne     = J.nnz
        valJ   = J.data

        m = J.shape[0] if m is None else m
        n = J.shape[1] if n is None else n

    else:
        raise TypeError


    indJ = indJ + 1
    locJ = locJ + 1

    iObj   = kwargs.get('iObj',0)
    ObjAdd = kwargs.get('ObjAdd',0.0)

    hs     = kwargs.get('states', np.zeros(n+m,int))
    bl     = kwargs.get('bl',-inf*np.ones(n+m,float))
    bu     = kwargs.get('bu', inf*np.ones(n+m,float))
    pi     = kwargs.get('pi',np.zeros(m,float))
    rc     = np.zeros(n+m,float) #kwargs.get('rc',np.zeros(n+m,float))
    Names  = kwargs.get('names',np.empty((1,8),dtype='|S1'))

    rc[n+1:n+m] = pi[1:m]

    assert hs.shape == (n+m,)
    assert bl.shape == (n+m,)
    assert bu.shape == (n+m,)
    assert x0.shape == (n+m,)
    assert pi.shape == (m,)
    assert rc.shape == (n+m,)


    # Deal with Names:
    nName = len(Names)
    assert Names.dtype.char == 'S'
    assert nName == 1 or nName == n+m
    assert Names.shape == (nName,) or Names.shape == (nName,8)


    if verbose:
        printInfo('SNOPTC',name,m,n,nName,Names,hs,x0,bl,bu,rc)


    # Set up workspace
    snwork  = SNOPT_work(505,5000,5000)
    usrwork = SNOPT_work(1,1,1)


    # Initialize SNOPT workspace
    # parameters are set to undefined values
    if name == '':
        prtfile = usropts.getOption('Print filename')
    else:
        prtfile = name.strip() + '.out'
    summOn  = 1 if (usropts.getOption('Summary')).lower() == "yes" else 0
    fsnopt.sninit_wrap(prtfile,summOn,snwork.cw,snwork.iw,snwork.rw)


    # Copy options to SNOPT workspace
    info = copyOpts(verbose,usropts,snwork)


    # Read specs file if one was given
    info = 0
    spcfile = usropts.getOption('Specs filename')
    if spcfile is not None:
        info = fsnopt.snspec_wrap(spcfile,snwork.cw,snwork.iw,snwork.rw)
        if info != 101 and info != 104:
            print('Specs read failed: INFO = {:d}'.format(info))
            return info


    # Check memory
    neG = nnCon*nnJac
    info,mincw,miniw,minrw = fsnopt.snmem_wrap(m,n,ne,neG,nnCon,nnJac,nnObj,
                                               snwork.cw,snwork.iw,snwork.rw)
    snwork.work_resize(mincw,miniw,minrw)


    # Solve problem
    Start = usropts.getOption('Start type')

    if Names.shape != (1,8):
        snNames = Names.view('S1').reshape((Names.size,-1))
    else:
        snNames = Names

    count = 1
    result = SNOPT_solution(name,Names)

    while True:
        result.states, \
        result.x, \
        result.pi, \
        result.rc, \
        result.info, \
        result.iterations, \
        result.major_itns, \
        mincw, miniw, minrw, \
        result.nS, \
        result.num_inf, \
        result.sum_inf, \
        result.objective = fsnopt.snoptc_wrap(Start, nName, nnCon, nnObj, nnJac,
                                              iObj, ObjAdd, name, userfun,
                                              valJ, indJ, locJ,
                                              bl, bu, snNames, hs, x0, pi,
                                              usrwork.cw, usrwork.iw, usrwork.rw,
                                              snwork.cw, snwork.iw, snwork.rw)
        if  result.info/10 == 8:
            count += 1
            if count > maxTries:
                print(' Could not allocate memory for SNOPT')
                return info
            snwork.work_resize(mincw,miniw,minrw)
        else:
            break

    # Finish up
    fsnopt.snend_wrap(snwork.cw,snwork.iw,snwork.rw)

    # Print solution?
    if verbose:
        print(result)

    return result

#-------------------------------------------------------------------------------#

def sqopt(H,x0,**kwargs):
    """ sqopt solves the quadratic optimization problem:
          min    f + c'x + half*x'Hx
          s.t.   xl <=   x  <= xu
          s.t.   al <=  Ax  <= au

    --------------------------------------------
    Input arguments:
    --------------------------------------------
    Keyword    Description
    ----------------------
    H          is the Hessian of the quadratic objective and should be a callable
               function

    x0         are the intial values of x (required)

    name       is the name of the problem (default '')

    xl, xu     are the lower and upper bounds of x (default -/+ infinity)

    A          is the linear constraint matrix
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

    m        = kwargs.get('m',None)
    n        = kwargs.get('n',None)
    A        = kwargs.get('A',None)

    # Linear constraint matrix
    if A is None:
        m    = 1
        n    = x0.shape[0]
        indA = np.ndarray([1])
        valA = np.ndarray([1.])
        locA = np.arange(n+1,int)
        al   = np.ndarray([-inf])
        au   = np.ndarray([ inf])

    else:
        if   type(A) is tuple:
            try:
                (valA,indA,locA) = A
                ne = valA.size
            except:
                raise TypeError

            if m is None or n is None:
                raise ValueError('m and n need to be set when J is a tuple')

        elif type(A) is np.ndarray and A.ndim == 2:
            m, n   = A.shape
            valA   = sp.csc_matrix(A)
            valA.sort_indices()

            indA   = valA.indices
            locA   = valA.indptr
            ne     = valA.nnz
            valA   = valA.data

        elif type(A) is sp.csc_matrix:
            m, n   = A.shape

            A.sort_indices()
            indA   = A.indices
            locA   = A.indptr
            ne     = A.nnz
            valA   = A.data

        else:
            raise TypeError('Type of A is unsupported')

    indA = indA + 1
    locA = locA + 1


    # Hessian user-defined function
    assert callable(H)
    try:
        Hx  = np.zeros(n)
        nnH = H(x0,Hx,0).size
    except:
        raise TypeError('Error with callable H')


    f     = kwargs.get('f',0.0)
    c     = kwargs.get('c',np.zeros(1))
    xl    = kwargs.get('xl',-inf*np.ones(n,float))
    xu    = kwargs.get('xu', inf*np.ones(n,float))
    al    = kwargs.get('al',-inf*np.ones(m,float))
    au    = kwargs.get('au', inf*np.ones(m,float))
    hs    = kwargs.get('states',np.zeros(n+m,'i'))
    eType = kwargs.get('eType',np.zeros(n+m,'i'))
    Names = kwargs.get('names',np.empty((1,8),dtype='|S1'))

    pi    = np.zeros(m,float)
    rc    = np.zeros(n+m,float)

    try:
        bl = np.concatenate([xl,al])
        bu = np.concatenate([xu,au])
    except:
        raise InputError('Check the bounds of the problem')

    x = np.concatenate([x0,np.zeros(m)])

    assert c.size   <= n
    assert bl.shape == (n+m,)
    assert bu.shape == (n+m,)
    assert hs.shape == (n+m,)


    # Deal with names
    nName = len(Names)
    assert Names.dtype.char == 'S'
    assert nName == 1 or nName == n+m
    assert Names.shape == (nName,) or Names.shape == (nName,8)


    if verbose:
        printInfo('SQOPT',name,m,n,nName,Names,hs,x,bl,bu,rc)


    # Set up workspace
    snwork  = SNOPT_work(505,5000,5000)
    usrwork = SNOPT_work(1,1,1)


    # Initialize SQOPT
    if name == '':
        prtfile = usropts.getOption('Print filename')
    else:
        prtfile = name.strip() + '.out'
    summOn  = 1 if (usropts.getOption('Summary')).lower() == "yes" else 0
    fsnopt.sqinit_wrap(prtfile,summOn,snwork.cw,snwork.iw,snwork.rw)


    # Copy options to SQIC
    info = copyOpts(verbose,usropts,snwork)


    # Read specs file if one was given
    info = 0
    spcfile = usropts.getOption('Specs filename')
    if spcfile is not None:
        info = fsnopt.sqspec_wrap(spcfile,snwork.cw,snwork.iw,snwork.rw)
        if info != 101 and info != 104:
            print('Specs read failed: INFO = {:d}'.format(info))
            return info


    # Solve the QP
    Start  = usropts.getOption('Start type')
    iObj   = 0

    if Names.shape != (1,8):
        snNames = Names.view('S1').reshape((Names.size,-1))
    else:
        snNames = Names


    result = SNOPT_solution(name)
    result.states, \
    result.x, \
    result.pi, \
    result.rc, \
    result.info, \
    result.iterations, \
    mincw, miniw, minrw, \
    result.nS, \
    result.num_inf, \
    result.sum_inf, \
    result.objective = fsnopt.sqopt_wrap(Start, H, nName, nnH, iObj, f, name,
                                         valA, indA, locA, bl, bu, c, snNames,
                                         eType, hs, x, pi,
                                         usrwork.cw,usrwork.iw,usrwork.rw,
                                         snwork.cw,snwork.iw,snwork.rw)

    # Finish up
    fsnopt.snend_wrap(snwork.cw,snwork.iw,snwork.rw)

    # Print solution?
    if verbose:
        print(result)

    # Return solution
    return result
