% quicksort paralel: https://www.codeproject.com/articles/42311/parallel-quicksort-using-mpi-performance-analysis
% la Time Complexity Analysis
% consider numarul de procesoare egal cu dimensiunea inputului si egale cu
% n
% o problema totusi e ca pentru numerele apropiate de puteri ale lui 2 gen
% 2.1, 2.2, 4.1, 4.2 etc. se creeaza outlieri, cel mai clar vazandu-se
% la 2.1 pentru care apare un numar foarte mare pe grafic si il strica
% asa ca am decis sa las step-ul 1

n = 1:32;
T_1 = n .* log(n);
O_1 = n .* log(n);
T_n = (n ./ nextpow2(n)) .* log((n ./ nextpow2(n)));
T_n(1) = 1;
O_n = n .* log(n);

S = T_1 ./ T_n;
E = S ./ n;

% din cauza outlierilor mentionat anterior eficienta ajunge la valori
% ciudate de peste 1, asa ca am normalizat impartind la 2
E = E / 2;

% pentru ca am considerat O_n si O_1 echivalente, redundanta este 1 si
% implicit utilizarea este echivalenta cu eficienta
R = O_n ./ O_1;
U = R .* E;
Q = (S .* E) ./ R;

figure
ax1 = nexttile;
plot(ax1, n, S)

hold on
plot(ax1, n, R)

hold on
plot(ax1, n, Q)

title(ax1, 'Plotarea factorilor S, R si Q la aceeasi scara normala')
xlabel('Numarul de procesoare')
ylabel(ax1, 'Rezultatul functiilor')
legend('S', 'R', 'Q')

ax2 = nexttile;
plot(ax2, n, E)

hold on
plot(ax2, n, U)

title(ax2, 'Plotarea factorilor E si U la aceeasi scara normala')
xlabel('Numarul de procesoare')
ylabel(ax2, 'Rezultatul functiilor')
legend('E', 'U')