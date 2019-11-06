arbre3(x,y):={
 DispG();
 if (abs(x-y)<0.2) {segment(x,y); return 0;}
 segment(x,(x+y)*0.5);
 arbre3((3*x+y)/4,(3*x+y)/4+(y-x)*0.25*exp(i*0.5));
 arbre3((3*x+y)/4,(3*x+y)/4+(y-x)*0.25*exp(-i*0.5));
 arbre3((x+y)/2,(x+y)/2+(y-x)*0.5*exp(i));
 arbre3((x+y)/2,(x+y)/2+(y-x)*0.5*exp(-i));
 arbre3((x+y)/2,(x+y)/2+(y-x)*0.5);}