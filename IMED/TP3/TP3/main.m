load('TP3_donnees.mat') % Chargement des donnees

%%% AGREGATION DE SEGMENTATIONS

Segmentation = Segm_binaire(:,:,3);

%displaySegOnMri(Segmentation, IRM_T1);


Intersection = prod(Segm_binaire, 3);

Union = sum(Segm_binaire, 3) > 0;

Majority = mode(Segm_binaire, 3);

%displaySegOnMri(Majority, IRM_T1);

%{
subplot(2,2,1)
imshow(Intersection)
title('Intersection')
subplot(2,2,2)
imshow(Union)
title('Union')
subplot(2,2,3)
imshow(Majority)
title('Majorite')
subplot(2,2,4)
imshow(mat2gray(mode(Segm_4classes, 3)))
title('Majorite 4 classes')
%}

%%% STAPLE
%{
[W, p, q] = staple(Segm_binaire);
W(W>0.5) = 1;
W(W<0.5) = 0;
%imshow(W)
%displaySegOnMri(W, IRM_T1);
%}

%%% ROC

VERITE = zeros(size(I_toy)); VERITE(30:70,30:70) = 1;

imshow(I_toy);
seuils = 0:0.01:1;
images = zeros(size(I_toy,1), size(I_toy,2), size(seuils,2));
false_n = zeros(size(seuils));
true_n = zeros(size(seuils));
false_p = zeros(size(seuils));
true_p = zeros(size(seuils));

for i = 1:size(seuils, 2)
    % Construire segmentation par seuillage
    tmp = zeros(size(I_toy));
    tmp(I_toy>seuils(i)) = 1;
    images(:,:,i) = tmp;
    %false_n(i) =

    false_n(i) = size(VERITE(VERITE(tmp==0)==1), 1);
    true_n(i) = size(VERITE(VERITE(tmp==0)==0), 1);
    false_p(i) = size(VERITE(VERITE(tmp==1)==0), 1);
    true_p(i) = size(VERITE(VERITE(tmp==1)==1), 1);

end

sensi = true_p./(true_p+false_n);
speci = true_n./(true_n+false_p);
% Afficher pq en fonction de seuils
hold on
title('Courbe ROC')
plot(seuils, sensi, 'green')
plot(seuils, speci, 'blue')
hold off
plot(seuils, sensi+speci, 'red')

% Avec segmentation par levelset

seuils = 1:1:20;
images = zeros(size(I_toy,1), size(I_toy,2), size(seuils,2));
false_n = zeros(size(seuils));
true_n = zeros(size(seuils));
false_p = zeros(size(seuils));
true_p = zeros(size(seuils));

for i = 1:size(seuils, 2)
    % Construire segmentation par seuillage
    images(:,:,i) = segmentation(I_toy,seuils(i));
    false_n(i) = size(VERITE(VERITE(tmp==0)==1), 1);
    true_n(i) = size(VERITE(VERITE(tmp==0)==0), 1);
    false_p(i) = size(VERITE(VERITE(tmp==1)==0), 1);
    true_p(i) = size(VERITE(VERITE(tmp==1)==1), 1);

end

sensi = true_p./(true_p+false_n);
speci = true_n./(true_n+false_p);
% Afficher pq en fonction de seuils
%{
hold on
title('Courbe ROC')
plot(seuils, sensi, 'green')
plot(seuils, speci, 'blue')
hold off
%plot(speci, sensi, 'red')
}%


