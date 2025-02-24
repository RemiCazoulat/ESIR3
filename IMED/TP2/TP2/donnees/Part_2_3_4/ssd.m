function [ result ] = ssd( I,J )

I = double(I(:));
J = double(J(:));
result = sum((I-J).^2);
end

