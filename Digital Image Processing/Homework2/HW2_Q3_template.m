clear; clc; close all;

testimg1 = imread('circuit.tif');
testimg2 = imread('face.tif');
testimg3 = imread('moon.tif');

%% Show (a)
Q3_show(testimg1, 'medianfilter(img, 5)', 'medianfilterred image')
Q3_show(testimg2, 'medianfilter(img, 5)', 'medianfilterred image')

%% Show (b)
Q3_show(testimg1, 'gaussfilter(img, 1, 5, 10)', 'gaussfilterred image')
Q3_show(testimg2, 'gaussfilter(img, 1, 5, 10)', 'gaussfilterred image')

%% Show (c)
Q3_show(testimg3, 'highboostfilter(img, 2, 5)', 'highboostfiltered image')

%%
function O = medianfilter(I, filter_size)
    % O is filtered image, I is original image
    [m, n] = size(I);
    if(m >= filter_size && n >= filter_size)
        padding_size = floor(filter_size/2);
        Ip = padarray(im2double(I), [padding_size, padding_size], 0);
        O = zeros(m,n);
        for i = 1 : m
            for j = 1 : n
                targetPixels = Ip(i : (i + filter_size - 1), j : (j + filter_size - 1));
                O(i, j) = median(targetPixels(:));
            end
        end
    end
    % your implementation here ...
end

function O = gaussfilter(I, K, filter_size, sigma)
    % O is filtered image, I is original image
    kernel = zeros(filter_size, filter_size);
    tW = 0;
    for i = 1 : filter_size
        for j = 1 : filter_size
            x = i - ceil(filter_size / 2);
            y = i - ceil(filter_size / 2);
            w = K * exp( - (x^2 + y^2) / (2 * sigma^2));
            kernel(i, j) = w;
            tW = tW + w;
        end
    end
    kernel = kernel/tW;

    [m, n] = size(I);
    O = zeros(m, n);
    padding_size = floor(filter_size / 2);
    Ip = padarray(im2double(I), [padding_size, padding_size], 0);
    for i = 1 : m
        for j = 1 : n
            sum = 0;
            for x = 0 : (filter_size - 1)
                for y = 0 : (filter_size - 1)
                    if(i + x <= m + 2 * padding_size && j + y <= n + 2 * padding_size)
                        sum = sum + kernel(x + 1, y + 1) * Ip(i + x, j + y);
                    end
                end
            end
            O(i, j) = sum;
        end
    end
    % your implementation here ...
end

function O = highboostfilter (I, A, filter_size)
    % O is filtered image, I is original image
    gImg = gaussfilter(I, 1, filter_size, 10);
    detailMat = im2double(I) - gImg;
    O = im2double(I) + A * detailMat;
    % your implementation here ...
end

function Q3_show(img, string, titlestr)
    figure()
    result = eval(string);
    subplot(1,2,1); imshow(img); title("Original image");
    subplot(1,2,2); imshow(result); title(titlestr);
end
