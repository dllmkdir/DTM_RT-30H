
p=[0 0; 1023 127];
m=(p(1,2)-p(2,2))/(p(1,1)-p(2,1))
b=p(1,2)-m*p(1,1)
f=@(x) m*x+b %linear equation
f1=@(x) 42.1884*log10(x+1) %logarithmic equation
f2=@(x) exp(x/210.8395)-1 %exponential equation
f3=@(x) (1/(1+exp(-(x-511.5)/(95))))*127%sigmoid equation
f4= @(x) (atan((x-511.5)/50)+pi/2)*(127/pi)%atan equation
f5= @(x) (tanh((x-511.5)/245)+1)*(127/2)%tanh equation
hold on
axis on
fplot(f,'Linewidth',2)
fplot(f1,'Linewidth',2)
fplot(f2,'Linewidth',2)
fplot(f3,'Linewidth',2)
fplot(f4,'Linewidth',2)
fplot(f5,'Linewidth',2)
legend('Linear','Logarithmic','Exponential','Sigmoid','Tan^-1(x)','Tanh(x)')%labels
xlim([0 1023])
ylim([0 127])
grid on
hold off