#!/usr/bin/python

# Tests with cvxopt
#
# Preliminar to implementation in C++
#
from cvxopt import solvers, matrix, sparse, spmatrix, uniform, printing, mul, div, misc


printing.options['dformat'] = '%.1f'
printing.options['width'] = 15


# n is the number of vectors used to build the basis
# r is the rank (number of basis vectors)
# [Note that n >= r]

choice = "random"

if choice == "random":
    # Builds up a random example
    n = 3
    r = 2
    k = 1000
    Lambda = 0.1

    vectors = uniform(k, n)
    g = vectors.T * vectors

    a = uniform(n * r)
    
elif choice == "simple-1":
    n = 2
    r = 2
    Lambda = 1
    g = matrix([1,0,0,1], (2,2))
    a = matrix([1,0,0,.5], (n*r, 1))
else:
    print "Unknown choice [%s]" % choice
    sys.exit(1)
    
# --- Builds and solve


# n is the number of feature vectors
# Lambda is the regularisation coefficient
# Gram matrix g (size n x n)
# the coefficients a (size nr x 1)

# We have n*r + n variables

zero_n = spmatrix([],[],[],(n,n))
id_n = spmatrix(1., range(n), range(n))

# Construct P

print "Constructing P..."
l = []
for i in range(r+1):
    sl = []
    for j in range(r+1):
        if i < r and i == j: sl.append(g)
        else: sl.append(zero_n)
    l.append(sl)
P = sparse(l)

print "Constructing q..."
q = matrix(0., (n * r + n, 1))
for i in range(r):
    q[i*n:(i+1)*n,0] = - g * a[i*n:(i+1)*n,0]
q[n*r:n*r+n] = Lambda / 2.

print "Constructing G (%d x %d) and q" % (2 * n*r, n*r + n)
id_nr = spmatrix(1., range(n*r), range(n*r))
id_col = []
for i in range(r):
    id_col.append(-id_n)
id_col = sparse([id_col])
G = sparse([ [ -id_nr, id_nr ], [id_col, id_col ] ])
h = matrix(0., (2*n*r,1))


dims = {"l": h.size[0], "q": 0, "s": 0}

# --- Custom

chol2 = misc.kkt_chol2(G, dims, spmatrix([], [], [], (0, q.size[0])))

def Fchol2(W):

    """
    Uses the Cholesky factorisation, in order to see how the 
    optimisation works
    """
    
    solve = chol2(W,P)
	
#    di = W['di']
    print W['di']
    
    def f(x, y, z):
        print "** SOLVING KKT **"
        return solve(x,y,z)

    return f


def F(W):

    """
    Returns a function f(x, y, z) that solves the KKT conditions

    """

    # Factor A = 4*P'*D*P where D = d1.*d2 ./(d1+d2) and
    # d1 = di[:m].^2, d2 = di[m:].^2.
    di = W['di']
    
    mC, mD = di[0:n*r]**2, di[n*r:2*n*r]**2

    def f(x, y, z):
        """
        On entry bx, bz are stored in x, z.  On exit x, z contain the solution,
        with z scaled: z./di is returned instead of z.
        """

        # Maps to our variables x,y,z and t
        a = x[0:n*r]
        b = x[n*r:n*r + n]
        c = z[0:n*r]
        d = z[n*r: 2*n*r]

        # Computes f = a + C c  - D d
        f = a + mul(mC, c) - mul(mD, d)

        # Maps back and 
        # x = bx by
        # z = W (bz bt)

        # x = [bx by]
        # z[m:] = mul(di[m:], bz bt)

    return f

# --- Init values (x, s and y, z)

initvals = 

# --- Solving 

print "   [[[Solving system...]]]"
sol=solvers.coneqp(P, q, G, h, kktsolver=Fchol2)
print sol['status']
if (n * r < 10): print sol['x']

print "\n\n   [[[Solving with default]]]"
sol = solvers.coneqp(P, q, G, h)
print sol['status']

print "Done"