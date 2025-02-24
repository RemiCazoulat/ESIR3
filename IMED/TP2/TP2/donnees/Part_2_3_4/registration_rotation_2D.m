clear;close all;clc;

load('TP2_donnees');
I = Brain_MRI_1; % L'image de reference
J = Brain_MRI_3; % L'image a recaler

epsilon = 5e-7; % Pas de la descente de gradient

theta = 0;
theta_old = theta+1;

[J_x,J_y] = grad_centre(J);
[X,Y] = ndgrid(1:size(J,1),1:size(J,2))

nrj = [];

while(abs(theta-theta_old)>1e-7)

    J_r = rotation(J,-theta);
    J_rx = rotation(J_x,-theta);
    J_ry = rotation(J_y,-theta);

    A = -X*sin(theta)-Y*cos(theta);
    B = X*cos(theta)-Y*sin(theta);
    d_theta = 2 * sum((J_r(:) - I(:)) .* (J_rx(:) .* A(:) + J_ry(:) .* B(:)));

    theta_old = theta;
    theta = theta - epsilon * d_theta;
    ssd_r = ssd(J_r, I); % Calcul la ssd entre l'image decalee et J
    nrj = [nrj, ssd_r]; % Stockage de l'energie pour l'affichage


    subplot(2,2,1)
    imshow(I)
    subplot(2,2,2)
    imshow(J_r)
    subplot(2,2,[3,4])
    plot(nrj)
    pause(0.01)
    drawnow
end
