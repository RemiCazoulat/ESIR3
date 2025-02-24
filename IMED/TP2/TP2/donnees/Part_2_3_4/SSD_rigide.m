function [ssd_, g] = SSD_rigide(x,I,J,X,Y)
% x -> les parametres theta, p, q, qui representent respectivement le decalage de l'angle, et de la position en 2D
% I -> l'image a recaler
% J -> l'image cible
% X -> une grille 2D de la taille de J allant de 1 a size(J,1) en x et size(J,2) en y
% Y -> une grille 2D de la taille de J allant de 1 a size(J,1) en x et size(J,2) en y
    [I_x,I_y] = grad_centre(I); % calcul de la derivee de l'image
    % on recupere les parametres
    theta = x(1);
    p = x(2);
    q = x(3);
    % On applique les formules
    I_r = rigid_transformation(I,-theta, -p, -q); % transformation rigide sur l'image a recaler
    I_dx = rigid_transformation(I_x,-theta, -p, -q); % transofrmation rigide sur derivee image en x
    I_dy = rigid_transformation(I_y,-theta, -p, -q); % transformation rigide sur derivee image en y
    A = -X*sin(theta)-Y*cos(theta);
    B = X*cos(theta)-Y*sin(theta);
    d_theta = 2 * sum((I_r(:) -J(:)) .* (I_dx(:) .* A(:) + I_dy(:) .* B(:)));
    d_p = 2 * sum((I_r(:) - J(:)) .* I_x(:));
    d_q = 2 * sum((I_r(:) - J(:)) .* I_y(:));
    g = [d_theta, d_p, d_q];
    ssd_ = ssd(I_r, J); % Calcul la ssd entre l'image decalee et J

    % Visualisation des resultats intermediaires
    %subplot(2,1,1)
    %imshow(J)
    %subplot(2,1,2)
    %imshow(I_r)
    %pause(0.01)
    %drawnow
end
