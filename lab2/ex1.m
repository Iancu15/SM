n = 1:0.01:32;
S = (n + 3) ./ 4;
E = (n + 3) ./ (4 .* n);
R = (n + log2(n)) ./ n;
U = ((n + 3) .* (n + log2(n))) ./ (4 .* (n.^2));
Q = ((n + 3).^2) ./ (16 .* (n + log2(n)));

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