I Methodes iteratives
//a partir d'un triangle ABC direct on trace son semblable sur AC etc...
//k=rapport et t=angle de la similitude
//n=nombre de triangles a construire
spirale(A,B,C,n):={
local k,t;
triangle(A,B,C);
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
for (j:=1;j<=n;j++){
 B:=C; 
 C:=similitude(A,k,t,B);
 triangle(A,B,C);
}
};
//meme dessin que spirale(A,B,C,n) mais sans repasser sur le meme trait
spirales(A,B,C,n):={
local k,t;
triangle(A,B,C);
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
for (j:=1;j<=n;j++){
 B:=C; 
 C:=similitude(A,k,t,B);
 segment(B,C);
 segment(A,C);
}
};
//a partir d'un triangle ABC direct on trace son semblable sur AC etc...n fois
// et on trace aussi le semblable du triangle ABC sur AB etc...(aussi n fois)
//Attention angle renvoie une valeur absolue (= valeur de l'angle non oriente)
spirale2(A,B,C,n):={
local k,t,B0,C0;
triangle(A,B,C);
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
B0:=B;
C0:=C;
for (j:=1;j<=n;j++){
 B:=C; 
 C:=similitude(A,k,t,B);
 triangle(A,B,C);
};
B:=B0;
C:=C0;
for (j:=1;j<=n;j++){
 C:=B; 
 B:=similitude(A,1/k,-t,C);
 triangle(A,B,C);
}
};
//meme dessin que spirale2(A,B,C,n) mais sans repasser sur le meme trait
spirales2(A,B,C,n):={
local k,t,B0,C0;
triangle(A,B,C);
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
B0:=B;
C0:=C;
for (j:=1;j<=n;j++){
 B:=C; 
 C:=similitude(A,k,t,B);
 segment(B,C);
 segment(A,C);
};
B:=B0;
C:=C0;
for (j:=1;j<=n;j++){
 C:=B; 
 B:=similitude(A,1/k,-t,C);
 segment(B,C);
 segment(A,B);
}
};
II Methodes recursives
//Le dessin obtenu a partir d'un triangle ABC direct peut etre decrit de 
//facon recursive si on a C=similitude(A,k,t,B), soit  C1=similitude(A,k,t,C) :
//spiraler(A,B,C,0)=triangle(A,B,C) 
// si n>0, spiraler(A,B,C,n)=triangle(A,B,C) puis spiraler(A,C,C1,n-1)
//n=nombre de triangles a construire=nombre d'appels recursifs
spiraler(A,B,C,n):={
local k,t;
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
if (n>0) {
 triangle(A,B,C);
 B:=C;
 C:=similitude(A,k,t,B);
 spiraler(A,B,C,n-1);
   } else
 triangle(A,B,C);
};
//meme dessin que spiraler(A,B,C,n) mais sans repasser sur le meme trait
//si on a C=similitude(A,k,t,B), soit  C1=similitude(A,k,t,C) 
//spiralers(A,B,C,0)=triangle(A,B,C) 
// si n>0, spiralers(A,B,C,n)=segment AB et BC puis spiralers(A,C,C1,n-1)
spiralers(A,B,C,n):={
local k,t;
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
if (n>0) {
 segment(A,B);
 segment(B,C);
 B:=C;
 C:=similitude(A,k,t,B);
 spiralers(A,B,C,n-1);
   } else
 triangle(A,B,C);
};
//meme dessin que spiraler(A,B,C,n) mais autre facon de voir la recursivite
//spiraler1(A,B,C,0)=triangle(A,B,C) 
// si n>0, spiraler1(A,B,C,n)=dernier petit triangle AMN + spiraler1(A,B,C,n-1)
//sans repasser sur le meme trait :
//spiraler1s(A,B,C,0)=triangle(A,B,C) 
//si n>0, spiraler1s(A,B,C,n)=segments AN et MN + spiraler1s(A,B,C,n-1)
spiraler1s(A,B,C,n):={
local k,t;
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
if (n>0) {
 M:=similitude(A,k^n,n*t,B);
 N:=similitude(A,k,t,M);
 segment(M,N);
 segment(A,N);
 spiraler1s(A,B,C,n-1);
   } else 
 triangle(A,B,C);
};
//meme dessin que spirale2(A,B,C,n) mais en recursif cette fois on a une seule 
//facon de voir le dessin (analogue a spiraler1s)
//et sans repasser sur le meme trait
//spiralers2(A,B,C,0)=triangle(A,B,C) 
//si n>0, spiralers2(A,B,C,n)=segments AN et MN + spiraler1s(A,B,C,n-1)+
//segments AQ et PQ 
spiralers2(A,B,C,n):={
local k,t,M,N,P,Q;
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
if (n>0) {
 M:=similitude(A,k^n,n*t,B);
 N:=similitude(A,k,t,M);
 segment(M,N);
 segment(A,N);
 spiralers2(A,B,C,n-1);
 P:=similitude(A,1/k^n,-n*t,C);
 Q:=similitude(A,1/k,-t,P);
 segment(P,Q);
 segment(A,Q);
   } else 
 triangle(A,B,C);
};
//spiraler2s(A,B,C,1,3) + spiraler2s(A,C,B,-1,3)
//equivalent a spiralers2(A,B,C,3)
//s= signe de l'angle oriente (AB,AC) car angle(A,B,C)=val abs de cet angle 
spiraler2s(A,B,C,s,n):={
local k,t;
k:=longueur(A,C)/longueur(A,B);
t:=angle(A,B,C);
if (n>0) {
 //triangle(A,B,C);
 segment(A,B);
 segment(B,C);
 B:=similitude(A,k,s*t,B);
 C:=similitude(A,k,s*t,C);
 spiraler2s(A,B,C,s,n-1);
   } else {
 triangle(A,B,C);
}
}
