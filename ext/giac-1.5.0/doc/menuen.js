<menu type="context" id="cmdmenu">
    <menu label="Expressions">
        <menuitem label="factor" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
        <menuitem label="simplify" onclick="UI.insert(UI.focused,'simplify(')"></menuitem>
        <menuitem label="subst" onclick="UI.insert(UI.focused,'subst(')"></menuitem>
        <menu label="Exp et ln">
            <menuitem label="exp2pow: exp(n*ln(x)) -> x^n" onclick="UI.insert(UI.focused,'exp2pow(')"></menuitem>
            <menuitem label="hyp2exp: hyperbolic to exp" onclick="UI.insert(UI.focused,'hyp2exp(')"></menuitem>
            <menuitem label="expexpand: expand exp" onclick="UI.insert(UI.focused,'expexpand(')"></menuitem>
            <menuitem label="lin: linearize exp" onclick="UI.insert(UI.focused,'lin(')"></menuitem>
            <menuitem label="lncollect: regroup log" onclick="UI.insert(UI.focused,'lncollect(')"></menuitem>
            <menuitem label="lnexpand: expand log" onclick="UI.insert(UI.focused,'lnexpand(')"></menuitem>
            <menuitem label="powexpand: expand ^" onclick="UI.insert(UI.focused,'powexpand(')"></menuitem>
            <menuitem label="pow2exp: ^ -> exp" onclick="UI.insert(UI.focused,'pow2exp(')"></menuitem>
        </menu>
        <menu label="Trigonometry">
            <menuitem label="trigexpand: expand sin,cos,tan" onclick="UI.insert(UI.focused,'trigexpand(')"></menuitem>
            <menuitem label="tlin: linearization (trigonometry)" onclick="UI.insert(UI.focused,'tlin(')"></menuitem>
            <menuitem label="tcollect: regroup cos and sin" onclick="UI.insert(UI.focused,'tcollect(')"></menuitem>
            <menuitem label="halftan: in terms of tan(x sur 2)" onclick="UI.insert(UI.focused,'halftan(')"></menuitem>
            <menuitem label="cos2sintan: cos -> sin over tan" onclick="UI.insert(UI.focused,'cos2sintan(')"></menuitem>
            <menuitem label="sin2costan: sin -> cos*tan" onclick="UI.insert(UI.focused,'sin2costan(')"></menuitem>
            <menuitem label="tan2sincos: tan -> sin over cos" onclick="UI.insert(UI.focused,'tan2sincos(')"></menuitem>
            <menuitem label="tan2cossin2: tan -> cos, sin 2*angle" onclick="UI.insert(UI.focused,'tan2cossin2(')"></menuitem>
            <menuitem label="tan2sincos2: tan -> sin. cos 2*angle" onclick="UI.insert(UI.focused,'tan2sincos2(')"></menuitem>
            <menuitem label="trigcos: sin^2 -> 1-cos^2" onclick="UI.insert(UI.focused,'trigcos(')"></menuitem>
            <menuitem label="trigsin: cos^2 -> 1-sin^2" onclick="UI.insert(UI.focused,'trigsin(')"></menuitem>
            <menuitem label="trigtan: cos^2 et sin^2 -> tan^2" onclick="UI.insert(UI.focused,'trigtan(')"></menuitem>
            <menuitem label="shift_phase" onclick="UI.insert(UI.focused,'shift_phase(')"></menuitem>
            <menuitem label="exp2trig: exp(i*x) -> cos(x)+i*sin(x)" onclick="UI.insert(UI.focused,'exp2trig(')"></menuitem>
            <menuitem label="trig2exp: cos and sin -> exp(i*angle)" onclick="UI.insert(UI.focused,'trig2exp(')"></menuitem>
            <menuitem label="tsimplify: simplify transcendent" onclick="UI.insert(UI.focused,'tsimplify(')"></menuitem>
            <menuitem label="texpand: expand trig, exp, ln" onclick="UI.insert(UI.focused,'texpand(')"></menuitem>
            <menuitem label="atrig2ln: trig inverse -> complex ln" onclick="UI.insert(UI.focused,'atrig2ln(')"></menuitem>
            <menuitem label="acos2asin: acos -> asin" onclick="UI.insert(UI.focused,'acos2asin(')"></menuitem>
            <menuitem label="acos2atan: acos -> atan" onclick="UI.insert(UI.focused,'acos2atan(')"></menuitem>
            <menuitem label="asin2acos: asin -> acos" onclick="UI.insert(UI.focused,'asin2acos(')"></menuitem>
            <menuitem label="asin2atan: asin -> atan" onclick="UI.insert(UI.focused,'asin2atan(')"></menuitem>
            <menuitem label="atan2acos: atan -> acos" onclick="UI.insert(UI.focused,'atan2acos(')"></menuitem>
            <menuitem label="atan2asin: atan -> asin" onclick="UI.insert(UI.focused,'atan2asin(')"></menuitem>
        </menu>
        <menu label="Rational">
            <menuitem label="collect" onclick="UI.insert(UI.focused,'collect(')"></menuitem>
            <menuitem label="normal" onclick="UI.insert(UI.focused,'normal(')"></menuitem>
            <menuitem label="eval" onclick="UI.insert(UI.focused,'eval(')"></menuitem>
            <menuitem label="equal2diff" onclick="UI.insert(UI.focused,'equal2diff(')"></menuitem>
            <menuitem label="f2nd" onclick="UI.insert(UI.focused,'f2nd(')"></menuitem>
            <menuitem label="ratnormal" onclick="UI.insert(UI.focused,'ratnormal(')"></menuitem>
            <menuitem label="reorder: variables" onclick="UI.insert(UI.focused,'reorder(')"></menuitem>
            <menuitem label="partfrac: partial fraction" onclick="UI.insert(UI.focused,'partfrac(')"></menuitem>
            <menuitem label="cpartfrac: partial fraction over C" onclick="UI.insert(UI.focused,'cpartfrac(')"></menuitem>
            <menuitem label="simp2" onclick="UI.insert(UI.focused,'simp2(')"></menuitem>
            <menuitem label="collect" onclick="UI.insert(UI.focused,'regroup(')"></menuitem>
            <menuitem label="factor" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
            <menuitem label="expand" onclick="UI.insert(UI.focused,'expand(')"></menuitem>
            <menuitem label="simplify" onclick="UI.insert(UI.focused,'simplify(')"></menuitem>
            <menuitem label="mult_conjugate" onclick="UI.insert(UI.focused,'mult_conjugate(')"></menuitem>
            <menuitem label="mult_c_conjugate" onclick="UI.insert(UI.focused,'mult_c_conjugate(')"></menuitem>
        </menu>
        <menu label="Reals">
            <menuitem label="epsilon2zero" onclick="UI.insert(UI.focused,'epsilon2zero(')"></menuitem>
            <menuitem label="evalf: approx evaluation" onclick="UI.insert(UI.focused,'evalf(')"></menuitem>
            <menuitem label="float2rational" onclick="UI.insert(UI.focused,'float2rational(')"></menuitem>
        </menu>
    </menu>

    <menu label="Solve">
        <menuitem label="linsolve: linear system" onclick="UI.insert(UI.focused,'linsolve(')"></menuitem>
        <menuitem label="solve: equation or system" onclick="UI.insert(UI.focused,'solve(')"></menuitem>
        <menuitem label="fsolve: numerically" onclick="UI.insert(UI.focused,'fsolve(')"></menuitem>
        <menuitem label="newton: solve by Newton method" onclick="UI.insert(UI.focused,'newton(')"></menuitem>
        <menuitem label="csolve: solve over C" onclick="UI.insert(UI.focused,'csolve(')"></menuitem>
        <menuitem label="desolve: solve differential eq." onclick="UI.insert(UI.focused,'desolve(')"></menuitem>
        <menuitem label="rsolve: solve recurrence relation" onclick="UI.insert(UI.focused,'rsolve(')"></menuitem>
        <menuitem label="odesolve: numerical diff. eq." onclick="UI.insert(UI.focused,'odesolve(')"></menuitem>
        <menuitem label="greduce: reduction mod gbasis" onclick="UI.insert(UI.focused,'greduce(')"></menuitem>
        <menuitem label="gbasis: Groebner basis" onclick="UI.insert(UI.focused,'gbasis(')"></menuitem>
    </menu>
    <menu label="Calculus">
        <menuitem label="limit" onclick="UI.insert(UI.focused,'limit(')"></menuitem>
        <menuitem label="series: Taylor expansion" onclick="UI.insert(UI.focused,'series(')"></menuitem>
        <menuitem label="diff: derivative" onclick="UI.insert(UI.focused,'diff(')"></menuitem>
        <menuitem label="ibpdv: integration by parts" onclick="UI.insert(UI.focused,'ibpdv(')"></menuitem>
        <menuitem label="ibpu: integration by parts" onclick="UI.insert(UI.focused,'ibpu(')"></menuitem>
        <menuitem label="integrate: antiderivative" onclick="UI.insert(UI.focused,'integrate(')"></menuitem>
        <menuitem label="preval: antiderivative evaluation" onclick="UI.insert(UI.focused,'preval(')"></menuitem>
        <menuitem label="sum: discret sum" onclick="UI.insert(UI.focused,'sum(')"></menuitem>
        <menuitem label="curl" onclick="UI.insert(UI.focused,'curl(')"></menuitem>
        <menuitem label="diff: gradient" onclick="UI.insert(UI.focused,'gradient(')"></menuitem>
        <menuitem label="divergence: " onclick="UI.insert(UI.focused,'divergence(')"></menuitem>
        <menuitem label="hessian: " onclick="UI.insert(UI.focused,'hessian(')"></menuitem>
        <menuitem label="laplacian: " onclick="UI.insert(UI.focused,'laplacian(')"></menuitem>
        <menuitem label="potential: scalar potential" onclick="UI.insert(UI.focused,'potential(')"></menuitem>
        <menuitem label="vpotential: vector potential" onclick="UI.insert(UI.focused,'vpotential(')"></menuitem>
        <menu label="Transformations">
            <menuitem label="fft: Fast Fourier Transform" onclick="UI.insert(UI.focused,'fft(')"></menuitem>
            <menuitem label="fourier_an: an coeff" onclick="UI.insert(UI.focused,'fourier_an(')"></menuitem>
            <menuitem label="fourier_bn: bn coeff" onclick="UI.insert(UI.focused,'fourier_bn(')"></menuitem>
            <menuitem label="fourier_cn: cn coeff" onclick="UI.insert(UI.focused,'fourier_cn(')"></menuitem>
            <menuitem label="ifft: inverse fft" onclick="UI.insert(UI.focused,'ifft(')"></menuitem>
            <menuitem label="ilaplace: inverse Laplace" onclick="UI.insert(UI.focused,'ilaplace(')"></menuitem>
            <menuitem label="laplace: transform" onclick="UI.insert(UI.focused,'laplace(')"></menuitem>
        </menu>
    </menu>

    <menu label="Integers">
        <menuitem label="euler: indicatrix" onclick="UI.insert(UI.focused,'euler(')"></menuitem>
        <menuitem label="fracmod: a mod p to n and d" onclick="UI.insert(UI.focused,'fracmod(')"></menuitem>
        <menuitem label="gcd: greatest common divisor" onclick="UI.insert(UI.focused,'gcd(')"></menuitem>
        <menuitem label="iabcuv: a,b,c to u,v such that au+bv=c" onclick="UI.insert(UI.focused,'iabcuv(')"></menuitem>
        <menuitem label="ichinrem: chinese remainder over Z" onclick="UI.insert(UI.focused,'ichinrem(')"></menuitem>
        <menuitem label="idivis: integer divisors" onclick="UI.insert(UI.focused,'idivis(')"></menuitem>
        <menuitem label="iegcd: extended GCD over Z" onclick="UI.insert(UI.focused,'iegcd(')"></menuitem>
        <menuitem label="ifactor: integer factorization" onclick="UI.insert(UI.focused,'ifactor(')"></menuitem>
        <menuitem label="ifactors: factors and multiplicities" onclick="UI.insert(UI.focused,'ifactors(')"></menuitem>
        <menuitem label="iquo: quotient (integer division)" onclick="UI.insert(UI.focused,'iquo(')"></menuitem>
        <menuitem label="iquorem: quotient and remainder" onclick="UI.insert(UI.focused,'iquorem(')"></menuitem>
        <menuitem label="irem: remainder" onclick="UI.insert(UI.focused,'irem(')"></menuitem>
        <menuitem label="is_prime: primality testing" onclick="UI.insert(UI.focused,'is_prime(')"></menuitem>
        <menuitem label="is_pseudoprime" onclick="UI.insert(UI.focused,'is_pseudoprime(')"></menuitem>
        <menuitem label="lcm" onclick="UI.insert(UI.focused,'lcm(')"></menuitem>
        <menuitem label="mod: class of a in Z over nZ" onclick="UI.insert(UI.focused,'mod(')"></menuitem>
        <menuitem label="nextprime" onclick="UI.insert(UI.focused,'nextprime(')"></menuitem>
        <menuitem label="nprimes: # of prime numbers &lt;=" onclick="UI.insert(UI.focused,'prevprime(')"></menuitem>
        <menuitem label="pa2b2" onclick="UI.insert(UI.focused,'pa2b2(')"></menuitem>
        <menuitem label="powmod: fast power" onclick="UI.insert(UI.focused,'powmod(')"></menuitem>
        <menuitem label="prevprime" onclick="UI.insert(UI.focused,'prevprime(')"></menuitem>
        <menuitem label="smod: symmetric remainder" onclick="UI.insert(UI.focused,'smod(')"></menuitem>
    </menu>
    <menu label="Polynomials">
        <menu label="Base">
            <menuitem label="canonical_form" onclick="UI.insert(UI.focused,'canonical_form(')"></menuitem>
            <menuitem label="cfactor: factorization over C" onclick="UI.insert(UI.focused,'cfactor(')"></menuitem>
            <menuitem label="coeff: coefficients list" onclick="UI.insert(UI.focused,'coeff(')"></menuitem>
            <menuitem label="cyclotomic" onclick="UI.insert(UI.focused,'cyclotomic(')"></menuitem>
            <menuitem label="degree" onclick="UI.insert(UI.focused,'degree(')"></menuitem>
            <menuitem label="expand" onclick="UI.insert(UI.focused,'expand(')"></menuitem>
            <menuitem label="factor_xn" onclick="UI.insert(UI.focused,'factor_xn(')"></menuitem>
            <menuitem label="factor" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
            <menuitem label="factors: factors list" onclick="UI.insert(UI.focused,'factors(')"></menuitem>
            <menuitem label="fcoeff" onclick="UI.insert(UI.focused,'fcoeff(')"></menuitem>
            <menuitem label="froot" onclick="UI.insert(UI.focused,'froot(')"></menuitem>
            <menuitem label="genpoly: P(a) -> P" onclick="UI.insert(UI.focused,'genpoly(')"></menuitem>
            <menuitem label="lcoeff: leading coefficient" onclick="UI.insert(UI.focused,'lcoeff(')"></menuitem>
            <menuitem label="normal: form" onclick="UI.insert(UI.focused,'normal(')"></menuitem>
            <menuitem label="pcoeff: roots -> coefficients" onclick="UI.insert(UI.focused,'pcoeff(')"></menuitem>
            <menuitem label="peval: P,a -> P(a)" onclick="UI.insert(UI.focused,'peval(')"></menuitem>
            <menuitem label="proot: coefficients -> roots" onclick="UI.insert(UI.focused,'proot(')"></menuitem>
            <menuitem label="poly2symb" onclick="UI.insert(UI.focused,'poly2symb(')"></menuitem>
            <menuitem label="randpoly: random polynomial" onclick="UI.insert(UI.focused,'randpoly(')"></menuitem>
            <menuitem label="symb2poly" onclick="UI.insert(UI.focused,'symb2poly(')"></menuitem>
            <menuitem label="tcoeff: lowest degree coefficient" onclick="UI.insert(UI.focused,'tcoeff(')"></menuitem>
            <menuitem label="valuation" onclick="UI.insert(UI.focused,'valuation(')"></menuitem>
        </menu>
        <menu label="Arithmetic">
            <menuitem label="abcuv: a,b,c -> u,v where au+bv=c" onclick="UI.insert(UI.focused,'abcuv(')"></menuitem>
            <menuitem label="chinrem: chinese remainder over Q[X]" onclick="UI.insert(UI.focused,'chinrem(')"></menuitem>
            <menuitem label="divis: divisors in Q[X]" onclick="UI.insert(UI.focused,'divis(')"></menuitem>
            <menuitem label="divpc: ascending power division" onclick="UI.insert(UI.focused,'divpc(')"></menuitem>
            <menuitem label="egcd: Bezout polynomials" onclick="UI.insert(UI.focused,'egcd(')"></menuitem>
            <menuitem label="factor" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
            <menuitem label="gcd" onclick="UI.insert(UI.focused,'gcd(')"></menuitem>
            <menuitem label="horner: P,a -> P(a)" onclick="UI.insert(UI.focused,'horner(')"></menuitem>
            <menuitem label="lagrange: interpolation" onclick="UI.insert(UI.focused,'lagrange(')"></menuitem>
            <menuitem label="lcm" onclick="UI.insert(UI.focused,'lcm(')"></menuitem>
            <menuitem label="normal" onclick="UI.insert(UI.focused,'normal(')"></menuitem>
            <menuitem label="ptayl: Taylor for polynomials" onclick="UI.insert(UI.focused,'ptayl(')"></menuitem>
            <menuitem label="quo: quotient (euclidean division)" onclick="UI.insert(UI.focused,'quo(')"></menuitem>
            <menuitem label="quorem: quotient and remainder" onclick="UI.insert(UI.focused,'quorem(')"></menuitem>
            <menuitem label="rem: remainder" onclick="UI.insert(UI.focused,'rem(')"></menuitem>
            <menuitem label="resultant" onclick="UI.insert(UI.focused,'resultant(')"></menuitem>
            <menuitem label="spline" onclick="UI.insert(UI.focused,'spline(')"></menuitem>
            <menuitem label="sturm: sequence" onclick="UI.insert(UI.focused,'sturm(')"></menuitem>
            <menuitem label="sturmab: roots number" onclick="UI.insert(UI.focused,'sturmab(')"></menuitem>
        </menu>
        <menu label="Orthogonal">
            <menuitem label="hermite" onclick="UI.insert(UI.focused,'hermite(')"></menuitem>
            <menuitem label="laguerre" onclick="UI.insert(UI.focused,'laguerre(')"></menuitem>
            <menuitem label="legendre" onclick="UI.insert(UI.focused,'legendre(')"></menuitem>
            <menuitem label="tchebyshev1" onclick="UI.insert(UI.focused,'tchebyshev1(')"></menuitem>
            <menuitem label="tchebyshev2" onclick="UI.insert(UI.focused,'tchebyshev2(')"></menuitem>
        </menu>
    </menu>
    <menu label="Linear algebra">
        <menu label="Vectors">
            <menuitem label="abs: " onclick="UI.insert(UI.focused,'abs(')"></menuitem>
            <menuitem label="cross" onclick="UI.insert(UI.focused,'cross(')"></menuitem>
            <menuitem label="dot: product" onclick="UI.insert(UI.focused,'dot(')"></menuitem>
            <menuitem label="l1norm: sum of |x_i|" onclick="UI.insert(UI.focused,'l1norm(')"></menuitem>
            <menuitem label="l2norm: euclidean norm" onclick="UI.insert(UI.focused,'l2norm(')"></menuitem>
            <menuitem label="maxnorm: max norm" onclick="UI.insert(UI.focused,'maxnorm(')"></menuitem>
        </menu>
        <menu label="Matrice">
            <menuitem label="hadamard: product" onclick="UI.insert(UI.focused,'hadamard(')"></menuitem>
            <menuitem label="hilbert: matrix" onclick="UI.insert(UI.focused,'hilbert(')"></menuitem>
            <menuitem label="identity: matrix" onclick="UI.insert(UI.focused,'identity(')"></menuitem>
            <menuitem label="makemat" onclick="UI.insert(UI.focused,'makemat(')"></menuitem>
            <menuitem label="ncols: columns number" onclick="UI.insert(UI.focused,'ncols(')"></menuitem>
            <menuitem label="nrows: rows number" onclick="UI.insert(UI.focused,'nrows(')"></menuitem>
            <menuitem label="ranm: random matrix" onclick="UI.insert(UI.focused,'ranm(')"></menuitem>
            <menuitem label="syst2mat: system matrix" onclick="UI.insert(UI.focused,'syst2mat(')"></menuitem>
            <menuitem label="vandermonde: matrix" onclick="UI.insert(UI.focused,'vandermonde(')"></menuitem>
            <menuitem label="trace" onclick="UI.insert(UI.focused,'trace(')"></menuitem>
            <menuitem label="trn: transconjugate" onclick="UI.insert(UI.focused,'trn(')"></menuitem>
            <menuitem label="tran: transposed" onclick="UI.insert(UI.focused,'tran(')"></menuitem>
            <menuitem label="l1norm: triple l1 norm" onclick="UI.insert(UI.focused,'l1norm(')"></menuitem>
            <menuitem label="l2norm: triple l2 norm" onclick="UI.insert(UI.focused,'l2norm(')"></menuitem>
            <menuitem label="linfnorm: triple l inf norm" onclick="UI.insert(UI.focused,'linfnorm(')"></menuitem>
            <menuitem label="frobenius_norm" onclick="UI.insert(UI.focused,'frobenius_norm(')"></menuitem>
        </menu>
        <menu label="Gauss">
            <menuitem label="basis" onclick="UI.insert(UI.focused,'basis(')"></menuitem>
            <menuitem label="changebase: A,P -> P^-1*A*P" onclick="UI.insert(UI.focused,'changebase(')"></menuitem>
            <menuitem label="det: determinant" onclick="UI.insert(UI.focused,'det(')"></menuitem>
            <menuitem label="det_minor: by minor expansion" onclick="UI.insert(UI.focused,'det_minor(')"></menuitem>
            <menuitem label="inv: inverse" onclick="UI.insert(UI.focused,'inv(')"></menuitem>
            <menuitem label="ibasis: intersection vector space" onclick="UI.insert(UI.focused,'ibasis)')"></menuitem>
            <menuitem label="ihermite: Hermite form over Z" onclick="UI.insert(UI.focused,'ihermite(')"></menuitem>
            <menuitem label="ismith: Smith form over Z" onclick="UI.insert(UI.focused,'ismith(')"></menuitem>
            <menuitem label="ker" onclick="UI.insert(UI.focused,'ker(')"></menuitem>
            <menuitem label="image" onclick="UI.insert(UI.focused,'image(')"></menuitem>
            <menuitem label="linsolve: solve linear system" onclick="UI.insert(UI.focused,'linsolve(')"></menuitem>
            <menuitem label="rank" onclick="UI.insert(UI.focused,'rank(')"></menuitem>
            <menuitem label="rref: reduction to echelon form" onclick="UI.insert(UI.focused,'rref(')"></menuitem>
            <menuitem label="simplex_reduce" onclick="UI.insert(UI.focused,'simplex_reduce(')"></menuitem>
            <menuitem label="cond: condition number" onclick="UI.insert(UI.focused,'cond(')"></menuitem>
        </menu>
        <menu label="Diagonalization">
            <menuitem label="adjoint_matrix" onclick="UI.insert(UI.focused,'adjoint_matrix(')"></menuitem>
            <menuitem label="companion" onclick="UI.insert(UI.focused,'companion(')"></menuitem>
            <menuitem label="egv: eigenvectors P" onclick="UI.insert(UI.focused,'egv(')"></menuitem>
            <menuitem label="egvl: D=P^-1*A*P" onclick="UI.insert(UI.focused,'egvl(')"></menuitem>
            <menuitem label="eigenvalues" onclick="UI.insert(UI.focused,'eigenvalues(')"></menuitem>
            <menuitem label="hessenberg: factorization" onclick="UI.insert(UI.focused,'hessenberg(')"></menuitem>
            <menuitem label="jordan: form" onclick="UI.insert(UI.focused,'jordan(')"></menuitem>
            <menuitem label="pcar: characteristic polynomial" onclick="UI.insert(UI.focused,'pcar(')"></menuitem>
            <menuitem label="pcar_hessenberg" onclick="UI.insert(UI.focused,'pcar_hessenberg(')"></menuitem>
            <menuitem label="pmin: minimal polynomial" onclick="UI.insert(UI.focused,'pmin(')"></menuitem>
            <menuitem label="svl: singular values" onclick="UI.insert(UI.focused,'svl(')"></menuitem>
        </menu>
        <menu label="Factorization">
            <menuitem label="cholesky" onclick="UI.insert(UI.focused,'cholesky(')"></menuitem>
            <menuitem label="lll" onclick="UI.insert(UI.focused,'lll(')"></menuitem>
            <menuitem label="lu: P*A=L*U" onclick="UI.insert(UI.focused,'lu(')"></menuitem>
            <menuitem label="qr: A=Q*R" onclick="UI.insert(UI.focused,'qr(')"></menuitem>
            <menuitem label="svd: singular value decomposition" onclick="UI.insert(UI.focused,'svd(')"></menuitem>
        </menu>
        <menu label="Iterations">
            <menuitem label="jacobi_linsolve" onclick="UI.insert(UI.focused,'jacobi_linsolve(')"></menuitem>
            <menuitem label="gauss_seidel_linsolve" onclick="UI.insert(UI.focused,'gauss_seidel_linsolve(')"></menuitem>
            <menuitem label="conjugate_gradient" onclick="UI.insert(UI.focused,'conjugate_gradient(')"></menuitem>
        </menu>
        <menu label="Quadratic forms">
            <menuitem label="a2q: matrix -> quad. form" onclick="UI.insert(UI.focused,'a2q(')"></menuitem>
            <menuitem label="gauss: decomposition + and - of squares" onclick="UI.insert(UI.focused,'gauss(')"></menuitem>
            <menuitem label="gramschmidt: orthonormalization" onclick="UI.insert(UI.focused,'gramschmidt(')"></menuitem>
            <menuitem label="q2a: quad. form -> matrix" onclick="UI.insert(UI.focused,'q2a(')"></menuitem>
        </menu>
        <menu label="Isometries">
            <menuitem label="isom: isometry elements" onclick="UI.insert(UI.focused,'isom(')"></menuitem>
            <menuitem label="mkisom: create an isometry" onclick="UI.insert(UI.focused,'mkisom(')"></menuitem>
        </menu>
    </menu>
    <menu label="Geometry">
        <menu label="Points">
            <menuitem label="barycenter" onclick="UI.insert(UI.focused,'barycenter(')"></menuitem>
            <menuitem label="center" onclick="UI.insert(UI.focused,'center(')"></menuitem>
            <menuitem label="element" onclick="UI.insert(UI.focused,'element(')"></menuitem>
            <menuitem label="single_inter: 1st intersection point" onclick="UI.insert(UI.focused,'single_inter(')"></menuitem>
            <menuitem label="inter: list of intersections" onclick="UI.insert(UI.focused,'inter(')"></menuitem>
            <menuitem label="isobarycenter" onclick="UI.insert(UI.focused,'isobarycenter(')"></menuitem>
            <menuitem label="midpoint" onclick="UI.insert(UI.focused,'midpoint(')"></menuitem>
            <menuitem label="orthocenter" onclick="UI.insert(UI.focused,'orthocenter(')"></menuitem>
            <menuitem label="point" onclick="UI.insert(UI.focused,'point(')"></menuitem>
            <menuitem label="point2d: random point" onclick="UI.insert(UI.focused,'point2d(')"></menuitem>
            <menuitem label="point3d: random point" onclick="UI.insert(UI.focused,'point3d(')"></menuitem>
            <menuitem label="vertices" onclick="UI.insert(UI.focused,'vertices(')"></menuitem>
        </menu>
        <menu label="Lines">
            <menuitem label="line_segments" onclick="UI.insert(UI.focused,'line_segments(')"></menuitem>
            <menuitem label="bisector" onclick="UI.insert(UI.focused,'bisector(')"></menuitem>
            <menuitem label="halfline" onclick="UI.insert(UI.focused,'halfline(')"></menuitem>
            <menuitem label="line" onclick="UI.insert(UI.focused,'line(')"></menuitem>
            <menuitem label="envelope" onclick="UI.insert(UI.focused,'envelope(')"></menuitem>
            <menuitem label="exbisector" onclick="UI.insert(UI.focused,'exbisector(')"></menuitem>
            <menuitem label="altitude" onclick="UI.insert(UI.focused,'altitude(')"></menuitem>
            <menuitem label="inter: list of intersection points" onclick="UI.insert(UI.focused,'inter(')"></menuitem>
            <menuitem label="locus" onclick="UI.insert(UI.focused,'locus(')"></menuitem>
            <menuitem label="LineTan: tangent to graph" onclick="UI.insert(UI.focused,'LineTan(')"></menuitem>
            <menuitem label="median" onclick="UI.insert(UI.focused,'median(')"></menuitem>
            <menuitem label="perpen_bisector" onclick="UI.insert(UI.focused,'perpen_bisector(')"></menuitem>
            <menuitem label="parallel" onclick="UI.insert(UI.focused,'parallel(')"></menuitem>
            <menuitem label="perpendicular" onclick="UI.insert(UI.focused,'perpendicular(')"></menuitem>
            <menuitem label="orthogonal" onclick="UI.insert(UI.focused,'orthogonal(')"></menuitem>
            <menuitem label="common_perpendicular" onclick="UI.insert(UI.focused,'common_perpendicular(')"></menuitem>
            <menuitem label="segment" onclick="UI.insert(UI.focused,'segment(')"></menuitem>
            <menuitem label="tangent" onclick="UI.insert(UI.focused,'tangent(')"></menuitem>
            <menuitem label="vector: graphic representation" onclick="UI.insert(UI.focused,'vector(')"></menuitem>
            <menuitem label="plotfunc" onclick="UI.insert(UI.focused,'plotfunc(')"></menuitem>
            <menuitem label="Ox_2d_unit_vector" onclick="UI.insert(UI.focused,'Ox_2d_unit_vector(')"></menuitem>
            <menuitem label="Oy_2d_unit_vector" onclick="UI.insert(UI.focused,'Oy_2d_unit_vector(')"></menuitem>
            <menuitem label="Ox_3d_unit_vector" onclick="UI.insert(UI.focused,'Ox_3d_unit_vector(')"></menuitem>
            <menuitem label="Oy_3d_unit_vector" onclick="UI.insert(UI.focused,'Oy_3d_unit_vector(')"></menuitem>
            <menuitem label="Oz_3d_unit_vector" onclick="UI.insert(UI.focused,'Oz_3d_unit_vector(')"></menuitem>
            <menuitem label="frame_2d" onclick="UI.insert(UI.focused,'frame_2d(')"></menuitem>
            <menuitem label="frame_3d" onclick="UI.insert(UI.focused,'frame_3d(')"></menuitem>
        </menu>
        <menu label="Polygons">
            <menuitem label="triangle" onclick="UI.insert(UI.focused,'triangle(')"></menuitem>
            <menuitem label="equilateral_triangle" onclick="UI.insert(UI.focused,'equilateral_triangle(')"></menuitem>
            <menuitem label="isosceles_triangle" onclick="UI.insert(UI.focused,'isosceles_triangle(')"></menuitem>
            <menuitem label="right_triangle" onclick="UI.insert(UI.focused,'right_triangle(')"></menuitem>
            <menuitem label="square" onclick="UI.insert(UI.focused,'square(')"></menuitem>
            <menuitem label="rhombus" onclick="UI.insert(UI.focused,'rhombus(')"></menuitem>
            <menuitem label="parallelogram" onclick="UI.insert(UI.focused,'parallelogram(')"></menuitem>
            <menuitem label="quadrilateral" onclick="UI.insert(UI.focused,'quadrilateral(')"></menuitem>
            <menuitem label="rectangle" onclick="UI.insert(UI.focused,'rectangle(')"></menuitem>
            <menuitem label="convexhull" onclick="UI.insert(UI.focused,'convexhull(')"></menuitem>
            <menuitem label="hexagon" onclick="UI.insert(UI.focused,'hexagon(')"></menuitem>
            <menuitem label="isopolygon" onclick="UI.insert(UI.focused,'isopolygon(')"></menuitem>
            <menuitem label="polygon" onclick="UI.insert(UI.focused,'polygon(')"></menuitem>
            <menuitem label="open_polygon" onclick="UI.insert(UI.focused,'open_polygon(')"></menuitem>
        </menu>
        <menu label="Conics, circles">
            <menuitem label="arc" onclick="UI.insert(UI.focused,'arc(')"></menuitem>
            <menuitem label="circle" onclick="UI.insert(UI.focused,'circle(')"></menuitem>
            <menuitem label="circumcircle" onclick="UI.insert(UI.focused,'circumcircle(')"></menuitem>
            <menuitem label="excircle" onclick="UI.insert(UI.focused,'excircle(')"></menuitem>
            <menuitem label="incircle" onclick="UI.insert(UI.focused,'incircle(')"></menuitem>
            <menuitem label="osculating_circle" onclick="UI.insert(UI.focused,'osculating_circle(')"></menuitem>
            <menuitem label="circle" onclick="UI.insert(UI.focused,'circle(')"></menuitem>
            <menuitem label="ellipse" onclick="UI.insert(UI.focused,'ellipse(')"></menuitem>
            <menuitem label="hyperbola" onclick="UI.insert(UI.focused,'hyperbola(')"></menuitem>
            <menuitem label="parabola" onclick="UI.insert(UI.focused,'parabola(')"></menuitem>
            <menuitem label="conic" onclick="UI.insert(UI.focused,'conic(')"></menuitem>
        </menu>
        <menu label="Surfaces">
            <menuitem label="quadric" onclick="UI.insert(UI.focused,'quadric(')"></menuitem>
            <menuitem label="cone" onclick="UI.insert(UI.focused,'cone(')"></menuitem>
            <menuitem label="cylinder" onclick="UI.insert(UI.focused,'cylinder(')"></menuitem>
            <menuitem label="half_cone" onclick="UI.insert(UI.focused,'half_cone(')"></menuitem>
            <menuitem label="plane" onclick="UI.insert(UI.focused,'plane(')"></menuitem>
            <menuitem label="sphere" onclick="UI.insert(UI.focused,'sphere(')"></menuitem>
            <menuitem label="plotfunc" onclick="UI.insert(UI.focused,'plotfunc(')"></menuitem>
            <menuitem label="perpen_bisector" onclick="UI.insert(UI.focused,'perpen_bisector(')"></menuitem>
            <menuitem label="tangent" onclick="UI.insert(UI.focused,'tangent(')"></menuitem>
        </menu>
        <menu label="Solids">
            <menuitem label="cube" onclick="UI.insert(UI.focused,'cube(')"></menuitem>
            <menuitem label="pyramid" onclick="UI.insert(UI.focused,'pyramid(')"></menuitem>
            <menuitem label="prism" onclick="UI.insert(UI.focused,'prism(')"></menuitem>
            <menuitem label="parallelepiped" onclick="UI.insert(UI.focused,'parallelepiped(')"></menuitem>
            <menuitem label="polyhedron" onclick="UI.insert(UI.focused,'polyhedron(')"></menuitem>
            <menuitem label="tetrahedron" onclick="UI.insert(UI.focused,'tetrahedron(')"></menuitem>
            <menuitem label="centered_tetrahedron" onclick="UI.insert(UI.focused,'centered_tetrahedron(')"></menuitem>
            <menuitem label="centered_cube" onclick="UI.insert(UI.focused,'centerd_cube(')"></menuitem>
            <menuitem label="octahedron" onclick="UI.insert(UI.focused,'octahedron(')"></menuitem>
            <menuitem label="dodecahedron" onclick="UI.insert(UI.focused,'dodecahedron(')"></menuitem>
            <menuitem label="icosahedron" onclick="UI.insert(UI.focused,'icosahedron(')"></menuitem>

            <menuitem label="abs" onclick="UI.insert(UI.focused,'abs(')"></menuitem>
            <menuitem label="abscissa" onclick="UI.insert(UI.focused,'abscissa(')"></menuitem>
            <menuitem label="affix" onclick="UI.insert(UI.focused,'affix(')"></menuitem>
            <menuitem label="area" onclick="UI.insert(UI.focused,'area(')"></menuitem>
            <menuitem label="angle" onclick="UI.insert(UI.focused,'angle(')"></menuitem>
            <menuitem label="arg" onclick="UI.insert(UI.focused,'arg(')"></menuitem>
            <menuitem label="coordinates" onclick="UI.insert(UI.focused,'coordinates(')"></menuitem>
            <menuitem label="reduced_conic" onclick="UI.insert(UI.focused,'reduced_conic(')"></menuitem>
        </menu>
        <menu label="Measures">
            <menuitem label="curvature" onclick="UI.insert(UI.focused,'curvature(')"></menuitem>
            <menuitem label="cote" onclick="UI.insert(UI.focused,'cote(')"></menuitem>
            <menuitem label="equation: cartesian" onclick="UI.insert(UI.focused,'equation(')"></menuitem>
            <menuitem label="extract_measure" onclick="UI.insert(UI.focused,'extract_measure(')"></menuitem>
            <menuitem label="distance" onclick="UI.insert(UI.focused,'distamce(')"></menuitem>
            <menuitem label="distance2" onclick="UI.insert(UI.focused,'distance2(')"></menuitem>
            <menuitem label="ordinate" onclick="UI.insert(UI.focused,'ordinate(')"></menuitem>
            <menuitem label="parameq: parametric equation" onclick="UI.insert(UI.focused,'parameq(')"></menuitem>
            <menuitem label="slope" onclick="UI.insert(UI.focused,'slope(')"></menuitem>
            <menuitem label="perimeter" onclick="UI.insert(UI.focused,'perimeter(')"></menuitem>
            <menuitem label="reduced_quadric" onclick="UI.insert(UI.focused,'reduced_quadric(')"></menuitem>
            <menuitem label="radius" onclick="UI.insert(UI.focused,'radius(')"></menuitem>
            <menuitem label="scalar_product" onclick="UI.insert(UI.focused,'scalar_product(')"></menuitem>
        </menu>
        <menu label="Transformations">
            <menuitem label="homothety" onclick="UI.insert(UI.focused,'homothety(')"></menuitem>
            <menuitem label="inversion" onclick="UI.insert(UI.focused,'inversion(')"></menuitem>
            <menuitem label="projection" onclick="UI.insert(UI.focused,'projection(')"></menuitem>
            <menuitem label="rotation" onclick="UI.insert(UI.focused,'rotation(')"></menuitem>
            <menuitem label="similarity" onclick="UI.insert(UI.focused,'similarity(')"></menuitem>
            <menuitem label="reflection" onclick="UI.insert(UI.focused,'reflection(')"></menuitem>
            <menuitem label="translation" onclick="UI.insert(UI.focused,'translation(')"></menuitem>
        </menu>
        <menu label="Properties">
            <menuitem label="is_collinear" onclick="UI.insert(UI.focused,'is_collinear(')"></menuitem>
            <menuitem label="is_concyclic" onclick="UI.insert(UI.focused,'is_concyclic(')"></menuitem>
            <menuitem label="is_coplanar" onclick="UI.insert(UI.focused,'is_coplanar(')"></menuitem>
            <menuitem label="is_element" onclick="UI.insert(UI.focused,'is_element(')"></menuitem>
            <menuitem label="is_parallel" onclick="UI.insert(UI.focused,'is_parallel(')"></menuitem>
            <menuitem label="is_orthogonal" onclick="UI.insert(UI.focused,'is_orthogonal(')"></menuitem>
            <menuitem label="is_equilateral" onclick="UI.insert(UI.focused,'is_equilateral(')"></menuitem>
            <menuitem label="is_isosceles" onclick="UI.insert(UI.focused,'is_isosceles(')"></menuitem>
            <menuitem label="is_rectangle" onclick="UI.insert(UI.focused,'is_rectangle(')"></menuitem>
            <menuitem label="is_square" onclick="UI.insert(UI.focused,'is_square(')"></menuitem>
            <menuitem label="is_rhombus" onclick="UI.insert(UI.focused,'is_rhombus(')"></menuitem>
            <menuitem label="is_parallelogram" onclick="UI.insert(UI.focused,'is_parallelogram(')"></menuitem>
            <menuitem label="is_conjugate" onclick="UI.insert(UI.focused,'is_conjugate(')"></menuitem>
            <menuitem label="is_harmonic_circle_bundle" onclick="UI.insert(UI.focused,'is_harmonic_circle_bundle (')"></menuitem>
            <menuitem label="is_harmonic_line_bundle" onclick="UI.insert(UI.focused,'is_harmonic_line_bundle (')"></menuitem>
        </menu>
        <menu label="Display">
            <menuitem label="display" onclick="UI.insert(UI.focused,'display')"></menuitem>
            <menuitem label="hidden_name" onclick="UI.insert(UI.focused,'hidden_name')"></menuitem>
            <menuitem label="filled" onclick="UI.insert(UI.focused,'filled')"></menuitem>
            <menuitem label="white" onclick="UI.insert(UI.focused,'whilte')"></menuitem>
            <menuitem label="black" onclick="UI.insert(UI.focused,'black')"></menuitem>
            <menuitem label="red" onclick="UI.insert(UI.focused,'red')"></menuitem>
            <menuitem label="green" onclick="UI.insert(UI.focused,'green')"></menuitem>
            <menuitem label="blue" onclick="UI.insert(UI.focused,'blue')"></menuitem>
            <menuitem label="yellow" onclick="UI.insert(UI.focused,'yellow')"></menuitem>
            <menuitem label="magenta" onclick="UI.insert(UI.focused,'magenta')"></menuitem>
            <menuitem label="cyan" onclick="UI.insert(UI.focused,'cyan')"></menuitem>
            <menuitem label="solid_line" onclick="UI.insert(UI.focused,'ligne_trait_plein(')"></menuitem>
            <menuitem label="dash_line" onclick="UI.insert(UI.focused,'ligne_tiret(')"></menuitem>
            <menuitem label="dashdot_line" onclick="UI.insert(UI.focused,'ligne_tiret_point(')"></menuitem>
            <menuitem label="dashdotdot_line" onclick="UI.insert(UI.focused,'ligne_tiret_pointpoint(')"></menuitem>
            <menuitem label="cap_flat_line" onclick="UI.insert(UI.focused,'ligne_chapeau_plat(')"></menuitem>
            <menuitem label="cap_round_line" onclick="UI.insert(UI.focused,'ligne_chapeau_rond(')"></menuitem>
            <menuitem label="cap_square_line" onclick="UI.insert(UI.focused,'ligne_chapeau_carre(')"></menuitem>
            <menuitem label="line_width_1" onclick="UI.insert(UI.focused,'line_width_1')"></menuitem>
            <menuitem label="line_width_2" onclick="UI.insert(UI.focused,'line_width_2')"></menuitem>
            <menuitem label="line_width_3" onclick="UI.insert(UI.focused,'line_width_3')"></menuitem>
            <menuitem label="line_width_4" onclick="UI.insert(UI.focused,'line_width_4')"></menuitem>
            <menuitem label="line_width_5" onclick="UI.insert(UI.focused,'line_width_5')"></menuitem>
            <menuitem label="line_width_6" onclick="UI.insert(UI.focused,'line_width_6')"></menuitem>
            <menuitem label="line_width_7" onclick="UI.insert(UI.focused,'line_width_7')"></menuitem>
            <menuitem label="rhombus_point" onclick="UI.insert(UI.focused,'rhombus_point')"></menuitem>
            <menuitem label="plus_point" onclick="UI.insert(UI.focused,'plus_point')"></menuitem>
            <menuitem label="square_point" onclick="UI.insert(UI.focused,'square_point')"></menuitem>
            <menuitem label="cross_point" onclick="UI.insert(UI.focused,'cross_point')"></menuitem>
            <menuitem label="triangle_point" onclick="UI.insert(UI.focused,'triangle_point')"></menuitem>
            <menuitem label="star_point" onclick="UI.insert(UI.focused,'star_point')"></menuitem>
            <menuitem label="point_point" onclick="UI.insert(UI.focused,'point_point')"></menuitem>
            <menuitem label="point_width_1" onclick="UI.insert(UI.focused,'point_width_1')"></menuitem>
            <menuitem label="point_width_2" onclick="UI.insert(UI.focused,'point_width_2')"></menuitem>
            <menuitem label="point_width_3" onclick="UI.insert(UI.focused,'point_width_3')"></menuitem>
            <menuitem label="point_width_4" onclick="UI.insert(UI.focused,'point_width_4')"></menuitem>
            <menuitem label="point_width_5" onclick="UI.insert(UI.focused,'point_width_5')"></menuitem>
            <menuitem label="point_width_6" onclick="UI.insert(UI.focused,'point_width_6')"></menuitem>
            <menuitem label="point_width_7" onclick="UI.insert(UI.focused,'point_width_7')"></menuitem>
            <menuitem label="quadrant1" onclick="UI.insert(UI.focused,'quadrant1')"></menuitem>
            <menuitem label="quadrant2" onclick="UI.insert(UI.focused,'quadrant2')"></menuitem>
            <menuitem label="quadrant3" onclick="UI.insert(UI.focused,'quadrant3')"></menuitem>
            <menuitem label="quadrant4" onclick="UI.insert(UI.focused,'quadrant4')"></menuitem>
        </menu>

        <menu label="Advanced">
            <menuitem label="radical_axis" onclick="UI.insert(UI.focused,'radical_axis(')"></menuitem>
            <menuitem label="cross_ratio" onclick="UI.insert(UI.focused,'cross_ratio(')"></menuitem>
            <menuitem label="pole" onclick="UI.insert(UI.focused,'pole(')"></menuitem>
            <menuitem label="polar" onclick="UI.insert(UI.focused,'polar(')"></menuitem>
            <menuitem label="reciprocation" onclick="UI.insert(UI.focused,'reciprocation(')"></menuitem>
            <menuitem label="harmonic_conjugate" onclick="UI.insert(UI.focused,'harmonic_conjugate(')"></menuitem>
            <menuitem label="harmonic_division" onclick="UI.insert(UI.focused,'harmonic_division(')"></menuitem>
            <menuitem label="powerpc" onclick="UI.insert(UI.focused,'powerpc(')"></menuitem>
            <menuitem label="evolute" onclick="UI.insert(UI.focused,'evolute(')"></menuitem>
        </menu>
    </menu>
    <menu label="Graphs">
        <menuitem label="plot" onclick="UI.insert(UI.focused,'plot(')"></menuitem>
        <menuitem label="plotcontour: isolevel" onclick="UI.insert(UI.focused,'plotcontour(')"></menuitem>
        <menuitem label="plotfield: tangent field" onclick="UI.insert(UI.focused,'plotfield(')"></menuitem>
        <menuitem label="plotfunc" onclick="UI.insert(UI.focused,'plotfunc(')"></menuitem>
        <menuitem label="plotimplicit" onclick="UI.insert(UI.focused,'plotimplicit(')"></menuitem>
        <menuitem label="plotode: integral curve" onclick="UI.insert(UI.focused,'plotode(')"></menuitem>
        <menuitem label="plotparam" onclick="UI.insert(UI.focused,'plotparam(')"></menuitem>
        <menuitem label="plotpolar" onclick="UI.insert(UI.focused,'plotpolar(')"></menuitem>
        <menuitem label="plotseq: recurrent sequence plot" onclick="UI.insert(UI.focused,'plotseq(')"></menuitem>
        <menuitem label="plotproba: probabilistic plot" onclick="UI.insert(UI.focused,'plotproba(')"></menuitem>
    </menu>

</menu>
