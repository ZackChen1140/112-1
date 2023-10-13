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
        Ip = padarray(I, [padding_size, padding_size], 0);
        O = zeros(m,n);
        for i = 1 : m
            for j = 1 : n
                targetPixels = Ip(i : (i + filter_size - 1), j : (j + filter_size - 1));
                Ip(i + floor(filter_size/2), j + floor(filter_size/2)) = median(targetPixels(:));
            end
        end
        O = Ip(1+padding_size:end-padding_size, 1+padding_size:end-padding_size);
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
            w = exp( - (x^2 + y^2) / (2 * sigma^2));
            kernel(i, j) = w;
            if(i >= ceil(filter_size / 2) - K && i <= ceil(filter_size / 2) + K && j >= ceil(filter_size / 2) - K && j <= ceil(filter_size / 2) + K)
                tW = tW + w;
            end
        end
    end
    kernel = kernel/tW;

    [m, n] = size(I);
    Ip = padarray(I, [K, K], 0);
    for i = 1 : m
        for j = 1 : n
            sum = 0;
            for x = 0 : 2 * K
                for y = 0 : 2 * K
                    if(i + x <= m + 2 * K && j + y <= n + 2 * K)
                        sum = sum + kernel(x - K + ceil(filter_size / 2), y - K + ceil(filter_size / 2)) * Ip(i + x, j + y);
                    end
                end
            end
            Ip(i + K, j + K) = sum;
        end
    end
    O = Ip(1 + K : end - K, 1 + K : end - K);
    % your implementation here ...
end

function O = highboostfilter (I, A, filter_size)
    % O is filtered image, I is original image
    gImg = gaussfilter(I, 2, filter_size, 10);
    detailMat = I - gImg;
    O = I + A * detailMat;
    % your implementation here ...
end

function Q3_show(img, string, titlestr)
    figure()
    result = eval(string);
    subplot(1,2,1); imshow(img); title("Original image");
    subplot(1,2,2); imshow(result); title(titlestr);
end