
vol_b = load('IRM_cerveau_avecbiais.mat');
vol_b = vol_b.M0;
vol = load('IRM_cerveau.mat');
vol = vol.M0;
%JFcoupe(vol, 'axiale', 0.01)

[seg_b, k_b] = k_moyennes(vol_b, 4);
[seg, k] = k_moyennes(vol, 4);


%figure;
%subplot(2,1,1);
%imagesc(seg(:,:,80));
%subplot(2,1,2);
%imagesc(seg_b(:,:,80));

%surf(vol_b(:,:,80))

%JFcoupe(seg, 'sagittale', 0.01)

%[M, N ,P] = size(seg);
%[X, Y, Z] = meshgrid(1:N, 1:M, 1:P);
%[F, V, col] = MarchingCubes(X, Y, Z, smooth3(seg), 2);

img = imread('image_test_biais.png');
redress = RedressementJud(img);
imshow(redress);
