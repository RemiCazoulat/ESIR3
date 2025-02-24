function [ recalee ] = recalage_translation_ssd( I, J, tol, epsilon)
% Permet de recaler l'image I sur l'image J par translation uniquement.
% Utilise la ssd comme critere.
% Parametres :
%   I -> L'image a recaler
%   J -> L'image de reference
%   tol -> Seuil de tolerance
%   epsilon -> Pas de la descente de gradient


p = 5; % Initialisation des translations
q = 5;
recalee = translation(I, p, q);
derSSDp = 0; % Initialisation des derivees
derSSDq = 0;
ssd_r = 0; % Initialisation du ssd
last_ssd = 1+tol; % SSD precedent
nrj = []; % Pour afficher l'energie

while abs(last_ssd - ssd_r) > tol % Iteration jusqu'a atteinte du seuil de tolerance
    
    % Calcul des derivees
    [fx,fy] = grad_centre(recalee); % derivee de l'image
    derSSDp = 2 * sum((recalee(:) - J(:)) .* fx(:));
    derSSDq = 2 * sum((recalee(:) - J(:)) .* fy(:));
    
    % Update des translations
    p = p - epsilon * derSSDp;
    q = q - epsilon * derSSDq;
    % Nouvelle image decalee
    recalee = translation(I, -p, -q);
    last_ssd = ssd_r; 
    ssd_r = ssd(recalee, J); % Calcul la ssd entre l'image decalee et J
    nrj = [nrj, ssd_r]; % Stockage de l'energie pour l'affichage
    
    % On affiche les images et l'energie a chaque iteration
    subplot(2,2,1)
    imshow(J)
    subplot(2,2,2)
    imshow(recalee)
    subplot(2,2,[3,4])
    plot(nrj)
    pause(0.01)
    drawnow
    
end



end

