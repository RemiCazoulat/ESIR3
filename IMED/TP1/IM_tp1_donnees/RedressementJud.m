function [ redressee ] = RedressementJud( criminelle )

n = size(criminelle, 1);
m = size(criminelle, 2);
x = repmat((1:n)', 1, m) / n;
y = x';
z = double(criminelle);

Xcolv = x(:);
Ycolv = y(:);
Zcolv = z(:);
Const = ones(size(Xcolv));

size(Xcolv)
size(Ycolv)
size(Zcolv)

Coefficients = [Xcolv Ycolv Const]\Zcolv;

XCoeff = Coefficients(1);
YCoeff = Coefficients(2);
CCoeff = Coefficients(3);

CCoeffvec = ones(n * m, 1) * CCoeff;

[xx, yy]=meshgrid(0:.1:1,0:.1:1); % genere une grille reguliere pour l'affichage du plan estime
zz = XCoeff * xx + YCoeff * yy + CCoeff;

surf(xx,yy,zz) % affiche le plan donne par l'equation estimee
title(sprintf('Plan z=(%f)*x+(%f)*y+(%f)',XCoeff, YCoeff, CCoeff)) %equation du plan estime (doit etre proche de z=2x-5y+3)

redressee = Zcolv + CCoeffvec + x(:) * XCoeff + y(:) * YCoeff;

reshape(redressee, size(criminelle));

end

