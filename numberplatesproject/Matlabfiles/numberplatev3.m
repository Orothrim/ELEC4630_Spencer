close all; %close all previous figures

%when debug is 0 only the gray image with the locating box is shown
debug = 1;

%the most effective number plate template so far
binarytemplate = 'numberplatetemplate5.jpg';
edgetemplate = 'numberplatetemplate7.jpg';

%Can be used on all images to varying success levels
filename = input('Please enter the filename to search: ', 's');

%Checks if the image is grayscale before trying to convert it
if size(imread(filename),3) == 3
    originalimage = rgb2gray(imread(filename));
elseif size(imread(filename),3) == 1
    originalimage = imread(filename);
end

%%

%Unchanged image for final display
image = originalimage;

%Creates a border to reduce false positve likelyhood
bottomrow= round(size(image,1)*5/6);
toprow = round(size(image,1)/6);
leftrow = round(size(image,2)/6);
rightrow = round(size(image,2)*5/6);

%Masks the outside border of the image (as all the images have the number
%plate within the center 67% of the image
image = image(toprow:bottomrow,leftrow:rightrow); %Creates a mask for the outside rim of the image

%If debug is 0 this image does not need to be shown at the end in
%comparison to the original image
if debug
    image1 = image;
end

%%

%Does the same as a binary conversion
thresholdvalue = 100;
binaryimage = image > thresholdvalue;


%%

%Fills holes in the binary image in for blob detection, was discarded
%filledimage = imfill(binaryimage,'holes');
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

%%

%Reads in the template image, converting it into binary at the same time
templatesizes = [1.0 0.9 0.8 0.7 0.6 0.5];
resultxcorr = [0 0 0 0 0 0];

%Finds the scale of number plate which has the highest peak value to reduce
%the size of the image again
for k = 1:6
    template = im2bw(rgb2gray(imread(binarytemplate)));
    template = imresize(template, templatesizes(k));
    if and(size(image,1) > size(template,1), size(image,2) > size(template,2))
        binaryresults = normxcorr2(template,binaryimage);
        resultxcorr(k) = max(binaryresults(:));
    end
end

%Reapplies the template with the highest peak value
[~, xmax] = max(resultxcorr);
template = im2bw(rgb2gray(imread(binarytemplate)));
template = imresize(template, templatesizes(xmax));
binaryresults = normxcorr2(template,binaryimage);

%Creates a box for the most successful template matching
[ypeak, xpeak] = find(binaryresults==max(binaryresults(:)));
yoffset = ypeak-size(template,1);
xoffset = xpeak-size(template,2);
hfig = figure;
hAx = axes;


%Converts the normxcorr2 value into binary for image reduction
thresholdvalue = 0.2;
binarybwcorr = binaryresults > thresholdvalue;

%Finds the left-most and right-most successful template values to reduce
%the image size
[~, leftcolumn] = find(binarybwcorr, 1, 'first');
[~, rightcolumn] = find(binarybwcorr, 1, 'last');

%Finds the top-most successful template value to reduce the image size
for k = 1:(size(binarybwcorr,1))
    if ~isempty(find(binarybwcorr(k,:),1))
        toprow = k;
        break
    end
end

%Finds the bottom-most successful template value to reduce the image size
for k = (size(binarybwcorr,1)):1
    if ~isempty(find(binarybwcorr(k,:),1))
        bottomrow = k;
        break
    end
end

%Performs the secondary image masking to reduce the number of false
%positives
image = image(max(1,round(toprow - (size(template,1)/2))):...
    min(size(image,1),round(bottomrow + (size(template,1)/2))),...
    max(1,round(leftcolumn - (size(template,2)/2))):...
    min(size(image,2),round(rightcolumn + (size(template,2)/2))));

if debug
    image2 = image;
end


%%


edgeimage = imadjust(image, [0.2; 1], [0.1; 1]);
edgeimage = edge(edgeimage,'canny', 0.5);
edgeimage = mat2gray(bwdist(edgeimage, 'euclidean'));
edgeimage = imcomplement(20*edgeimage);


%imagefilter = fspecial('gaussian', [10 10], 10);
%edgeimage = imfilter(edgeimage,imagefilter);
%edgeresultsprevious = 

for k = 1:6

    template = im2bw(rgb2gray(imread(edgetemplate)));
    template = imresize(template, templatesizes(k));
    if and(size(image,1) > size(template,1), size(image,2) > size(template,2))
        edgeresults = normxcorr2(template,edgeimage);
        %if k > 1
        %    edgeresultstest = edgeresults.*edgeresultsprevious;

        %bottomrow= round(size(image,1)*5/6);
        %toprow = round(size(image,1)/6);
        %leftrow = round(size(image,2)/6);
        %rightrow = round(size(image,2)*5/6);

        %Masks the outside border of the image (as all the images have the number
        %plate within the center 67% of the image
        %mask = false(size(image)); 
        %mask(toprow:bottomrow,leftrow:rightrow) = true; %Creates a mask for the outside rim of the image
        %image(~mask) = 1;

        resultxcorr(k) = max(edgeresults(:));
        %edgeresultsprevious = edgeresults;
    end
end

%Reapplies the template with the highest peak value
[ymax, xmax] = max(resultxcorr);
template = im2bw(rgb2gray(imread(edgetemplate)));
template = imresize(template, templatesizes(xmax));
edgeresults = normxcorr2(template,edgeimage);

%Converts the normxcorr2 value into binary for image reduction
thresholdvalue = 0.18;
binaryedgecorr = edgeresults > thresholdvalue;

%Finds the left-most and right-most successful template values to reduce
%the image size
[leftrow, leftcolumn] = find(binaryedgecorr, 1, 'first');
[rightrow, rightcolumn] = find(binaryedgecorr, 1, 'last');

%Finds the top-most successful template value to reduce the image size
for k = 1:(size(binaryedgecorr,1))
    if ~isempty(find(binaryedgecorr(k,:),1))
        toprow = k;
        break
    end
end

%Finds the bottom-most successful template value to reduce the image size
for k = (size(binaryedgecorr,1)):1
    if ~isempty(find(binaryedgecorr(k,:),1))
        bottomrow = k;
        break
    end
end

%Performs the tertiary image masking to reduce the number of false
%positives
image = image(max(1,round(toprow - 29/2)):...
    min(round(bottomrow + 29/2),size(image,1)),...
    max(1,round(leftcolumn - 74/2)):...
    min(size(image,2),round(rightcolumn + 74/2)));

if debug
    image3 = image;
end

%%


%If debug is 1 displays each of the images created
if debug
    imshow(image, 'Parent', hAx);
    figure, imshow(image1);
    figure, imshow(image2);
    figure, imshow(image3);
    figure, imshow(template);
    %imrect(hAx, [xoffset, yoffset, size(template,2), size(template,1)]);
else
    imshow(image,'Parent', hAx);
    %imrect(hAx, [xoffset, yoffset, size(template,2), size(template,1)]);
end