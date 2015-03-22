filename = input('Please enter the filename to search: ', 's')

if size(imread(filename),3) == 3 
    image = rgb2gray(imread(filename));
elseif size(imread(filename),3) == 1
    image = imread(filename);
end

bottomRow= round(size(image,1)*7/8);
topRow = round(size(image,1)/8);
leftRow = round(size(image,2)/8);
rightRow = round(size(image,2)*7/8);

mask = false(size(image)); 
mask(topRow:bottomRow,leftRow:rightRow) = true; %Creates a mask for the outside rim of the image
image(~mask) = 1;

template = rgb2gray(imread('numberplatetemplate1.jpg'));
c = normxcorr2(template,image);
[ypeak, xpeak] = find(c==max(c(:)));
yoffset = ypeak-size(template,1);
xoffset = xpeak-size(template,2);
hfig = figure;
hAx = axes;
imshow(image,'Parent', hAx);
imrect(hAx, [xoffset, yoffset, size(template,2), size(template,1)]);
