import numpy as np
import cv2
import os

def getGrayImg(img):
    img_gray = np.zeros((img.shape[0], img.shape[1], 1), np.uint8)
    for i in range(0, img.shape[0]):
        for j in range(0, img.shape[1]):
            img_gray[i, j] = 0.299 * img[i, j, 2] + 0.587 * img[i, j, 1] + 0.114 * img[i, j, 0]
    return img_gray

def convolution(img, kernel, stride):
    kernel = np.rot90(kernel, 2)
    padding_size = kernel.shape[0]//2
    kernel_size = kernel.shape[0]
    img_pad = np.pad(img, ((padding_size, padding_size), (padding_size, padding_size), (0, 0)), mode='constant')
    img_conv = np.zeros(((img.shape[0] + 2 * padding_size - kernel_size)//stride + 1, (img.shape[1] + 2 * padding_size - kernel_size)//stride + 1), dtype=int)
    for i in range(0, img_conv.shape[0]):
        for j in range(0, img_conv.shape[1]):
            if(i * stride + kernel_size < img_pad.shape[0] and j * stride + kernel_size < img_pad.shape[1]):
                conv_mat = img_pad[i * stride : i * stride + kernel_size, j * stride : j * stride + kernel_size] * kernel
                img_conv[i, j] = conv_mat.sum()
    return np.clip(img_conv, 0, 255).astype(np.uint8)

def pooling(img, kernel_size, stride):
    img_pool = np.zeros(((img.shape[0] - kernel_size) // stride + 1, (img.shape[1] - kernel_size) // stride + 1, 1), np.uint8)
    for i in range(0, img_pool.shape[0]):
        for j in range(0, img_pool.shape[1]):
            img_pool[i, j] = np.max(img[i * stride : i * stride + kernel_size, j * stride : j * stride + kernel_size])
    return img_pool

def binarization(img):
    return np.where(img >= 128, 255, 0).astype(np.uint8)

if not(os.path.isdir(os.getcwd() + '/result_img')):
    os.makedirs(os.getcwd() + '/result_img')
img1 = cv2.imread('test_img/taipei101.png')
img2 = cv2.imread('test_img/aeroplane.png')

img1_gray = getGrayImg(img1)
img2_gray = getGrayImg(img2)

#cv2.imshow('taipei101_gray', img1_gray)
#cv2.imshow('aeroplane_gray', img2_gray)
#cv2.waitKey(0)
cv2.imwrite('result_img/taipei101 _Q1.png', img1_gray)
cv2.imwrite('result_img/aeroplane _Q1.png', img2_gray)

kernel = np.array([[[-1], [-1], [-1]],[[-1], [8], [-1]],[[-1], [-1], [-1]]])

img1_conv = convolution(img1_gray, kernel, 1)
img2_conv = convolution(img2_gray, kernel, 1)

#cv2.imshow('taipei101_conv', img1_conv)
#cv2.imshow('aeroplane_conv', img2_conv)
#cv2.waitKey(0)
cv2.imwrite('result_img/taipei101 _Q2.png', img1_conv)
cv2.imwrite('result_img/aeroplane _Q2.png', img2_conv)

img1_pool = pooling(img1_conv, 2, 2)
img2_pool = pooling(img2_conv, 2, 2)

#cv2.imshow('taipei101_pool', img1_pool)
#cv2.imshow('aeroplane_pool', img2_pool)
#cv2.waitKey(0)
cv2.imwrite('result_img/taipei101 _Q3.png', img1_pool)
cv2.imwrite('result_img/aeroplane _Q3.png', img2_pool)

img1_bin = binarization(img1_pool)
img2_bin = binarization(img2_pool)

#cv2.imshow('taipei101_binary', img1_bin)
#cv2.imshow('aeroplane_binary', img2_bin)
#cv2.waitKey(0)
cv2.imwrite('result_img/taipei101 _Q4.png', img1_bin)
cv2.imwrite('result_img/aeroplane _Q4.png', img2_bin)

#cv2.destroyAllWindows()