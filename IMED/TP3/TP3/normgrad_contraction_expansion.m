function NG = normgrad_contraction_expansion(Phi,C)

% Exo 3.1
%{
     G = grad(Phi); %fonction grad du TP2
     NG = sum(G.^2,3);
%}

% Exo 3.2

Phi_ouest = [Phi(:,1) Phi(:,1:end-1)];
Phi_est = [Phi(:,2:end) Phi(:,end)];
Phi_nord = [Phi(2:end,:) ; Phi(end,:)];
Phi_sud = [Phi(1,:) ; Phi(1:end-1,:)];

D_moins_x = Phi - Phi_ouest;
D_plus_x = Phi_est - Phi;
D_moins_y = Phi - Phi_sud;
D_plus_y = Phi_nord - Phi;

D_plus  = sqrt( max(D_moins_x,0).^2 + min(D_plus_x,0).^2  + max(D_moins_y,0).^2 + min(D_plus_y,0).^2  );
D_moins = sqrt( max(D_plus_x,0).^2  + min(D_moins_x,0).^2 + max(D_plus_y,0).^2  + min(D_moins_y,0).^2 );

NG = max(C,0).*D_moins + min(C,0).*D_plus;