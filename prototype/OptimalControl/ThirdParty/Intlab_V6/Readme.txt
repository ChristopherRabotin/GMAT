A short overview how to use INTLAB (see also file FAQ):
=======================================================

INTLAB was started with the introduction of an operator concept in Matlab. 
It is continuously developed since the first version in 1998. 

The inventor and only developer is Siegfried M. Rump, head of the 
Institue for Reliable Computing at the Hamburg University of Technology. 
In any publication or other material using INTLAB please cite

S.M. Rump. INTLAB - INTerval LABoratory. In Tibor Csendes, editor, 
Developments in Reliable Computing, pages 77-104. Kluwer Academic Publishers, Dordrecht, 1999.
@incollection{Ru99a,
   author = {Rump, {S.M.}},
   title = {{INTLAB - INTerval LABoratory}},
   editor = {Tibor Csendes},
   booktitle = {{Developments~in~Reliable Computing}},
   publisher = {Kluwer Academic Publishers},
   address = {Dordrecht},
   pages = {77--104},
   year = {1999},
   url = {http://www.ti3.tu-harburg.de/rump/}
}

===> If you used INTLAB before with an older Matlab version, you had to
    set the system variable BLAS_VERSION:
===> Please delete this system variable. It was necessary to avoid IMKL,
  now with new rounding routines the Intel Math Kernel Library (IMKL),
  which is the default and very fast, can safely be used (thanks to 
  Dr. Takeshi Ogita from Tokyo).


Few routines had to be renamed within Versions of INTLAB, in particular
isempty/isempty_ changed their role, and the routine "emptyintersection"
was introduced. See "Changes" in the FAQ.txt file.


The easiest way to start is create a file startup.m (or to replace the content 
of the existing one) in the Matlab path \toolbox\local by

   cd ... insert the INTLAB path ...
   startintlab

Then the startintlab.m file is started and should do the rest. For
some user-specific options you may adjust the file startintlab.m  .

INTLAB is successfully tested under Matlab versions
   5.3,  6.0.0, 6.5.0, 6.5.1, 7.0, 7.0.1 (SP1), 7.0.4 (SP2), 7.1 (SP3), 
   7.2 (R2006a), 7.3 (R2006b), 7.4 (R2007a), R2007b, R2008a, R2008b, R2009a.
The very old version 5.3 has some problems with huge sparse matrices 
(more than 10^10 elements).
The Matlab version 7.0.4 (R14) Service Pack 2, however, is not very 
stable, sometimes a segmentation violation may occur.
There are some problems in Matlab version 7.2 (R2006a). For example,
const*sparse for larger dimension can be slow:
>> n=1e4, a=sparse([],[],[],n,n), tic,b=2*a,toc, tic,c=a/2,toc
n =
       10000
a =
   All zero sparse: 10000-by-10000
b =
   All zero sparse: 10000-by-10000
Elapsed time is 3.101008 seconds.
c =
   All zero sparse: 10000-by-10000
Elapsed time is 0.000131 seconds.
>> 
or, still in Matlab version 7.2 (R2006a) under Windows XP, 
>> m=1e6, n=1e4, I=100, J=300, A=sparse(I,J,NaN,m,n), sqrt(-1)*A
the last multiplication produces an infinite loop.
Matlab Version R2006b produces sometimes a core dump with segment violation.

For Matlab Version 5.3 and higher on PCs and Windows, INTLAB is
entirely written in Matlab. There is no system dependency. This is because 
the Mathworks company was so kind to add into the routine "system_dependent"
a possibility to change the rounding mode (my dear thanks to Cleve).

For other platforms or Matlab Version 5.3-, INTLAB is written
entirely in Matlab language except one routine

   setround

for switching rounding mode. A corresponding C-routine together with a
dll-file is in directory \setround. This routine is the only portability
constraint (see also function "setround").

The file setround.dll is put in a separate path "setround". If rounding
does not work properly, this path is removed. So INTLAB should detect automatically
whether a rounding routine is necessary. If not, for some reason, 

===>  and you are using Matlab Version 5.3+ on a PC under Windows, **delete**
===>  the file "setround.dll" .

===>  Prerequisite for INTLAB is the possibility to switch the processor
===>  permanently into a specific rounding mode "downwards", "upwards" and
===>  "to nearest". This is always checked when starting INTLAB under Matlab.
===>  If the check fails, INTLAB will be terminated not to produce incorrect results!

The progress in the different INTLAB versions can be viewed using help, for example

   help Version5_3

Note that '_' is used rather than a dot, otherwise the help function does not work.

INTLAB supports

     - interval scalars, vectors and matrices, real and complex,
     - full and sparse matrices,
     - interval standard functions, real and complex,
     - and a number of toolboxes for intervals, gradients, hessians, slopes,
         polynomials, multi-precision arithmetic and more.

There are some demo-routines to get acquainted with INTLAB such as

   demointlab
   demointval
   demogradient
   demohessian
   demoslope
   demopolynom
   demolong

Call "demos" and access the INTLAB-demos. 

INTLAB results are verified to be correct including I/O and standard
functions. Interval input is rigorous when using string constants, see

   help intval               .

Interval output is always rigorous. For details, try e.g.

   "help intval\display"  and  "demointval".

You may switch your display permanently to infimum/supremum notation,
or midpoint/radius, or display using "_" for uncertainties; see
"help intvalinit" for more information. For example

   format long, x = midrad(pi,1e-14);
   infsup(x)
   midrad(x)
   disp_(x)

produces

   intval x =
   [   3.14159265358978,   3.14159265358981]
   intval x =
   <   3.14159265358979,  0.00000000000002>
   intval x =
      3.1415926535898_

Display with uncertainties represents the interval produces by subtracting
and adding 1 from and to the last displayed digit of the mantissa. Note that
the output is written in a way that "what you see is correct". For example,

   midrad(4.99993,0.0004)

produces

   intval ans = 
   <    4.9999,   0.0005> 

in "format short" and mid-rad representation. Due to non-representable real numbers
this is about the best what can be done with four decimal places after the decimal point.

A possible trap is for example

>> Z=[1,2]+i*[-1,1]
Z =
  1.0000 - 1.0000i   2.0000 + 1.0000i

The brackets in the definition of Z might lead to the conclusion that Z is a 
complex interval (rectangle) with lower left endpoint 1-i and upper right 
endpoint 2+i. This is not the case. The above statement is a standard Matlab
statement defining a (row) vector of length 2. It cannot be an interval:
Otherwise Z would be preceded in the output by "intval".

Standard functions are rigorous. This includes trigonometric functions
with large argument. For example,

   x=2^500; sin(x), sin(intval(x))

produces

   ans =
      0.429257392342428
   intval ans =
      0.42925739234242

the latter being correct to the last digit. All functions support vector 
and matrix input to minimize interpretation overhead. For real interval input
causing an exception for a real standard function, one may switch between
changing to complex standard functions with or without warning or, to
stay with real standard functions causing NaN result. For example,

   intvalinit('DisplayMidRad')
   intvalinit('RealStdFctsExcptnAuto'), sqrt(infsup(-3,-2))

produces

   ===> Complex interval stdfct used automatically for real interval input
            out of range (without warning)
   intval ans =
   <   0.0000 +  1.5811i,  0.1670>

whereas

   intvalinit('RealStdFctsExcptnWarn'), sqrt(infsup(-3,-2))

produces

   ===> Complex interval stdfct used automatically for real interval input
            out of range, but with warning

   Warning: SQRT: Real interval input out of range changed to be complex
   > In c:\matlab_v5.1\toolbox\intlab\intval\@intval\sqrt.m at line 81
   intval ans =
   <   0.0000 +  1.5811i,  0.1670>

Input out of range for standard functions may be ignored, please see the demo on interval
arithmetic. This is possible using

   intvalinit('RealStdFctsExcptnIgnore'), sqrt(infsup(-3,4))
   ===> !!! Caution: Input arguments out of range are ignored !!!
   intval ans = 
   [   0.00000000000000,   2.00000000000000] 

Using Brouwer's Fixed Point Theorem by checking f(X) in X is only possible if 
the interval vector X is completely in the range of definition of f. Consider

   f = inline('sqrt(x)-2');  X = infsup(-3,4);  Y = f(X)

which produces

   intval Y = 
   [   -2.0000,    0.0000] 

Obviousy, Y is contained in X, but f has no real fixed point at all. You may
check whether an input out of range occurred by

   NotDefined = intvalinit('RealStdFctsExcptnOccurred')

which gives

   NotDefined =
        1

Checking the out-of-range flag resets it to zero.

Certain data necessary for rigorous input/output and for rigorous standard
functions are stored in files power10tobinaryVVV.mat and stdfctsdataVVV.mat,
where VVV stands for Matlab version number. Those files must be in the
search path of Matlab. They are generated automatically when starting the
system the first time. Generation of the first is fast, the second file
needs some 2 minutes on a 1 Ghz PC. 

The documentation is included in every routine. INTLAB-code, i.e.
Matlab-code, is (hopefully) self-explaining. INTLAB stays to the
philosophy to implement everything in Matlab.

To start under Windows:
   -  create new directory     c:\matlab\toolbox\intlab
      and copy INTLAB and subdirectories into it;
   -  adapt your STARTINTLAB file according to the included sample;
   -  add a call of startintlab to your startup file.

For other operating systems similar directions apply. Especially,
directory names should be adapted.

For a quick overview of functions, use

   help intval               or
   help gradient             or
   help slope                or   
   help polynom              or
   help long                 or
   help utility              .

For some examples, try the demos.

For specific help try for example

   help verifylss            or
   lookfor reshape           or
   lookfor gradientinit      or
   lookfor 'linear systems'  and alike.

Certain flags like display-mode etc. are stored in global variables.
Therefore

   ===> try to avoid a statement like "clear global" or "clear all" <===

because it clears all global variables and results in subsequent errors
in INTLAB. The statements "clear" or "clear variables" do not do any
harm. Try to clear only specific global variables using, for example,
statements like "clear global v*". Note that all global variables used
in INTLAB start with "INTLAB_". Try "who global" to list them.

If a "clear global" statement is necessary for some reason, let it
be followed by

   startintlab

to restore global variables. In fact, this restores the (your) default options.

Intlab uses infimum-supremum representation for real intervals, and
midpoint-radius representation for complex intervals. However, this is not
visible to the user. For multiplication of two real interval matrices,
both of them being thick, there is a choice between

   - fast midpoint-radius implementation with a little overestimation
       issued by the Matlab command  intvalinit('FastIVmult'),
   - slower infimum-supremum representation
       issued by the Matlab command  intvalinit('SharpIVmult'),

Needless to say that both results are verified to be correct.
There is only a difference between "fast" and "sharp"
   - if both operands being contain intervals of nonzero diameter, and
   - if the inner dimension is greater than one.
Otherwise, all results will be computed sharp and fast. For example,
interval scalar products are always computed in sharp mode (not with
long accumulator, but without overestimation due to midpoint-radius
representation). For the accuracy and overestimation of the "fast"-mode,
see the file FAQ.

For timing use for example

   n=500; A=midrad(rand(n),1e-12); Amid=mid(A); k=10;
   tic; for i=1:k, Amid*Amid; end, toc/k
   tic; for i=1:k, intval(Amid)*Amid; end, toc/k
   tic; for i=1:k, A*Amid; end, toc/k
   intvalinit('FastIVmult'), tic; for i=1:k, A*A; end, toc/k
   intvalinit('SharpIVmult'), tic; A*A; toc

for the multiplication of two 500x500 interval matrices. Results on a
1 Ghz Pentium IV are

    0.3 sec  for floating point multiplication,
    0.6 sec  for multiplication of point matrices (both sharp and fast),
    0.9 sec  for multiplication of an thick and a thin matrix (both sharp and fast),
    1.2 sec  for fast multiplication of interval matrices, and
    0.7 sec  for sharp multiplication of interval matrices.

Note that the interval multiplication operator checks on the data, so that the
same code is used if at least one factor is a point matrix. This is true inde-
pendent of the 'FastIVmult' or 'SharpIVmult' mode.
It seems, however, strange to call a mode "fast" when it takes longer for 
multiplying two thick matrices. There is tremendous improvement in the
interpretation speed in recent releases of Matlab. This is the reason for
the 'SharpIVmult' being so fast. However, this is only true if the factors
do not contain zero intervals. Consider

   n=500; A=midrad(randn(n),1);    
   intvalinit('FastIVmult'), tic; A*A; toc
   intvalinit('SharpIVmult'), tic; A*A; toc
 
Then the timing is

    1.2 sec  for fast multiplication of interval matrices, and
   39.7 sec  for sharp multiplication of interval matrices.

The default is fast multiplication. This may be changed in the startintlab
file.

INTLAB is optimized for performance. Among many things various case distinctions
are performed before a matrix multiplication, say, is executed. For example, 
I distinguish between point and thick intervals, real and complex, zero intervals
and intervals not containing zero, and so forth. Thus it would slow down ALL
computations when checking for special operations such as 0*inf and alike.

This is the reason why there are no empty intervals in INTLAB. A component NaN is
interpreted as "no information available", much as in the IEEE 754 floating-point
standard.

At first sight it may sound strange that there are no empty intervals in INTLAB.
However, in most applications there are no calculations with empty intervals but
rather the information is needed whether some components are empty or not. A typical
example is the following. Let f:R^n->R^n and a vector xs and an interval vector X be
given. Of course, if one component of f(X) does not contain zero, then f cannot
have a zero in X. This is easy to check by 

   in( 0 , f(X) ) .

But if 0 is contained in all components of f(X), we may consider one interval Newton step

   Y = xs - Z   where Z contains the set of solutions of Jf(X) z = f(xs) .

If the intersection of X and Y is empty, then f cannot have a zero in X. For example

   f = inline('exp(x-6*sin(x))-2*x*atan(x)+asinh(x+1)');
   X = infsup(1,1.25);
   xs = mid(X);
   y = f(gradientinit(X))
   Y = xs-y.dx\f(intval(xs))
   is = intersect(X,Y)
   empty_is = emptyintersect(X,Y)

produces

   intval gradient value y.x = 
   [   -0.7874,    0.0018] 
   intval gradient derivative(s) y.dx = 
   [   -2.6862,   -1.9122] 
   intval Y = 
   [    0.9219,    0.9910] 
   intval is = 
   [       NaN,       NaN] 
   empty_is =
        1

So the inclusion of the range of f over X contains zero, but the interval Newton
step has an empty intersection with X. This is checked with "emptyintersect".

To improve performance, INTLAB also produces sometimes a NaN result where one might
not expect it. Remember, a result NaN means "don't know, no information available".
Consider

   >> A = infsup(2,inf), X = inf-A
   intval A = 
   [    2.0000,       Inf] 
   intval X = 
   [       NaN,       Inf] 

One may expect the result [-inf,inf]. However, to achieve this I have to check
EVERY time for infinite bounds. 
In my applications infinite intervals are very rare. And INTLAB is designed for 
fast interval operations, in particular interval vector and matrix operations. 

Checking for infinite bounds would slow down EVERY interval matrix addition or 
subtraction by up to almost a factor 3. This is why I choose to produce the X as 
shown above; not much information, but correct.


To start with INTLAB, try for example the following commands:

   n=10; A=2*rand(n)-1; b=A*ones(n,1); X=verifylss(A,b)

This solves a randomly generated 10x10 linear system with result
verification, matrix entries uniformly distributed in [-1,1]. The
result is a real interval vector. You may display the result in
mid-rad representation by

   midrad(X)                     or
   intvalinit('DisplayMidrad')

For a larger example, try

   n=1000; A=2*rand(n)-1; b=A*ones(n,1);
   tic; A\b; toc
   tic; verifylss(A,b); toc

On a 1 Ghz Pentium IV Laptop this takes about

   0.9 seconds   using floating point approximation,
   8.1 seconds   using fast multiplication, and
   8.5 seconds   using sharp multiplication.

The time difference comes from interval matrix by interval vector
multiplication C*X, see verifylss line 189. However, you would barely
see a difference in the results because rather than the solution
itself, the error with respect to an approximate solution is included.

Use the routine "verifylss" to be sure to calculate verified results.
Using "A\b" instead calls the built-in linear system solver, without
verification (if both operands are non-interval).

If the matrix or right hand side is an interval, you may use "\"
safely (it calls verifylss), e.g.

   n=1000; A=midrad(2*rand(n)-1,1e-12); b=A*ones(n,1); tic; A\b; toc   .

This generates a 1000x1000 matrix with entrywise radius 10^(-12) and
solves the linear system with verified bounds. The
timing is

   10.6 seconds .

To change display format of intervals, use the built-in commands
"format long", "format short e", etc.

The linear system solver works for sparse s.p.d. and/or rectangular
matrices as well. To generate a random sparse s.p.d. linear system try

   n=1000; A=sprand(n,n,10/n)+speye(n); A=A*A'; b=A*ones(n,1);

and solve it (with timing) without verification by

   tic; x = A\b; toc

and with verification by

   tic; X = verifylss(A,b); toc

On a 1 Ghz Pentium IV the timing is

   0.5 seconds   for built-in floating point, and
   1.9  seconds   with verification.

Verified solution of linear systems works for complex matrices as well.
Unfortunately, there is a bug in the Cholesky decomposition of Matlab 5.2
for complex sparse matrices such that verification does not work either
in that case.

For nonlinear systems, automatic differentiation is used. For example, look
at the function "intval\test". The code for the first function (omitting
comments) is

   function y = test(x)
     y = x;
     c1 = typeadj( 1 , typeof(x) );
     cpi = typeadj( midrad(3.14159265358979323,1e-16) , typeof(x) );
     y(1) = .5*sin(x(1)*x(2)) - x(2)/(4*cpi) - x(1)/2;
     y(2) = (1-1/(4*cpi))*(exp(2*x(1))-exp(c1)) + ...
            exp(c1)*x(2)/cpi - 2*exp(c1)*x(1);
     return

The call

   x=[-3;7]; y=test(x)

calls "test" with input vector [-3;7] and usual floating point
arithmetic. The call

   x=intval([-3;7]); y=test(x)

uses interval arithmetic, the call

   x=gradientinit([-3;7]); y=test(x)

uses automatic differentiation with access

   y.x    to the function value and
   y.dx   to the Jacobian.

Finally, the call

   x=gradientinit(intval([-3;7])); y=test(x)

evaluates "test" with automatic differentiation and interval artihmetic.
The function "typeadj" is necessary for interval evaluations. It adjusts
the type of constants to the type of the input argument "x". For example,
exp(1) is replaced by "exp(c1)" to ensure correct rounding.

The nonlinear system (Broyden's example) can be solved by

   X = verifynlss('test',[.6;3],'g')   .

The third parameter 'g' indicates function expansion by gradients; the call

   X = verifynlss('test',[.6;3],'s')

does the same using slopes. Use of slopes does not imply uniqueness of
the zero, but allows inclusion of clusters of zeros.

The programs and operators support vector and matrix notation in order to
diminish slow-down by interpretation. The user is encouraged to use matrix
and vector notation whereever possible. See, for example, Brent's example
in the function "test" in directory \intval (copy it from the end to top).
The call

   n=200;  X = verifynlss('test',10*ones(n,1),'g',1)

solves Brent's example with the initial approximation 10*ones(n,1) given in
his paper. The last parameter "1" tells the function "verifynlss" to
display intermediate information. It shows that 10 floating point and 1
interval iteration is used together with information on the residual. The
timing on an 1 Ghz Pentium III is 

      n    time for fl-pt approximation and verification
    -------------------------------------------
     200                0.3 sec
    1000                2.1 sec

and the sum of the two columns is the total computing time. Without the 
vector notation (see "test") this would not achievable.

For sample programs, consult

   verifylss
   verifynlss       
   verifyeig
   verifypoly

in directories \intval and \polynom.

For the homo ludens in you, try

   intlablogo                .

If you like Japan, look at intlablogo_jap .


If you have comments, suggestions, found bugs or similar, contact

    rump(at) tu-harburg.de       .





DISCLAIMER:   Extensive tests have been performed to ensure reliability
===========   of the algorithms. However, neither an error-free processor
              nor an error-free program can be guaranteed.


INTLAB LICENSE INFORMATION
==========================


Copyright (c) 1998 - 2010 Siegfried M. Rump
                          Institute for Scientific Computing
                          Hamburg University of Technology
                          Germany
                                       

All rights reserved.

===> INTLAB is free for private use and for purely academic purposes provided 
proper reference is given acknowledging that the software package INTLAB has 
been developed by Siegfried M. Rump, head of the Institute for Scientific Computing
at the Hamburg University of Technology, Germany. For any other use of INTLAB a 
license is required. 

===> For commercial use of INTLAB as well as its use in any connection with the 
development or distribution of a commercial product a license is required. 
Especially any use of a commercial product which needs INTLAB or parts of INTLAB 
to work properly requires an INTLAB license. This is independent of whether the 
commercial product is used privately or for commercial purposes. To obtain an 
INTLAB license contact Siegfried M. Rump. 

===> Neither the name of the university nor the name of the author may be used 
to endorse or promote products derived with or from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, WITHOUT LIMITATIONS, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE. 

