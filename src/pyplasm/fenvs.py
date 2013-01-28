import sys
import types
import math

from hpc import *

PI = math.pi
SIN = math.sin
SINH = math.sinh
ASIN = math.asin
COS = math.cos
COSH = math.cosh
ACOS = math.acos
TAN = math.tan
TANH = math.tanh
ATAN = math.atan
ATAN2 = math.atan2
SQRT = math.sqrt
EXP = math.exp
LN = math.log
CEIL = math.ceil
FLOOR = math.floor
ABS = abs
CHAR = chr
ORD = ord
FALSE = False
TRUE = True
LEN = len
And=all
AND=all 
Or=any
OR=any
greater=max
BIGGEST=max
SMALLEST = min


# C
def C(fun):
  return lambda arg1: lambda arg2: fun([arg1,arg2])

# ATAN2
def ATAN2(l): 
	return math.atan2(l[1],l[0])

# MOD
def MOD(l): 
	return float(l[0] % l[1])

# CAT
def CAT(args):  
  """
  >>> CAT([[1,2],[3,4]])
  [1, 2, 3, 4]
  """
  return reduce(lambda x, y: x + y, args)

# INV
def INV(mat):
  """
  >>> INV([[1,0],[0,1]])
  [[1.0, 0.0], [0.0, 1.0]]
  """
  return Matrix(mat).invert().toList()

# EVERY
def EVERY(predicate, iterable):
  """
  >>> EVERY(lambda x: x>=0,[1,2,3,4])
  True
  >>> EVERY(lambda x: x>0,[1,-2,3,4])
  False
  """  
  for x in iterable:
    if not(predicate(x)):return False
  return True

# CURRY
def CURRY(fn, *cargs, **ckwargs):
  def call_fn(*fargs, **fkwargs):
    d = ckwargs.copy()
    d.update(fkwargs)
    return fn(*(cargs + fargs), **d)
  return call_fn

# AND
def AND(list):
  """ 
  >>> AND([True,True])
  True
  >>> AND([True,False])
  False
  """
  for i in list:
    if not(i):return False
  return True

# ID
def ID(anyValue):
  """
  >>> ID(True)
  True
  """
  return anyValue

# K
def K(AnyValue):
  """
  >>> K(1)(2)
  1
  """
  def K0(obj): return AnyValue
  return K0

TT = K(TRUE)


# DISTL
def DISTL(args):
  """
  >>> DISTL([1,[2,3,4]])
  [[1, 2], [1, 3], [1, 4]]
  """
  Element, List = args
  return [ [Element, e] for e in List ]
  
# DISTR
def DISTR(args):
  """
  >>> DISTR([[1,2,3],0])
  [[1, 0], [2, 0], [3, 0]]
  """
  List, Element = args
  return [ [e,Element] for e in List ]

# COMP
def COMP(Funs):
  """
  >>> COMP([lambda x: x+[3],lambda x: x+[2],lambda x: x+[1]])([0])
  [0, 1, 2, 3]
  """
  def compose(f,g):
    def h(x): return f(g(x))
    return h
  return reduce(compose,Funs)


# AA
def AA(f):
  """
  >>> AA(lambda x: x*2)([1,2,3])
  [2, 4, 6]
  """
  def AA0(args): return map(f, args)
  return AA0


# Eq 
def Eq(x,y): 
  return x == y

# EQ
def EQ(List):
  """
  >>> EQ([1,1,1])
  True
  >>> EQ([1,1,2])
  False
  """
  for i in List:
    if not i==List[0]: return False
  return True

# NEQ
def NEQ(List): 
  """
  >>> NEQ([1,1,2])
  True
  >>> NEQ([1,1,2/2])
  False
  """
  return not EQ(List)

def LT(a):return lambda b: b<a
def LE(a):return lambda b: b<=a
def GT(a):return lambda b: b>a
def GE(a):return lambda b: b>=a


def ISGT(args):A , B = args;return GT(A)(B)
def ISLT(args):A , B = args;return LT(A)(B)
def ISGE(args):A , B = args;return GE(A)(B)
def ISLE(args):A , B = args;return LE(A)(B)

# BIGGER
def BIGGER(args): 
	A , B = args
	return A if A>=B else B

# SMALLER
def SMALLER(args):
	A , B = args
	return A if A<=B else B

# FILTER
def FILTER(predicate):
  """
  >>> FILTER(LE(0))([-1,0,1,2,3,4])
  [-1, 0]
  >>> FILTER(GE(0))([-1,0,1,2,3,4])
  [0, 1, 2, 3, 4]
  """
  def FILTER0(sequence):
  	ret=[]
  	for item in sequence:
  		if predicate(item): ret+=[item]
  	return ret
  return FILTER0


# APPLY 
def APPLY(args):
  """
  APPLY([lambda x:x*2,2])
  4
  """
  f,x = args
  return apply(f,[x])


# INSR
def INSR(f):
  """
  >>> INSR(lambda x: x[0]-x[1])([1,2,3])
  2
  """
  def INSR0(seq):
    length=len(seq)
    res = seq[-1]
    for i in range(length - 2,-1,-1): res = f([seq[i], res])
    return res
  return INSR0 


# INSL
def INSL(f):
  """
  >>> INSL(lambda x: x[0]-x[1])([1,2,3])
  -4
  """
  def INSL0(seq):
    res = seq[0]
    for item in seq[1:]: res = f([res,item])
    return res
  return INSL0   

   
# CONS
def CONS(Funs): 
  """
  >>> CONS([lambda x : x+1,lambda x : x+2])(0)
  [1, 2]
  """
  return lambda x : [f(x) for f in Funs]


# IF
def IF(funs):
  """
  >>> IF([lambda x: x, K(True),K(False)])(True)
  True
  >>> IF([lambda x: x, K(True),K(False)])(False)
  False
  """
  def IF1(arg):
    f1, f2, f3 = funs
    return f2(arg) if f1(arg) else f3(arg)
  return IF1

# LIFT  
def LIFT(f):
  """
  >>> LIFT(ADD)([math.cos,math.sin])(PI/2)
  1.0
  """
  return lambda funs: COMP([f, CONS(funs)])

# RAISE
def RAISE(f):
  """
  >>> RAISE(ADD)([1,2])
  3
  >>> RAISE(ADD)([math.cos,math.sin])(PI/2)
  1.0
  """
  def RAISE0(args):
    return IF([ISSEQOF(ISFUN), LIFT(f), f])(args)
  return RAISE0

# ISNUM 
def ISNUM(x): 
  """ 
  >>> ISNUM(0.0)
  True
  """
  return  isinstance(x, int) or isinstance(x, long) or  isinstance(x, float) or  isinstance(x, complex) 
		
# NUMBER_FROM_ZERO_TO_ONE_P
def NUMBER_FROM_ZERO_TO_ONE_P(x):  
  return  ISNUM(x) and x >= 0 and x <= 1

# ISFUN
def ISFUN(x): 
  """
  >>> ISFUN(lambda x:x) and ISFUN(abs)
  True
  >>> ISFUN(3)
  False
  """
  return callable(x)


def ISNUMPOS(x): return ISNUM(x) and x > 0
def ISNUMNEG(x): return ISNUM(x) and x < 0
def ISINT(x): return isinstance(x,int)
def ISLONG(x): return isinstance(x,long)
def ISINTPOS(x): return isinstance(x,int) and x > 0
def ISINTNEG(x): return isinstance(x,int) and x < 0
def ISREAL(x): return isinstance(x,float)
def ISREALPOS(x): return isinstance(x,float) and x > 0
def ISREALNEG(x): return isinstance(x,float) and x < 0
def ISCOMPLEX(x): return isinstance(x,complex)
def ISSEQ(x): return isinstance(x,list)
def ISSEQ_NOT_VOID(x): return True if(isinstance(x,list) and(len(x) >= 1)) else False
def ISNULL(x): return isinstance(x,list) and len(x) == 0
def ISBOOL(x): return isinstance(x,bool)
def ISPAIR(x): return isinstance(x,list) and len(x) == 2
def ISCHAR(x): return isinstance(x,str) and len(x) == 1
def ISSTRING(x): return isinstance(x,str)
def ISEVEN(N): return isinstance(N,int) and(N%2)==1
def ISNAT(N): return isinstance(N,int) and N>=0
def ISZERO(N): return N==0
def ISODD(N): return not ISEVEN(N)
  
# ISSEQOF
def ISSEQOF(type_checker):
  """
  >>> ISSEQOF(lambda x: isinstance(x,int))([1,2,3])
  True
  >>> ISSEQOF(lambda x: isinstance(x,int))([1,2,3.0])
  False
  """
  def ISSEQOF0(arg):
    if not isinstance(arg, list): return False
    for item in arg:
       if not type_checker(item): return False
    return True
  return ISSEQOF0

# ISMAT
def ISMAT(x): 
  """
  >>> ISMAT([[1,2],[3,4]]) and ISMAT(numpy.identity(4)) and ISMAT(Matrix(3))
  True
  >>> ISMAT([1,2,3,4]))
  False
  """
  return (isinstance(x, list) and AND([isinstance(e,list) for e in x])) or (isinstance(x,numpy.matrix)) or (isinstance(x,Matrix))

# VECTSUM
def VECTSUM(vects):  
  """
  >>> VECTSUM([[10,11,12],[0,1,2],[1,1,1]])
  [11, 13, 15]
  """
  return map(sum,zip(*vects))
  
# VECTDIFF
def VECTDIFF(vects): 
  """
  >>> VECTDIFF([[10,11,12],[0,1,2],[1,1,1]])
  [9, 9, 9]
  """
  return map(lambda l: l[0]-sum(l[1:]),zip(*vects))

# IS_PLASM_POINT_2D
def IS_PLASM_POINT_2D(obj): 
	return isinstance(obj, list) and(len(obj) == 2) 


# MEANPOINT
def MEANPOINT(points):
  """
  >>> MEANPOINT([[0,0,0],[1,1,1],[2,2,2]])
  [1.0, 1.0, 1.0]
  """
  coeff=1.0/len(points)
  return map(lambda x:coeff*x,VECTSUM(points))


# SUM
def SUM(args):

  """
  >>> SUM([1,2,3])
  6
  >>> SUM([[1,2,3],[4,5,6]])
  [5, 7, 9]
  >>> SUM([[[1,2],[3,4]],[[10,20],[30,40]],[[100,200],[300,400]] ])
  [[111, 222], [333, 444]]
  """
  
  if isinstance(args,list) and ISPOL(args[0]): 
  	return UNION(args)
  
  if isinstance(args,list) and ISNUM(args[0]): 
  	return sum(args)
  
  if isinstance(args,list) and isinstance((args[0]),list): 
  
  	#matrix sum
  	if isinstance(args[0][0],list):
  		return AA(VECTSUM)(zip(*args))
  
  	# vector sum
  	else:
  		return VECTSUM(args)
  
  raise Exception("\'+\' function has been applied to %s!" % repr(args))    

ADD = SUM

# DIFF
def DIFF(args):

  """
  >>> DIFF(2)
  -2
  >>> DIFF([1,2,3])
  -4
  >>> DIFF([[1,2,3],[1,2,3]])
  [0, 0, 0]
  """
  
  if isinstance(args,list) and ISPOL(args[0]): 
  	return DIFFERENCE(args)
  
  if ISNUM(args): 
  	return -1 * args
  
  if isinstance(args,list) and  ISNUM(args[0]): 
  		return reduce(lambda x,y: x - y, args)
  
  if isinstance(args,list) and  isinstance(args[0],list): 
  
  	#matrix difference
  	if isinstance(args[0][0],list):
  		return AA(VECTDIFF)(zip(*args))
  
  	# vector diff
  	else:
  		return VECTDIFF(args)
  
  raise Exception("\'-\' function has been applied to %s!" % repr(args))   

# PROD 
def PROD(args):  
  """
  >>> PROD([1,2,3,4])
  24
  >>> PROD([[1,2,3],[4,5,6]])
  32
  """  
  if isinstance(args,list) and ISPOL(args[0]): return  POWER(args)
  if isinstance(args,list) and ISSEQOF(ISNUM)(args): return reduce(lambda x,y: x * y, args)
  if isinstance(args,list) and len(args) == 2 and ISSEQOF(ISNUM)(args[0]) and ISSEQOF(ISNUM)(args[1]): return  sum([a*b for a,b in zip(args[0],args[1])])
  raise Exception("PROD function has been applied to %s!" % repr(args))

# SQR
SQR = RAISE(RAISE(PROD))([ID,ID])


# DIV 
def DIV(args):
  """
  >>> DIV([10.0,2.0,5.0])
  1.0
  """
  return reduce(lambda x,y: x/float(y), args)

# REVERSE
def REVERSE(List): 
  """
  >>> REVERSE([1,2,3])
  [3, 2, 1]
  >>> REVERSE([1])
  [1]
  """
  ret=[x for x in List]
  ret.reverse()
  return ret


# TRANS
def TRANS(List): 
  """
  >>> TRANS([[1,2],[3,4]])
  [[1.0, 3.0], [2.0, 4.0]]
  """
  return Matrix(List).transpose().toList()

def FIRST(List): return List[0]
def LAST(List): return List[-1]
def TAIL(List): return List[1:]
def RTAIL(List): return List[:-1]
  
# AR
def AR(args): 
  """
  >>> AR([[1,2,3],0,])
  [1, 2, 3, 0]
  """
  return args[0] + [args[-1]]
  
# AL
def AL(args): 
  """
  >>> AL([0,[1,2,3]])
  [0, 1, 2, 3]
  """
  return [args[0]] + args[-1]

# LIST
def LIST(x): 
  return [x]

# Not
def Not(x): 
	return not x
	
# NOT
NOT = AA(Not) 


# PROGRESSIVESUM
def PROGRESSIVESUM(arg):
  """
  >>> PROGRESSIVESUM([1,2,3,4])
  [1, 3, 6, 10]
  """
  ret,acc=[],0
  for value in arg:
  	acc+=value
  	ret+=[acc]
  return ret


# INTSTO
def INTSTO(n): 
  """
  >>> INTSTO(5)
  [1, 2, 3, 4, 5]
  """
  return range(1,n+1)

# FROMTO
def FROMTO(args):  
  """ 
  >>> FROMTO([1,4])
  [1, 2, 3, 4]
  """ 
  return range(args[0],args[-1]+1)


# PLASM  selectors
def SEL(n): 
  """
  >>> S1([1,2,3])
  1
  >>> S2([1,2,3])
  2
  """
  return lambda lista: lista[int(n)-1]

S1 = SEL(1)
S2 = SEL(2)
S3 = SEL(3)
S4 = SEL(4)
S5 = SEL(5)
S6 = SEL(6)
S7 = SEL(7)
S8 = SEL(8)
S9 = SEL(9)
S10 = SEL(10)

# N  repeat operators 
def N(n): 
  """
  >>> N(3)(10)
  [10, 10, 10]
  """
  return lambda List: [List]*int(n)

# DIESIS
def DIESIS(n): 
  """
  >>> DIESIS(3)(10)
  [10, 10, 10]
  """
  return lambda List: [List]*int(n)

# NN
def NN(n): 
  """
  >>> NN (3)([10])
  [10, 10, 10]
  """
  return lambda List: List*int(n)

# DOUBLE_DIESIS
def DOUBLE_DIESIS(n): 
  """
  >>> DOUBLE_DIESIS(3)([10])
  [10, 10, 10]
  """
  return lambda List: List*int(n)


# AS
def AS(fun):
  """
  >>> AS(SEL)([1,2,3])([10,11,12])
  [10, 11, 12]
  """
  return lambda args: COMP([CONS, AA(fun)])(args)

# AC
def AC(fun):
  """
  >>> AC(SEL)([1,2,3])([10,11,[12,[13]]])
  13
  """
  return lambda args: COMP(AA(fun)(args))

# CHARSEQ
def CHARSEQ(String):
  """
  >>> CHARSEQ('hello')
  ['h', 'e', 'l', 'l', 'o']
  """
  return [String[i] for i in range(len(String))]

# STRING
def STRING(Charseq): 
  """
  >>> STRING(CHARSEQ('hello'))
  'hello'
  """
  return reduce(lambda x,y: x+y,Charseq)

# RANGE
def RANGE(Pair):
  """
  >>> RANGE([1,3])
  [1, 2, 3]
  >>> RANGE([3,1])
  [3, 2, 1]
  """
  if((Pair[-1]-Pair[0]) >= 0 ): return range(Pair[0], Pair[-1] + 1)
  return range(Pair[0], Pair[-1] - 1, -1)

# SIGN
def SIGN(Number): 
  """
  >>> SIGN(10)
  1
  >>> SIGN(-10)
  -1
  """
  return +1 if Number>=0 else -1

# PRINT
def PRINT(AnyValue):
  print(AnyValue)
  return AnyValue

# PRINTPOL
def PRINTPOL(PolValue):
  print PolValue
  sys.stdout.flush()
  return PolValue

# TREE
def TREE(f):
  """
  >>> TREE(lambda x: x[0] if x[0]>=x[-1] else x[-1])([1,2,3,4,3,2,1])
  4
  >>> TREE(lambda x: x[0] if x[0]>=x[-1] else x[-1])([1,2,3,4,3,2])
  4
  """
  def TREE_NO_CURRIED(fun,List):
    length = len(List)
    if length == 1: return List[0]
    k = int(len(List)/2)
    return f([TREE_NO_CURRIED(f, List[:k])] + [TREE_NO_CURRIED(f, List[k:])])
  return lambda x: TREE_NO_CURRIED(f,x)
  

# MERGE
def MERGE(f):
  """
  >>> MERGE(lambda x,y:x>y)([[1,3,4,5],[2,4,8]])
  [1, 2, 3, 4, 4, 5, 8]
  """
  def MERGE_NO_CURRIED(f, List):
    list_a, list_b = List
    if len(list_a) == 0: return list_b
    if len(list_b) == 0: return list_a
    res = f(list_a[0], list_b[0])
    if not(res):
      return [list_a[0]] + MERGE_NO_CURRIED(f,[list_a[1:], list_b])
    else:
      return [list_b[0]] + MERGE_NO_CURRIED(f,[list_a, list_b[1:]])
  return lambda x: MERGE_NO_CURRIED(f,x)


# CASE
def CASE(ListPredFuns):
  """
  >>> CASE([[LT(0),K(-1)],[C(EQ)(0),K(0)],[GT(0),K(+1)]])(-10)
  -1
  >>> CASE([[LT(0),K(-1)],[C(EQ)(0),K(0)],[GT(0),K(+1)]])(0)
  0
  >>> CASE([[LT(0),K(-1)],[C(EQ)(0),K(0)],[GT(0),K(+1)]])(10)
  1
  """
  def CASE_NO_CURRIED(ListPredFuns, x):
    for p in ListPredFuns:
       if p[0](x): return p[1](x)
  return lambda arg: CASE_NO_CURRIED(ListPredFuns, arg)


# PERMUTATIONS
def PERMUTATIONS(SEQ):
  """
  >>> len(PERMUTATIONS([1,2,3]))
  6
  """
  if len(SEQ)<=1: return [SEQ]
  ret=[]
  for i in range(len(SEQ)):
  	element =SEQ[i]
  	rest  =PERMUTATIONS(SEQ[0:i] + SEQ[i+1:])
  	for r in rest: ret+=[[element] + r]
  return ret



# IDNT
def IDNT(N):
  """
  >>> IDNT(0)
  []
  >>> IDNT(2)
  [[1.0, 0.0], [0.0, 1.0]]
  """
  return Matrix(N).toList()


# SPLIT_2PI
def SPLIT_2PI(N):
  """
  >>> abs(SPLIT_2PI(4)[2]-PI)<1e-4
  True
  """
  delta=2*PI/N
  return [i*delta for i in range(0,N)]


# VECTPROD 
def VECTPROD(args):
  """
  >>> VECTPROD([[1,0,0],[0,1,0]])
  [0.0, 0.0, 1.0]
  >>> VECTPROD([[0,1,0],[0,0,1]])
  [1.0, 0.0, 0.0]
  >>> VECTPROD([[0,0,1],[1,0,0]])
  [0.0, 1.0, 0.0]
  """
  return numpy.cross([float(c) for c in args[0]],[float(c) for c in args[1]]).tolist()


# VECTNORM
def VECTNORM(u):
  """
  >>> VECTNORM([1,0,0])
  1.0
  """
  return math.sqrt(sum(x*x for x in u))


# INNERPROD
def INNERPROD(args):
  """
  >>> INNERPROD ([[1,2,3],[4,5,6]])
  32.0
  """
  return COMP([COMP([RAISE(SUM), AA(RAISE(PROD))]), TRANS])(args)

# SCALARVECTPROD
def SCALARVECTPROD(args):
  """
  >>> SCALARVECTPROD([2,[0,1,2]])
  [0, 2, 4]
  >>> SCALARVECTPROD([[0,1,2],2])
  [0, 2, 4]
  """
  s,l=args
  if not isinstance(l,list): s,l=l,s
  return [s*l[i] for i in range(len(l))]

# MIXEDPROD
def MIXEDPROD(args):
  """
  >>> MIXEDPROD([[1.0,0.0,0.0],[0.0,1.0,0.0],[0.0,0.0,1.0]])
  1.0
  """
  A , B , C = args
  return INNERPROD([VECTPROD([A,B]),C])

# UNITVECT
def UNITVECT(V):
  """
  >>> UNITVECT([2,0,0])
  [1.0, 0.0, 0.0]
  >>> UNITVECT([1,1,1])==UNITVECT([2,2,2])
  True
  """
  norm=VECTNORM(V)
  return [coord/norm for coord in V]
	
# DIRPROJECT
def DIRPROJECT(E):
  """
  >>> DIRPROJECT([1.0,0.0,0.0])([2.0,0.0,0.0])
  [2.0, 0.0, 0.0]
  >>> DIRPROJECT([1.0,0.0,0.0])([0.0,1.0,0.0])
  [0.0, 0.0, 0.0]
  """
  E=UNITVECT(E)
  def DIRPROJECT0(V):
  	return SCALARVECTPROD([(INNERPROD([E,V])),E])
  return DIRPROJECT0

# ORTHOPROJECT
def ORTHOPROJECT(E):
  """
  >>> ORTHOPROJECT([1.0,0.0,0.0])([1.0,1.0,0.0])
  [0.0, 1.0, 0.0]
  """
  def ORTHOPROJECT0(V):
    return VECTDIFF([V,DIRPROJECT((E))(V)])
  return ORTHOPROJECT0


# FACT  
def FACT(N):
  """
  >>> FACT(4)
  24
  >>> FACT(0)
  1
  """
  return PROD(INTSTO(N)) if N>0 else 1


ISREALVECT = ISSEQOF(ISREAL)
ISFUNVECT = ISSEQOF(ISFUN)
ISVECT = COMP([OR, CONS([ISREALVECT, ISFUNVECT])])
ISPOINT = ISVECT
ISPOINTSEQ = COMP([AND, CONS([ISSEQOF(ISPOINT), COMP([EQ, AA(LEN)])])])
ISMAT = COMP([AND, CONS([COMP([OR, CONS([ISSEQOF(ISREALVECT), ISSEQOF(ISFUNVECT)])]), COMP([EQ, AA(LEN)])])])
ISSQRMAT = COMP([AND, CONS([ISMAT, COMP([EQ, CONS([LEN, COMP([LEN, S1])])])])])
def ISMATOF(ISTYPE): return COMP([COMP([AND, AR]), CONS([COMP([AA(ISTYPE), CAT]), COMP([ISMAT,(COMP([AA, AA]))((K(1)))])])])


# CHOOSE(binomial factors)
def CHOOSE(args):
  """
  >>> CHOOSE([7,3])
  35.0
  """
  N , K = args
  return FACT(N)/float(FACT(K)*FACT(N-K))

# TRACE
def TRACE(MATRIX):
  """
  >>> TRACE([[5,0],[0,10]])
  15.0
  """
  acc=0.0
  dim=len(MATRIX)
  for i in range(dim):acc+=MATRIX[i][i]
  return acc


# PASCALTRIANGLE
def PASCALTRIANGLE(N):
  """
  >>> PASCALTRIANGLE(4)
  [[1], [1, 1], [1, 2, 1], [1, 3, 3, 1], [1, 4, 6, 4, 1]]
  """
  if(N==0): return [[1]]
  if(N==1): return [[1],[1,1]]
  prev=PASCALTRIANGLE(N-1)
  last_row=prev[-1]
  cur=[1]+[last_row[i-1]+last_row[i]  for i in range(1,len(last_row))] + [1]
  return prev+[cur]


# MATHOM
def MATHOM(M):
  """
  >>> MATHOM([[1,2],[3,4]])
  [[1.0, 0.0, 0.0], [0.0, 1.0, 2.0], [0.0, 3.0, 4.0]]
  """
  ret=Matrix(len(M)+1)
  ret[1:,1:]=M
  return ret.toList()


# SCALARMATPROD
def SCALARMATPROD(args):
  """
  >>> SCALARMATPROD([10.0,[[1,2],[3,4]]])
  [[10.0, 20.0], [30.0, 40.0]]
  """
  scalar,mat=float(args[0]),args[1]
  return [[scalar*coord for coord in row] for row in mat]

# MATDOTPROD
def MATDOTPROD(args):
  """
  >>> MATDOTPROD([[[1,2],[3,4]],[[1,0],[0,1]]])
  5.0
  """ 
  return COMP([INNERPROD, AA(CAT)])(args)

# ORTHO
def ORTHO(matrix):
  """
  >>> ORTHO([[1,0],[0,1]])
  [[1.0, 0.0], [0.0, 1.0]]
  """
  return SCALARMATPROD([0.5,SUM([matrix,TRANS(matrix)])])

# SKEW
def SKEW(matrix):
  """
  >>> SKEW ([[1,0],[0,1]])
  [[0.0, 0.0], [0.0, 0.0]]
  """
  return SCALARMATPROD([0.5,DIFF([matrix,TRANS(matrix)])])


# SUBSEQ
def SUBSEQ(I_J):
	def SUBSEQ0(SEQ):
		return SEQ[I_J[0]-1:I_J[1]]
	return SUBSEQ0


# VECT2MAT
def VECT2MAT(v):
	n=len(v)
	return [[0 if r!=c else v[r] for c in range(n)] for r in range(n)]
	

# VECT2DTOANGLE
def VECT2DTOANGLE(v):
	v=UNITVECT(v)
	return math.acos(v[0])*(1 if v[1]>=0 else -1)

# CART
def CART(l):
  """
  >>> len(CART([[1, 2, 3], ['a', 'b'],[10,11]]))
  12
  """
  CART2 = COMP([COMP([CAT, AA(DISTL)]), DISTR])
  F1 = AA((AA(CONS([ID]))))
  return TREE(COMP([AA(CAT), CART2]))(F1(l))

# POWERSET
def POWERSET(l):
  """
  >>> len(POWERSET([1, 2, 3]))
  8
  """
  return COMP([COMP([AA(CAT), CART]), AA((CONS([CONS([ID]), K([])])))])(l)

# ARC
def ARC(args):
	degrees , cents = args
	return PI*(degrees+cents)/(100.0*180.0)



# ===================================================
# GEOMETRIC FUNCTION 
# ===================================================

# ISPOL
def ISPOL(obj):
  """
  >>> ISPOL(Hpc.cube(2))
  True
  """
  return isinstance(obj, Hpc)

# PRINT POL
def PRINTPOL(obj):
  print obj
  return obj

def PRINT(obj):
  print obj
  return obj

# view
def VIEW(obj):
	obj.view()
	
# GRID
def GRID(sequence):
  """
  >>> GRID([1,-1,1]).box()
  Box([0.0], [3.0])
  >>> GRID([-1,1,-1,1]).box()
  Box([1.0], [4.0])
  """  
  cursor,points,hulls=(0,[[0]],[])
  for value in sequence:
    points = points + [[cursor + abs(value)]] 
    if value>=0: hulls += [[len(points)-2,len(points)-1]]
    cursor = cursor + abs(value)
  return  Hpc.mkpol(points, hulls)   

QUOTE = GRID


# Q
Q = COMP([QUOTE, IF([ISSEQ, ID, CONS([ID])])])

# INTERVALS 
def INTERVALS(A):
  """
  >>> INTERVALS(10)(8).box()
  Box([0.0], [10.0])
  """  
  def INTERVALS0(N):
    return QUOTE([float(A)/float(N) for i in range(N)])
  return INTERVALS0

# CUBOID
def CUBOID(sizes_list):
  """
  >>> CUBOID([1,2,3]).box()
  Box([0.0, 0.0, 0.0], [1.0, 2.0, 3.0])
  """  
  return Hpc.cube(len(sizes_list)).scale(sizes_list)

# CUBE
def CUBE(side):
  return CUBOID([side, side, side])

# HEXAHEDRON
HEXAHEDRON=Hpc.cube(3,-1.0/math.sqrt(3.0),+1.0/math.sqrt(3.0))


# SIMPLEX
def SIMPLEX(dim):
  """
  >>> SIMPLEX(3).box()
  Box([0.0, 0.0, 0.0], [1.0, 1.0, 1.0])
  """  
  return Hpc.simplex(dim)

# RN
def RN(pol): 
  """
  >>> RN(Hpc.cube(2))
  2
  """  
  return pol.dim()
  
# DIM
def DIM(pol):
  """
  >>> DIM(Hpc.cube(2))
  2
  """   
  return pol.dim()

# ISPOLDIM
def ISPOLDIM(dims):
  def ISPOLDIM1(pol):
    d = dims[0]
    n = dims[1]
    return (d == DIM(pol)) and(n == RN(pol))
  return ISPOLDIM1

# MKPOL
def MKPOL(args_list):
  """
  >>> MKPOL([[[0,0],[1,0],[1,1],[0,1]],[[1,2,3,4]],None]).box()
  Box([0.0, 0.0], [1.0, 1.0])
  """
  points, hulls, pols = args_list
  return Hpc.mkpol(points,[[cell-1 for cell in hull] for hull in hulls])

# MK
MK = COMP([MKPOL, CONS([LIST, K([[1]]), K([[1]])])])

# CONVEXHULL
def CONVEXHULL(points):
  return MKPOL([points, [range(1,len(points)+1)], [[1]]])

# UKPOL
def UKPOL(pol):  
  points,hulls=pol.ukpol() 
  hulls=[[idx+1 for idx in hull] for hull in hulls] 
  return [points, hulls, [[1]]]

# UK
UK = COMP([COMP([S1, S1]), UKPOL])

# OPTIMIZE
def OPTIMIZE(pol): 
  return pol

# UKPOLF
if False:
  def UKPOLF(pol):  
    """
    >>> temp=UKPOLF(Hpc.cube(3))
    >>> print len(temp[0]),len(temp[0][0]), len(temp[1]), len(temp[1][0]), len(temp[2])
    6 4 1 6 1
    """
    f = StdVectorFloat()
    u = StdVectorStdVectorInt()
    pointdim=Plasm.ukpolf(pol, f, u) 
    faces=[]
    for i in xrange(0, len(f), pointdim+1):
    	faces+=[[f[i] for i in range(i,i+pointdim+1)]]
    hulls=map(lambda x: [i + 1 for i in x], u)
    pols=[[1]]
    return  [faces, hulls, pols]
  

# TRANSLATE
def TRANSLATE(axis):
  """
  >>> (TRANSLATE(3)(2)(Hpc.cube(2))).box()
  Box([0.0, 0.0, 2.0], [1.0, 1.0, 2.0])
  >>> (TRANSLATE([1,3])([1,2])(Hpc.cube(2))).box()
  Box([1.0, 0.0, 2.0], [2.0, 1.0, 2.0])
  """  
  def TRANSLATE1(axis,values):
    def TRANSLATE2(axis,values,pol):
      axis  = [axis  ] if ISNUM(axis  ) else axis
      values= [values] if ISNUM(values) else values
      vt= [0.0]*max(axis)
      for a,t in zip(axis,values): vt[a-1]=t
      return pol.translate(vt)
    return lambda pol: TRANSLATE2(axis,values,pol)  
  return lambda values: TRANSLATE1(axis,values)
T = TRANSLATE

# SCALE
def SCALE(axis):
  """
  >>> (SCALE(3)(2)(Hpc.cube(3))).box()
  Box([0.0, 0.0, 0.0], [1.0, 1.0, 2.0])
  >>> (SCALE([3,1])([4,2])(Hpc.cube(3))).box()
  Box([0.0, 0.0, 0.0], [2.0, 1.0, 4.0])
  """  
  def SCALE1(axis,values):
    def SCALE2(axis,values,pol):
      axis  = [axis  ] if ISNUM(axis  ) else axis
      values= [values] if ISNUM(values) else values
      vs=[1.0]*max(axis)
      for a,t in zip(axis,values):vs[a-1]=t
      return pol.scale(vs)
    return lambda pol: SCALE2(axis,values,pol)  
  return lambda values: SCALE1(axis,values)
S = SCALE


# ROTATE
def ROTATE(plane_indexes):
  """
  >>> (ROTATE([1,2])(PI/2)(Hpc.cube(2))).box().fuzzyEqual(Box([-1,0],[0,+1]))
  True
  """
  def ROTATE1(angle):
    def ROTATE2(pol):
      dim = max(plane_indexes)
      return pol.rotate(plane_indexes[0] , plane_indexes[1], angle)
    return ROTATE2  
  return ROTATE1
R = ROTATE

# SHEARING
def SHEARING(i):
  def SHEARING1(shearing_vector_list):
    def SHEARING2(pol):
      raise Exception("shearing not implemented!")
    return SHEARING2  
  return SHEARING1
H = SHEARING
     
# MAT
def MAT(matrix):
  """
  >>> (MAT([[1,0,0],[1,1,0],[2,0,1]])(Hpc.cube(2))).box()
  Box([1.0, 2.0], [2.0, 3.0])
  """  
  def MAT0(pol):
    return pol.transform(Matrix(matrix))
  return MAT0   

# EMBED
def EMBED(up_dim):
  def EMBED1(pol):
    new_dim_pol = pol.dim() + up_dim
    return Hpc(Matrix(new_dim_pol+1),[pol])
  return EMBED1  

# STRUCT
def STRUCT(seq,nrec=0):
    
  """
  >>> (STRUCT([Hpc.cube(2),T([1,2])([1,1]),T([1,2])([1,1]),Hpc.cube(2),Hpc.cube(2,1,2)])).box()
  Box([0.0, 0.0], [4.0, 4.0])
  >>> (STRUCT([T([1,2])([1,1]),T([1,2])([1,1]),Hpc.cube(2),T([1,2])([1,1]),T([1,2])([1,1]),Hpc.cube(2),Hpc.cube(2,1,2)])).box()
  Box([2.0, 2.0], [6.0, 6.0])
  """  
  
  if not isinstance(seq,list) : 
  	raise Exception("STRUCT must be applied to a list")
  
  if(len(seq)==0):
  	raise Exception("STRUCT must be applied to a not empty list")
  
  # avoid side effect
  if(nrec==0): seq=[x for x in seq] 
  
  # accumulate pols without transformations
  pols=[]
  while len(seq)>0 and ISPOL(seq[0]):
  	pols+=[seq[0]]; seq=seq[1:]
  
  # accumulate transformations for pols
  transformations=[]
  while len(seq)>0 and ISFUN(seq[0]): 
  	transformations+=[seq[0]]; seq=seq[1:]
  
  # avoid deadlock, i.e. call the recursion on invalid arguments
  if len(seq)>0 and not ISPOL(seq[0]) and not ISFUN(seq[0]):
  	raise Exception("STRUCT arguments not valid, not all elements are pols or transformations")
  
  if len(seq)>0:
  	assert ISPOL(seq[0]) # eaten all trasformations, the next must be a pol!
  	child=STRUCT(seq,nrec+1)
  	assert ISPOL(child)
  	if(len(transformations)>0): child=COMP(transformations)(child)
  	pols+=[child]
  
  if len(pols)==0:
  	raise Exception("Cannot find geometry in STRUCT, found only transformations")
  
  return Hpc.Struct(pols)   


# BOOLEAN OP
if(False):
  
  #UNION
  def UNION(objs_list):
    """
    >>> (UNION([Hpc.cube(2,0,1),Hpc.cube(2,0.5,1.5)])).box().fuzzyEqual(Box([0.0,0.0],[1.5,1.5])))
    True
    """    
    return Plasm.boolop(BOOL_CODE_OR, objs_list)
  
  #INTERSECTION
  def INTERSECTION(objs_list):
    """
    >>> (INTERSECTION([Hpc.cube(2,0,1),Hpc.cube(2,0.5,1.5)])).box().fuzzyEqual(Box([0.5,0.5],[1.0,1.0])))
    True
    """
    return Plasm.boolop(BOOL_CODE_AND)
  
  #DIFFERENCE
  def DIFFERENCE(objs_list):
    """
    >>> (DIFFERENCE([Hpc.cube(2,0,1),Hpc.cube(2,0.5,1.5)])).box().fuzzyEqual(Box([0.0,0.0],[1.0,1.0])))
    True
    """    
    return Plasm.boolop(BOOL_CODE_DIFF)
  
  # xor
  def XOR(objs_list):
    """
    >>> (XOR([Hpc.cube(2,0,1),Hpc.cube(2,0.5,1.5)])).box().fuzzyEqual(Box([0.0,0.0],[1.5,1.5])))
    True
    """    
    return Plasm.boolop(BOOL_CODE_XOR)


# JOIN
def JOIN(pol_list):
  """
  >>> JOIN([Hpc.cube(2,0,1)]).box()
  Box([0.0, 0.0], [1.0, 1.0])
  """
  if  ISPOL(pol_list): pol_list=[pol_list]
  return Hpc.join(pol_list)
   

# POWER
def POWER(objs_list):
   """
   >>> POWER([2,2])
   4.0
   >>> (POWER([Hpc.cube(2),Hpc.cube(1)])).box()
   Box([0.0, 0.0, 0.0], [1.0, 1.0, 1.0])
   """
   if not isinstance(objs_list,list) or len(objs_list)!=2:
    raise Exception("POWER can only be applied to a list of 2 arguments") 

   if ISNUM(objs_list[0]) and ISNUM(objs_list[1]):
    return math.pow(objs_list[0], objs_list[1])

   return Hpc.power(objs_list[0], objs_list[1])

# SKELETON
if False:
  def SKELETON(ord):
    """
    >>> SKELETON(0)(Hpc.cube(2)).box().fuzzyEqual(Box([0,0],[1,1])))
    True
    """
    def SKELETON_ORDER(pol):
      return Plasm.skeleton(pol, ord)
    return SKELETON_ORDER
  
  SKEL_0 =  SKELETON(0)
  SKEL_1 =  SKELETON(1)
  SKEL_2 =  SKELETON(2)
  SKEL_3 =  SKELETON(3)
  SKEL_4 =  SKELETON(4)
  SKEL_5 =  SKELETON(5)
  SKEL_6 =  SKELETON(6)
  SKEL_7 =  SKELETON(7)
  SKEL_8 =  SKELETON(8)
  SKEL_9 =  SKELETON(9)
  

# SIZE
def SIZE(List):
  """
  >>> SIZE(1)(Hpc.cube(2))
  1.0
  >>> SIZE([1,3])(SCALE([1,2,3])([1,2,3])(Hpc.cube(3)))
  [1.0, 3.0]
  """  
  def SIZE1(pol): 
    size = pol.box().size()
    return [size[i-1] for i in List] if isinstance(List,list) else size[List-1]
  return SIZE1

# MIN
def MIN(List):
  """
  >>> MIN(1)(Hpc.cube(2))
  0.0
  >>> MIN([1,3])(TRANSLATE([1,2,3])([10,20,30])(Hpc.cube(3)))
  [10.0, 30.0]
  """  
  def MIN1(pol):
    box = pol.box()
    return [box.p1[i-1] for i in List] if isinstance(List,list) else box.p1[List-1]
  return MIN1
  
# MAX
def MAX(List):
  """
  >>> MAX(1)(Hpc.cube(2))
  1.0
  >>> MAX([1,3])(TRANSLATE([1,2,3])([10,20,30])(Hpc.cube(3)))
  [11.0, 31.0]
  """  
  def MAX1(pol):
    box = pol.box()
    return [box.p2[i-1] for i in List] if isinstance(List,list) else box.p2[List-1]
  return MAX1

# MED
def MED(List):
  """
  >>> MED(1)(Hpc.cube(2))
  0.5
  >>> MED([1,3])(Hpc.cube(3))
  [0.5, 0.5]
  """  
  def MED1(pol):
    center = pol.box().center()
    return [center [i-1] for i in List] if isinstance(List,list) else center[List-1]
  return MED1


# alignment
def ALIGN(args):
  """
  >>> (ALIGN([3,MAX,MIN])([Hpc.cube(3),Hpc.cube(3)])).box()
  Box([0.0, 0.0, 0.0], [1.0, 1.0, 2.0])
  >>> (TOP([Hpc.cube(3),Hpc.cube(3)])).box()
  Box([0.0, 0.0, 0.0], [1.0, 1.0, 2.0])
  >>> (BOTTOM([Hpc.cube(3),Hpc.cube(3)])).box()
  Box([0.0, 0.0, -1.0], [1.0, 1.0, 1.0])
  >>> (LEFT([Hpc.cube(3),Hpc.cube(3)])).box()
  Box([-1.0, 0.0, 0.0], [1.0, 1.0, 1.0])
  >>> (RIGHT([Hpc.cube(3),Hpc.cube(3)])).box()
  Box([0.0, 0.0, 0.0], [2.0, 1.0, 1.0])
  >>> (UP([Hpc.cube(3,0,1),Hpc.cube(3,5,6)])).box()
  Box([0.0, 0.0, 0.0], [6.0, 2.0, 1.0])
  >>> (DOWN([Hpc.cube(3,0,1),Hpc.cube(3,5,6)])).box()
  Box([0.0, -1.0, 0.0], [6.0, 1.0, 1.0])
  """  
  def ALIGN0(args,pols):
    pol1 , pol2 = pols
    box1,box2=pol1.box(),pol2.box()
    if isinstance(args,list) and len(args)>0 and ISNUM(args[0]): 
        args=[args] # if I get something like [index,pos1,pos2]... i need [[index,pos1,pos2],[index,pos1,pos2],...]
    max_index=max([index for index,pos1,po2 in args])
    vt=[0.0]*max_index 
    for index,pos1,pos2 in args:
        p1=box1.p1 if pos1 is MIN else(box1.p2 if pos1 is MAX else box1.center());p1=p1[index-1] if index<=len(p1) else 0.0
        p2=box2.p1 if pos2 is MIN else(box2.p2 if pos2 is MAX else box2.center());p2=p2[index-1] if index<=len(p2) else 0.0        
        vt[index-1]-=(p2-p1)
    return Hpc.Struct([pol1,pol2.translate(vt)])
  return lambda pol: ALIGN0(args,pol)

TOP = ALIGN([[3, MAX, MIN], [1, MED, MED], [2, MED, MED]])
BOTTOM=ALIGN([[3, MIN, MAX], [1, MED, MED], [2, MED, MED]])
LEFT=ALIGN([[1, MIN, MAX], [3, MIN, MIN]])
RIGHT=ALIGN([[1, MAX, MIN], [3, MIN, MIN]])
UP=ALIGN([[2, MAX, MIN], [3, MIN, MIN]])
DOWN=ALIGN([[2, MIN, MAX], [3, MIN, MIN]])


# BOX
def BOX(List):
  """
  >>> (BOX([1,3])(Hpc.cube(3).translate([1,2,3]))).box()
  Box([1.0, 3.0], [2.0, 4.0])
  >>> (BOX(3)(Hpc.cube(3).translate([1,2,3]))).box()
  Box([3.0], [4.0])
  """  
  def BOX0(List,pol):
    if not isinstance(List,list): List=[List]
    dim = len(List) 
    box=pol.box()
    vt = [box.p1  [i-1] for i in List]
    vs = [box.size()[i-1] for i in List] 
    return Hpc.cube(dim).scale(vs).translate(vt)
  return lambda pol: BOX0(List,pol)


# MAP
def MAP(fn):
  """
  >>> (MAP([S1,S2])(Hpc.cube(2))).box()
  Box([0.0, 0.0], [1.0, 1.0])
  >>> (MAP(ID)(Hpc.cube(2))).box()
  Box([0.0, 0.0], [1.0, 1.0])
  """  
  def MAP0(fn,pol):
    if isinstance(fn,(list,tuple)): 
      map_fn=lambda point: [f(point) for f in fn]
    else:
      map_fn=fn
    return pol.mapFn(map_fn)
  return lambda pol: MAP0(fn,pol)


# ===================================================
# OTHER TESTS
# ===================================================

# circle 
def CIRCLE_POINTS(R,N):
   return [ [R*math.cos(i*2*PI/N),R*math.sin(i*2*PI/N)] for i in range(0,N) ]

# CIRCUMFERENCE
def CIRCUMFERENCE(R):
  """
  >>> CIRCUMFERENCE(1)(8).box()
  Box([-1.0, -1.0], [1.0, 1.0])
  """
  return lambda N: MAP(lambda p: [R*math.cos(p[0]),R*math.sin(p[0]) ])(INTERVALS(2*PI)(N))

# NGON
def NGON(N):
  return CIRCUMFERENCE(1)(N)

# RING 
def RING(radius):
  """
  >>> (RING([0.5,1])([8,8])).box()
  Box([-1.0, -1.0], [1.0, 1.0])
  """  
  R1 , R2 = radius
  def RING0(subds):
    N , M = subds
    domain=(POWER([INTERVALS(2*PI)(N),INTERVALS(R2-R1)(M)]).translate([0.0,R1]))
    fun=lambda p: [p[1]*math.cos(p[0]),p[1]*math.sin(p[0])]
    return MAP(fun)(domain)
  return RING0

# TUBE
def TUBE(args):
  r1 , r2 , height= args
  def TUBE0(N):
    return Hpc.power(RING([r1, r2])([N, 1]),QUOTE([height]))
  return TUBE0

# CIRCLE 
def CIRCLE(R):
  """
  >>> CIRCLE(1.0)([8,8]).box()
  Box([-1.0, -1.0], [1.0, 1.0])
  """
  def CIRCLE0(subs):
    N , M = subs
    domain= POWER([INTERVALS(2*PI)(N), INTERVALS(R)(M)])
    fun=lambda p: [p[1]*math.cos(p[0]),p[1]*math.sin(p[0])]
    return MAP(fun)(domain)
  return CIRCLE0

# CYLINDER 
def MY_CYLINDER(args):
  """
  >>> CYLINDER([1.0,2.0])(8).box().fuzzyEqual(Box([-1,-1,0],[+1,+1,2]))
  True
  """  
  R , H = args
  def MY_CYLINDER0(N):
    points=CIRCLE_POINTS(R,N)
    circle=Hpc.mkpol(points,[range(N)])
    return Hpc.power(circle,Hpc.mkpol([[0],[H]],[[0,1]]))
  return MY_CYLINDER0

CYLINDER =MY_CYLINDER 

# SPHERE
def SPHERE(radius):
  """
  >>> (SPHERE(1)([8,8])).box()
  Box([-1.0, -1.0, -1.0], [1.0, 1.0, 1.0])
  >>> VIEW(SPHERE(1)([16,16]))
  """
  def SPHERE0(subds):
    N , M = subds
    domain = Hpc.power(INTERVALS(PI)(N),INTERVALS(2*PI)(M)).translate([-PI/2,0])
    fx  = lambda p: radius * math.cos(p[0])  * math.sin (p[1])
    fy  = lambda p: radius * math.cos(p[0]) * math.cos(p[1])
    fz  = lambda p: radius * math.sin(p[0]) 
    ret=  MAP([fx, fy, fz])(domain)
    return ret
  return SPHERE0

# TORUS
def TORUS(radius):
  """
  >>> (TORUS([1,2])([8,8])).box().fuzzyEqual(Box([-2,-2,-0.5],[+2,+2,+0.5]))
  True
  >>> VIEW(TORUS([1,2])([20,20]))
  """  
  r1 , r2 = radius
  def TORUS0(subds):
    N , M = subds
    a=0.5*(r2-r1)
    c=0.5*(r1+r2)
    domain=Hpc.power(  INTERVALS(2*PI)(N),  INTERVALS(2*PI)(M)  )
    fx =   lambda p:(c+a*math.cos(p[1])) * math.cos(p[0])
    fy =   lambda p:(c+a*math.cos(p[1])) * math.sin(p[0])
    fz =   lambda p: a*math.sin(p[1])
    return MAP(([fx,fy,fz]))(domain)
  return TORUS0

# CONE
def CONE(args):
  """
  >>> (CONE([1.0,3.0])(16)).box().fuzzyEqual(Box([-1,-1,0],[+1,+1,3]))
  True
  """  
  radius , height = args
  def CONE0(N):
    basis = CIRCLE(radius)([N,1])
    apex = T(3)(height)(SIMPLEX(0))
    return  JOIN([basis, apex])
  return CONE0

# TRUNCONE
def TRUNCONE(args):
	R1 , R2 , H = args
	def TRUNCONE0(N):
		domain = Hpc.power( QUOTE([2*PI/N for i in range(N)]) , QUOTE([1])  )
		def fn(p):
			return [
				(R1+p[1]*(R2-R1))*math.cos(p[0]),	
				(R1+p[1]*(R2-R1))*math.sin(p[0]),
				(H*p[1])]
		return MAP(fn)(domain)
	return TRUNCONE0

# DODECAHEDRON
def build_DODECAHEDRON():
	a = 1.0/(math.sqrt(3.0))
	g = 0.5*(math.sqrt(5.0)-1)
	top = MKPOL([[[1-g,1,0-g],[1+g,1,0-g]],[[1, 2]],[[1]]])
	basis = EMBED(1)(CUBOID([2, 2]))
	roof = T([1, 2, 3])([-1,-1,-1])(JOIN([basis, top]))
	roofpair = STRUCT([roof, R([2, 3])(PI), roof])
	return S([1, 2, 3])([a, a, a])(STRUCT([ 
		Hpc.cube(3,-1,+1),
		roofpair, 
		R([1, 3])(PI/2), R([1, 2])(PI/2), 
		roofpair, 
		R([1, 2])(PI/2), R([2, 3])(PI/2), 
		roofpair]))

DODECAHEDRON = build_DODECAHEDRON()

# ICOSAHEDRON
def build_ICOSAHEDRON():
  g = 0.5*(math.sqrt(5)-1)
  b = 2.0/(math.sqrt(5*math.sqrt(5)))
  rectx = T([1, 2])([-g, -1])(CUBOID([2*g, 2]))
  recty = R([1, 3])(PI/2)(R([1, 2])(PI/2)(rectx))
  rectz = R([2, 3])(PI/2)(R([1, 2])(PI/2)(rectx))
  return S([1, 2, 3])([b, b, b])(JOIN([rectx, recty, rectz]))

ICOSAHEDRON = build_ICOSAHEDRON()

# TETRAHEDRON
def build_TETRAHEDRON():
	return JOIN([  T(3)(-1.0/3.0)(NGON(3)),  MK([0, 0, 1])  ])

TETRAHEDRON = build_TETRAHEDRON()

# POLYPOINT 
def POLYPOINT(points):
	return Hpc.mkpol(points,[ [i] for i in range(len(points))])

# POLYLINE 
def POLYLINE(points):
	return Hpc.mkpol(points,[[i,i+1] for i in range(len(points)-1)])

# TRIANGLESTRIPE 
def TRIANGLESTRIPE(points):
	cells=[ [i,i+1,i+2] if(i%2==0) else [i+1,i,i+2] for i in range(len(points)-2)]
	return Hpc.mkpol(points,cells)

# TRIANGLEFAN 
def TRIANGLEFAN(points):
	cells=[[0,i-1,i] for i in range(2,len(points))]
	return Hpc.mkpol(points,cells)

# MIRROR 
def MIRROR(D):
  def MIRROR0(pol):
    return  STRUCT([S(D)(-1)(pol),pol])
  return MIRROR0

# POLYMARKER
def POLYMARKER(type,MARKERSIZE=0.1):
	A,B=(MARKERSIZE,-MARKERSIZE)
	marker0=Hpc.mkpol([[A], [0], [0], [A], [B], [0], [0], [B]]  , [[0, 1], [1, 2], [2, 3], [3, 0]])
	marker1=Hpc.mkpol([[A], [A], [B], [A], [B], [B], [A], [B]], [[0, 2], [1, 3]])
	marker2=Hpc.mkpol([[A], [A], [B], [A], [B], [B], [A], [B]], [[0, 1], [1, 2], [2, 3], [3, 0]])
	marker3=STRUCT([marker0,marker1])
	marker4=STRUCT([marker0,marker2])
	marker5=STRUCT([marker1,marker2])
	marker=[marker0, marker1, marker2, marker3, marker4,marker5][type % 6]
	def POLYMARKER_POINTS(points):
		dim=len(points[0])
		axis=range(1,dim+1)
		return Hpc.Struct([T(axis)(point)(marker) for point in points])
	return POLYMARKER_POINTS


# BEZIER
def BEZIER(U):
  """
  >>> VIEW(MAP(BEZIER(S1)([[-0,0],[1,0],[1,1],[2,1],[3,1]]))(INTERVALS(1)(32)))
  >>> C0 = BEZIER(S1)([[0,0,0],[10,0,0]])
  >>> C1 = BEZIER(S1)([[0,2,0],[8,3,0],[9,2,0]])
  >>> C2 = BEZIER(S1)([[0,4,1],[7,5,-1],[8,5,1],[12,4,0]])
  >>> C3 = BEZIER(S1)([[0,6,0],[9,6,3],[10,6,-1]])
  >>> VIEW(MAP(BEZIER(S2)([C0,C1,C2,C3]))(Hpc.power(INTERVALS(1)(10),INTERVALS(1)(10))))
  """  
  def BEZIER0(controldata_fn):
  	N=len(controldata_fn)-1
  	def map_fn(point):
  		t=U(point)
  		controldata=[fun(point) if callable(fun) else fun for fun in controldata_fn]
  		ret=[0.0 for i in range(len(controldata[0]))]		
  		for I in range(N+1):
  			weight=CHOOSE([N,I])*math.pow(1-t,N-I)*math.pow(t,I)
  			for K in range(len(ret)):  ret[K]+=weight*(controldata[I][K])
  		return ret
  	return map_fn
  return BEZIER0

def BEZIERCURVE(controlpoints):
  return BEZIER(S1)(controlpoints)

# COONSPATCH
def COONSPATCH(args):
  """
  >>> Su0=BEZIER(S1)([[0,0,0],[10,0,0]])
  >>> Su1=BEZIER(S1)([[0,10,0],[2.5,10,3],[5,10,-3],[7.5,10,3],[10,10,0]])
  >>> Sv0=BEZIER(S2)([[0,0,0],[0,0,3],[0,10,3],[0,10,0]])
  >>> Sv1=BEZIER(S2)([[10,0,0],[10,5,3],[10,10,0]])
  >>> VIEW(MAP(COONSPATCH([Su0,Su1,Sv0,Sv1]))(Hpc.power(INTERVALS(1)(10),INTERVALS(1)(10))))
  """  
  su0_fn , su1_fn , s0v_fn , s1v_fn = args
  def map_fn(point):
  	u,v=point
  	su0=su0_fn(point) if callable(su0_fn) else su0_fn
  	su1=su1_fn(point) if callable(su1_fn) else su1_fn
  	s0v=s0v_fn(point) if callable(s0v_fn) else s0v_fn
  	s1v=s1v_fn(point) if callable(s1v_fn) else s1v_fn
  	ret=[0.0 for i in range(len(su0))]	
  	for K in range(len(ret)):
  		ret[K]=(1-u)*s0v[K]+u*s1v[K]+(1-v)*su0[K]+v*su1[K]+(1-u)*(1-v)*s0v[K]+(1-u)*v*s0v[K]+u*(1-v)*s1v[K]+u*v*s1v[K] 
  	return ret
  return map_fn


# RULED SURFACE
def RULEDSURFACE(args):
  """
  >>> alpha= lambda point: [point[0],point[0],       0 ]
  >>> beta = lambda point: [      -1,      +1,point[0] ]
  >>> domain= T([1,2])([-1,-1])(Hpc.power(INTERVALS(2)(10),INTERVALS(2)(10)))
  >>> VIEW(MAP(RULEDSURFACE([alpha,beta]))(domain))
  """  
  alpha_fn , beta_fn = args
  def map_fn(point):
  	u,v=point
  	alpha,beta=alpha_fn(point),beta_fn(point)
  	ret=[0.0 for i in range(len(alpha))]	
  	for K in range(len(ret)): ret[K]=alpha[K]+v*beta[K]
  	return ret
  return map_fn


# PROFILE SURFACE
def PROFILEPRODSURFACE(args):
  """
  >>> alpha=BEZIER(S1)([[0.1,0,0],[2,0,0],[0,0,4],[1,0,5]])
  >>> beta =BEZIER(S2)([[0,0,0],[3,-0.5,0],[3,3.5,0],[0,3,0]])
  >>> domain=Hpc.power(INTERVALS(1)(20),INTERVALS(1)(20))
  >>> VIEW(Hpc.Struct([MAP(alpha)(domain),MAP(beta )(domain),MAP(PROFILEPRODSURFACE([alpha,beta]))(domain)]))
  """
  profile_fn,section_fn = args
  def map_fun(point):
  	u,v=point
  	profile,section=profile_fn(point),section_fn(point)
  	ret=[profile[0]*section[0],profile[0]*section[1],profile[2]]
  	return ret
  return map_fun

 
# ROTATIONALSURFACE
def ROTATIONALSURFACE(args):
  """
  >>> profile=BEZIER(S1)([[0,0,0],[2,0,1],[3,0,4]]) # defined in xz!
  >>> domain=Hpc.power(INTERVALS(1)(10),INTERVALS(2*PI)(30)) # the first interval should be in 0,1 for bezier
  >>> VIEW(MAP(ROTATIONALSURFACE(profile))(domain))
  """  
  profile = args
  
  def map_fn(point):
  	u,v=point
  	f,h,g= profile(point)
  	ret=[f*math.cos(v),f*math.sin(v),g]
  	return ret
  return map_fn


# CYLINDRICALSURFACE
def CYLINDRICALSURFACE(args):
  """
  >>> alpha=BEZIER(S1)([[1,1,0],[-1,1,0],[1,-1,0],[-1,-1,0]])
  >>> Udomain=INTERVALS(1)(20)
  >>> Vdomain=INTERVALS(1)(6)
  >>> domain=Hpc.power(Udomain,Vdomain)
  >>> fn=CYLINDRICALSURFACE([alpha,[0,0,1]])
  >>> VIEW(MAP(fn)(domain))
  """  
  alpha_fun   = args[0]
  beta_fun  = CONS(AA(K)(args[1]))
  return RULEDSURFACE([alpha_fun,beta_fun])


# CONICALSURFACE
def CONICALSURFACE(args):
  """
  >>> domain=Hpc.power(INTERVALS(1)(20),INTERVALS(1)(6))
  >>> beta=BEZIER(S1)([ [1,1,0],[-1,1,0],[1,-1,0],[-1,-1,0] ])
  >>> VIEW(MAP(CONICALSURFACE([[0,0,1],beta]))(domain))
  """  
  apex=args[0]
  alpha_fn   = lambda point: apex
  beta_fn  = lambda point: [ args[1](point)[i]-apex[i] for i in range(len(apex))]
  return RULEDSURFACE([alpha_fn, beta_fn])

# CUBICHERMITE
def CUBICHERMITE(U):
  """
  >>> domain=INTERVALS(1)(20)
  >>> VIEW(Hpc.Struct([MAP(CUBICHERMITE(S1)([[1,0],[1,1],[ -1, 1],[ 1,0]]))(domain),MAP(CUBICHERMITE(S1)([[1,0],[1,1],[ -2, 2],[ 2,0]]))(domain),MAP(CUBICHERMITE(S1)([[1,0],[1,1],[ -4, 4],[ 4,0]]))(domain),MAP(CUBICHERMITE(S1)([[1,0],[1,1],[-10,10],[10,0]]))(domain)]))
  >>> c1=CUBICHERMITE(S1)([[1  ,0,0],[0  ,1,0],[0,3,0],[-3,0,0]])
  >>> c2=CUBICHERMITE(S1)([[0.5,0,0],[0,0.5,0],[0,1,0],[-1,0,0]])
  >>> sur3=CUBICHERMITE(S2)([c1,c2,[1,1,1],[-1,-1,-1]])
  >>> domain=Hpc.power(INTERVALS(1)(14),INTERVALS(1)(14))
  >>> VIEW(MAP(sur3)(domain))
  """    
  def CUBICHERMITE0(args):
  	p1_fn , p2_fn , s1_fn , s2_fn = args
  	def map_fn(point):
  		u=U(point);u2=u*u;u3=u2*u
  		p1,p2,s1,s2=[f(point) if callable(f) else f for f in [p1_fn , p2_fn , s1_fn , s2_fn]]
  		ret=[0.0 for i in range(len(p1))]		
  		for i in range(len(ret)):				
  			ret[i]+=(2*u3-3*u2+1)*p1[i] +(-2*u3+3*u2)*p2[i]+(u3-2*u2+u)*s1[i]+(u3-u2)*s2[i]
  		return ret
  	return map_fn
  return CUBICHERMITE0

# HERMITE
def HERMITE(args):
  P1 , P2 , T1 , T2 = args
  return CUBICHERMITE(S1)([P1, P2, T1, T2])

# Q
def Q(H):
	return Hpc.mkpol([[0],[H]],[[0,1]])

# EXTRUDE
def EXTRUDE(args):
	__N, Pol, H = args
	return Hpc.power(Pol,Q(H))

# MULTEXTRUDE
def MULTEXTRUDE(P):
	def MULTEXTRUDE0(H):
		return Hpc.power(P,Q(H))
	return MULTEXTRUDE0

# PROJECT
def PROJECT(M):
	def PROJECT0(POL):
		vertices,cells,pols=UKPOL(POL)
		vertices=[vert[0:-M] for vert in vertices]
		return MKPOL([vertices,cells,pols])
	return PROJECT0

# SPLITCELLS
def SPLITCELLS(scene):
	vertices,cells,pols= UKPOL(scene)
	ret=[]
	for c in cells: ret+=[MKPOL([vertices,[c],[[1]]])]
	return ret

# EXTRACT_WIRES
def EXTRACT_WIRES(scene):
  return SPLITCELLS(SKEL_1(scene))

# no notion of pols for xge mkpol!
SPLITPOLS=SPLITCELLS

# PERMUTAHEDRON
def PERMUTAHEDRON(d):
  """
  >>> VIEW(Hpc.Struct([PERMUTAHEDRON(2),(PERMUTAHEDRON(2))]))
  >>> VIEW(Hpc.Struct([PERMUTAHEDRON(3),(PERMUTAHEDRON(3))]))
  """  
  vertices = PERMUTATIONS(range(1,d+2))
  center = MEANPOINT(vertices)
  cells=[range(1,len(vertices)+1)]
  object=MKPOL([vertices,cells,[[1]]])
  object=object.translate([-coord for coord in center])
  for i in range(1,d+1): object=R([i,d+1])(PI/4)(object)
  object=PROJECT(1)(object)
  return object


# STAR
def STAR(N):
  def CIRCLEPOINTS(STARTANGLE):
    def CIRCLEPOINTS1(R):
      def CIRCLEPOINTS0(N):
        return AA((COMP([CONS([RAISE(PROD)([K(R),COS]), RAISE(PROD)([K(R),SIN])]),(RAISE(SUM)([ID,K(STARTANGLE)]))])))(((COMP([COMP([AA(RAISE(PROD)), TRANS]), CONS([K((FROMTO([1,N]))), DIESIS(N)])]))((2*PI/N))))
      return CIRCLEPOINTS0
    return CIRCLEPOINTS1
  return (COMP([COMP([TRIANGLEFAN, CAT]), TRANS]))([CIRCLEPOINTS(0)(1)(N), CIRCLEPOINTS((PI/N))(2.5)(N)])


# SCHLEGEL
def SCHLEGEL2D(D):
	def map_fn(point):
		return [D*point[0]/point[2],D*point[1]/point[2]]
	return MAP(map_fn)

# SCHLEGEL3D
def SCHLEGEL3D(D):
  """
  >>> VIEW(SCHLEGEL3D(0.2)((T([1,2,3,4])([-1.0/3.0,-1.0/3.0,-1,+1])(SIMPLEX(4)))))
  >>> VIEW(SCHLEGEL3D(0.2)((T([1,2,3,4])([-1,-1,-1,1])(CUBOID([2,2,2,2])))))
  >>> VIEW(SCHLEGEL3D(0.2)((T([1,2,3,4])([-1.0/3.0,-1.0/3.0,-1,+1])(Hpc.power(SIMPLEX(2),SIMPLEX(2))))))
  """  
  def map_fn(point):
  	return [D*point[0]/point[3],D*point[1]/point[3],D*point[2]/point[3]]
  return MAP(map_fn)

# FINITECONE
def FINITECONE(pol):
	point=[0 for i in range(RN(pol))]
	return JOIN([pol,MK(point)])

# PRISM
def PRISM(HEIGHT):
  def PRISM0(BASIS):
    return Hpc.power(BASIS,QUOTE([HEIGHT]))
  return PRISM0

# CROSSPOLYTOPE
def CROSSPOLYTOPE(D):
	points=[]
	for i in range(D):
		point_pos=[0 for x in range(D)];point_pos[i]=+1
		point_neg=[0 for x in range(D)];point_neg[i]=-1
		points+=[point_pos,point_neg]
	cells=[range(1,D*2+1)]
	pols=[[1]]
	return MKPOL([points,cells,pols])

# OCTAHEDRON
OCTAHEDRON = CROSSPOLYTOPE(2)


# ROTN
def ROTN(args):
	alpha, N = args
	N=UNITVECT(N)
	QX = UNITVECT((VECTPROD([[0, 0, 1],N])))
	QZ = UNITVECT(N)
	QY = VECTPROD([QZ,QX])
	Q  = MATHOM([QX, QY, QZ])
	ISUP = COMP([AND, CONS([COMP([C(EQ)(0), S1]), COMP([C(EQ)(0), S2]), COMP([COMP([NOT, C(EQ)(0)]), S3])])])
	if N[0]==0 and N[1]==0:
		return R([1, 2])(alpha)
	else:
		return COMP([MAT(TRANS(Q)),R([1,2])(alpha),MAT(Q)])

# MKVECTOR
MKVERSORK = TOP([CYLINDER([1.0/100.0, 7.0/8.0])(6),CONE([1.0/16.0,1.0/8])(8)])

def MKVECTOR(P1):
  def MKVECTOR0(P2):
    TR = T([1, 2, 3])(P1)
    U = VECTDIFF([P2,P1])
    ALPHA = ACOS((INNERPROD([[0, 0, 1],UNITVECT(U)])))
    B = VECTNORM(U)
    SC = S([1, 2, 3])([B, B, B])
    N = VECTPROD([[0, 0, 1],U])
    ROT = ROTN([ALPHA, N])
    return(COMP([COMP([TR, ROT]), SC]))(MKVERSORK)
  return MKVECTOR0


# CUBICUBSPLINE
def CUBICUBSPLINE(domain):
  """
  >>> domain=INTERVALS(1)(20)
  >>> points = [[-3,6],[-4,2],[-3,-1],[-1,1],[1.5,1.5],[3,4],[5,5],[7,2],[6,-2],[2,-3]]
  >>> VIEW(SPLINE(CUBICCARDINAL(domain))(points))
  >>> VIEW(SPLINE(CUBICUBSPLINE(domain))(points))
  """  
  def CUBICUBSPLINE0(args):
  	q1_fn, q2_fn , q3_fn , q4_fn = args
  	def map_fn(point):
  		u=S1(point)
  		u2=u*u
  		u3=u2*u
  		q1,q2,q3,q4=[f(point) if callable(f) else f for f in [q1_fn,q2_fn,q3_fn,q4_fn]]
  		ret=[0 for x in range(len(q1))]
  		for i in range(len(ret)):
  			ret[i]=(1.0/6.0)* ((-u3+3*u2-3*u+1)*q1[i] +(3*u3-6*u2+4)*q2[i]  +(-3*u3+3*u2+3*u+1)*q3[i] +(u3)*q4[i]  )
  		return ret
  	return MAP(map_fn)(domain)
  return CUBICUBSPLINE0


# CUBICCARDINAL
def CUBICCARDINAL(domain,h=1):
	def CUBICCARDINAL0(args):
		q1_fn , q2_fn , q3_fn , q4_fn = args
		def map_fn(point):
			u=S1(point)
			u2=u*u
			u3=u2*u
			q1,q2,q3,q4=[f(point) if callable(f) else f for f in [q1_fn,q2_fn,q3_fn,q4_fn]]
			ret=[0.0 for i in range(len(q1))]	
			for i in range(len(ret)):
				ret[i]=(-h*u3+2*h*u2-h*u)*q1[i] +((2-h)*u3+(h-3)*u2+1)*q2[i] +((h-2)*u3+(3-2*h)*u2+h*u)*q3[i] +(h*u3-h*u2)*q4[i]
			return ret
		return MAP(map_fn)(domain)
	return CUBICCARDINAL0


# SPLINE
def SPLINE(curve):
	def SPLINE0(points):
		ret=[]
		for i in range(len(points)-4+1):
			P=points[i:i+4]
			ret+=[curve(P)]
		return Hpc.Struct(ret)
	return SPLINE0

# CUBICUBSPLINE
def JOINTS(curve):
	knotzero = MK([0])
	def JOINTS0(points):
		points,cells,pols=UKPOL(SPLINE(curve(knotzero)))
		return POLYMARKER(2)(points)


# BERNSTEINBASIS
def BERNSTEINBASIS(U):
	def BERNSTEIN0(N):
		def BERNSTEIN1(I):
			def map_fn(point):
				t=U(point)
				ret=CHOOSE([N,I])*math.pow(1-t,N-I)*math.pow(t,I)
				return ret
			return map_fn
		return [BERNSTEIN1(I) for I in range(0,N+1)]
	return BERNSTEIN0


# TENSORPRODSURFACE
def TENSORPRODSURFACE(args):
	ubasis , vbasis = args
	def TENSORPRODSURFACE0(controlpoints_fn):
		def map_fn(point):
			u,v=point
			U=[f([u]) for f in ubasis]
			V=[f([v]) for f in vbasis]
			controlpoints=[f(point) if callable(f) else f for f in controlpoints_fn]
			target_dim=len(controlpoints[0][0])
			ret=[0 for x in range(target_dim)]
			for i in range(len(ubasis)):
				for j in range(len(vbasis)):
					for M in range(len(ret)):
						for M in range(target_dim): 
							ret[M]+= U[i]*V[j] * controlpoints[i][j][M]
			return ret
		return map_fn
	return TENSORPRODSURFACE0

# BILINEARSURFACE
def BILINEARSURFACE(controlpoints):
  """
  >>> controlpoints=[[[0,0,0],[2,-4,2]],[[0,3,1],[4,0,0]]]
  >>> domain=Hpc.power(INTERVALS(1)(10),INTERVALS(1)(10))
  >>> mapping=BILINEARSURFACE(controlpoints)
  >>> VIEW(MAP(mapping)(domain))
  """  
  return TENSORPRODSURFACE([BERNSTEINBASIS(S1)(1),BERNSTEINBASIS(S1)(1)])(controlpoints)


# BIQUADRATICSURFACE
def BIQUADRATICSURFACE(controlpoints):
  """
  >>> controlpoints=[[[0,0,0],[2,0,1],[3,1,1]],[[1,3,-1],[3,2,0],[4,2,0]],[[0,9,0],[2,5,1],[3,3,2]]]
  >>> domain=Hpc.power(INTERVALS(1)(10),INTERVALS(1)(10))
  >>> mapping=BIQUADRATICSURFACE(controlpoints)
  >>> VIEW(MAP(mapping)(domain))
  """  
  def u0(point):u=S1(point);return 2*u*u-u
  def u1(point):u=S1(point);return 4*u-4*u*u
  def u2(point):u=S1(point);return 2*u*u-3*u+1
  basis = [u0, u1, u2]
  return TENSORPRODSURFACE([basis, basis])(controlpoints)

# HERMITESURFACE
def HERMITESURFACE(controlpoints):
  """
  >>> controlpoints=[[[0,0,0 ],[2,0,1],[3,1,1],[4,1,1]],[[1,3,-1],[3,2,0],[4,2,0],[4,2,0]],[[0,4,0 ],[2,4,1],[3,3,2],[5,3,2]],[[0,6,0 ],[2,5,1],[3,4,1],[4,4,0]]]
  >>> domain=Hpc.power(INTERVALS(1)(10),INTERVALS(1)(10))
  >>> mapping=HERMITESURFACE(controlpoints)
  >>> VIEW(MAP(mapping)(domain))
  """  
  def H0(point):u=S1(point);u2=u*u;u3=u2*u;return u3-u2
  def H1(point):u=S1(point);u2=u*u;u3=u2*u;return u3-2*u2+u
  def H2(point):u=S1(point);u2=u*u;u3=u2*u;return 3*u2-2*u3
  def H3(point):u=S1(point);u2=u*u;u3=u2*u;return 2*u3-3*u2+1
  basis=[H3, H2, H1, H0]
  return  TENSORPRODSURFACE([basis, basis])(controlpoints)

# BEZIERSURFACE
def BEZIERSURFACE(controlpoints):
  """
  >>> controlpoints=[[[ 0,0,0],[0 ,3  ,4],[0,6,3],[0,10,0]],[[ 3,0,2],[2 ,2.5,5],[3,6,5],[4,8,2]],[[ 6,0,2],[8 ,3 , 5],[7,6,4.5],[6,10,2.5]],[[10,0,0],[11,3  ,4],[11,6,3],[10,9,0]]]
  >>> domain=Hpc.power(INTERVALS(1)(10),INTERVALS(1)(10))
  >>> mapping=BEZIERSURFACE(controlpoints)
  >>> VIEW(MAP(mapping)(domain))
  """  
  M = len(controlpoints   )-1
  N = len(controlpoints[0])-1
  return TENSORPRODSURFACE([BERNSTEINBASIS(S1)(M), BERNSTEINBASIS(S1)(N)])(controlpoints)

# generic tensor product
def TENSORPRODSOLID(args):
	ubasis,vbasis,wbasis = args
	def TENSORPRODSOLID0(controlpoints_fn):
		def map_fn(point):
			u,v,w=point
			U=[f([u]) for f in ubasis]
			V=[f([v]) for f in vbasis]
			W=[f([w]) for f in wbasis]
			controlpoints=[f(point) if callable(f) else f for f in controlpoints_fn]
			target_dim=len(controlpoints[0][0][0])
			ret=[0 for x in range(target_dim)]
			for i in range(len(ubasis)):
				for j in range(len(vbasis)):
					for k in range(len(wbasis)):
						for M in range(target_dim): 
							ret[M]+= U[i]*V[j]*W[k] * controlpoints[M][i][j][k]
			return ret
		return map_fn
	return TENSORPRODSOLID0

# BEZIERMANIFOLD
def BEZIERMANIFOLD(degrees):
  """
  >>> grid1D = INTERVALS(1)(5)
  >>> domain3D = Hpc.power(Hpc.power(grid1D,grid1D),grid1D)
  >>> degrees = [2,2,2]
  >>> Xtensor =  [[[0,1,2],[-1,0,1],[0,1,2]],[[0,1,2],[-1,0,1],[0,1,2]],[[0,1,2],[-1,0,1],[0,1,2]]]
  >>> Ytensor =  [[[0,0,0.8],[1,1,1],[2,3,2]],[[0,0,0.8],[1,1,1],[2,3,2]],[[0,0,0.8],[1,1,1],[2,3,2]]]
  >>> Ztensor =  [[[0,0,0],[0,0,0],[0,0,0]],[[1,1,1],[1,1,1],[1,1,1]],[[2,2,1],[2,2,1],[2,2,1]]] 
  >>> mapping = BEZIERMANIFOLD(degrees)([Xtensor,Ytensor,Ztensor])
  >>> VIEW(MAP(mapping)(domain3D))
  """  
  basis=[BERNSTEINBASIS(S1)(d) for d in degrees]
  return TENSORPRODSOLID(basis)


# LOCATE
def LOCATE(args):
	pol, a, distances = args
	ret=[]
	for d in distances:
		ret+=[T(a)(d),pol]
	return STRUCT(ret)

# ===================================================
# NORTH,SOUTH,WEST,EAST
# ===================================================

NORTH  = CONS([CONS([MAX(1), MAX(2)]), CONS([MIN(1), MIN(2)])])
SOUTH  = CONS([CONS([MIN(1), MIN(2)]), CONS([MAX(1), MIN(2)])])
WEST   = CONS([CONS([MIN(1), MAX(2)]), CONS([MIN(1), MIN(2)])])
EAST   = CONS([CONS([MAX(1), MIN(2)]), CONS([MAX(1), MAX(2)])])

MXMY = COMP([STRUCT, CONS([COMP([COMP([T([1, 2]), AA(RAISE(DIFF))]), MED([1, 2])]), ID])])
MXBY = COMP([STRUCT, CONS([COMP([COMP([T([1, 2]), AA(RAISE(DIFF))]), CONS([MED(1), MIN(2)])]), ID])])
MXTY = COMP([STRUCT, CONS([COMP([COMP([T([1, 2]), AA(RAISE(DIFF))]), CONS([MED(1), MAX(2)])]), ID])])
LXMY = COMP([STRUCT, CONS([COMP([COMP([T([1, 2]), AA(RAISE(DIFF))]), CONS([MIN(1), MED(2)])]), ID])])
RXMY = COMP([STRUCT, CONS([COMP([COMP([T([1, 2]), AA(RAISE(DIFF))]), CONS([MAX(1), MED(2)])]), ID])])

# RIF
def RIF(size):
	thin = 0.01*size
	x=COLOR(RED)(CUBOID([size, thin, thin]))
	y=COLOR(GREEN)(CUBOID([thin, size, thin]))
	z=COLOR(BLUE)(CUBOID([thin, thin, size]))
	return Hpc.Struct([x,y,z])


# FRACTALSIMPLEX
def FRACTALSIMPLEX(D):
  def FRACTALSIMPLEX0(N):
		mkpols = COMP([COMP([COMP([COMP([STRUCT, AA(MKPOL)]), AA(AL)]), DISTR]), CONS([ID, K([[FROMTO([1,D+1])], [[1]]])])])
		def COMPONENT(args):
			i, seq = args
			firstseq =   seq[0:i-1]
			pivot  = seq[i-1]
			lastseq = seq[i:len(seq)]
			firstpart = AA(MEANPOINT)(DISTR([firstseq, pivot]))
			lastpart  = AA(MEANPOINT)(DISTR([lastseq , pivot]))
			return CAT([firstpart, [pivot], lastpart])
		expand = COMP([COMP([AA(COMPONENT), DISTR]), CONS([COMP([INTSTO, LEN]), ID])])
		splitting =(COMP([COMP, DIESIS(N)]))((COMP([CAT, AA(expand)])))
		return(COMP([COMP([COMP([COMP([mkpols, splitting]), CONS([S1])])])]))(UKPOL(SIMPLEX(D)))
  return FRACTALSIMPLEX0


# PYRAMID
def PYRAMID(H):
	def PYRAMID0(pol):
		barycenter=MEANPOINT(UKPOL(pol)[0])
		return JOIN([MK(barycenter+[H]),pol])
	return PYRAMID0

# MESH
def MESH(seq):
	return INSL(RAISE(PROD))([QUOTE(i) for i in seq])


# NU_GRID
def NU_GRID(data):
	polylines=[POLYLINE(i) for i in data]
	return INSL(RAISE(PROD))(polylines)


# CURVE2MAPVECT
def CURVE2MAPVECT(CURVE):
  """
  >>> CURVE2MAPVECT(lambda t: [t[0]+1,t[0]+2])[0]([10])
  11
  >>> CURVE2MAPVECT(lambda t: [t[0]+1,t[0]+2])[1]([10])
  12
  """
  D = len((CURVE([0])))
  return [ COMP([SEL(i),CURVE]) for i in FROMTO([1,D]) ]

# SEGMENT
def SEGMENT(sx):
	def SEGMENT0(args):
		N=len(args[0])
		A,B=args
		P0=A
		P1=[A[i]+(B[i]-A[i])*sx for i in range(N)]
		return POLYLINE([P0,P1])
	return SEGMENT0

# SOLIDIFY
if False:
  
  def SOLIDIFY(pol):
  	
  	box=pol.box()
  	min=box.p1[0]
  	max=box.p2[0]
  	siz=max-min
  	far_point=max+siz*100
  
  	def InftyProject(pol):
  		verts,cells,pols=UKPOL(pol)
  		verts=[[far_point] + v[1:] for v in verts]
  		return MKPOL([verts,cells,pols])
  
  	def IsFull(pol):
  		return DIM(pol)==RN(pol)
  
  	ret=SPLITCELLS(pol)
  	ret=[JOIN([pol,InftyProject(pol)]) for pol in ret]
  	return XOR(FILTER(IsFull)(ret))
  


# EXTRUSION
def EXTRUSION(angle):
	def EXTRUSION1(height):
		def EXTRUSION0(pol):
			dim = DIM(pol)
			cells=SPLITCELLS( SKELETON(dim)(pol) )
			slice=[EMBED(1)(c) for c in cells]
			tensor=COMP([T(dim+1)(1.0/height),R([dim-1,dim])(angle/height)])
			layer=Hpc.Struct([JOIN([p,tensor(p)]) for p in slice])
			return(COMP([COMP([STRUCT, CAT]), DIESIS(height)]))([layer, tensor])
		return EXTRUSION0
	return EXTRUSION1

# EX
def EX(args):
	x1 ,x2 = args
	def EX0(pol):
		dim = DIM(pol)
		return T(dim+1)(x1)(S(dim+1)(x2-x1)(EXTRUSION(0.0)(1.0)(pol)))
	return EX0

# LEX
def LEX(args):
	x1 , x2 = args
	def LEX0(pol):
		def SHEARTENSOR(A):
			def SHEARTENSOR0(POL):
				dim = DIM(POL)
				newrow = K((AR([CAT([[0, 1],DIESIS((dim-2))(0)]),A])))
				update =(COMP([CONS, CAT]))([[S1, newrow],AA(SEL)((FROMTO([3,dim+1])))])
				matrix=  update(IDNT(dim+1))        
				return(MAT(matrix))(POL)
			return SHEARTENSOR0

		ret=EXTRUSION(0)(1)(pol)
		ret=SHEARTENSOR(x2-x1)(ret)
		ret=S(DIM(pol)+1)(x2-x1)(ret)
		ret=T(DIM(pol)+1)(x1)(ret)
		return ret
	return LEX0

# SEX 

def SEX(args):
	x1 , x2 = args
	def SEX1(height):
		def SEX0(pol):
			dim = DIM(pol)
			ret=EXTRUSION(x2-x1)(height)(pol)
			ret=S(dim+1)(x2-x1)(ret)
			ret=R([dim,dim-1])(x1)(ret)
			return ret
		return SEX0
	return SEX1
  

# POLAR 
if False:

  def POLAR(pol,precision=1e-6):
  	faces,cells,pols=UKPOLF(pol)
  	for i in range(len(faces)):
  		mod=-1*faces[i][0]
  		if math.fabs(mod)<precision:mod=1
  		faces[i]=[value/mod for value in faces[i][1:]]
  	return MKPOL([faces,cells,pols]) 
  	

# SWEEP 
def SWEEP(v):
  def SWEEP0(pol):
		ret=Hpc.power(pol,QUOTE([1]))
		mat=IDNT(len(v)+2)
		for i in range(len(v)):
			mat[i+1][len(v)+1]=v[i]
		ret=MAT(mat)(ret)
		return PROJECT(1)(ret)
  return SWEEP0

# MINKOWSKI
def MINKOWSKI(vects):
	def MINKOWSKI0(pol):
		ret=pol
		for i in range(len(vects)-1,-1,-1):
			ret=SWEEP(vects[i])(ret)
		return ret
	return MINKOWSKI0

# OFFSET 
def OFFSET(v):
  """
  >>> verts = [[0,0,0],[3,0,0],[3,2,0],[0,2,0],[0,0,1.5],[3,0,1.5],[3,2,1.5],[0,2,1.5],[0,1,2.2],[3,1,2.2]]
  >>> cells = [[1,2],[2,3],[3,4],[4,1],[5,6],[6,7],[7,8],[8,5],[1,5],[2,6],[3,7],[4,8],[5,9],[8,9],[6,10],[7,10], [9,10]]
  >>> pols = [[1]]
  >>> House = MKPOL([verts,cells,pols])
  >>> VIEW(Hpc.Struct([OFFSET([0.1,0.2,0.1])(House), T(1)(1.2*SIZE(1)(House))(House)]))
  """  
  def OFFSET0(pol):
  
  	ret=pol
  	for i in range(len(v)):
  
  		# shear vector
  		shear=[0 if j!=i else v[i] for j in range(len(v))] + [0 for j in range(i)]
  
  		# shear operation
  		mat=IDNT(len(shear)+2)
  		for i in range(len(shear)):
  			mat[i+1][len(shear)+1]=shear[i]
  		
  		# apply shearing
  		ret=MAT(mat)((Hpc.power(ret,QUOTE([1]))))
  
  	return PROJECT(len(v))(ret)
  return OFFSET0

# THINSOLID
def THINSOLID(surface,delta=1e-4):
  """
  >>> Su0 = COMP([BEZIERCURVE([[0,0,0],[10,0,0]]),CONS([S1])])
  >>> Su1 = COMP([BEZIERCURVE([[0,10,0],[2.5,10,3],[5,10,-3],[7.5,10,3],[10,10,0]]),CONS([S1]) ])
  >>> S0v = COMP([BEZIERCURVE([[0,0,0],[0,0,3],[0,10,3],[0,10,0]]) , CONS([S2]) ]) 
  >>> S1v = COMP([BEZIERCURVE([[10,0,0],[10,5,3],[10,10,0]]) ,CONS([S2])   ])
  >>> surface=COONSPATCH([Su0,Su1,S0v,S1v])
  >>> VIEW(MAP(  surface ) (Hpc.power(INTERVALS(1)(10),INTERVALS(1)(10))))
  >>> solidMapping = THINSOLID(surface)
  >>> Domain3D = Hpc.power(Hpc.power(INTERVALS(1)(5),INTERVALS(1)(5)),INTERVALS(0.5)(5))
  >>> VIEW(MAP(solidMapping)(Domain3D))
  """  
  def map_fn(point):
  	u,v,w=point
  	P0=surface([u,v])
  	PX=surface([u+delta,v])
  	PY=surface([u,v+delta])
  	GX=[PX[i]-P0[i] for i in range(3)]
  	GY=[PY[i]-P0[i] for i in range(3)]
  	normal=UNITVECT(VECTPROD([GX,GY]))
  	ret=[P0[i]+w*normal[i] for i in range(3)]
  	return ret
  
  return map_fn


# PLANE
def PLANE(args):
	p0 , p1 , p2 = args
	v1 = VECTDIFF([p1,p0])
	v2 = VECTDIFF([p2,p0])
	side1 = VECTNORM(v1)
	side2 = VECTNORM(v2)
	normal=UNITVECT(VECTPROD([v1,v2]))
	axis=VECTPROD([[0, 0, 1],normal])
	angle = math.acos((INNERPROD([[0, 0, 1],normal])))
	geometry=T([1,2,3])(p0)(ROTN([angle, axis])(T([1,2])([-1*side1,-1*side2])(CUBOID([2*side1, 2*side2]))))
	return  [normal, p0, geometry]

# RATIONAL BEZIER
def RATIONALBEZIER(controlpoints_fn):
	degree = len(controlpoints_fn)-1
	basis=BERNSTEINBASIS(S1)(degree)
	def map_fn(point):
		controlpoints=[f(point) if callable(f) else f for f in controlpoints_fn]
		target_dim=len(controlpoints[0])
		ret=[0 for i in range(target_dim)]
		for i in range(len(basis)):
			coeff=basis[i](point)
			for M in range(target_dim):
				ret[M]+=coeff * controlpoints[i][M] 
		last=ret[-1]
		if last!=0: ret=[value/last for value in ret]
		ret=ret[:-1]
		return  ret
	return map_fn

# ELLIPSE
def ELLIPSE(args):
  """
  >>> VIEW(ELLIPSE([1,2])(8))
  """
  A , B = args
  def ELLIPSE0(N):
    C = 0.5*math.sqrt(2)
    mapping = RATIONALBEZIER([[A, 0, 1], [A*C, B*C, C], [0, B, 1]])
    quarter = MAP(mapping)((INTERVALS(1.0)(N)))
    half = STRUCT([quarter, S(2)(-1)(quarter)])
    return STRUCT([half, S(1)(-1)(half)])
  return ELLIPSE0


# NORM2(==normal of a curve)
def CURVE_NORMAL(curve):
	def map_fn(point):
		xu,yu=curve(point)
		mod2=xu*xu+yu*yu
		den=math.sqrt(mod2) if mod2>0 else 0
		return [-yu/den,xu/den]
	return map_fn


# DERBEZIER
def DERBEZIER(controlpoints_fn):
	degree = len(controlpoints_fn)-1

	# derivative of bernstein
	def DERBERNSTEIN(N):
		def DERBERNSTEIN0(I):
			def map_fn(point):
				t=S1(point)
				return CHOOSE([N,I]) * math.pow(t,I-1) * math.pow(1-t,N-I-1) *(I-N*t)
			return  map_fn
		return DERBERNSTEIN0

	basis=[DERBERNSTEIN(degree)(i) for i in range(degree+1)]

	def map_fn(point):

		# if control points are functions
		controlpoints=[f(point) if callable(f) else f for f in controlpoints_fn]

		target_dim=len(controlpoints[0])

		ret=[0 for i in range(target_dim)]
		for i in range(len(basis)):
			coeff=basis[i](point)
			for M in range(target_dim):
				ret[M]+=coeff * controlpoints[i][M] 

		return ret

	return map_fn


# BEZIERSTRIPE
def BEZIERSTRIPE(args):
  """
  >>> vertices=[[0,0],[1.5,0],[-1,2],[2,2],[2,0]]
  >>> VIEW(Hpc.Struct([POLYLINE(vertices),Hpc.power(BEZIERSTRIPE([vertices,0.25,22]),QUOTE([0.9]))]))
  """  
  controlpoints, width, n = args
  bezier  = BEZIERCURVE(controlpoints)
  normal  = CURVE_NORMAL(DERBEZIER(controlpoints))
  def map_fn(point):
  	u,v=point
  	bx,by=bezier(point)
  	nx,ny=normal(point)
  	ret=[bx+v*nx,by+v*ny]
  	return ret
  domain=S(2)(width)(T(1)(0.00001)(Hpc.power(INTERVALS(1)(n),INTERVALS(1)(1))))
  return MAP(map_fn)(domain)


# BSPLINE
def BSPLINE(degree):
	def BSPLINE0(knots):
		def BSPLINE1(points_fn):

			n=len(points_fn)-1
			m=len(knots)-1
			k=degree+1
			T=knots
			tmin,tmax=T[k-1],T[n+1]

			# see http://www.na.iac.cnr.it/~bdv/cagd/spline/B-spline/bspline-curve.html
			if len(knots)!=(n+k+1):
				raise Exception("Invalid point/knots/degree for bspline!")

			# de boord coefficients
			def N(i,k,t):

				# Ni1(t)
				if k==1: 
					if(t>=T[i] and t<T[i+1]) or(t==tmax and t>=T[i] and t<=T[i+1]): # i use strict inclusion for the max value
						return 1
					else:
						return 0

				# Nik(t)
				ret=0

				num1,div1= t-T[i], T[i+k-1]-T[i]  
				if div1!=0: ret+=(num1/div1) * N(i,k-1,t)

				num2,div2=T[i+k]-t, T[i+k]-T[i+1]
				if div2!=0:  ret+=(num2/div2) * N(i+1,k-1,t)

				return ret

			# map function
			def map_fn(point):
				t=point[0]

				# if control points are functions
				points=[f(point) if callable(f) else f for f in points_fn]

				target_dim=len(points[0])
				ret=[0 for i in range(target_dim)];
				for i in range(n+1):
					coeff=N(i,k,t) 
					for M in range(target_dim):
						ret[M]+=points[i][M]*coeff
				return ret

			return map_fn

		return BSPLINE1
	return BSPLINE0



# NUBSPLINE
def NUBSPLINE(degree,totpoints=80):
	def NUBSPLINE1(knots):
		def NUBSPLINE2(points):
			m=len(knots)
			tmin=min(knots)
			tmax=max(knots)
			tsiz=tmax-tmin
			v=[tsiz/float(totpoints-1) for i in range(totpoints-1)]
			assert len(v)+1==totpoints
			v=[-tmin] + v
			domain=QUOTE(v)
			return MAP(BSPLINE(degree)(knots)(points))(domain)
		return NUBSPLINE2
	return NUBSPLINE1

# DISPLAYNUBSPLINE
def DISPLAYNUBSPLINE(args,marker_size=0.1):
  """
  >>> ControlPoints=[[0,0],[-1,2],[1,4],[2,3],[1,1],[1,2],[2.5,1], [2.5,3], [4,4],[5,0]]
  >>> VIEW(DISPLAYNUBSPLINE([3,[0,0,0,0, 1,2,3,4,5, 6    ,7,7,7,7], ControlPoints]))
  """  
  degree, knots, points = args
  spline_view_knots=POLYMARKER(2,marker_size)(UKPOL(NUBSPLINE(degree,len(knots))(knots)(points))[0])
  return  STRUCT([
  	NUBSPLINE(degree)(knots)(points) if degree>0 else POLYMARKER(3,marker_size)(points)
  	,spline_view_knots
  	,POLYLINE(points)
  	,POLYMARKER(1,marker_size)(points)
  ])


# RATIONALBSPLINE
def RATIONALBSPLINE(degree):
	def RATIONALBSPLINE0(knots):
		def RATIONALBSPLINE1(points):
			bspline=BSPLINE(degree)(knots)(points)
			def map_fn(point):			
				ret=bspline(point)
				last=ret[-1]
				if last!=0: ret=[value/last for value in ret]
				ret=ret[:-1]
				return ret
			return map_fn
		return RATIONALBSPLINE1
	return RATIONALBSPLINE0


# NURBSPLINE
def NURBSPLINE(degree,totpoints=80):
	def NURBSPLINE1(knots):
		def NURBSPLINE2(points):
			m=len(knots)
			tmin=min(knots)
			tmax=max(knots)
			tsiz=tmax-tmin
			v=[tsiz/float(totpoints-1) for i in range(totpoints-1)]
			assert len(v)+1==totpoints
			v=[-tmin] + v
			domain=QUOTE(v)
			return MAP(RATIONALBSPLINE(degree)(knots)(points))(domain)
		return NURBSPLINE2
	return NURBSPLINE1

# DISPLAYNURBSPLINE
def DISPLAYNURBSPLINE(args,marker_size=0.1):
  """
  >>> knots = [0,0,0,1,1,2,2,3,3,4,4,4]
  >>> _p=math.sqrt(2)/2.0
  >>> controlpoints = [[-1,0,1], [-_p,_p,_p], [0,1,1], [_p,_p,_p],[1,0,1], [_p,-_p,_p], [0,-1,1], [-_p,-_p,_p], [-1,0,1]]
  >>> VIEW(DISPLAYNURBSPLINE([2, knots, controlpoints]))
  """
    
  degree, knots, points = args
  
  spline_view_knots=POLYMARKER(2,marker_size)(UKPOL(NURBSPLINE(degree,len(knots))(knots)(points))[0])
  
  return  STRUCT([
  	NURBSPLINE(degree)(knots)(points) if degree>0 else POLYMARKER(3,marker_size)(points)
  	,spline_view_knots
  	,POLYLINE(points)
  	,POLYMARKER(1,marker_size)(points)
  ])


# Colors(wants a list [R,G,B] or [R,G,B,A]
# Example COLOR([1,0,0])(pol)
if False:
  
  def COLOR(C):
  
  	def formatColor(C):
  		assert isinstance(C,Color4f) 
  		return "%s %s %s %s" %(C.r,C.g,C.b,C.a)
  
  	# convert list to Color
  	if isinstance(C,list) and len(C) in(3,4):
  		C=Color4f(C[0],C[1],C[2],C[3] if len(C)>=4 else 1.0)
  
  	if not isinstance(C,Color4f):
  		raise Exception("cannot transform " + repr(C) + " to Color4f")
  
  	def COLOR0(pol):
  		return pol.addProperty("RGBcolor", formatColor(C))
  
  	return COLOR0
  
  GRAY  = Color4f([0.5, 0.5, 0.5, 1.0])
  GREEN   = Color4f([0.0, 1.0, 0.0, 1.0])
  BLACK   = Color4f([0.0, 0.0, 0.0, 1.0])
  BLUE  = Color4f([0.0, 0.0, 1.0, 1.0])
  BROWN   = Color4f([0.5, 0.5, 0.5, 1.0])
  CYAN  = Color4f([0.0, 1.0, 1.0, 1.0])
  MAGENTA = Color4f([1.0, 0.0, 1.0, 1.0])
  ORANGE  = Color4f([1.0, 0.5, 1.0, 1.0])
  PURPLE  = Color4f([0.5, 0.0, 0.5, 1.0])
  WHITE   = Color4f([1.0, 1.0, 1.0, 1.0])
  RED   = Color4f([1.0, 0.0, 0.0, 1.0])
  YELLOW  = Color4f([1.0, 1.0, 0.0, 1.0])
  
  
# Materials(want a list of 17 elements(ambientRGBA, diffuseRGBA specularRGBA emissionRGBA shininess)
# Example MATERIAL([1,0,0,1,  0,1,0,1,  0,0,1,0, 0,0,0,1, 100])(pol)
if False:
  
  def MATERIAL(M):
  
  	def MATERIAL0(pol):
  
  		svalue="%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s" %(M[ 0],M[ 1],M[ 2],M[ 3],M[ 4],M[ 5],M[ 6],M[ 7],M[ 8],M[ 9],M[10],M[11],M[12],M[13],M[14],M[15],M[16])
  		return pol.addProperty("VRMLmaterial", svalue) 
  
  	# convert list to Material
  	if isinstance(M,list) and(len(M)==3 or len(M)==4):
  		r,g,b=M[0:3]
  		a=M[3] if len(M)==4 else 1.0
  		ambient =[r*0.4,g*0.4,b*0.4,alpha]
  		diffuse =[r*0.6,g*0.6,b*0.6,alpha]
  		specular=[0  ,0  ,0  ,alpha]
  		emission=[0  ,0  ,0  ,alpha]
  		shininess
  		M=ambient + diffuse + specular + emission + [shininess]
  
  	#convert the list to a XGE material
  	if not(isinstance(M,list) and len(M)==17):
  		raise Exception("cannot transform " + repr(M) + " in a material(which is a list of 17 floats, ambient,diffuse,specular,emission,shininess)")
  
  	return MATERIAL0
  

# Textures(wants a list [url:string,repeatS:bool,repeatT:bool,cx::float,cy::float,rot::float,sx::float,sy::float,tx::float,ty::float]
# Example TEXTURE('filename.png')(pol)
if False:
  
  def TEXTURE(params):
  
  	def TEXTURE0(params,pol):
  
  		# is simply an URL
  		if isinstance(params,str):
  			url=params
  			params=[]
  		# is a list with a configuration
  		else:
  			assert isinstance(params,list) and len(params)>=1
  			url=params[0]
  			if not isinstance(url,str): 
  				raise Exception("Texture error " + repr(url) + " is not a path")
  			params=params[1:]
  
  		# complete with default parameters
  		params+=[True,True, 0.0,0.0,  0.0,   1.0,1.0,  0.0,0.0   ][len(params):]
  
  		# unpack
  		repeatS, repeatT, cx, cy, rot, sx, sy, tx, ty=params
  
  		spacedim = pol.dim()
  		
  		if not(spacedim in(2,3)):
  			# raise Exception("Texture cannot be applyed only to 2 or 3 dim pols")
  			return pol.copy()
  
  		box = pol.box()
  		ref0,ref1=[box.maxsizeidx(),box.minsizeidx()]
  
  		if(spacedim==3):
  			ref1=1 if(ref0!=1 and ref1!=1) else(2 if(ref0!=2 and ref1!=2) else 3)
  			
  		assert ref0!=ref1
  		
  		# empty box
  		if(box.size()[ref0]==0 or box.size()[ref1]==0):
  			return pol.copy()
  		
  		# translate vector
  		vt=[
  			-box.p1[1] if box.dim()>=1 else 0.0, \
  			-box.p1[2] if box.dim()>=2 else 0.0, \
  			-box.p1[3] if box.dim()>=3 else 0.0]
  		
  		# scale vector
  		vs=[
  			1.0/(box.size()[1]) if box.dim()>=1 and box.size()[1] else 1.0, \
  			1.0/(box.size()[2]) if box.dim()>=2 and box.size()[2] else 1.0, \
  			1.0/(box.size()[3]) if box.dim()>=3 and box.size()[3] else 1.0]
  		
  		# permutation
  		refm=1 if(ref0!=1 and ref1!=1) else(2 if(ref0!=2 and ref1!=2) else 3)
  		assert ref0!=ref1 and ref1!=refm and ref0!=refm
  		perm=[0,0,0,0]
  		perm[ref0]=1
  		perm[ref1]=2
  		perm[refm]=3 
  
  		project_uv=Matrix.translate([+cx,+cy,0]) \
  				* Matrix.scale([sx,sy,1]) \
  				* Matrix.rotate(1,2,-rot) \
  				* Matrix.translate([-cx,-cy,0]) \
  				* Matrix.translate([tx,ty,0])  \
  				* Matrix(3).swapCols(perm) \
  				* Matrix.scale(vs) \
  				* Matrix.translate(vt)
  
  		return Plasm.Skin(pol,url,project_uv)
  
  	return lambda pol: TEXTURE0(params,pol)



######################################################################
if __name__=="__main__":
  import doctest
  failed, total = doctest.testmod()
  print "%d/%d failed" % (failed, total)