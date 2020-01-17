clc
clear
close all

d=0:0.01:20;

beta = 0.6;

T=1/20;
Rf = zeros(1,size(d,2));
for i=1:size(d,2)
   if (d(i) < ((1-beta)/(2*T))) 
       Rf(i) = 1;
   elseif (d(i) < ((1+beta)/(2*T))) 
       Rf(i) = 0.5*(1 + cos((pi*T/beta)*(d(i)-((1-beta)/(2*T)))));
   else
       Rf(i) = 0;
   end
end
plot(d,Rf);
