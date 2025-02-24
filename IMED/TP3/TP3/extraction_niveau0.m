function niveau0 = extraction_niveau0(Phi)
% Phi : fonction distance signée
% niveau0 : image binaire valant 1 partout sauf aux pixels (i,j) tels qu'il y a un changement de signe entre la
% valeur du pixel courant et celle de ses voisins (i,j+1), (i+1,j) ou (i+1,j+1).

Phi_est = [Phi(:,2:end) Phi(:,end)];
Phi_nord = [Phi(2:end,:) ; Phi(end,:)];
Phi_nord_est = [Phi_nord(:,2:end) Phi_nord(:,end)];

niveau0 = (Phi.*Phi_est > 0) .* (Phi.*Phi_nord > 0) .* (Phi.*Phi_nord_est > 0);
