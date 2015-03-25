templatesizes = [1.0 0.9 0.8 0.7 0.6 0.5];
resultxcorr = [0 0 0 0 0 0];
templateslist = ['Atemp.jpg', 'Btemp.jpg', 'Ctemp.jpg', 'Dtemp.jpg', 'Etemp.jpg',...
    'Ftemp.jpg', 'Htemp.jpg', 'Ktemp.jpg', 'Ltemp.jpg', 'Mtemp.jpg',...
    'Ptemp.jpg', 'Rtemp.jpg', 'Stemp.jpg', 'Ttemp.jpg', 'Wtemp.jpg',...
    'Ytemp.jpg', '1temp.jpg', '2temp.jpg', '3temp.jpg', '4temp.jpg',...
    '5temp.jpg', '6temp.jpg', '7temp.jpg']

symbolsfound = 0;
peakthreshold = 0.5;

%Finds the scale of number plate which has the highest peak value to reduce
%the size of the image again
for i = 1:23
    if symbolsfound < 6
        for k = 1:6
            template = im2bw(rgb2gray(imread(templateslist(i))));
            template = imresize(template, templatesizes(k));
            if and(size(image,1) > size(template,1), size(image,2) > size(template,2))
                binaryresults = normxcorr2(template,binaryimage);
                if max(binaryresults(:)) > peakthreshold
                resultxcorr(k) = max(binaryresults(:));
            end
        end
    else 
        break
    end
end