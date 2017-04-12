% Load data
matrix = csvread('tests/sample.csv');
labels = matrix(:,1);
data = matrix(:,2:end);

% Train SVM model
svmm = fitcsvm(data, labels)

% Try it out!
newX = (sign(randn(1,8191))+1)/2;
[l,s]=predict(svmm, newX)

newX = (sign(randn(1,8191) + 0.5)+1)/2;
[l,s]=predict(svmm, newX)

newX = (sign(randn(1,8191) - 0.5)+1)/2;
[l,s]=predict(svmm, newX)

% TODO: improved plot
% Plot support vectors and data points
sv = svmm.SupportVectors;
figure
gscatter(sum(data(:,1:end/2),2),sum(data(:,end/2:end),2),y)
hold on
plot(sum(sv(:,1:end/2),2),sum(sv(:,end/2:end),2),'ko','MarkerSize',10)
legend('ham','spam','Support Vector')
hold off

% Save as PNG
print('SVM', '-dpng')
