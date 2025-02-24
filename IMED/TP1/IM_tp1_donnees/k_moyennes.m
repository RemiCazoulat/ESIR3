function [Segmentation,Seuils] = k_moyennes(M0,k)
    k = k - 1;
    vecM0 = M0(:);
    maxVal = max(vecM0);
    minVal = min(vecM0);
    Classes = zeros(size(M0));

    %initialisation des k seuils
    Moyennes = zeros(k);
    Seuils = linspace(minVal, maxVal, k + 1) % size : k + 1
    
     
    normSeuils = 1;
    while (normSeuils>0) 
        %Mise a jour des moyennes de chaque classe 
        Seuils_old = Seuils;
        % on boucle sur chaque seuils pour faire les moyennes
        for i = 1:k
            Moyennes(i) = mean(M0(Seuils(i) < M0 & M0 <= Seuils(i + 1)));
        end
        
        % on boucle sur les moyennes pour faire les nouveaux seuils
        for i = 2:k
            Seuils(i) = (Moyennes(i - 1) + Moyennes(i)) / 2;
        end
       
        % recalcul de la norme
        normSeuils = norm(Seuils-Seuils_old);
    end
    
    % On construit les classes a partir des seuils
    for i = 1:k
        Classes(Seuils(i) < M0 & M0 <= Seuils(i + 1)) = i;
    end
    
    Segmentation = Classes;
end