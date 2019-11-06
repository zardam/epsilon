// CodeMirror, copyright (c) by Marijn Haverbeke and others
// Distributed under an MIT license: http://codemirror.net/LICENSE

(function(mod) {
  if (typeof exports == "object" && typeof module == "object") // CommonJS
    mod(require("../../lib/codemirror"));
  else if (typeof define == "function" && define.amd) // AMD
    define(["../../lib/codemirror"], mod);
  else // Plain browser env
    mod(CodeMirror);
})(function(CodeMirror) {
  "use strict";

  function wordRegexp(words) {
    return new RegExp("^((" + words.join(")|(") + "))\\b");
  }

  var wordOperators = wordRegexp(["and", "or", "not", "is"]);
  var commonKeywords = ["as", "assert", "break", "class", "continue",
                        "def", "del", "elif", "else", "except", "finally",
                        "for", "from", "global", "if", "import",
                        "lambda", "pass", "raise", "return",
                        "try", "while", "with", "yield", "in"];
  var commonBuiltins = ["abs", "all", "any", "bin", "bool", "bytearray", "callable", "chr",
                        "classmethod", "compile", "complex", "delattr", "dict", "dir", "divmod",
                        "enumerate", "eval", "filter", "float", "format", "frozenset",
                        "getattr", "globals", "hasattr", "hash", "help", "hex", "id",
                        "input", "int", "isinstance", "issubclass", "iter", "len",
                        "list", "locals", "map", "max", "memoryview", "min", "next",
                        "object", "oct", "open", "ord", "pow", "property", "range",
                        "repr", "reversed", "round", "set", "setattr", "slice",
                        "sorted", "staticmethod", "str", "sum", "super", "tuple",
                        "type", "vars", "zip", "__import__", "NotImplemented",
                        "Ellipsis", "__debug__"];

  function top(state) {
    return state.scopes[state.scopes.length - 1];
  }

  CodeMirror.defineMode("python", function(conf, parserConf) {
    var ERRORCLASS = "error";

    var delimiters = parserConf.delimiters || parserConf.singleDelimiters || /^[\(\)\[\]\{\}@,:`=;\.]/;
    //               (Backwards-compatiblity with old, cumbersome config system)
    var operators = [parserConf.singleOperators, parserConf.doubleOperators, parserConf.doubleDelimiters, parserConf.tripleDelimiters,
                     parserConf.operators || /^([-+*/%\/&|^]=?|[<>=]+|\/\/=?|\*\*=?|!=|[~!@])/]
    for (var i = 0; i < operators.length; i++) if (!operators[i]) operators.splice(i--, 1)

    var hangingIndent = parserConf.hangingIndent || conf.indentUnit;

    var myKeywords = commonKeywords, myBuiltins = commonBuiltins;
    if (parserConf.extra_keywords != undefined)
      myKeywords = myKeywords.concat(parserConf.extra_keywords);

    if (parserConf.extra_builtins != undefined)
      myBuiltins = myBuiltins.concat(parserConf.extra_builtins);

    var py3 = !(parserConf.version && Number(parserConf.version) < 3)
    if (py3) {
      // since http://legacy.python.org/dev/peps/pep-0465/ @ is also an operator
      var identifiers = parserConf.identifiers|| /^[_A-Za-z\u00A1-\uFFFF][_A-Za-z0-9\u00A1-\uFFFF]*/;
      myKeywords = myKeywords.concat(["nonlocal", "False", "True", "None", "async", "await"]);
      myBuiltins = myBuiltins.concat(["ascii", "bytes", "exec", "print"]);
      var stringPrefixes = new RegExp("^(([rbuf]|(br))?('{3}|\"{3}|['\"]))", "i");
    } else {
      var identifiers = parserConf.identifiers|| /^[_A-Za-z][_A-Za-z0-9]*/;
      myKeywords = myKeywords.concat(["exec", "print"]);
      myBuiltins = myBuiltins.concat(["apply", "basestring", "buffer", "cmp", "coerce", "execfile",
                                      "file", "intern", "long", "raw_input", "reduce", "reload",
                                      "unichr", "unicode", "xrange", "False", "True", "None"]);
      var stringPrefixes = new RegExp("^(([rubf]|(ur)|(br))?('{3}|\"{3}|['\"]))", "i");
    }
    myBuiltins = myBuiltins.concat(["ACOSH","ACOT","ACSC","ASEC","ASIN","ASINH","ATAN","ATANH","Airy_Ai","Airy_Bi","Archive","BesselJ","BesselY","Beta","BlockDiagonal","COND","COS","COSH","COT","CSC","CST","Celsius2Fahrenheit","Ci","Circle","ClrDraw","ClrGraph","ClrIO","Col","CopyVar","CyclePic","DIGITS","DOM_COMPLEX","DOM_FLOAT","DOM_FUNC","DOM_IDENT","DOM_INT","DOM_LIST","DOM_RAT","DOM_STRING","DOM_SYMBOLIC","DOM_int","DelFold","DelVar","Det","Dialog","Digits","Dirac","Disp","DispG","DispHome","DrawFunc","DrawInv","DrawParm","DrawPol","DrawSlp","DropDown","DrwCtour","ERROR","EXP","Ei","EndDlog","FALSE","Factor","Fahrenheit2Celsius","False","Fill","GF","Gamma","Gcd","GetFold","Graph","Heaviside","IFTE","Input","InputStr","Int","Inverse","JordanBlock","LN","LQ","LSQ","LU","Li","Line","LineHorz","LineTan","LineVert","NORMALD","NewFold","NewPic","Nullspace","Output","Ox_2d_unit_vector","Ox_3d_unit_vector","Oy_2d_unit_vector","Oy_3d_unit_vector","Oz_3d_unit_vector","Pause","Phi","Pi","PopUp","Psi","QR","Quo","REDIM","REPLACE","RandSeed","RclPic","Rem","Request","Resultant","Row","RplcPic","Rref","SCALE","SCALEADD","SCHUR","SIN","SVD","SVL","SWAPCOL","SWAPROW","SetFold","Si","SortA","SortD","StoPic","Store","TAN","TRUE","TeX","Text","Title","True","UTPC","UTPF","UTPN","UTPT","Unarchiv","VARS","VAS","VAS_positive","WAIT","Zeta","a2q","abcuv","about","abs","abscissa","accumulate_head_tail","acos","acos2asin","acos2atan","acosh","acot","acsc","acyclic","add","add_arc","add_edge","add_vertex","additionally","adjacency_matrix","adjoint_matrix","affix","algsubs","algvar","all_trig_solutions","allpairs_distance","alog10","alors","altitude","and","angle","angle_radian","angleat","angleatraw","animate","animate3d","animation","ans","antiprism_graph","append","apply","approx","approx_mode","arc","arcLen","arccos","arccosh","archive","arclen","arcsin","arcsinh","arctan","arctanh","area","areaat","areaatraw","areaplot","arg","args","array","arrivals","articulation_points","as_function_of","asc","asec","asin","asin2acos","asin2atan","asinh","assert","assign","assign_edge_weights","assume","at","atan","atan2acos","atan2asin","atanh","atrig2ln","augment","auto_correlation","autosimplify","avance","avgRC","axes","back","backquote","backward","baisse_crayon","bandwidth","bar_plot","bartlett_hann_window","barycenter","base","basis","batons","begin","bernoulli","besselJ","besselY","betad","betad_cdf","betad_icdf","bezier","bezout_entiers","biconnected_components","binomial","binomial_cdf","binomial_icdf","bins","bipartite","bipartite_matching","bisection_solver","bisector","bit_depth","bitand","bitor","bitxor","black","blackman_harris_window","blackman_window","bloc","blockmatrix","blue","bohman_window","border","boxwhisker","break","breakpoint","brent_solver","bvpsolve","by","c1oc2","c1op2","cFactor","cSolve","cZeros","cache_tortue","camembert","canonical_form","canonical_labeling","cap","cap_flat_line","cap_round_line","cap_square_line","cartesian_product","cas_setup","case","cat","catch","cauchy","cauchy_cdf","cauchy_icdf","cauchyd","cauchyd_cdf","cauchyd_icdf","cd","cdf","ceil","ceiling","center","center2interval","centered_cube","centered_tetrahedron","cfactor","cfsolve","changebase","channel_data","channels","char","charpoly","chinrem","chisquare","chisquare_cdf","chisquare_icdf","chisquared","chisquared_cdf","chisquared_icdf","chisquaret","choice","cholesky","choosebox","chr","chrem","chromatic_index","chromatic_number","chromatic_polynomial","circle","circumcircle","classes","clear","click","clique_cover","clique_cover_number","clique_number","clique_stats","close","clustering_coefficient","coeff","coeffs","col","colDim","colNorm","colSwap","coldim","collect","colnorm","color","colspace","colswap","comDenom","comb","combine","comment","common_perpendicular","companion","compare","complete_binary_tree","complete_graph","complete_kary_tree","complex","complex_mode","complex_variables","complexroot","concat","cond","cone","confrac","conic","conj","conjugate_gradient","connected","connected_components","cont","contains","content","continue","contourplot","contract_edge","convert","convertir","convexhull","convolution","coordinates","copy","correlation","cos","cos2sintan","cosh","cosine_window","cot","cote","count","count_eq","count_inf","count_sup","courbe_parametrique","courbe_polaire","covariance","covariance_correlation","cpartfrac","crationalroot","crayon","createwav","cross","crossP","cross_correlation","cross_point","cross_ratio","crossproduct","csc","csolve","csv2gen","cube","cumSum","cumsum","cumulated_frequencies","curl","current_sheet","curvature","curve","cyan","cycle2perm","cycle_graph","cycleinv","cycles2permu","cyclotomic","cylinder","dash_line","dashdot_line","dashdotdot_line","dayofweek","dayofweek","de","deSolve","debug","debut_enregistrement","default","degree","degree_sequence","del","delcols","delete_arc","delete_edge","delete_vertex","delrows","deltalist","denom","densityplot","departures","derive","deriver","desolve","dessine_tortue","det","det_minor","developper","developper_transcendant","dfc","dfc2f","diag","diff","digraph","dijkstra","dim","directed","discard_edge_attribute","discard_graph_attribute","discard_vertex_attribute","disjoint_union","display","disque","disque_centre","distance","distance2","distanceat","distanceatraw","div","divergence","divide","divis","division_point","divisors","divmod","divpc","dnewton_solver","do","dodecahedron","domain","dot","dotP","dot_paper","dotprod","double","draw_arc","draw_circle","draw_graph","draw_line","draw_pixel","draw_polygon","draw_rectangle","droit","droite_tangente","dsolve","duration","e","e2r","ecart_type","ecart_type_population","ecris","edge_connectivity","edges","efface","egcd","egv","egvl","eigVc","eigVl","eigenvals","eigenvalues","eigenvectors","eigenvects","element","elif","eliminate","ellipse","else","end","end_for","end_if","end_while","entry","envelope","epaisseur","epaisseur_ligne_1","epaisseur_ligne_2","epaisseur_ligne_3","epaisseur_ligne_4","epaisseur_ligne_5","epaisseur_ligne_6","epaisseur_ligne_7","epaisseur_point_1","epaisseur_point_2","epaisseur_point_3","epaisseur_point_4","epaisseur_point_5","epaisseur_point_6","epaisseur_point_7","epsilon","epsilon2zero","equal","equal2diff","equal2list","equation","equilateral_triangle","erase","erase3d","erf","erfc","error","est_permu","et","euler","euler_gamma","euler_lagrange","eval","eval_level","evala","evalb","evalc","evalf","evalm","even","evolute","exact","exbisector","excircle","execute","exp","exp2list","exp2pow","exp2trig","expand","expexpand","expln","exponential","exponential_cdf","exponential_icdf","exponential_regression","exponential_regression_plot","exponentiald","exponentiald_cdf","exponentiald_icdf","export_graph","expr","expression","extend","extract_measure","extrema","ezgcd","f2nd","fMax","fMin","fPart","faces","facteurs_premiers","factor","factor_xn","factorial","factoriser","factoriser_entier","factoriser_sur_C","factors","fadeev","faire","false","falsepos_solver","fclose","fcoeff","fdistrib","feuille","ffaire","ffonction","fft","ffunction","fi","fieldplot","filled","fin_enregistrement","find","findhelp","fisher","fisher_cdf","fisher_icdf","fisherd","fisherd_cdf","fisherd_icdf","fitdistr","flatten","float","float2rational","floor","flow_polynomial","foldl","foldr","fonction	","fonction_derivee","fopen","for","format","forward","fourier_an","fourier_bn","fourier_cn","fpour","fprint","frac","fracmod","frame_2d","frame_3d","frames","frequencies","frobenius_norm","from","froot","fsi","fsi","fsolve","ftantque","fullparfrac","func","funcplot","function","function_diff","fxnd","gammad","gammad_cdf","gammad_icdf","gauche","gauss","gauss15","gauss_seidel_linsolve","gaussian_window","gaussjord","gaussquad","gbasis","gbasis_max_pairs","gbasis_simult_primes","gcd","gcdex","genpoly","geometric","geometric_cdf","geometric_icdf","getDenom","getKey","getNum","getType","get_edge_attribute","get_edge_weight","get_graph_attribute","get_vertex_attribute","girth","gl_ortho","gl_quaternion","gl_rotation","gl_showaxes","gl_shownames","gl_texture","gl_x","gl_x_axis_color","gl_x_axis_name","gl_x_axis_unit","gl_xtick","gl_y","gl_y_axis_color","gl_y_axis_name","gl_y_axis_unit","gl_ytick","gl_z","gl_z_axis_color","gl_z_axis_name","gl_z_axis_unit","gl_ztick","gnuplot","goto","grad","gramschmidt","graph","graph2tex","graph3d2tex","graph_automorphisms","graph_charpoly","graph_complement","graph_diameter","graph_equal","graph_join","graph_power","graph_rank","graph_spectrum","graph_union","graph_vertices","graphe","graphe3d","graphe_suite","greduce","greedy_color","green","grid_graph","grid_paper","groupermu","hadamard","half_cone","half_line","halftan","halftan_hyp2exp","halt","hamdist","hamming_window","hann_poisson_window","hann_window","harmonic_conjugate","harmonic_division","has","has_arc","has_edge","hasard","head","heading","heapify","heappop","heappush","hermite","hessenberg","hessian","heugcd","hexagon","hidden_name","highlight_edges","highlight_subgraph","highlight_trail","highlight_vertex","highpass","hilbert","histogram","hold","homothety","horner","hybrid_solver","hybridj_solver","hybrids_solver","hybridsj_solver","hyp2exp","hyperbola","hypercube_graph","i","iPart","i[]","iabcuv","ibasis","ibpdv","ibpu","icdf","ichinrem","ichrem","icontent","icosahedron","id","identifier","identity","idivis","idn","iegcd","if","ifactor","ifactors","ifft","ifte","igamma","igcd","igcdex","ihermite","ilaplace","im","imag","image","implicitdiff","implicitplot","import_graph","in","inString","in_ideal","incidence_matrix","incident_edges","incircle","increasing_power","independence_number","indets","index","induced_subgraph","inequationplot","inf","infinity","infnorm","input","inputform","insert","insmod","int","intDiv","integer","integrate","integrer","inter","interactive_odeplot","interactive_plotode","interp","intersect","interval","interval2center","interval_graph","inv","inverse","inversion","invisible_point","invlaplace","invztrans","iquo","iquorem","iratrecon","irem","isPrime","is_acyclic","is_arborescence","is_biconnected","is_bipartite","is_clique","is_collinear","is_concyclic","is_conjugate","is_connected","is_coplanar","is_cospheric","is_cut_set","is_cycle","is_directed","is_element","is_equilateral","is_eulerian","is_forest","is_graphic_sequence","is_hamiltonian","is_harmonic","is_harmonic_circle_bundle","is_harmonic_line_bundle","is_included","is_inside","is_integer_graph","is_isomorphic","is_isosceles","is_network","is_orthogonal","is_parallel","is_parallelogram","is_permu","is_perpendicular","is_planar","is_prime","is_pseudoprime","is_rectangle","is_regular","is_rhombus","is_square","is_strongly_connected","is_strongly_regular","is_tournament","is_tree","is_triconnected","is_two_edge_connected","is_vertex_colorable","is_weighted","ismith","isobarycenter","isom","isomorphic_copy","isopolygon","isosceles_triangle","isprime","ithprime","jacobi_linsolve","jacobi_symbol","jordan","jusqu_a","jusqua","jusque","kde","keep_algext","keep_pivot","ker","kernel","kernel_density","kill","kneser_graph","kolmogorovd","kolmogorovt","l1norm","l2norm","label","labels","lagrange","laguerre","laplace","laplacian","laplacian_matrix","latex","lcf_graph","lcm","lcoeff","ldegree","left","left_rectangle","legend","legendre","legendre_symbol","len","length","leve_crayon","lgcd","lhs","ligne_chapeau_carre","ligne_chapeau_plat","ligne_chapeau_rond","ligne_polygonale","ligne_polygonale_pointee","ligne_tiret","ligne_tiret_point","ligne_tiret_pointpoint","ligne_trait_plein","limit","limite","lin","line","line_graph","line_inter","line_paper","line_segments","line_width_1","line_width_2","line_width_3","line_width_4","line_width_5","line_width_6","line_width_7","linear_interpolate","linear_regression","linear_regression_plot","lineariser","lineariser_trigo","linfnorm","linsolve","linspace","lis","lis_phrase","list","list2exp","list2mat","list_edge_attributes","list_graph_attributes","list_vertex_attributes","listplot","lll","ln","lname","lncollect","lnexpand","local","locus","log","log10","logarithmic_regression","logarithmic_regression_plot","logb","logistic_regression","logistic_regression_plot","lower","lowest_common_ancestor","lowpass","lp_assume","lp_bestprojection","lp_binary","lp_binaryvariables","lp_breadthfirst","lp_depthfirst","lp_depthlimit","lp_firstfractional","lp_gaptolerance","lp_hybrid","lp_initialpoint","lp_integer","lp_integertolerance","lp_integervariables","lp_interiorpoint","lp_iterationlimit","lp_lastfractional","lp_maxcuts","lp_maximize","lp_method","lp_mostfractional","lp_nodelimit","lp_nodeselect","lp_nonnegative","lp_nonnegint","lp_pseudocost","lp_simplex","lp_timelimit","lp_variables","lp_varselect","lp_verbose","lpsolve","lsmod","lsq","lu","lvar","mRow","mRowAdd","magenta","make_directed","make_weighted","makelist","makemat","makesuite","makevector","map","maple2mupad","maple2xcas","maple_ifactors","maple_mode","markov","mat2list","mathml","matpow","matrix","matrix_norm","max","maxflow","maximal_independent_set","maximize","maximum_clique","maximum_degree","maximum_independent_set","maximum_matching","maxnorm","mean","median","median_line","member","mgf","mid","middle_point","midpoint","min","minimal_edge_coloring","minimal_spanning_tree","minimal_vertex_coloring","minimax","minimize","minimum_degree","minus","mkisom","mksa","mod","modgcd","mods","montre_tortue","moustache","moyal","moyenne","mul","mult_c_conjugate","mult_conjugate","multinomial","multiplier_conjugue","multiplier_conjugue_complexe","multiply","mupad2maple","mupad2xcas","mycielski","nCr","nDeriv","nInt","nPr","nSolve","ncols","negbinomial","negbinomial_cdf","negbinomial_icdf","neighbors","network_transitivity","newList","newMat","newton","newton_solver","newtonj_solver","nextperm","nextprime","nlpsolve","nodisp","nom_cache","non","non_recursive_normal","nop","nops","norm","normal","normal_cdf","normal_icdf","normald","normald_cdf","normald_icdf","normalize","normalt","not","nprimes","nrows","nuage_points","nullspace","number_of_edges","number_of_spanning_trees","number_of_triangles","number_of_vertices","numer","octahedron","od","odd","odd_girth","odd_graph","odeplot","odesolve","of","op","open","open_polygon","option","or","ord","order_size","ordinate","orthocenter","orthogonal","osculating_circle","otherwise","ou","output","p1oc2","p1op2","pa2b2","pade","parabola","parallel","parallelepiped","parallelogram","parameq","parameter","paramplot","parfrac","pari","part","partfrac","parzen_window","pas","pas_de_cote","path_graph","pcar","pcar_hessenberg","pcoef","pcoeff","pencolor","pendown","penup","perimeter","perimeterat","perimeteratraw","periodic","perm","perminv","permu2cycles","permu2mat","permuorder","permute_vertices","perpen_bisector","perpendicular","petersen_graph","peval","pi","piecewise","pivot","pixoff","pixon","planar","plane","plane_dual","playsnd","plex","plot","plot3d","plotarea","plotcdf","plotcontour","plotdensity","plotfield","plotfunc","plotimplicit","plotinequation","plotlist","plotode","plotparam","plotpolar","plotproba","plotseq","plotspectrum","plotwav","plus_point","pmin","point","point2d","point3d","point_carre","point_croix","point_etoile","point_invisible","point_losange","point_milieu","point_plus","point_point","point_triangle","point_width_1","point_width_2","point_width_3","point_width_4","point_width_5","point_width_6","point_width_7","poisson","poisson_cdf","poisson_icdf","poisson_window","polar","polar_coordinates","polar_point","polarplot","pole","poly2symb","polyEval","polygon","polygone_rempli","polygonplot","polygonscatterplot","polyhedron","polynom","polynomial_regression","polynomial_regression_plot","position","poslbdLMQ","posubLMQ","potential","pour","pow","pow2exp","power_regression","power_regression_plot","powermod","powerpc","powexpand","powmod","prepend","preval","prevperm","prevprime","primpart","print","printf","prism","prism_graph","proc","product","program","projection","proot","propFrac","propfrac","psrgcd","ptayl","purge","pwd","pyramid","python_compat","q2a","qr","quadrant1","quadrant2","quadrant3","quadrant4","quadric","quadrilateral","quantile","quartile1","quartile3","quartiles","quest","quo","quorem","quote","r2e","radical_axis","radius","ramene","rand","randMat","randNorm","randPoly","randbetad","randbinomial","randchisquare","randexp","randfisher","randgammad","randgeometric","randint","randmarkov","randmatrix","randmultinomial","randnorm","random","random_bipartite_graph","random_digraph","random_graph","random_network","random_planar_graph","random_regular_graph","random_sequence_graph","random_tournament","random_tree","random_variable","randperm","randpoisson","randpoly","randseed","randstudent","randvar","randvector","randweibulld","range","rank","ranm","ranv","rassembler_trigo","rat_jordan","rational","rationalroot","ratnormal","rcl","rdiv","re","read","readrgb","readwav","real","realroot","reciprocation","rectangle","rectangle_droit","rectangle_gauche","rectangle_plein","rectangular_coordinates","recule","red","redim","reduced_conic","reduced_quadric","ref","reflection","regroup","relabel_vertices","reliability_polynomial","rem","remain","remove","reorder","repeat","repete","repeter","replace","resample","residue","resoudre","resoudre_dans_C","resoudre_systeme_lineaire","restart","resultant","return","reverse","reverse_graph","reverse_rsolve","revert","revlex","revlist","rgb","rhombus","rhombus_point","rhs","riemann_window","right","right_rectangle","right_triangle","risch","rm_a_z","rm_all_vars","rmbreakpoint","rmmod","rmwatch","romberg","rombergm","rombergt","rond","root","rootof","roots","rotate","rotation","round","row","rowAdd","rowDim","rowNorm","rowSwap","rowdim","rownorm","rowspace","rowswap","rref","rsolve","same","sample","samplerate","sans_factoriser","saute","sauve","save_history","scalarProduct","scalar_product","scale","scaleadd","scatterplot","schur","sec","secant_solver","segment","seidel_spectrum","seidel_switch","select","semi_augment","seq","seqplot","seqsolve","sequence_graph","series","set[]","set_edge_attribute","set_edge_weight","set_graph_attribute","set_pixel","set_vertex_attribute","set_vertex_positions","shift","shift_phase","shortest_path","show_pixels","shuffle","si","sierpinski_graph","sign","signature","signe","similarity","simp2","simplex_reduce","simplifier","simplify","simpson","simult","sin","sin2costan","sincos","single_inter","sinh","sinon","size","sizes","slope","slopeat","slopeatraw","smith","smith","smod","snedecor","snedecor_cdf","snedecor_icdf","snedecord","snedecord_cdf","snedecord_icdf","solid_line","solve","somme","sommet","sort","sorta","sortd","sorted","soundsec","spanning_tree","sphere","spline","split","spring","sq","sqrfree","sqrt","square","square_point","srand","sst","sst_in","st_ordering","stack","star_graph","star_point","start","stdDev","stddev","stddevp","steffenson_solver","step","stereo2mono","sto","str","string","string","strongly_connected_components","student","student_cdf","student_icdf","studentd","studentt","sturm","sturmab","sturmseq","style","subMat","subdivide_edges","subgraph","subs","subsop","subst","substituer","subtype","sum","sum_riemann","suppress","surd","svd","swapcol","swaprow","switch","switch_axes","sylvester","symb2poly","symbol","syst2mat","tCollect","tExpand","table","tablefunc","tableseq","tabvar","tail","tan","tan2cossin2","tan2sincos","tan2sincos2","tangent","tangente","tanh","tantque","taux_accroissement","taylor","tchebyshev1","tchebyshev2","tcoeff","tcollect","tdeg","tensor_product","test","tetrahedron","texpand","textinput","then","thickness","thiele","threshold","throw","time","title","titre","tlin","to","topologic_sort","topological_sort","torus_grid_graph","tourne_droite","tourne_gauche","tpsolve","trace","trail","trail2edges","trames","tran","transitive_closure","translation","transpose","trapeze","trapezoid","traveling_salesman","tree","tree_height","triangle","triangle_paper","triangle_plein","triangle_point","triangle_window","trig2exp","trigcos","trigexpand","triginterp","trigsimplify","trigsin","trigtan","trn","true","trunc","truncate","try","tsimplify","tukey_window","tutte_polynomial","two_edge_connected_components","type","ufactor","ugamma","unapply","unarchive","underlying_graph","unfactored","uniform","uniform_cdf","uniform_icdf","uniformd","uniformd_cdf","uniformd_icdf","union","unitV","unquote","until","upper","user_operator","usimplify","valuation","vandermonde","var","variables_are_files","variance","vector","vector","vers","version","vertex_connectivity","vertex_degree","vertex_distance","vertex_in_degree","vertex_out_degree","vertices","vertices_abc","vertices_abca","vpotential","watch","web_graph","weibull","weibull_cdf","weibull_icdf","weibulld","weibulld_cdf","weibulld_icdf","weight_matrix","weighted","weights","welch_window","wheel_graph","when","while","white","widget_size","wilcoxonp","wilcoxons","wilcoxont","with_sqrt","write","writergb","writewav","wz_certificate","xcas_mode","xor","xyztrange","yellow","zeros","zip","ztrans",
				    "ΔLIST","ΠLIST","Σ","ΣLIST","∂","∫"]);
    var keywords = wordRegexp(myKeywords);
    var builtins = wordRegexp(myBuiltins);
    CodeMirror.registerHelper("hintWords", "python", commonKeywords.concat(commonBuiltins).concat(myBuiltins));

    // tokenizers
    function tokenBase(stream, state) {
      if (stream.sol()) state.indent = stream.indentation()
      // Handle scope changes
      if (stream.sol() && top(state).type == "py") {
        var scopeOffset = top(state).offset;
        if (stream.eatSpace()) {
          var lineOffset = stream.indentation();
          if (lineOffset > scopeOffset)
            pushPyScope(state);
          else if (lineOffset < scopeOffset && dedent(stream, state) && stream.peek() != "#")
            state.errorToken = true;
          return null;
        } else {
          var style = tokenBaseInner(stream, state);
          if (scopeOffset > 0 && dedent(stream, state))
            style += " " + ERRORCLASS;
          return style;
        }
      }
      return tokenBaseInner(stream, state);
    }

    function tokenBaseInner(stream, state) {
      if (stream.eatSpace()) return null;

      var ch = stream.peek();

      // Handle Comments
      if (ch == "#") {
        stream.skipToEnd();
        return "comment";
      }

      // Handle Number Literals
      if (stream.match(/^[0-9\.]/, false)) {
        var floatLiteral = false;
        // Floats
        if (stream.match(/^[\d_]*\.\d+(e[\+\-]?\d+)?/i)) { floatLiteral = true; }
        if (stream.match(/^[\d_]+\.\d*/)) { floatLiteral = true; }
        if (stream.match(/^\.\d+/)) { floatLiteral = true; }
        if (floatLiteral) {
          // Float literals may be "imaginary"
          stream.eat(/J/i);
          return "number";
        }
        // Integers
        var intLiteral = false;
        // Hex
        if (stream.match(/^0x[0-9a-f_]+/i)) intLiteral = true;
        // Binary
        if (stream.match(/^0b[01_]+/i)) intLiteral = true;
        // Octal
        if (stream.match(/^0o[0-7_]+/i)) intLiteral = true;
        // Decimal
        if (stream.match(/^[1-9][\d_]*(e[\+\-]?[\d_]+)?/)) {
          // Decimal literals may be "imaginary"
          stream.eat(/J/i);
          // TODO - Can you have imaginary longs?
          intLiteral = true;
        }
        // Zero by itself with no other piece of number.
        if (stream.match(/^0(?![\dx])/i)) intLiteral = true;
        if (intLiteral) {
          // Integer literals may be "long"
          stream.eat(/L/i);
          return "number";
        }
      }

      // Handle Strings
      if (stream.match(stringPrefixes)) {
        state.tokenize = tokenStringFactory(stream.current());
        return state.tokenize(stream, state);
      }

      for (var i = 0; i < operators.length; i++)
        if (stream.match(operators[i])) return "operator"

      if (stream.match(delimiters)) return "punctuation";

      if (state.lastToken == "." && stream.match(identifiers))
        return "property";

      if (stream.match(keywords) || stream.match(wordOperators))
        return "keyword";

      if (stream.match(builtins))
        return "builtin";

      if (stream.match(/^(self|cls)\b/))
        return "variable-2";

      if (stream.match(identifiers)) {
        if (state.lastToken == "def" || state.lastToken == "class")
          return "def";
        return "variable";
      }

      // Handle non-detected items
      stream.next();
      return ERRORCLASS;
    }

    function tokenStringFactory(delimiter) {
      while ("rubf".indexOf(delimiter.charAt(0).toLowerCase()) >= 0)
        delimiter = delimiter.substr(1);

      var singleline = delimiter.length == 1;
      var OUTCLASS = "string";

      function tokenString(stream, state) {
        while (!stream.eol()) {
          stream.eatWhile(/[^'"\\]/);
          if (stream.eat("\\")) {
            stream.next();
            if (singleline && stream.eol())
              return OUTCLASS;
          } else if (stream.match(delimiter)) {
            state.tokenize = tokenBase;
            return OUTCLASS;
          } else {
            stream.eat(/['"]/);
          }
        }
        if (singleline) {
          if (parserConf.singleLineStringErrors)
            return ERRORCLASS;
          else
            state.tokenize = tokenBase;
        }
        return OUTCLASS;
      }
      tokenString.isString = true;
      return tokenString;
    }

    function pushPyScope(state) {
      while (top(state).type != "py") state.scopes.pop()
      state.scopes.push({offset: top(state).offset + conf.indentUnit,
                         type: "py",
                         align: null})
    }

    function pushBracketScope(stream, state, type) {
      var align = stream.match(/^([\s\[\{\(]|#.*)*$/, false) ? null : stream.column() + 1
      state.scopes.push({offset: state.indent + hangingIndent,
                         type: type,
                         align: align})
    }

    function dedent(stream, state) {
      var indented = stream.indentation();
      while (state.scopes.length > 1 && top(state).offset > indented) {
        if (top(state).type != "py") return true;
        state.scopes.pop();
      }
      return top(state).offset != indented;
    }

    function tokenLexer(stream, state) {
      if (stream.sol()) state.beginningOfLine = true;

      var style = state.tokenize(stream, state);
      var current = stream.current();

      // Handle decorators
      if (state.beginningOfLine && current == "@")
        return stream.match(identifiers, false) ? "meta" : py3 ? "operator" : ERRORCLASS;

      if (/\S/.test(current)) state.beginningOfLine = false;

      if ((style == "variable" || style == "builtin")
          && state.lastToken == "meta")
        style = "meta";

      // Handle scope changes.
      if (current == "pass" || current == "return")
        state.dedent += 1;

      if (current == "lambda") state.lambda = true;
      if (current == ":" && !state.lambda && top(state).type == "py")
        pushPyScope(state);

      var delimiter_index = current.length == 1 ? "[({".indexOf(current) : -1;
      if (delimiter_index != -1)
        pushBracketScope(stream, state, "])}".slice(delimiter_index, delimiter_index+1));

      delimiter_index = "])}".indexOf(current);
      if (delimiter_index != -1) {
        if (top(state).type == current) state.indent = state.scopes.pop().offset - hangingIndent
        else return ERRORCLASS;
      }
      if (state.dedent > 0 && stream.eol() && top(state).type == "py") {
        if (state.scopes.length > 1) state.scopes.pop();
        state.dedent -= 1;
      }

      return style;
    }

    var external = {
      startState: function(basecolumn) {
        return {
          tokenize: tokenBase,
          scopes: [{offset: basecolumn || 0, type: "py", align: null}],
          indent: basecolumn || 0,
          lastToken: null,
          lambda: false,
          dedent: 0
        };
      },

      token: function(stream, state) {
        var addErr = state.errorToken;
        if (addErr) state.errorToken = false;
        var style = tokenLexer(stream, state);

        if (style && style != "comment")
          state.lastToken = (style == "keyword" || style == "punctuation") ? stream.current() : style;
        if (style == "punctuation") style = null;

        if (stream.eol() && state.lambda)
          state.lambda = false;
        return addErr ? style + " " + ERRORCLASS : style;
      },

      indent: function(state, textAfter) {
        if (state.tokenize != tokenBase)
          return state.tokenize.isString ? CodeMirror.Pass : 0;

        var scope = top(state), closing = scope.type == textAfter.charAt(0)
        if (scope.align != null)
          return scope.align - (closing ? 1 : 0)
        else
          return scope.offset - (closing ? hangingIndent : 0)
      },

      electricInput: /^\s*[\}\]\)]$/,
      closeBrackets: {triples: "'\""},
      lineComment: "#",
      fold: "indent"
    };
    return external;
  });

  CodeMirror.defineMIME("text/x-python", "python");

  var words = function(str) { return str.split(" "); };

  CodeMirror.defineMIME("text/x-cython", {
    name: "python",
    extra_keywords: words("by cdef cimport cpdef ctypedef enum except "+
                          "extern gil include nogil property public "+
                          "readonly struct union DEF IF ELIF ELSE")
  });

});
