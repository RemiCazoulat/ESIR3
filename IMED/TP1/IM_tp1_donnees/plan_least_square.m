close all;clear;clc
n=100;

%on genere les donnees 
x = rand(1,n);
y = rand(1,n);
z = 2*x-5*y+3 + randn(1,n); %equation du plan z=2x-5y+3 et on ajoute du bruit blanc gaussien(mu=0 std=1)
% c'est cette equation qu'on veut retrouver malgre le bruit present dans les donnees
% on cherche donc a estimer a,b et c tel que a*X + b*Y + c soit le plus proche de Z possible

%on affiche les donnees
L=plot3(x,y,z,'ro'); % affiche les points (x,y,z) (donnees)
set(L,'Markersize',2*get(L,'Markersize')) % augmente la taille des cercles
set(L,'Markerfacecolor','r') % remplit les cercles

pause %appuyez sur une touche pour continuer

Xcolv = x(:); % on transforme en vecteur colonne
Ycolv = y(:); 
Zcolv = z(:); 
Const = ones(size(Xcolv)); %vecteur de 1 pour le terme constant

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Coefficients = [Xcolv Ycolv Const]\Zcolv; % Trouve les coefficients (moindre carre)
%help \   -->  
%     If A is an M-by-N matrix with M < or > N and B is a column
%     vector with M components, or a matrix with several such columns,
%     then X = A\B is the solution in the least squares sense to the
%     under- or overdetermined system of equations A*X = B.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

XCoeff = Coefficients(1); % coefficient dvt le terme en X (a)
YCoeff = Coefficients(2); % coefficient dvt le terme en Y (b)
CCoeff = Coefficients(3); % terme constant (c)
% avec les variables ci-dessus on a z " a peu pres egal a" XCoeff * x + YCoeff * y + CCoeff
% il s'agit du plan qui "explique" le mieux les donnees.

% On affiche le plan pour verifier
hold on
[xx, yy]=meshgrid(0:.1:1,0:.1:1); % genere une grille reguliere pour l'affichage du plan estime
zz = XCoeff * xx + YCoeff * yy + CCoeff;
surf(xx,yy,zz) % affiche le plan donne par l'equation estimee
title(sprintf('Plan z=(%f)*x+(%f)*y+(%f)',XCoeff, YCoeff, CCoeff)) %equation du plan estime (doit etre proche de z=2x-5y+3)
% En tournant autour de la surface on voit que les points (x,y,z) sont "a peu pres" sur le plan estime