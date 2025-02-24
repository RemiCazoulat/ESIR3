%%% PARTIE 1

%% Chargement des images
I1 = imread('I1.jpg');
J1 = imread('J1.jpg');

%% Question 1
hist = hist2(I1, J1); % On fait l'histogramme joint
sum(hist(:)) % = 262144
size(I1, 1)*size(I1, 2) % Multiplication des dimensions de l'image, = 262144

% On a bien sum(hist(:)) = N*P pour des images de taille N x P.

%% Question 2
ssd(I1, J1) % = 3.0146e+07
correlation(I1, J1) % = 0.9721

%% Question 4 VOIR DEMO_TP2.m
