%% DEMOINTLAB  A little demonstration of INTLAB, the Matlab toolbox for Reliable Computing
% by Siegfried M. Rump

%% Welcome to INTLAB, the Matlab toolbox for Reliable Computing 
% Following are some examples how to use INTLAB, the Matlab toolbox for Reliable Computing. 
% Since I like pictures, some features using the graphical capabilities of INTLAB are demonstrated.
% Please consult "demo intval" to get acquainted to define and use intervals. See also
% the other demos like gradient, hessian, long, polynom, etc.
%
%% Possible overestimation by interval operations
% As a first example watch possible overestimation by interval operations. 
% Define a 2 x 2 rotation matrix Q rotating by 30 degrees and consider the
% two-dimensional box X with lower left vertex (1,1) and upper right vertex

format compact short infsup

phi = 30*pi/180;
Q = [ cos(phi) -sin(phi) ; sin(phi) cos(phi) ]

X = [ infsup(1,2) ; infsup(2,4) ]
 
 
%% The product Q*X: rotation of an interval
% Distinguish between the power set operation Q*X and the interval operation Q*X.
% By linearity, the power set result is an n-dimensional parallelogram. The interval
% product Q*X, however, must be an interval vector. By definition, it is the smallest interval
% vector containg all possible Q*x for x in X. In blue is the interval result
% of Q*X, while in red is the true range of Q*x for x in X.
 
Y = Q*X
plotintval(Y), hold on
x = [1 1 2 2;2 4 2 4]; y = Q*x;
index = convhull(y(1,:),y(2,:));
plot(0,0,'o')
plot(x(1,index),x(2,index),'k--')
plot(y(1,index),y(2,index),'r-o')
axis equal
hold off
 
%% Another example of interval overestimation: complex multiplication
% It is the principle of interval operations that they always produce a result which
% contains all possible results of operations between operands in the input operands.
% Another example of overestimation is complex multiplication. The data for the following
% picture is taken from Markus Grimmer, Wuppertal, presented at the Dagstuhl meeting on 
% "Numerical Software with Result Verification", January 2003. The blue circle is the
% (complex) interval result. Real intervals in INTLAB use inf-sup representation, while
% complex intervals use mid-rad representation.
%
 
kmax = 40; i = sqrt(-1); a=midrad(1,1); b=midrad(-1+i,1); 
plotintval(a*b); hold on
phi = linspace(0,2*pi,kmax);
[A,B] = meshgrid( mid(a)+rad(a)*exp(i*phi) , mid(b)+rad(b)*exp(i*phi) );
plot(A.*B)
hold off
 
%% A model problem for global optimization proposed by Griewank
% The amount of overestimation depends on many things, especially on the formulation of
% a function. As a rule thumb one might say that to diminish overestimation, a single
% variable should not occur too many times. However, there are many counterexamples
% to that. 
%
% Sometimes there is not much overestimation. As an example take the following well known
% test function for global optimization routines proposed by Griewank:
 
f = inline(' (x.^2+y.^2)/4000 + cos(x).*cos(y)/sqrt(2) + 1 ')
 
%% Find the minimum function value, kmax = 20
% The task for a global optimizer is to find the minimum function value for 
%    -60 <= x <= 60
%    -60 <= y <= 60
% To get an impression of the function we first take a look at a plot. The following is
% how it really happened to me.
 
kmax = 20;
[x,y] = meshgrid(linspace(-60,60,kmax));
surf(x,y,f(x,y))

%%  Find the minimum function value, kmax = 50
% The surface looks smooth and one might argue why should this be a serious 
% test function for global optimization. However, if we repeat the plot with 
% 50 meshpoints in x- and
% y-direction, then the function decovers its ugly behaviour.
 
kmax = 50;
[x,y] = meshgrid(linspace(-60,60,kmax));
surf(x,y,f(x,y))
 
%% Estimation of the range of the Griewank-function over the whole domain by taking the minimum and maximum in the nodes
% The first plot looks nice just because of advantageous plotting nodes. 
% We may try to estimate the range of the function over the whole domain
% by taking the minimum and maximum in the nodes. This is, by construction,
% always an inner inclusion. 
 
min(min(f(x,y))), max(max(f(x,y)))

%% Inclusion of the range by interval arithmetic
% A true inclusion is easily obtained by interval arithmetic.
%
% Note that the first estimate required 50*50 = 2500 function evaluation, whereas
% the validated inclusion requires only one interval evaluation. A closer inspection
% shows that the interval bounds are sharp up to two digits,
% so in this case there is not much overestimation. Unfortunately, this is not always typical.

X = infsup(-60,60); Y = X; f(X,Y)
 
%% A linear system in n=50 unknowns with random entries A_ij
% For a given linear system with data afflicted with tolerances one may want to 
% compute the smallest box around the set of all solution for data varying within
% the tolerances. A common approach for that is Monte Carlo analysis. 
% 
% Consider a randomly generated linear system with interval matrix A and right hand side b
% such that  A.mid * x = b  for x = ones(n,1). The following generates such a linear system
% in n=50 unknowns with random entries A_ij uniformly distributed within [-0.5,0.5] and
% each A_ij afflicted with a relative tolerance of 1e-5. 
 
n = 50; rand('state',0)
A = rand(n)-.5; A = midrad(A,1e-5*abs(A)); b = A.mid*ones(n,1);
c = cond(A.mid)
 
%% Inclusion interval vector X of solution of the interval linear system Ax=b
% The condition number c~1e2 suggests that we may expect a variation of the solution
% of the order c*1e-5 ~ 1e-3. 
%
% The following computes an inclusion X of the solution set of the 
% interval linear system Ax=b, that is for every C in A the true solution of Cx=b is in X.
% The interval box of the first two components X(1) versus X(2) is plotted.
 
X = verifylss(A,b); 
plotintval(X(1:2)), hold on
 
%% Try a Monte Carlo approach
% The size of the box is of the expected order, but is it an overestimation? Note that we 
% know the expected size of X only because we computed the condition number of the (midpoint)
% matrix. 
% 
% We may try a Monte Carlo approach. In order to obtain large variations of the solution
% we randomly choose only "vertex" matrices, that is matrices C such that C_ij is equal to A_ij.inf 
% or A_ij.sup. We solve  n  such linear systems and plot the first two 
% components of the result into the computed inclusion. 
%
% Note that this approach took of the order of n^4 operations. The random points suggest
% that the computed inclusion is quite an overestimation of the true range of the first
% two components. 
% 
 
for i=1:n
  a = A.mid+(rand(size(A))>.5).*rad(A); 
  x = a\b; 
  plot(x(1),x(2),'o');
end
 
%% Use sign information of an approximate inverse of A.mid to obtain the true range
% However, we may use sign information of an approximate inverse of A.mid to approximate
% extreme points of the true range of the solution. We do this for 
% four linear systems, and the result is plotted into our figure by a circle 'o'.
 
R = inv(A.mid);
for i=1:2
  for s=[-1 1]
    a = A.mid - s*(sign(R(i,:)')*ones(1,n)).*A.rad;
    x = a\b;
    plot(x(1),x(2),'o')
  end
end
 
%% Solution of 500 random vertex linear systems
% These four carefully choosen 'vertex' matrices show that the computed inclusion is 
% indeed sharp! We mention that it does not help very much to increase the number of
% random vertex matrices. The following computes the solution of 500 random vertex
% linear systems and plots the result of first and second component into the figure.
 
for i=1:500
  a = A.mid+(rand(size(A))>.5).*rad(A); 
  x = a\b; 
  plot(x(1),x(2),'o');
end
hold off

%% Solution of n  vertex linear systems
% As expected such an approach does not help very much. Note that we solved 500 linear
% systems of dimension 50. 
%
% The observed behaviour is not untypical. If we do the same for other components of the
% solution, for example the 13th and 38th, say, and solve again  n  vertex linear systems
% the result is as follows.
 
i1 = 13; i2 = 38;
plotintval(X([i1 i2])), hold on
for i=1:n
  a = A.mid+(rand(size(A))>.5).*rad(A); 
  x = a\b; 
  plot(x(i1),x(i2),'o');
end
for i=[i1 i2]
  for s=[-1 1]
    a = A.mid - s*(sign(R(i,:)')*ones(1,n)).*A.rad;
    x = a\b;
    plot(x(i1),x(i2),'o')
  end
end
hold off

%% Roots of the polynomial in the complex plane
% Consider the following polynomial of degree 20 with randomly generated coefficients
% uniformly distributed in [-1,1]. We plot the roots of the polynomial in the complex plane.
 
n = 20, rand('state',0)
P = randpoly(n)
r = roots(P); plot(real(r),imag(r),'o')

%% Calculate the range of P in the interval [-1,.7]
% The roots are not too far from the unit circle, as expected.
%
% Suppose we wish to calculate the range of P in the real interval [-1,.7]. The easiest
% way is interval evaluation. We plot the polynomial and the inclusion of the range
% computed by (naive) interval arithmetic.
%
 
a = -1; b = .7; 
X = infsup(a,b); 
Y = P{X}
plotpoly(P,a,b)
hold on

%% Bernstein coefficients
%
% Obviously, the computed inclusion Y is a true inclusion of the range, but an overestimation.
% We can improve using Bernstein coefficients. 
%
% Bernstein coefficients are computed with respect to the defining interval [0,1]. If we
% transform [a,b] into that interval and compute the Bernstein coefficients, then plot
% the convex hull of the latter are an inclusion of the range of the polynomial. 
  
B = bernsteincoeff(ptrans(P,a,b,0,1)); 
plotbernstein(B,a,b)
hold off

%% Improvement of Bernstein bounds
% The green line is the convex hull of the Bernstein coefficients. The upper bound is
% sharp, but the lower bound is still improvable. We may do so by splitting the interval
% into two parts, for example into [-1,-.2] and [-.2 .7], and transform both into [-1,1],
% one after the other.
 
plotpoly(P,a,b), hold on
a = -1; b = -.2; 
B = bernsteincoeff(ptrans(P,a,b,0,1)); 
plotbernstein(B,a,b), 
a = -.2; b = .7; 
B = bernsteincoeff(ptrans(P,a,b,0,1)); 
plotbernstein(B,a,b), hold off

%% Bernstein coefficients for interval polynomials
% Bernstein coefficients may be computed for interval polynomials as well. Following is an
% example plotted in the interval [-1,2]. The blue and read lines are the lower and upper
% bound of the interval polynomial, respectively, and the green lines form the Bernstein bounds.
 
P = polynom([infsup(1,2) infsup(-4,2) infsup(-3,1)])
 
a = -1; b = 2;
plotpoly(P,a,b), hold on
 
B = bernsteincoeff(ptrans(P,a,b,0,1)); 
plotbernstein(B,a,b)
hold off
 
%% The solution complex of a linear interval system
% The solution complex of a linear interval system is known to be convex in every orthant. 
% Here is a simple example.
  
A = [infsup(2,4) infsup(-1,0);infsup(0,1) infsup(1,2)]; 
b = [infsup(-2,5);1];
plotlinsol(A,b)
 
%% Convexity of the solution set of an interval linear system
% At first sight this looks like a contradiction. However, when plotting the axes, convexity
% in every orthant becomes clear.
 
plotlinsol(A,b,1)

%% A batman like example
% Some solution sets have nice shapes. Following is a batman-like example.
 
A = [infsup(2,4) infsup(-1,1);infsup(-1,1) infsup(2,4)]
b = [infsup(-3,3);.8],
plotlinsol(A,b)
title('batman')

%% Enjoy INTLAB
% INTLAB was designed and written by S.M. Rump, head of the Institute for Reliable Computing,
% Hamburg University of Technology. Suggestions are always welcome to rump (at) tuhh.de

 