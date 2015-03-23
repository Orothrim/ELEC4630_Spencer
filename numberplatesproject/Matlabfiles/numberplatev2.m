close all;

debug = 1;
tfilename = 'numberplatetemplate5.jpg';

filename = input('Please enter the filename to search: ', 's')

if size(imread(filename),3) == 3 
    originalimage = rgb2gray(imread(filename));
elseif size(imread(filename),3) == 1
    originalimage = imread(filename);
end

image = originalimage;

bottomrow= round(size(image,1)*5/6);
toprow = round(size(image,1)/6);
leftrow = round(size(image,2)/6);
rightrow = round(size(image,2)*5/6);

mask = false(size(image)); 
mask(toprow:bottomrow,leftrow:rightrow) = true; %Creates a mask for the outside rim of the image
image(~mask) = 1;

if debug == 1
    image1 = image;
end

thresholdvalue = 100;
binaryimage = image > thresholdvalue;
filledimage = imfill(binaryimage,'holes');
%figure, imshow(filledimage)

%symbols = [2temp.jpg 7temp.jpg

%for k = 1 : 4
%    stemplate = im2bw(rgb2gray(imread('2temp.jpg')));
%    c[k] = normxcorr2(stemplate,binaryimage);
%    if c[k] > 0.4
%        [ypeak, xpeak] = find(c==max(c(:)));
%        yoffset = ypeak-size(stemplate,1);
%        xoffset = xpeak-size(stemplate,2);
%end
%mask = false(size(image)); 
%mask(ypeak-(size(stemplate,1)*2):ypeak+(size(stemplate,1)*2),xpeak-(size(stemplate,2)*7):xpeak+(size(stemplate,2)*7)) = true; %Creates a mask for the outside rim of the image
%image(~mask) = 1;

template = im2bw(rgb2gray(imread(tfilename)));

templatesizes = [1.0 0.95 0.9 0.85];

for k = 1:4
    template = im2bw(rgb2gray(imread(tfilename)));
    template = imresize(template, templatesizes(k));
    c = normxcorr2(template,binaryimage);
    resultxcorr(k) = max(c(:));
end

[ymax, xmax] = max(resultxcorr);
template = im2bw(rgb2gray(imread(tfilename)));
template = imresize(template, templatesizes(xmax));
c = normxcorr2(template,binaryimage);

thresholdvalue = 0.15;
binarycorr = c > thresholdvalue;
[leftrow, leftcolumn] = find(binarycorr, 1, 'first');
[rightrow, rightcolumn] = find(binarycorr, 1, 'last');

mask = false(size(image)); 
mask(toprow:bottomrow,leftcolumn - size(template,2):rightcolumn + size(template,2)) = true; %Creates a mask for the outside rim of the image
image(~mask) = 1;

[ypeak, xpeak] = find(c==max(c(:)));
yoffset = ypeak-size(template,1);
xoffset = xpeak-size(template,2);
hfig = figure;
hAx = axes;

if debug == 1
    imshow(image1)
    figure, imshow(image)
    figure, imshow(template)
    imrect(hAx, [xoffset, yoffset, size(template,2), size(template,1)]);
else
    imshow(image,'Parent', hAx);
    imrect(hAx, [xoffset, yoffset, size(template,2), size(template,1)]);
end