function y = convolution_sym(x,h)

n = size(x);
p = size(h);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% symmetric boundary conditions

d1 = floor( p/2 );  % padding before
d2 = p-d1-1;            % padding after

%%%%%%%%%%%%%%%%%%%%%%%%%%%% 2D %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% double symmetry
xx = x;
xx = [ xx(d1(1):-1:1,:); xx; xx(end:-1:end-d2(1)+1,:) ];
xx = [ xx(:,d1(2):-1:1), xx, xx(:,end:-1:end-d2(2)+1) ];

y = conv2(xx,h);
y = y( (2*d1(1)+1):(2*d1(1)+n(1)), (2*d1(2)+1):(2*d1(2)+n(2)) );

end