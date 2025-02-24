function [W,p,q] = staple(L,eps)
% Algorithme STAPLE dans le cas binaire
% L contient les segmentations initiales
% eps (optionnel) pour le test d'arret

if (nargin<2)
    eps = 1e-7;
end

n = size(L,3); %nombre de segmentations initiales

%a priori
P0 = sum(L(:)==0)/numel(L); % Pr(T_i=0)
P1 = sum(L(:)==1)/numel(L); % Pr(T_i=1)

% initialisation
p = ones(1,n); p = p-.001; % sensibilite
q = ones(1,n); q = q-.001; % specificite

iter = 1

arret=0;
while(~arret)
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %   E-Step (on connait p et q, on met a jour W)  %
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
    A = ones(size(L(:,:,1)));
    B = ones(size(L(:,:,1)));

    for j = 1:n
        A = A .* ((L(:,:,j) * p(j)) + ((1 - L(:,:,j)) * (1 - p(j))));
        B = B .* (((1 - L(:,:,j)) * q(j)) + (L(:,:,j) * (1 - q(j))));
        if j == 1
        end
    end
    
    A = P1 * A;
    B = P0 * B;
    
    W = A./(A+B);

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %   M-Step (on connait W, on met a jour p et q)  %
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    p_old=p;
    q_old=q;
    
    for j = 1:n
        p(j) = sum(sum(L(:,:,j) .* W)) / sum(sum(W));
        q(j) = sum(sum((1 - L(:,:,j)) .* (1 - W))) / sum(sum(1 - W));
    end

    p
    q


    iter = iter + 1;
    % test d'arret sur la variation de p et q
    dpq = max(norm(p_old-p),norm(q_old-q));
    arret = dpq<eps;
    disp(num2str(dpq));
end