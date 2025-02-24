clear; close all; clc;

% On charge les donnees
load('TP2_donnees');
I = Brain_MRI_1; % notre image cible (ici I, mais dans SSD_rigide J)
J = rigid_transformation(I, pi / 10, 1, 1); % creation de l'image a recaler
%J = Brain_MRI_4;
%I = Sclerose_en_Plaques_M0;
%J = Sclerose_en_Plaques_M3;
[X,Y] = ndgrid(1:size(J,1),1:size(J,2)); % creation des attributs X et Y pour SSD_rigide


param = [0 0 0]; %parametres de recalage (theta p q) initiaux

%options = optimoptions('fminunc','Display','iter','Algorithm','trust-region','SpecifyObjectiveGradient',true); % les options donnees
options = optimoptions('fminunc','GradObj','on' ); % modifications des options donnees car la compilation ne marchait pas


f = @(x)SSD_rigide(x,R,I,X,Y); % on creee un "pointeur" de fonction pour "cacher" les arguments autres que les parametres a optmiser

% ==========================================================
% Essai d'une boucle while afin d'ameliorer les performances
% ==========================================================
%last_fval = 1;
%fval = 0;
%R = J;
%index = 0;
%while abs(last_fval - fval) > 1e-3
    %last_fval = fval;

    %[param, fval] = fminunc(f,param,options)
    %R = rigid_transformation(R, -param(1), -param(2), -param(3));

    %subplot(2,2,1)
    %imshow(I)
    %subplot(2,2,2)
    %imshow(J)
    %subplot(2,2,4)
    %imshow(R)
    %pause(0.01)
    %drawnow
    %index = index + 1
%end
%index

[param,fval] = fminunc(f,param,options) % execution de la descente de gradient avec le pas adaptatif, suivant la fonction SSD_rigide

%visualisation du resultat
R = rigid_transformation(J, -param(1), -param(2), -param(3));
subplot(2,1,1)
imshow(I)
subplot(2,1,2)
imshow(R)
