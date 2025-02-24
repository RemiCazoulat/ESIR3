function S=segmentation(I,sigma)


    reinit_dist = 30;

    nb_affiche=100;
    Bin = ones(size(I));
    marge = max(3,round(min(size(I))/100));
    Bin(marge:end-marge,marge:end-marge)=0;
    Dist_int = (Bin==0).*(-bwdist(Bin)+.5);
    Dist_ext = (Bin>0).*(bwdist(Bin==0)-.5);
    Phi = Dist_int + Dist_ext;
    C=1;
    dt=.1;
    alpha = .5;

niveau0 = extraction_niveau0(Phi);

%G: edge-stopping function
% 
Gradient = grad_centre(floute(I,sigma));
norm_Gradient = sqrt(sum(Gradient.^2,3));
G = 1./(1+norm_Gradient);
G = G - min(G(:));
G(G<.2*max(G(:))) = 0;
W=G;


cont=true;
t=0;
while(cont)
    t=t+1;
    G = W.*(alpha*courbure(Phi)+normgrad_contraction_expansion(Phi,C)); 
    
    Phi = Phi + dt*G;
    

    if (mod(t,reinit_dist)==0)
         Phi = reinit_distance(Phi);
    end
    if (mod(t,nb_affiche)==0)
        niveau0_old=niveau0;
        niveau0 = extraction_niveau0(Phi);
        if norm(niveau0_old-niveau0)==0
            cont=false;
        end
        I_couleur = repmat(I,[1 1 3]);
        R = I_couleur(:,:,1);
        R(niveau0==0)=max(I(:));
        I_couleur(:,:,1) = R;
        I_couleur(:,:,2) = I_couleur(:,:,2).*(niveau0);
        I_couleur(:,:,3) = I_couleur(:,:,2).*(niveau0);
        %imagesc((I_couleur-min(I(:)))/(max(I(:))-min(I(:))));axis off;axis equal;
        %drawnow;
    end
end

S = Phi<0;