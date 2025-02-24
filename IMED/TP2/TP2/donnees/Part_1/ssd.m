function [ result ] = ssd( I,J )
%%
% Calcul la ssd entre 2 images (Sum of squared differences)
% Avec I et J les deux images
%%

% On met les 2 images en vecteurs
I = double(I(:));
J = double(J(:));
result = sum((I-J).^2); % On applique simplement la formule
end

