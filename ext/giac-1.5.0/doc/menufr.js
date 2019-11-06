<menu type="context" id="cmdmenu">
    <menu label="Expressions">
        <menuitem label="factor" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
        <menuitem label="simplify" onclick="UI.insert(UI.focused,'simplify(')"></menuitem>
        <menuitem label="subst" onclick="UI.insert(UI.focused,'subst(')"></menuitem>
        <menu label="Exp et ln">
            <menuitem label="exp2pow: exp(n*ln(x)) -> x^n" onclick="UI.insert(UI.focused,'exp2pow(')"></menuitem>
            <menuitem label="hyp2exp: hyperbolique en exp" onclick="UI.insert(UI.focused,'hyp2exp(')"></menuitem>
            <menuitem label="expexpand: developpe les exp" onclick="UI.insert(UI.focused,'expexpand(')"></menuitem>
            <menuitem label="lin: linearise les exp" onclick="UI.insert(UI.focused,'lin(')"></menuitem>
            <menuitem label="lncollect: regroupe les log" onclick="UI.insert(UI.focused,'lncollect(')"></menuitem>
            <menuitem label="lnexpand: developpe les log" onclick="UI.insert(UI.focused,'lnexpand(')"></menuitem>
            <menuitem label="powexpand: developpe les ^" onclick="UI.insert(UI.focused,'powexpand(')"></menuitem>
            <menuitem label="pow2exp: ^ -> exp" onclick="UI.insert(UI.focused,'pow2exp(')"></menuitem>
        </menu>
        <menu label="Trigonométrie">
            <menuitem label="trigexpand: developpe sin,cos,tan" onclick="UI.insert(UI.focused,'trigexpand(')"></menuitem>
            <menuitem label="tlin: linearisation trigonometrique" onclick="UI.insert(UI.focused,'tlin(')"></menuitem>
            <menuitem label="tcollect: regroupe cos et sin" onclick="UI.insert(UI.focused,'tcollect(')"></menuitem>
            <menuitem label="halftan: en fonction de tan(x sur 2)" onclick="UI.insert(UI.focused,'halftan(')"></menuitem>
            <menuitem label="cos2sintan: cos -> sin sur tan" onclick="UI.insert(UI.focused,'cos2sintan(')"></menuitem>
            <menuitem label="sin2costan: sin -> cos*tan" onclick="UI.insert(UI.focused,'sin2costan(')"></menuitem>
            <menuitem label="tan2sincos: tan -> sin sur cos" onclick="UI.insert(UI.focused,'tan2sincos(')"></menuitem>
            <menuitem label="tan2cossin2: tan -> cos, sin 2*angle" onclick="UI.insert(UI.focused,'tan2cossin2(')"></menuitem>
            <menuitem label="tan2sincos2: tan -> sin. cos 2*angle" onclick="UI.insert(UI.focused,'tan2sincos2(')"></menuitem>
            <menuitem label="trigcos: sin^2 -> 1-cos^2" onclick="UI.insert(UI.focused,'trigcos(')"></menuitem>
            <menuitem label="trigsin: cos^2 -> 1-sin^2" onclick="UI.insert(UI.focused,'trigsin(')"></menuitem>
            <menuitem label="trigtan: cos^2 et sin^2 -> tan^2" onclick="UI.insert(UI.focused,'trigtan(')"></menuitem>
            <menuitem label="shift_phase" onclick="UI.insert(UI.focused,'shift_phase(')"></menuitem>
            <menuitem label="exp2trig: exp(i*x) -> cos(x)+i*sin(x)" onclick="UI.insert(UI.focused,'exp2trig(')"></menuitem>
            <menuitem label="trig2exp: cos et sin -> exp(i*angle)" onclick="UI.insert(UI.focused,'trig2exp(')"></menuitem>
            <menuitem label="tsimplify: simplifier transcendant" onclick="UI.insert(UI.focused,'tsimplify(')"></menuitem>
            <menuitem label="texpand: developper trigo, exp, ln" onclick="UI.insert(UI.focused,'texpand(')"></menuitem>
            <menuitem label="atrig2ln: inverse trigo -> ln complexe" onclick="UI.insert(UI.focused,'atrig2ln(')"></menuitem>
            <menuitem label="acos2asin: acos -> asin" onclick="UI.insert(UI.focused,'acos2asin(')"></menuitem>
            <menuitem label="acos2atan: acos -> atan" onclick="UI.insert(UI.focused,'acos2atan(')"></menuitem>
            <menuitem label="asin2acos: asin -> acos" onclick="UI.insert(UI.focused,'asin2acos(')"></menuitem>
            <menuitem label="asin2atan: asin -> atan" onclick="UI.insert(UI.focused,'asin2atan(')"></menuitem>
            <menuitem label="atan2acos: atan -> acos" onclick="UI.insert(UI.focused,'atan2acos(')"></menuitem>
            <menuitem label="atan2asin: atan -> asin" onclick="UI.insert(UI.focused,'atan2asin(')"></menuitem>
        </menu>
        <menu label="Rationnel">
            <menuitem label="collect" onclick="UI.insert(UI.focused,'collect(')"></menuitem>
            <menuitem label="normal" onclick="UI.insert(UI.focused,'normal(')"></menuitem>
            <menuitem label="eval" onclick="UI.insert(UI.focused,'eval(')"></menuitem>
            <menuitem label="equal2diff" onclick="UI.insert(UI.focused,'equal2diff(')"></menuitem>
            <menuitem label="f2nd" onclick="UI.insert(UI.focused,'f2nd(')"></menuitem>
            <menuitem label="ratnormal: forme normale" onclick="UI.insert(UI.focused,'ratnormal(')"></menuitem>
            <menuitem label="reorder: reordonner les variables" onclick="UI.insert(UI.focused,'reorder(')"></menuitem>
            <menuitem label="partfrac: elements simples" onclick="UI.insert(UI.focused,'partfrac(')"></menuitem>
            <menuitem label="cpartfrac: elements simples sur C" onclick="UI.insert(UI.focused,'cpartfrac(')"></menuitem>
            <menuitem label="simp2" onclick="UI.insert(UI.focused,'simp2(')"></menuitem>
            <menuitem label="collect: regrouper" onclick="UI.insert(UI.focused,'collect(')"></menuitem>
            <menuitem label="factor: factoriser" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
            <menuitem label="expand: developper" onclick="UI.insert(UI.focused,'expand(')"></menuitem>
            <menuitem label="simplify" onclick="UI.insert(UI.focused,'simplify(')"></menuitem>
            <menuitem label="mult_conjugate" onclick="UI.insert(UI.focused,'mult_conjugate(')"></menuitem>
            <menuitem label="mult_c_conjugate" onclick="UI.insert(UI.focused,'mult_c_conjugate(')"></menuitem>
        </menu>
        <menu label="Réel">
            <menuitem label="epsilon2zero" onclick="UI.insert(UI.focused,'epsilon2zero(')"></menuitem>
            <menuitem label="evalf: evaluation approchee" onclick="UI.insert(UI.focused,'evalf(')"></menuitem>
            <menuitem label="float2rational" onclick="UI.insert(UI.focused,'float2rational(')"></menuitem>
        </menu>
    </menu>

    <menu label="Résoudre">
        <menuitem label="linsolve: resoudre syst. lineaire" onclick="UI.insert(UI.focused,'linsolve(')"></menuitem>
        <menuitem label="solve: resoudre equation ou systeme" onclick="UI.insert(UI.focused,'solve(')"></menuitem>
        <menuitem label="fsolve: resoudre numeriquement" onclick="UI.insert(UI.focused,'fsolve(')"></menuitem>
        <menuitem label="newton: resoudre par la methode de" onclick="UI.insert(UI.focused,'newton(')"></menuitem>
        <menuitem label="csolve: resoudre sur C" onclick="UI.insert(UI.focused,'csolve(')"></menuitem>
        <menuitem label="desolve: resoudre equa. diff." onclick="UI.insert(UI.focused,'desolve(')"></menuitem>
        <menuitem label="rsolve: resoudre recurrence" onclick="UI.insert(UI.focused,'rsolve(')"></menuitem>
        <menuitem label="odesolve: resoudre equa. diff." onclick="UI.insert(UI.focused,'odesolve(')"></menuitem>
        <menuitem label="greduce: reduction mod base" onclick="UI.insert(UI.focused,'greduce(')"></menuitem>
        <menuitem label="gbasis: base de Groebner" onclick="UI.insert(UI.focused,'gbasis(')"></menuitem>
    </menu>
    <menu label="Analyse">
        <menuitem label="limit: limite" onclick="UI.insert(UI.focused,'limit(')"></menuitem>
        <menuitem label="series: de Taylor" onclick="UI.insert(UI.focused,'series(')"></menuitem>
        <menuitem label="diff: dérivée" onclick="UI.insert(UI.focused,'diff(')"></menuitem>
        <menuitem label="ibpdv: integration par parties" onclick="UI.insert(UI.focused,'ibpdv(')"></menuitem>
        <menuitem label="ibpu: integration par parties" onclick="UI.insert(UI.focused,'ibpu(')"></menuitem>
        <menuitem label="integrate: primitive" onclick="UI.insert(UI.focused,'integrate(')"></menuitem>
        <menuitem label="preval: evalue une primitive" onclick="UI.insert(UI.focused,'preval(')"></menuitem>
        <menuitem label="sum: somme discrete" onclick="UI.insert(UI.focused,'sum(')"></menuitem>
        <menuitem label="curl: rotationnel" onclick="UI.insert(UI.focused,'curl(')"></menuitem>
        <menuitem label="diff: gradient" onclick="UI.insert(UI.focused,'gradient(')"></menuitem>
        <menuitem label="divergence: " onclick="UI.insert(UI.focused,'divergence(')"></menuitem>
        <menuitem label="hessian: " onclick="UI.insert(UI.focused,'hessian(')"></menuitem>
        <menuitem label="laplacian: " onclick="UI.insert(UI.focused,'laplacian(')"></menuitem>
        <menuitem label="potential: potentiel scalaire" onclick="UI.insert(UI.focused,'potential(')"></menuitem>
        <menuitem label="vpotential: potentiel vecteur" onclick="UI.insert(UI.focused,'vpotential(')"></menuitem>
        <menu label="Transformations">
            <menuitem label="fft: Fast Fourier Transform" onclick="UI.insert(UI.focused,'fft(')"></menuitem>
            <menuitem label="fourier_an: coeff an de" onclick="UI.insert(UI.focused,'fourier_an(')"></menuitem>
            <menuitem label="fourier_bn: coeff bn de" onclick="UI.insert(UI.focused,'fourier_bn(')"></menuitem>
            <menuitem label="fourier_cn: coeff cn de" onclick="UI.insert(UI.focused,'fourier_cn(')"></menuitem>
            <menuitem label="ifft: fft inverse" onclick="UI.insert(UI.focused,'ifft(')"></menuitem>
            <menuitem label="ilaplace: Laplace inverse" onclick="UI.insert(UI.focused,'ilaplace(')"></menuitem>
            <menuitem label="laplace: transf. de Laplace" onclick="UI.insert(UI.focused,'laplace(')"></menuitem>
        </menu>
    </menu>

    <menu label="Entiers">
        <menuitem label="euler: indicatrice" onclick="UI.insert(UI.focused,'euler(')"></menuitem>
        <menuitem label="fracmod: a mod p donne n et d" onclick="UI.insert(UI.focused,'fracmod(')"></menuitem>
        <menuitem label="gcd: PGCD des arguments" onclick="UI.insert(UI.focused,'gcd(')"></menuitem>
        <menuitem label="iabcuv: a,b,c donnent u,v avec au+bv=c" onclick="UI.insert(UI.focused,'iabcuv(')"></menuitem>
        <menuitem label="ichinrem: restes chinois sur Z" onclick="UI.insert(UI.focused,'ichinrem(')"></menuitem>
        <menuitem label="idivis: diviseurs entiers" onclick="UI.insert(UI.focused,'idivis(')"></menuitem>
        <menuitem label="iegcd: identite de Bezout sur Z" onclick="UI.insert(UI.focused,'iegcd(')"></menuitem>
        <menuitem label="ifactor: entier factorise" onclick="UI.insert(UI.focused,'ifactor(')"></menuitem>
        <menuitem label="ifactors: liste des facteurs" onclick="UI.insert(UI.focused,'ifactors(')"></menuitem>
        <menuitem label="iquo: quotient division eucl." onclick="UI.insert(UI.focused,'iquo(')"></menuitem>
        <menuitem label="iquorem: quotient et reste" onclick="UI.insert(UI.focused,'iquorem(')"></menuitem>
        <menuitem label="irem: reste division eucl." onclick="UI.insert(UI.focused,'irem(')"></menuitem>
        <menuitem label="is_prime: test de primalite" onclick="UI.insert(UI.focused,'is_prime(')"></menuitem>
        <menuitem label="is_pseudoprime: pseudoprimalite" onclick="UI.insert(UI.focused,'is_pseudoprime(')"></menuitem>
        <menuitem label="lcm: PPCM des arguments" onclick="UI.insert(UI.focused,'lcm(')"></menuitem>
        <menuitem label="mod: classe de a dans Z sur nZ" onclick="UI.insert(UI.focused,'mod(')"></menuitem>
        <menuitem label="nextprime: premier suivant" onclick="UI.insert(UI.focused,'nextprime(')"></menuitem>
        <menuitem label="nprimes: nombre premiers &lt;=" onclick="UI.insert(UI.focused,'prevprime(')"></menuitem>
        <menuitem label="pa2b2" onclick="UI.insert(UI.focused,'pa2b2(')"></menuitem>
        <menuitem label="powmod: puissance rapide" onclick="UI.insert(UI.focused,'powmod(')"></menuitem>
        <menuitem label="prevprime: premier precedent" onclick="UI.insert(UI.focused,'prevprime(')"></menuitem>
        <menuitem label="smod: reste symetrique" onclick="UI.insert(UI.focused,'smod(')"></menuitem>
    </menu>
    <menu label="Polynomes">
        <menu label="Base">
            <menuitem label="canonical_form: forme canonique" onclick="UI.insert(UI.focused,'canonical_form(')"></menuitem>
            <menuitem label="cfactor: factoriser sur C" onclick="UI.insert(UI.focused,'cfactor(')"></menuitem>
            <menuitem label="coeff: liste des coefficients" onclick="UI.insert(UI.focused,'coeff(')"></menuitem>
            <menuitem label="cyclotomic" onclick="UI.insert(UI.focused,'cyclotomic(')"></menuitem>
            <menuitem label="degree" onclick="UI.insert(UI.focused,'degree(')"></menuitem>
            <menuitem label="expand: developper" onclick="UI.insert(UI.focused,'expand(')"></menuitem>
            <menuitem label="factor_xn" onclick="UI.insert(UI.focused,'factor_xn(')"></menuitem>
            <menuitem label="factor: factoriser" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
            <menuitem label="factors: liste des facteurs" onclick="UI.insert(UI.focused,'factors(')"></menuitem>
            <menuitem label="fcoeff" onclick="UI.insert(UI.focused,'fcoeff(')"></menuitem>
            <menuitem label="froot" onclick="UI.insert(UI.focused,'froot(')"></menuitem>
            <menuitem label="genpoly: P(a) -> P" onclick="UI.insert(UI.focused,'genpoly(')"></menuitem>
            <menuitem label="lcoeff: coefficient dominant" onclick="UI.insert(UI.focused,'lcoeff(')"></menuitem>
            <menuitem label="normal: forme normale" onclick="UI.insert(UI.focused,'normal(')"></menuitem>
            <menuitem label="pcoeff: racines -> coefficients" onclick="UI.insert(UI.focused,'pcoeff(')"></menuitem>
            <menuitem label="peval: P,a -> P(a)" onclick="UI.insert(UI.focused,'peval(')"></menuitem>
            <menuitem label="proot: coefficients -> racines" onclick="UI.insert(UI.focused,'proot(')"></menuitem>
            <menuitem label="poly2symb" onclick="UI.insert(UI.focused,'poly2symb(')"></menuitem>
            <menuitem label="randpoly: polynome aleatoire" onclick="UI.insert(UI.focused,'randpoly(')"></menuitem>
            <menuitem label="symb2poly" onclick="UI.insert(UI.focused,'symb2poly(')"></menuitem>
            <menuitem label="tcoeff: coefficient de + bas degre" onclick="UI.insert(UI.focused,'tcoeff(')"></menuitem>
            <menuitem label="valuation" onclick="UI.insert(UI.focused,'valuation(')"></menuitem>
        </menu>
        <menu label="Arithmétique">
            <menuitem label="abcuv: a,b,c -> u,v avec au+bv=c" onclick="UI.insert(UI.focused,'abcuv(')"></menuitem>
            <menuitem label="chinrem: restes chinois Q[X]" onclick="UI.insert(UI.focused,'chinrem(')"></menuitem>
            <menuitem label="divis: diviseurs Q[X]" onclick="UI.insert(UI.focused,'divis(')"></menuitem>
            <menuitem label="divpc: en puissance croissantes" onclick="UI.insert(UI.focused,'divpc(')"></menuitem>
            <menuitem label="egcd: Bezout polynomes" onclick="UI.insert(UI.focused,'egcd(')"></menuitem>
            <menuitem label="factor: factoriser" onclick="UI.insert(UI.focused,'factor(')"></menuitem>
            <menuitem label="gcd: PGCD des arguments" onclick="UI.insert(UI.focused,'gcd(')"></menuitem>
            <menuitem label="horner: P,a -> P(a)" onclick="UI.insert(UI.focused,'horner(')"></menuitem>
            <menuitem label="lagrange: interpolation" onclick="UI.insert(UI.focused,'lagrange(')"></menuitem>
            <menuitem label="lcm: PPCM des arguments" onclick="UI.insert(UI.focused,'lcm(')"></menuitem>
            <menuitem label="normal: forme normale" onclick="UI.insert(UI.focused,'normal(')"></menuitem>
            <menuitem label="ptayl: Taylor pour polynomes" onclick="UI.insert(UI.focused,'ptayl(')"></menuitem>
            <menuitem label="quo: quotient euclidien" onclick="UI.insert(UI.focused,'quo(')"></menuitem>
            <menuitem label="quorem: quotient et reste" onclick="UI.insert(UI.focused,'quorem(')"></menuitem>
            <menuitem label="rem: reste euclidien" onclick="UI.insert(UI.focused,'rem(')"></menuitem>
            <menuitem label="resultant" onclick="UI.insert(UI.focused,'resultant(')"></menuitem>
            <menuitem label="spline" onclick="UI.insert(UI.focused,'spline(')"></menuitem>
            <menuitem label="sturm: suite de Sturm" onclick="UI.insert(UI.focused,'sturm(')"></menuitem>
            <menuitem label="sturmab: nombre de racines" onclick="UI.insert(UI.focused,'sturmab(')"></menuitem>
        </menu>
        <menu label="Orthogonaux">
            <menuitem label="hermite" onclick="UI.insert(UI.focused,'hermite(')"></menuitem>
            <menuitem label="laguerre" onclick="UI.insert(UI.focused,'laguerre(')"></menuitem>
            <menuitem label="legendre" onclick="UI.insert(UI.focused,'legendre(')"></menuitem>
            <menuitem label="tchebyshev1" onclick="UI.insert(UI.focused,'tchebyshev1(')"></menuitem>
            <menuitem label="tchebyshev2" onclick="UI.insert(UI.focused,'tchebyshev2(')"></menuitem>
        </menu>
    </menu>
    <menu label="Algèbre linéaire">
        <menu label="Vecteur">
            <menuitem label="abs: " onclick="UI.insert(UI.focused,'abs(')"></menuitem>
            <menuitem label="cross" onclick="UI.insert(UI.focused,'cross(')"></menuitem>
            <menuitem label="dot: produit scalaire" onclick="UI.insert(UI.focused,'dot(')"></menuitem>
            <menuitem label="l1norm: somme des |x_i|" onclick="UI.insert(UI.focused,'l1norm(')"></menuitem>
            <menuitem label="l2norm: norme euclidienne" onclick="UI.insert(UI.focused,'l2norm(')"></menuitem>
            <menuitem label="maxnorm: norme du max" onclick="UI.insert(UI.focused,'maxnorm(')"></menuitem>
        </menu>
        <menu label="Matrice">
            <menuitem label="hadamard: produit de" onclick="UI.insert(UI.focused,'hadamard(')"></menuitem>
            <menuitem label="hilbert: matrice de" onclick="UI.insert(UI.focused,'hilbert(')"></menuitem>
            <menuitem label="identity: matrice identite" onclick="UI.insert(UI.focused,'identity(')"></menuitem>
            <menuitem label="makemat" onclick="UI.insert(UI.focused,'makemat(')"></menuitem>
            <menuitem label="ncols: nombre de colonnes" onclick="UI.insert(UI.focused,'ncols(')"></menuitem>
            <menuitem label="nrows: nombre de lignes" onclick="UI.insert(UI.focused,'nrows(')"></menuitem>
            <menuitem label="ranm: matrice aleatoire" onclick="UI.insert(UI.focused,'ranm(')"></menuitem>
            <menuitem label="syst2mat: matrice du systeme" onclick="UI.insert(UI.focused,'syst2mat(')"></menuitem>
            <menuitem label="vandermonde: matrice de" onclick="UI.insert(UI.focused,'vandermonde(')"></menuitem>
            <menuitem label="trace" onclick="UI.insert(UI.focused,'trace(')"></menuitem>
            <menuitem label="trn: transconjuguee" onclick="UI.insert(UI.focused,'trn(')"></menuitem>
            <menuitem label="tran: transposee" onclick="UI.insert(UI.focused,'tran(')"></menuitem>
            <menuitem label="l1norm: norme triple l1" onclick="UI.insert(UI.focused,'l1norm(')"></menuitem>
            <menuitem label="l2norm: norme triple l2" onclick="UI.insert(UI.focused,'l2norm(')"></menuitem>
            <menuitem label="linfnorm: norme triple l inf" onclick="UI.insert(UI.focused,'linfnorm(')"></menuitem>
            <menuitem label="frobenius_norm" onclick="UI.insert(UI.focused,'frobenius_norm(')"></menuitem>
        </menu>
        <menu label="Gauss">
            <menuitem label="basis: base" onclick="UI.insert(UI.focused,'basis(')"></menuitem>
            <menuitem label="changebase: A,P -> P^-1*A*P" onclick="UI.insert(UI.focused,'changebase(')"></menuitem>
            <menuitem label="det: determinant" onclick="UI.insert(UI.focused,'det(')"></menuitem>
            <menuitem label="det_minor: par dvpt des mineurs" onclick="UI.insert(UI.focused,'det_minor(')"></menuitem>
            <menuitem label="inv: inverse" onclick="UI.insert(UI.focused,'inv(')"></menuitem>
            <menuitem label="ibasis: intersection esp. vectoriel" onclick="UI.insert(UI.focused,'ibasis)')"></menuitem>
            <menuitem label="ihermite: forme de Hermite sur Z" onclick="UI.insert(UI.focused,'ihermite(')"></menuitem>
            <menuitem label="ismith: forme de Smith sur Z" onclick="UI.insert(UI.focused,'ismith(')"></menuitem>
            <menuitem label="ker: noyau" onclick="UI.insert(UI.focused,'ker(')"></menuitem>
            <menuitem label="image" onclick="UI.insert(UI.focused,'image(')"></menuitem>
            <menuitem label="linsolve: resoudre syst. lineaire" onclick="UI.insert(UI.focused,'linsolve(')"></menuitem>
            <menuitem label="rank: rang" onclick="UI.insert(UI.focused,'rank(')"></menuitem>
            <menuitem label="rref: reduction echelonnee" onclick="UI.insert(UI.focused,'rref(')"></menuitem>
            <menuitem label="simplex_reduce" onclick="UI.insert(UI.focused,'simplex_reduce(')"></menuitem>
            <menuitem label="cond: conditionnement" onclick="UI.insert(UI.focused,'cond(')"></menuitem>
        </menu>
        <menu label="Diagonalisation">
            <menuitem label="adjoint_matrix" onclick="UI.insert(UI.focused,'adjoint_matrix(')"></menuitem>
            <menuitem label="companion" onclick="UI.insert(UI.focused,'companion(')"></menuitem>
            <menuitem label="egv: vecteurs propres P" onclick="UI.insert(UI.focused,'egv(')"></menuitem>
            <menuitem label="egvl: P^-1*A*P" onclick="UI.insert(UI.focused,'egvl(')"></menuitem>
            <menuitem label="eigenvalues: valeurs propres" onclick="UI.insert(UI.focused,'eigenvalues(')"></menuitem>
            <menuitem label="hessenberg: factorisation de" onclick="UI.insert(UI.focused,'hessenberg(')"></menuitem>
            <menuitem label="jordan: forme de" onclick="UI.insert(UI.focused,'jordan(')"></menuitem>
            <menuitem label="pcar: polynome caracteristique" onclick="UI.insert(UI.focused,'pcar(')"></menuitem>
            <menuitem label="pcar_hessenberg" onclick="UI.insert(UI.focused,'pcar_hessenberg(')"></menuitem>
            <menuitem label="pmin: polynome minimal" onclick="UI.insert(UI.focused,'pmin(')"></menuitem>
            <menuitem label="svl: valeurs singulieres" onclick="UI.insert(UI.focused,'svl(')"></menuitem>
        </menu>
        <menu label="Factorisation">
            <menuitem label="cholesky" onclick="UI.insert(UI.focused,'cholesky(')"></menuitem>
            <menuitem label="lll: base courte de reseau" onclick="UI.insert(UI.focused,'lll(')"></menuitem>
            <menuitem label="lu: P*A=L*U" onclick="UI.insert(UI.focused,'lu(')"></menuitem>
            <menuitem label="qr: A=Q*R" onclick="UI.insert(UI.focused,'qr(')"></menuitem>
            <menuitem label="svd: singular value decomposition" onclick="UI.insert(UI.focused,'svd(')"></menuitem>
        </menu>
        <menu label="Iterations">
            <menuitem label="jacobi_linsolve" onclick="UI.insert(UI.focused,'jacobi_linsolve(')"></menuitem>
            <menuitem label="gauss_seidel_linsolve" onclick="UI.insert(UI.focused,'gauss_seidel_linsolve(')"></menuitem>
            <menuitem label="conjugate_gradient" onclick="UI.insert(UI.focused,'conjugate_gradient(')"></menuitem>
        </menu>
        <menu label="Forme quadratiques">
            <menuitem label="a2q: matrice -> forme quad." onclick="UI.insert(UI.focused,'a2q(')"></menuitem>
            <menuitem label="gauss: decomposition + et - de carres" onclick="UI.insert(UI.focused,'gauss(')"></menuitem>
            <menuitem label="gramschmidt: orthonormalisation" onclick="UI.insert(UI.focused,'gramschmidt(')"></menuitem>
            <menuitem label="q2a: forme quad. -> matrice" onclick="UI.insert(UI.focused,'q2a(')"></menuitem>
        </menu>
        <menu label="Isométries">
            <menuitem label="isom: elements d'une isometrie" onclick="UI.insert(UI.focused,'isom(')"></menuitem>
            <menuitem label="mkisom: creer une isometrie" onclick="UI.insert(UI.focused,'mkisom(')"></menuitem>
        </menu>
    </menu>
    <menu label="Géométrie">
        <menu label="Points">
            <menuitem label="barycentre" onclick="UI.insert(UI.focused,'barycentre(')"></menuitem>
            <menuitem label="centre" onclick="UI.insert(UI.focused,'centre(')"></menuitem>
            <menuitem label="element" onclick="UI.insert(UI.focused,'element(')"></menuitem>
            <menuitem label="inter_unique: premiere intersection" onclick="UI.insert(UI.focused,'inter_unique(')"></menuitem>
            <menuitem label="inter: liste des intersections" onclick="UI.insert(UI.focused,'inter(')"></menuitem>
            <menuitem label="isobarycentre" onclick="UI.insert(UI.focused,'isobarycentre(')"></menuitem>
            <menuitem label="milieu" onclick="UI.insert(UI.focused,'milieu(')"></menuitem>
            <menuitem label="orthocentre" onclick="UI.insert(UI.focused,'orthocentre(')"></menuitem>
            <menuitem label="papier_pointe" onclick="UI.insert(UI.focused,'papier_pointe(')"></menuitem>
            <menuitem label="papier_quadrille" onclick="UI.insert(UI.focused,'papier_quadrille(')"></menuitem>
            <menuitem label="point" onclick="UI.insert(UI.focused,'point(')"></menuitem>
            <menuitem label="point2d: point aleatoire" onclick="UI.insert(UI.focused,'point2d(')"></menuitem>
            <menuitem label="point3d: point aleatoire" onclick="UI.insert(UI.focused,'point3d(')"></menuitem>
            <menuitem label="sommets" onclick="UI.insert(UI.focused,'sommets(')"></menuitem>
        </menu>
        <menu label="Lignes">
            <menuitem label="aretes" onclick="UI.insert(UI.focused,'aretes(')"></menuitem>
            <menuitem label="bissectrice: interieure" onclick="UI.insert(UI.focused,'bissectrice(')"></menuitem>
            <menuitem label="demi_droite" onclick="UI.insert(UI.focused,'demi_droite(')"></menuitem>
            <menuitem label="droite" onclick="UI.insert(UI.focused,'droite(')"></menuitem>
            <menuitem label="enveloppe" onclick="UI.insert(UI.focused,'enveloppe(')"></menuitem>
            <menuitem label="exbissectrice: exterieure" onclick="UI.insert(UI.focused,'exbissectrice(')"></menuitem>
            <menuitem label="hauteur" onclick="UI.insert(UI.focused,'hauteur(')"></menuitem>
            <menuitem label="inter: liste des intersections" onclick="UI.insert(UI.focused,'inter(')"></menuitem>
            <menuitem label="lieu" onclick="UI.insert(UI.focused,'lieu(')"></menuitem>
            <menuitem label="LineTan: tangente au graphe" onclick="UI.insert(UI.focused,'LineTan(')"></menuitem>
            <menuitem label="mediane" onclick="UI.insert(UI.focused,'mediane(')"></menuitem>
            <menuitem label="mediatrice" onclick="UI.insert(UI.focused,'mediatrice(')"></menuitem>
            <menuitem label="parallele" onclick="UI.insert(UI.focused,'parallele(')"></menuitem>
            <menuitem label="perpendiculaire" onclick="UI.insert(UI.focused,'perpendiculaire(')"></menuitem>
            <menuitem label="orthogonal" onclick="UI.insert(UI.focused,'orthogonal(')"></menuitem>
            <menuitem label="perpendiculaire_commune" onclick="UI.insert(UI.focused,'perpendiculaire_commune(')"></menuitem>
            <menuitem label="segment" onclick="UI.insert(UI.focused,'segment(')"></menuitem>
            <menuitem label="tangent" onclick="UI.insert(UI.focused,'tangent(')"></menuitem>
            <menuitem label="vecteur: represente graphiquement" onclick="UI.insert(UI.focused,'vecteur(')"></menuitem>
            <menuitem label="funcplot" onclick="UI.insert(UI.focused,'funcplot(')"></menuitem>
            <menuitem label="vecteur_unitaire_Ox_2d" onclick="UI.insert(UI.focused,'vecteur_unitaire_Ox_2d(')"></menuitem>
            <menuitem label="vecteur_unitaire_Oy_2d" onclick="UI.insert(UI.focused,'vecteur_unitaire_Oy_2d(')"></menuitem>
            <menuitem label="vecteur_unitaire_Ox_3d" onclick="UI.insert(UI.focused,'vecteur_unitaire_Ox_3d(')"></menuitem>
            <menuitem label="vecteur_unitaire_Oy_3d" onclick="UI.insert(UI.focused,'vecteur_unitaire_Oy_3d(')"></menuitem>
            <menuitem label="vecteur_unitaire_Oz_3d" onclick="UI.insert(UI.focused,'vecteur_unitaire_Oz_3d(')"></menuitem>
            <menuitem label="repere_2d" onclick="UI.insert(UI.focused,'repere_2d(')"></menuitem>
            <menuitem label="repere_3d" onclick="UI.insert(UI.focused,'repere_3d(')"></menuitem>
        </menu>
        <menu label="Polygones">
            <menuitem label="triangle" onclick="UI.insert(UI.focused,'triangle(')"></menuitem>
            <menuitem label="triangle_equilateral" onclick="UI.insert(UI.focused,'triangle_equilateral(')"></menuitem>
            <menuitem label="triangle_isocele" onclick="UI.insert(UI.focused,'triangle_isocele(')"></menuitem>
            <menuitem label="triangle_rectangle" onclick="UI.insert(UI.focused,'triangle_rectangle(')"></menuitem>
            <menuitem label="carre" onclick="UI.insert(UI.focused,'carre(')"></menuitem>
            <menuitem label="losange" onclick="UI.insert(UI.focused,'losange(')"></menuitem>
            <menuitem label="parallelogramme" onclick="UI.insert(UI.focused,'parallelogramme(')"></menuitem>
            <menuitem label="quadrilatere" onclick="UI.insert(UI.focused,'quadrilatere(')"></menuitem>
            <menuitem label="rectangle" onclick="UI.insert(UI.focused,'rectangle(')"></menuitem>
            <menuitem label="convexhull: enveloppe convexe" onclick="UI.insert(UI.focused,'convexhull(')"></menuitem>
            <menuitem label="hexagone" onclick="UI.insert(UI.focused,'hexagone(')"></menuitem>
            <menuitem label="isopolygone" onclick="UI.insert(UI.focused,'isopolygone(')"></menuitem>
            <menuitem label="polygone" onclick="UI.insert(UI.focused,'polygone(')"></menuitem>
            <menuitem label="polygone_ouvert" onclick="UI.insert(UI.focused,'polygone_ouvert(')"></menuitem>
        </menu>
        <menu label="Coniques, cercles">
            <menuitem label="arc" onclick="UI.insert(UI.focused,'arc(')"></menuitem>
            <menuitem label="cercle" onclick="UI.insert(UI.focused,'cercle(')"></menuitem>
            <menuitem label="circonscrit" onclick="UI.insert(UI.focused,'circonscrit(')"></menuitem>
            <menuitem label="exinscrit" onclick="UI.insert(UI.focused,'exinscrit(')"></menuitem>
            <menuitem label="inscrit" onclick="UI.insert(UI.focused,'inscrit(')"></menuitem>
            <menuitem label="cercle_osculateur" onclick="UI.insert(UI.focused,'cercle_osculateur(')"></menuitem>
            <menuitem label="cercle" onclick="UI.insert(UI.focused,'cercle(')"></menuitem>
            <menuitem label="ellipse" onclick="UI.insert(UI.focused,'ellipse(')"></menuitem>
            <menuitem label="hyperbole" onclick="UI.insert(UI.focused,'hyperbole(')"></menuitem>
            <menuitem label="parabole" onclick="UI.insert(UI.focused,'parabole(')"></menuitem>
            <menuitem label="conique" onclick="UI.insert(UI.focused,'conique(')"></menuitem>
        </menu>
        <menu label="Surfaces">
            <menuitem label="quadrique" onclick="UI.insert(UI.focused,'quadrique(')"></menuitem>
            <menuitem label="cone" onclick="UI.insert(UI.focused,'cone(')"></menuitem>
            <menuitem label="cylindre" onclick="UI.insert(UI.focused,'cylindre(')"></menuitem>
            <menuitem label="demi_cone" onclick="UI.insert(UI.focused,'demi_cone(')"></menuitem>
            <menuitem label="plan" onclick="UI.insert(UI.focused,'plan(')"></menuitem>
            <menuitem label="sphere" onclick="UI.insert(UI.focused,'sphere(')"></menuitem>
            <menuitem label="funcplot" onclick="UI.insert(UI.focused,'funcplot(')"></menuitem>
            <menuitem label="mediatrice" onclick="UI.insert(UI.focused,'mediatrice(')"></menuitem>
            <menuitem label="tangent" onclick="UI.insert(UI.focused,'tangent(')"></menuitem>
        </menu>
        <menu label="Solides">
            <menuitem label="cube" onclick="UI.insert(UI.focused,'cube(')"></menuitem>
            <menuitem label="pyramide" onclick="UI.insert(UI.focused,'pyramide(')"></menuitem>
            <menuitem label="prisme" onclick="UI.insert(UI.focused,'prisme(')"></menuitem>
            <menuitem label="parallelepipede" onclick="UI.insert(UI.focused,'parallelepipede(')"></menuitem>
            <menuitem label="polyedre" onclick="UI.insert(UI.focused,'polyedre(')"></menuitem>
            <menuitem label="tetraedre" onclick="UI.insert(UI.focused,'tetraedre(')"></menuitem>
            <menuitem label="tetraedre_centre" onclick="UI.insert(UI.focused,'tetraedre_centre(')"></menuitem>
            <menuitem label="cube_centre" onclick="UI.insert(UI.focused,'cube_centre(')"></menuitem>
            <menuitem label="octaedre" onclick="UI.insert(UI.focused,'octaedre(')"></menuitem>
            <menuitem label="dodecaedre" onclick="UI.insert(UI.focused,'dodecaedre(')"></menuitem>
            <menuitem label="icosaedre" onclick="UI.insert(UI.focused,'icosaedre(')"></menuitem>

            <menuitem label="abs" onclick="UI.insert(UI.focused,'abs(')"></menuitem>
            <menuitem label="abscisse" onclick="UI.insert(UI.focused,'abscisse(')"></menuitem>
            <menuitem label="affixe" onclick="UI.insert(UI.focused,'affixe(')"></menuitem>
            <menuitem label="aire" onclick="UI.insert(UI.focused,'aire(')"></menuitem>
            <menuitem label="angle" onclick="UI.insert(UI.focused,'angle(')"></menuitem>
            <menuitem label="arg" onclick="UI.insert(UI.focused,'arg(')"></menuitem>
            <menuitem label="coordonnees" onclick="UI.insert(UI.focused,'coordonnees(')"></menuitem>
            <menuitem label="conique_reduite" onclick="UI.insert(UI.focused,'conique_reduite(')"></menuitem>
        </menu>
        <menu label="Mesures">
            <menuitem label="courbure" onclick="UI.insert(UI.focused,'courbure(')"></menuitem>
            <menuitem label="cote" onclick="UI.insert(UI.focused,'cote(')"></menuitem>
            <menuitem label="equation: cartesienne" onclick="UI.insert(UI.focused,'equation(')"></menuitem>
            <menuitem label="extraire_mesure" onclick="UI.insert(UI.focused,'extraire_mesure(')"></menuitem>
            <menuitem label="longueur" onclick="UI.insert(UI.focused,'longueur(')"></menuitem>
            <menuitem label="longueur2" onclick="UI.insert(UI.focused,'longueur2(')"></menuitem>
            <menuitem label="ordonnee" onclick="UI.insert(UI.focused,'ordonnee(')"></menuitem>
            <menuitem label="parameq: eq. parametrique" onclick="UI.insert(UI.focused,'parameq(')"></menuitem>
            <menuitem label="pente" onclick="UI.insert(UI.focused,'pente(')"></menuitem>
            <menuitem label="perimetre" onclick="UI.insert(UI.focused,'perimetre(')"></menuitem>
            <menuitem label="quadrique_reduite" onclick="UI.insert(UI.focused,'quadrique_reduite(')"></menuitem>
            <menuitem label="rayon" onclick="UI.insert(UI.focused,'rayon(')"></menuitem>
            <menuitem label="scalar_product" onclick="UI.insert(UI.focused,'scalar_product(')"></menuitem>
        </menu>
        <menu label="Transformations">
            <menuitem label="homothetie" onclick="UI.insert(UI.focused,'homothetie(')"></menuitem>
            <menuitem label="inversion" onclick="UI.insert(UI.focused,'inversion(')"></menuitem>
            <menuitem label="projection" onclick="UI.insert(UI.focused,'projection(')"></menuitem>
            <menuitem label="rotation" onclick="UI.insert(UI.focused,'rotation(')"></menuitem>
            <menuitem label="similitude" onclick="UI.insert(UI.focused,'similitude(')"></menuitem>
            <menuitem label="symetrie" onclick="UI.insert(UI.focused,'symetrie(')"></menuitem>
            <menuitem label="translation" onclick="UI.insert(UI.focused,'translation(')"></menuitem>
        </menu>
        <menu label="Propriétés">
            <menuitem label="est_aligne" onclick="UI.insert(UI.focused,'est_aligne(')"></menuitem>
            <menuitem label="est_cocyclique" onclick="UI.insert(UI.focused,'est_cocyclique(')"></menuitem>
            <menuitem label="est_coplanaire" onclick="UI.insert(UI.focused,'est_coplanaire(')"></menuitem>
            <menuitem label="est_element" onclick="UI.insert(UI.focused,'est_element(')"></menuitem>
            <menuitem label="est_parallele" onclick="UI.insert(UI.focused,'est_parallele(')"></menuitem>
            <menuitem label="est_orthogonal" onclick="UI.insert(UI.focused,'est_orthogonal(')"></menuitem>
            <menuitem label="est_equilateral" onclick="UI.insert(UI.focused,'est_equilateral(')"></menuitem>
            <menuitem label="est_isocele" onclick="UI.insert(UI.focused,'est_isocele(')"></menuitem>
            <menuitem label="est_rectangle" onclick="UI.insert(UI.focused,'est_rectangle(')"></menuitem>
            <menuitem label="est_carre" onclick="UI.insert(UI.focused,'est_carre(')"></menuitem>
            <menuitem label="est_losange" onclick="UI.insert(UI.focused,'est_losange(')"></menuitem>
            <menuitem label="est_parallelogramme" onclick="UI.insert(UI.focused,'est_parallelogramme(')"></menuitem>
            <menuitem label="est_conjugue" onclick="UI.insert(UI.focused,'est_conjugue(')"></menuitem>
            <menuitem label="est_faisceau_cercle" onclick="UI.insert(UI.focused,'est_faisceau_cercle(')"></menuitem>
            <menuitem label="est_faisceau_droite" onclick="UI.insert(UI.focused,'est_faisceau_droite(')"></menuitem>
        </menu>
        <menu label="Affichage">
            <menuitem label="affichage" onclick="UI.insert(UI.focused,'affichage')"></menuitem>
            <menuitem label="nom_cache" onclick="UI.insert(UI.focused,'nom_cache')"></menuitem>
            <menuitem label="rempli" onclick="UI.insert(UI.focused,'rempli')"></menuitem>
            <menuitem label="blanc" onclick="UI.insert(UI.focused,'blanc')"></menuitem>
            <menuitem label="noir" onclick="UI.insert(UI.focused,'noir')"></menuitem>
            <menuitem label="rouge" onclick="UI.insert(UI.focused,'rouge')"></menuitem>
            <menuitem label="vert" onclick="UI.insert(UI.focused,'vert')"></menuitem>
            <menuitem label="bleu" onclick="UI.insert(UI.focused,'bleu')"></menuitem>
            <menuitem label="jaune" onclick="UI.insert(UI.focused,'jaune')"></menuitem>
            <menuitem label="magenta" onclick="UI.insert(UI.focused,'magenta')"></menuitem>
            <menuitem label="cyan" onclick="UI.insert(UI.focused,'cyan')"></menuitem>
            <menuitem label="ligne_trait_plein" onclick="UI.insert(UI.focused,'ligne_trait_plein')"></menuitem>
            <menuitem label="ligne_tiret" onclick="UI.insert(UI.focused,'ligne_tiret')"></menuitem>
            <menuitem label="ligne_tiret_point" onclick="UI.insert(UI.focused,'ligne_tiret_point')"></menuitem>
            <menuitem label="ligne_tiret_pointpoint" onclick="UI.insert(UI.focused,'ligne_tiret_pointpoint')"></menuitem>
            <menuitem label="ligne_chapeau_plat" onclick="UI.insert(UI.focused,'ligne_chapeau_plat')"></menuitem>
            <menuitem label="ligne_chapeau_rond" onclick="UI.insert(UI.focused,'ligne_chapeau_rond')"></menuitem>
            <menuitem label="ligne_chapeau_carre" onclick="UI.insert(UI.focused,'ligne_chapeau_carre')"></menuitem>
            <menuitem label="epaisseur_ligne_1" onclick="UI.insert(UI.focused,'epaisseur_ligne_1')"></menuitem>
            <menuitem label="epaisseur_ligne_2" onclick="UI.insert(UI.focused,'epaisseur_ligne_2')"></menuitem>
            <menuitem label="epaisseur_ligne_3" onclick="UI.insert(UI.focused,'epaisseur_ligne_3')"></menuitem>
            <menuitem label="epaisseur_ligne_4" onclick="UI.insert(UI.focused,'epaisseur_ligne_4')"></menuitem>
            <menuitem label="epaisseur_ligne_5" onclick="UI.insert(UI.focused,'epaisseur_ligne_5')"></menuitem>
            <menuitem label="epaisseur_ligne_6" onclick="UI.insert(UI.focused,'epaisseur_ligne_6')"></menuitem>
            <menuitem label="epaisseur_ligne_7" onclick="UI.insert(UI.focused,'epaisseur_ligne_7')"></menuitem>
            <menuitem label="point_losange" onclick="UI.insert(UI.focused,'point_losange')"></menuitem>
            <menuitem label="point_plus" onclick="UI.insert(UI.focused,'point_plus')"></menuitem>
            <menuitem label="point_carre" onclick="UI.insert(UI.focused,'point_carre')"></menuitem>
            <menuitem label="point_croix" onclick="UI.insert(UI.focused,'point_croix')"></menuitem>
            <menuitem label="point_triangle" onclick="UI.insert(UI.focused,'point_triangle')"></menuitem>
            <menuitem label="point_etoile" onclick="UI.insert(UI.focused,'point_etoile')"></menuitem>
            <menuitem label="point_point" onclick="UI.insert(UI.focused,'point_point')"></menuitem>
            <menuitem label="epaisseur_point_1" onclick="UI.insert(UI.focused,'epaisseur_point_1')"></menuitem>
            <menuitem label="epaisseur_point_2" onclick="UI.insert(UI.focused,'epaisseur_point_2')"></menuitem>
            <menuitem label="epaisseur_point_3" onclick="UI.insert(UI.focused,'epaisseur_point_3')"></menuitem>
            <menuitem label="epaisseur_point_4" onclick="UI.insert(UI.focused,'epaisseur_point_4')"></menuitem>
            <menuitem label="epaisseur_point_5" onclick="UI.insert(UI.focused,'epaisseur_point_5')"></menuitem>
            <menuitem label="epaisseur_point_6" onclick="UI.insert(UI.focused,'epaisseur_point_6')"></menuitem>
            <menuitem label="epaisseur_point_7" onclick="UI.insert(UI.focused,'epaisseur_point_7')"></menuitem>
            <menuitem label="quadrant1" onclick="UI.insert(UI.focused,'quadrant1')"></menuitem>
            <menuitem label="quadrant2" onclick="UI.insert(UI.focused,'quadrant2')"></menuitem>
            <menuitem label="quadrant3" onclick="UI.insert(UI.focused,'quadrant3')"></menuitem>
            <menuitem label="quadrant4" onclick="UI.insert(UI.focused,'quadrant4')"></menuitem>
        </menu>

        <menu label="Avancé">
            <menuitem label="axe_radical" onclick="UI.insert(UI.focused,'axe_radical(')"></menuitem>
            <menuitem label="birapport" onclick="UI.insert(UI.focused,'birapport(')"></menuitem>
            <menuitem label="pole" onclick="UI.insert(UI.focused,'pole(')"></menuitem>
            <menuitem label="polaire" onclick="UI.insert(UI.focused,'polaire(')"></menuitem>
            <menuitem label="polaire_reciproque" onclick="UI.insert(UI.focused,'polaire_reciproque(')"></menuitem>
            <menuitem label="conjugue_harmonique" onclick="UI.insert(UI.focused,'conjugue_harmonique(')"></menuitem>
            <menuitem label="division_harmonique" onclick="UI.insert(UI.focused,'division_harmonique(')"></menuitem>
            <menuitem label="point_division_harmonique" onclick="UI.insert(UI.focused,'point_division_harmonique(')"></menuitem>
            <menuitem label="puissance" onclick="UI.insert(UI.focused,'puissance(')"></menuitem>
            <menuitem label="developpee" onclick="UI.insert(UI.focused,'developpee(')"></menuitem>
        </menu>
    </menu>
    <menu label="Graphes">
        <menuitem label="plot: graphe fonction" onclick="UI.insert(UI.focused,'plot(')"></menuitem>
        <menuitem label="plotcontour: lignes de niveau" onclick="UI.insert(UI.focused,'plotcontour(')"></menuitem>
        <menuitem label="plotfield: champ des tangentes" onclick="UI.insert(UI.focused,'plotfield(')"></menuitem>
        <menuitem label="plotfunc: graphe fonction" onclick="UI.insert(UI.focused,'plotfunc(')"></menuitem>
        <menuitem label="plotimplicit: courbe implicite" onclick="UI.insert(UI.focused,'plotimplicit(')"></menuitem>
        <menuitem label="plotode: courbe integrale" onclick="UI.insert(UI.focused,'plotode(')"></menuitem>
        <menuitem label="plotparam: courbe en parametriques" onclick="UI.insert(UI.focused,'plotparam(')"></menuitem>
        <menuitem label="plotpolar: courbe en polaires" onclick="UI.insert(UI.focused,'plotpolar(')"></menuitem>
        <menuitem label="plotseq: graphe de suite recurrente" onclick="UI.insert(UI.focused,'plotseq(')"></menuitem>
        <menuitem label="plotproba: graphe probabiliste      " onclick="UI.insert(UI.focused,'plotproba(')"></menuitem>
    </menu>

</menu>
