close all; clear; clc;

for k=1:4
    I =double(imread(['I' num2str(k) '.jpg']));
    J =double(imread(['J' num2str(k) '.jpg']));

    H=hist2(I,J);

    SSD = ssd(I,J);
    CORR = correlation(I,J);
    IM = mutual_information(H);

    disp(['SSD=' num2str(SSD)]);
    disp(['correlation=' num2str(CORR)]);
    disp(['info mutuelle=' num2str(IM)]);
    disp('-----------------');
    subplot(1,3,1)
    imagesc(I);axis equal;axis off;colormap gray;freezeColors;
    subplot(1,3,2)
    imagesc(J);axis equal;axis off;colormap gray;freezeColors;
    subplot(1,3,3)
    imagesc(log(H));axis equal;axis off;axis xy;colormap jet;
    pause
end

%%% Commentaires sur les resultats

%% Couple I1 - J1
% On voit que J1 est une version bruitee de I1, d'ou le fait que la ssd renvoie une valeur plutot elevee. L'information mutuelle et la correlation marche bien !
% L'histogramme joint est une diagonale épaisse.

%% Couple I2 - J2
% J2 est un version ou l'histogramme de I2 a ete reserre. Cela fait que la correlation ne marche pas tres bien (<0.5). L'information mutuelle fonctionne toujours.
% L'histogramme joint est une droite

%% Couple I3 - J3
% Les deux images sont des visages differents non recales. La ssd et la correlation ne peuvent pas etre utilisees et renvoient des valeurs tres basses signifiant que ces images sont tres differentes.
% L'histogramme joint est un nuage de point

%% Couple I4 - J4
% Cette fois les deux visages sont recales, la ssd baisse, la correlation augmente (>0.5) et l'information mutuelle egalement.
% Dans l'histogramme joint le nuage de point se concentre autour d'une droite.


