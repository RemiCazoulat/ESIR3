%% Chargement des images

load('TP2_donnees');
I = Brain_MRI_1;


%% =====[ PARTIE 2 : translation + SSD ]=====
% - Question 1
p = 20.5;
q = 10.2;

% DECOMMENTER LES DEUX LIGNES POUR EXECUTION
%IT = translation(I, p, q);
%imshow(IT); % l'image est decalee de 20.5 vers le bas et 10.2 vers la droite, donc on a J(x,y) = I(x+q, y+p)

% - Question 2
tolerance = 1e-3;
% DECOMMENTER LA LIGNE POUR EXECUTION
%recalage_translation_ssd(Brain_MRI_1, Brain_MRI_2, tolerance, 0.01);

% - Question 3

%

%% =====[ PARTIE 3 : SSD + Rotation ]=====
% - Question 1 : regarder script registration_rotation_2D

%% =====[ PARTIE 4 : SS + Transformations rigides 2D ]=====
% - Question 1 : voir fonction rigid_transformation
% - Question 2
% J = rigid_transformation( I, pi / 10, 1, 1);
% recalage_rigide_ssd(J, I, tolerance, 8e-6);
% Ce qu'il se passe est que soit le pas est trop eleve et la minimisaiton ne peut pas se faire car cela ne converge pas
% Soit le pas est trop faible et la convergence prend un tres long moment a se faire.
% L'image de l'enonce explique bien ce phenomene en montrant qu'il y a 2 pentes imbriquees,
% l'une tres pentue qui ne permet pas d'avoir un pas eleve au risque de diverger (verger),
% et une autre pente plus douce qui, si le pas est petit, ne va pas permettre une convergence rapide vers un minimum.
% L'ideal serait donc d'avoir un pas qui s'adapte au Gradient.
% Plus celui-ci est fort, plus le pas est petit, et plus il est faible, plus le pas est grand.


% - Question 3-4 : regarder script squelette_fminunc et fonction SSD_rigide


