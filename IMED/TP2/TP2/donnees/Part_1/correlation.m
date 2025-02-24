function [ result] = correlation( I, J )
%%
% Calcul du coefficient de correlation entre 2 images
% Avec I et J les deux images
%%

% On met les 2 images en vecteurs
I = double(I(:));
J = double(J(:));

% On calcule les moyennes
meanI = mean(I);
meanJ = mean(J);


result = sum((I - meanI) .* (J - meanJ)) / (sqrt(sum((I - meanI).^2)) * sqrt(sum((I - meanJ).^2))); % On applique la formule

end

