function displaySegOnMri( Seg, MRI )
%
% Affiche une segmentation par dessus une segmentation moyenne
%

MRI = mat2gray(MRI); % La segmentation moyenne
Seg = mat2gray(Seg); % La segmentation a afficher par dessus
img = zeros(size(MRI, 1), size(MRI, 2), 3); % Image qui combinera les 2, dim 3 a la fin pour le rgb

% On met les 3 channels egaux a MRI -> nuances de gris
img(:,:,1) = MRI;
img(:,:,2) = MRI;
img(:,:,3) = MRI;

% On met en rouge ce qui correspond a Seg
red = ones(size(MRI, 1), size(MRI, 2), 3);

red(:,:,1) = 255 * Seg + 1 - Seg;
red(:,:,2) = 1-Seg;
red(:,:,3) = 1-Seg;

img = img .* red;

imshow(img); % On affiche



end

