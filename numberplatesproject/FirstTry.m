
load('PositiveInstances.mat');
negativeFolder = fullfile(matlabroot, 'toolbox', 'nope');
trainCascadeObjectDetector('numberplatedetector.xml', positiveInstances, negativeFolder, 'FalseAlarmRate', 0.005, 'NumCascadeStages', 6)
detector = vision.CascadeObjectDetector('numberplatedetector.xml');
img = imread('bluecar.jpg');
bbox = step(detector, img);
disp(bbox)
detectedImg = insertObjectAnnotation(img, 'rectangle', bbox, 'cup');
figure; imshow(detectedImg);


