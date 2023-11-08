import numpy as np
import cv2
import math
import os
import matplotlib.pyplot as plt

def flatten(mat):
    flatten_list = list()
    for i in range(0, mat.shape[0]):
        for j in range(0, mat.shape[1]):
            flatten_list.append(mat[i, j])
    return flatten_list

def zero_padding(img, padding_size):
    img_channel = len(img.shape)
    if img_channel == 2 : img_channel += 1
    pad_img = np.zeros((img.shape[0] + 2 * padding_size, img.shape[1] + 2 * padding_size, img_channel - 2), dtype=float)
    for i in range(0, img.shape[0]):
        for j in range(0, img.shape[1]):
            pad_img[i + padding_size, j + padding_size] = img[i, j]
    return pad_img

def getGrayImg(img):
    img_gray = np.zeros((img.shape[0], img.shape[1]), dtype=np.uint8)
    for i in range(0, img.shape[0]):
        for j in range(0, img.shape[1]):
            img_gray[i, j] = 0.299 * img[i, j, 2] + 0.587 * img[i, j, 1] + 0.114 * img[i, j, 0]
    return img_gray

def convolution(img, kernel, stride):
    kernel = np.rot90(kernel, 2)
    padding_size = kernel.shape[0]//2
    kernel_size = kernel.shape[0]
    img_pad = zero_padding(img, padding_size)
    img_conv = np.zeros(((img.shape[0] + 2 * padding_size - kernel_size)//stride + 1, (img.shape[1] + 2 * padding_size - kernel_size)//stride + 1), dtype=np.uint8)
    for i in range(0, img_conv.shape[0]):
        for j in range(0, img_conv.shape[1]):
            if(i * stride + kernel_size <= img_pad.shape[0] and j * stride + kernel_size <= img_pad.shape[1]):
                conv_mat = img_pad[i * stride : i * stride + kernel_size, j * stride : j * stride + kernel_size] * kernel
                sum = conv_mat.sum()
                if sum > 255:
                    sum = 255
                elif sum < 0:
                    sum = 0
                img_conv[i, j] = math.floor(sum)
    return img_conv

def quick_sort(array, left, right):
    if left>=right : return
    k = (left + right)//2
    tmp = array[k]
    array[k] = array[right]
    array[right] = tmp
    pivot = array[right]

    i = left
    for j in range(left, right):
        if array[j] <= pivot:
            tmp = array[i]
            array[i] = array[j]
            array[j] = tmp
            i += 1

    tmp = array[i]
    array[i] = array[right]
    array[right] = tmp

    quick_sort(array, left, i - 1)
    quick_sort(array, i + 1, right)

def median_filter(img, kernel_size):
    padding_size = kernel_size//2
    img_pad = zero_padding(img, padding_size)
    img_mf = np.zeros((img.shape[0], img.shape[1]), dtype=np.uint8)
    for i in range(0, img.shape[0]):
        for j in range(0, img.shape[1]):
            kernel_list = flatten(img_pad[i : i + kernel_size, j : j + kernel_size])
            quick_sort(kernel_list, 0, kernel_size**2 - 1)
            img_mf[i, j] = kernel_list[math.ceil((kernel_size**2)/2)]
    return img_mf

def statistics(img):
    stat_list = [0 for _ in range(256)]
    sum_of_list = 0
    for i in range(0, img.shape[0]):
        for j in range(0, img.shape[1]):
            stat_list[img[i, j]] += 1
            sum_of_list += 1
    return stat_list


def create_histogram(statistics_list, titleName, path):
    plt.title(titleName)
    plt.bar(np.arange(256), statistics_list, width=1)
    plt.savefig(path)
    plt.cla()
            
            

if not(os.path.isdir(os.getcwd() + '/result_img')):
    os.makedirs(os.getcwd() + '/result_img')

img1 = cv2.imread('test_img/noise1.png')
img2 = cv2.imread('test_img/noise2.png')

img1 = getGrayImg(img1)
img2 = getGrayImg(img2)

create_histogram(statistics(img1), 'noise1_his', 'result_img/noise1_his.png')
create_histogram(statistics(img2), 'noise2_his', 'result_img/noise2_his.png')

kernel_size = 7

mean_filter = np.zeros((kernel_size, kernel_size, 1), dtype=float)
for i in range(0, mean_filter.shape[0]):
    for j in range(0, mean_filter.shape[1]):
        mean_filter[i, j] = 1/(kernel_size**2)

img1_uf = convolution(img1, mean_filter, 1)
img2_uf = convolution(img2, mean_filter, 1)

cv2.imwrite('result_img/noise1_q1.png', img1_uf)
cv2.imwrite('result_img/noise2_q1.png', img2_uf)

create_histogram(statistics(img1_uf), 'noise1_q1 _his', 'result_img/noise1_q1 _his.png')
create_histogram(statistics(img2_uf), 'noise2_q1 _his', 'result_img/noise2_q1 _his.png')

img1_mf = median_filter(img1, 7)
img2_mf = median_filter(img2, 7)

cv2.imwrite('result_img/noise1_q2.png', img1_mf)
cv2.imwrite('result_img/noise2_q2.png', img2_mf)

create_histogram(statistics(img1_mf), 'noise1_q2 _his', 'result_img/noise1_q2 _his.png')
create_histogram(statistics(img2_mf), 'noise2_q2 _his', 'result_img/noise2_q2 _his.png')