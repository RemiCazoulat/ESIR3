function M = floute(M, sigma)
n = max(size(M));

eta = 4;
p = round((sigma*eta)/2)*2+1;
p = min(p,round(n/2)*2-1);

A = [1 1];
h = fspecial('gaussian', p*A, sigma/4);
M = convolution_sym(M, h);