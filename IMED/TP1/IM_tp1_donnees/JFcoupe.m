function [] = JFcoupe( volume, vue, vitesse )
  
    max = 0;
    if strcmp(vue,'axiale')
        max = size(volume,1);
    elseif strcmp(vue,'sagittale')
        max = size(volume,2);
    elseif strcmp(vue,'coronale')
        max = size(volume,3);
    end
    
    for i = 1:max
        if strcmp(vue,'axiale')
            imshow(uint16(squeeze(volume(i,:,:))),[])
        elseif strcmp(vue,'sagittale')
            imshow(uint16(squeeze(volume(:,i,:))),[])
        elseif strcmp(vue,'coronale')
            imshow(uint16(squeeze(volume(:,:,i))),[])
        end
        pause(vitesse);
    end

end

