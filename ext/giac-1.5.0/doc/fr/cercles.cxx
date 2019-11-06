//puissance d'un point par rapport a un cercle
puissance(C,A):={
local R,O;
R:=rayon(C);
O:=centre(C);
return longueur2(A,O)-R^2;
};

// axe radical de 2 cercles
axe_radical(C1,C2):={
local R1,O1,R2,O2,k,H;
R1:=rayon(C1);
O1:=centre(C1);
R2:=rayon(C2);
O2:=centre(C2);
k:=(R1^2-R2^2)/longueur2(O1,O2);
H:=(O1+O2+k*(O2-O1))/2;
return perpendiculaire(H, droite(O1,O2));
};

//teste si 2 cercles sont orthogonaux
est_orthogonal(C1,C2):={
local R1,O1,R2,O2;
R1:=rayon(C1);
O1:=centre(C1);
R2:=rayon(C2);
O2:=centre(C2);
return normal(longueur2(O1,O2)-R1^2-R2^2)==0;
};

//c'est est_cocyclique!!!
est_inscriptible(A,B,C,D):={
local P;
P:= inter(droite(A,C),droite(B,D))[0];
return normal(coordonnees(A-P)*coordonnees(C-P)-coordonnees(B-P)*coordonnees(D-P))==0;
}

//polaire d'un point par rapport a un cercle
polaire(C,A):={
local R,O,P1,P2,a1,a2;
R:=rayon(C);
O:=centre(C);
a1:=affixe(re(A-O));
a2:=affixe(im(A-O));
if ((a1==0) and (a2==0)){
  return erreur;}
if (a1==0){
P1:=normal(O+i*R^2/a2);
P2:=O+1+i*R^2/a2;
return droite(P1,P2);
}
if (a2==0){
P1:=O+R^2/a1;
P2:=O+i+R^2/a1;
return droite(P1,P2);
}
P1:=O+R^2/a1;
P2:=O+i*R^2/a2;

return droite(P1,P2);
};

//pole d'une droite par rapport a un cercle
pole0(D,C):={
local R,O,P,A1,A2;
R:=rayon(C);
O:=affixe(centre(C));
P:=projection(D,O);
//A1:=projection(C,P)[0];
//A2:=projection(C,P)[1];
A1:=O+R*(P-O)/abs(P-O);
A2:=O-R*(P-O)/abs(P-O);
return conj_harmonique(A1,A2,P);
};
//pole d'une droite par rapport a un cercle
poleD(C,D):={
local R,O,P,A1,A2;
R:=rayon(C);
O:=centre(C);
A1:=affixe(D[1][0]);
A2:=affixe(D[1][1]);
a:=im(A1-A2);
b:=re(A2-A1);
c:=im((A1-O)*conj(A2-O));
return point(evalc(O+R^2*(a+i*b)/c));
};

//point tel que MA/MB=k avec k complexe c'est point_div
divisionAB(A,B,k):=ifte(k==1,infinity,point((A-k*B)/(1-k)));

//polaire d'un point par rapport a 2 droites
polaireD(D1,D2,A):={
local  D3,P1,P2;
if (est_parallele(D1,D2)) {
D3:=perpendiculaire(A,D1);
 P1:=inter(D3,D1)[0];
 P2:=inter(D3,D2)[0];
 return parallele(conj_harmonique(P1,P2,A),D1); 
  }
 D3:=parallele(A,D1);  
 P2:=inter(D3,D2)[0];
return droite(inter(D1,D2)[0],rotation(P2,pi,A));
};

//savoir si A et B sont conj par rapport au cercle C
est_conjC(C,A,B):={
  return est_orthogonal(C,cercle(A,B));
};
//savoir si A et B sont conj par rapport aux droites D1,D2
est_conjD(A,B,D1,D2):={
 local D3;
    D3:=polaireD(D1,D2,A);
    return est_element(B,D3);
};

//savoir siles droites  D1 et D2 sont conj par rapport au cercle C
est_conjCD(C,D1,D2):={
 local P1;
    P1:=pole(D1,C);
    return est_element(P1,D2);
};
