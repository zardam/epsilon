card(l):={
  repete(4,saute(l/3),saute(l/3),saute(l/3),tourne_gauche);
  tourne_gauche(45);
  dessine_tortue;avance(l*sqrt(2));
  tourne_droite(45);pa
}; 
card9(l):={
  card(l/3);tourne_droite;card(l/3);tourne_gauche;card(l/3);tourne_gauche;
  card(l/3);tourne_gauche;card(l/3);tourne_droite;card(l/3);tourne_droite;
  card(l/3);tourne_droite;card(l/3);tourne_gauche;card(l/3);tourne_gauche(45);
};
peano(l):={
  si (l<10) alors avance(l);
  sinon
  peano(l/3);
  tourne_droite;peano(l/3);
  tourne_gauche;peano(l/3);
  tourne_gauche;peano(l/3);
  tourne_gauche;peano(l/3);
  tourne_droite;peano(l/3);
  tourne_droite;peano(l/3);
  tourne_droite;peano(l/3);
  tourne_gauche;peano(l/3);
  fsi
};
peanob(l,s):={
  si (l<10) alors avance(l);
  sinon
  tourne_gauche(-90*s);peanob(l/2,-s);
  tourne_droite(-90*s);peanob(l/2,s);
  peanob(l/2,s);tourne_droite(-90*s);
  peanob(l/2,-s);tourne_gauche(-90*s);
  fsi
};

peanot1(l,s):={
  si (s==-1) alors 
    tourne_droite(45);saute(-l*sqrt(2)/6);rond(round(l*sqrt(2)/6),90);
  sinon 
    si (s==1) alors
      tourne_gauche(135);saute(-l*sqrt(2)/6);rond(round(-l*sqrt(2)/6),90);
    sinon
      tourne_gauche(45);
      saute(l*sqrt(2)/6);
    fsi;
  fsi;
  rond(round((l*sqrt(2)/6),90);
  rond(round(-l*sqrt(2)/6),270);
  rond(round(l*sqrt(2)/6),270);
  rond(round(-l*sqrt(2)/6),90);
  saute(l*sqrt(2)/6);
  tourne_droite(45);
};

peanot(l,s):={
  si (l<30) alors 
    peanot1(l,s);
  sinon
    si (s==-1) alors 
      peanot(l/3,-1);
    sinon
      si (s==1) alors 
          peanot(l/3,1);
      sinon 
         peanot(l/3,0);
      fsi;
    fsi;
    tourne_droite;peanot(l/3,1);
    tourne_gauche;peanot(l/3,-1);
    tourne_gauche;peanot(l/3,-1);
    tourne_gauche;peanot(l/3,-1);
    tourne_droite;peanot(l/3,1);
    tourne_droite;peanot(l/3,1);
    tourne_droite;peanot(l/3,1);
    tourne_gauche;peanot(l/3,-1);
 fsi;
};
