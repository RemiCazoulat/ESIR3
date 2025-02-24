function IM = mutual_information(H)
% calcule l'information mutuelle correspondante a l'histograme joint H(I,J)
Pij = H/(sum(H(:)));
Pi = sum(Pij,1);    % Toutes les probabilites necessaires au calcul de l'information mutuelle peuvent etre retrouvees avec l'histogramme joint
Pj = sum(Pij,2);
TMP = Pij.*log2(Pij./(Pj*Pi));
TMP(Pij==0)=0;
IM = sum(TMP(:));
