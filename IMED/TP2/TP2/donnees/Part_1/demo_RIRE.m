close all; clear; clc;
load('RIRE_database');

%#ok<*AGROW>
fig1=figure;
set(fig1,'Units','Normalized','Position',[.1 .1 .8 .8]);
fig2=figure;
set(fig2,'Units','Normalized','Position',[.1 .1 .8 .8]);


donnees = {'CT'; 'PET' ; 'T1'; 'PD' };

for k=1:numel(donnees)
    tab_SSD = [];
    tab_CORR = [];
    tab_IM = [];
    I = PD;
    J_init = eval(donnees{k});

    figure(fig1);
    for a=10:-1:0

        J = imtranslate(J_init,[2*a -a a/4]);
        J = imrotate(J,a,'crop');

        H=hist2(I,round(J)); % round pour garder valeurs entieres (plus simple pour hist2)

        SSD = ssd(I,J);
        CORR = correlation(I,J);
        IM = mutual_information(H);

        tab_SSD = [tab_SSD SSD];
        tab_CORR = [tab_CORR CORR];
        tab_IM = [tab_IM IM];

        disp(['SSD=' num2str(SSD)]);
        disp(['correlation=' num2str(CORR)]);
        disp(['info mutuelle=' num2str(IM)]);
        disp('-----------------');
        subplot(1,3,1)
        imagesc(I(:,:,3));axis equal;axis off;colormap gray;freezeColors; title('Densite de protons (IRM)');
        subplot(1,3,2)
        imagesc(J(:,:,3));axis equal;axis off;colormap gray;freezeColors; title(donnees{k});
        subplot(1,3,3)
        imagesc(log(H));axis equal;axis off;colormap jet;axis xy;
        drawnow;
    end
    pause
    figure(fig2);

    subplot(1,3,1);
    plot(tab_SSD);
    title('SSD')
    subplot(1,3,2);
    plot(tab_CORR);
    title('Correlation');
    subplot(1,3,3);
    plot(tab_IM);
    title('Information Mutuelle');
    pause

end
%========[ ANALYSE DES RESULTATS ]========

% Nous avons pu voir que malgre l'amelioration du recalage entre les images
% au fur et a mesure que les etapes avancent, le resultat de la SSD ou de
% la correlation n'augmente pas forcement, alors que le resultat de
% l'information mutuelle si. Nous avons pu noter que la correlation est
% quand meme plus efficace que la SSD dans plus de cas de figures. Aussi,
% les resultats de la SSD sont tres vite extremement grand dans des cas
% multi modaux, ce qui rend difficile leur interpretation (est ce que
% l'image est persque recalee ou pas du tout recalee ?).

%==========================================================================
% PPPPPP   RRRRRR    OOOOO   U     U  TTTTTTT  H     H  Y     Y  SSSSSSS
% P     P  R     R  O     O  U     U     T     H     H   Y   Y   S
% P     P  R     R  O     O  U     U     T     H     H    Y Y    S
% PPPPPP   RRRRRR   O     O  U     U     T     HHHHHHH     Y     SSSSSSS
% P        R   R    O     O  U     U     T     H     H     Y           S
% P        R    R   O     O  U     U     T     H     H     Y           S
% P        R     R   OOOOO    UUUUU      T     H     H     Y     SSSSSSS
%
%                 *****   *****
%              **     ** **     **
%             **        *        **
%              **               **
%                **           **
%                   **     **
%                      ***
%
%==========================================================================



