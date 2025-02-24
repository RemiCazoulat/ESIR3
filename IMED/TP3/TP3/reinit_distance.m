function Phi=reinit_distance(Phi_old)
Bin=Phi_old>0;
Dist_int = (Bin==0).*(-bwdist(Bin)+.5);
Dist_ext = (Bin>0).*(bwdist(Bin==0)-.5);
Phi = Dist_int + Dist_ext;

