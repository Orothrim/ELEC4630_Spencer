
filename = input('Please enter the filename to search: ', 's')

[A, map] = imread(filename);

B = rgb2gray(A);
bottomRow= round(size(B,1)*7/8);
topRow = round(size(B,1)/8);
leftRow = round(size(B,2)/8);
rightRow = round(size(B,2)*7/8);

mask = false(size(B)); 
mask(topRow:bottomRow,leftRow:rightRow) = true; %Creates a mask for the outside rim of the image
B(~mask) = 1;  %Applies the mask to the image

Car1 = edge(B,'canny');
C = fspecial('gaussian', size(Car1), 1.0);


image = rgb2gray(imread('testing3.png'));
template = rgb2gray(imread('testing4.png'));
c = normxcorr2(template,image);
[ypeak, xpeak] = find(c==max(c(:)));
yoffset = ypeak-size(template,1);
xoffset = xpeak-size(template,2);
hfig = figure;
hAx = axes;
imshow(image,'Parent', hAx);
imrect(hAx, [xoffset, yoffset, size(template,2), size(template,1)]);
