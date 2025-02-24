function [ I_r ] = recalage_rigide_ssd( I, J, tol, epsilon)
% Permet de recaler l'image I sur l'image J par translation et rotation.
% Utilise la ssd comme critère.
% Paramètres :
%   I -> L'image à recaler
%   J -> L'image de référence
%   tol -> Seuil de tolérance
%   epsilon -> Pas de la descente de gradient

theta = 0;
p = 0;
q = 0;
var = 0;
var_old = var + 1 + tol;

[I_x,I_y] = grad_centre(I);
[X,Y] = ndgrid(1:size(I,1),1:size(I,2));

nrj = [];

while(abs(var - var_old)>1e-7)
    
    I_r = rigid_transformation(I,-theta, p, q);
    I_dx = rigid_transformation(I_x,-theta, p, q);
    I_dy = rigid_transformation(I_y,-theta, p, q);
    
    A = -X*sin(theta)-Y*cos(theta);
    B = X*cos(theta)-Y*sin(theta);
    d_theta = 2 * sum((I_r(:) -J(:)) .* (I_dx(:) .* A(:) + I_dy(:) .* B(:)));
    d_p = 2 * sum((I_r(:) - J(:)) .* I_x(:));
    d_q = 2 * sum((I_r(:) - J(:)) .* I_y(:));
    
    var_old = var;
    theta = theta - epsilon * d_theta;
    p = p - epsilon * d_p;
    q = q - epsilon * d_q;
    var = theta + p + q;
    ssd_r = ssd(I_r, J); % Calcul la ssd entre l'image décalée et J
    nrj = [nrj, ssd_r]; % Stockage de l'énergie pour l'affichage

    
    subplot(2,2,1)
    imshow(J)
    subplot(2,2,2)
    imshow(I_r)
    subplot(2,2,[3,4])
    plot(nrj)
    pause(0.01)
    drawnow
end

end

