//Upsampling a given Image. 
//input image size: 128*128, gray scale.
//upsampling to 512*512
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<windows.h>
int main(int argc, char* argv[]) {
    BITMAPFILEHEADER bmpFile;
    BITMAPINFOHEADER bmpInfo;
    FILE* inputFile = NULL;
    inputFile = fopen("test.bmp", "rb");

    fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile);
    fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile);



    int width = bmpInfo.biWidth;
    int height = bmpInfo.biHeight;
    int size = bmpInfo.biSizeImage;
    int bitCnt = bmpInfo.biBitCount;
    int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;

    int ratio = 2;
    int width2 = bmpInfo.biWidth << ratio;
    int height2 = bmpInfo.biHeight << ratio;
    int stride2 = (((bitCnt / 8) * width2) + 3) / 4 * 4;
    int size2 = stride2 * height2;
    unsigned char* inputImg = NULL, * outputImg = NULL;
    inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
    outputImg = (unsigned char*)calloc(size2, sizeof(unsigned char));
    double* Y1 = (double*)calloc(width * height, sizeof(double));
    double* Y2 = (double*)calloc(width2 * height2, sizeof(double));
    fread(inputImg, sizeof(unsigned char), size, inputFile);
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Y1[j * width + i] = inputImg[j * stride + 3 * i];
        }
    }
    /*
   설명 기준
   -> downsampling 된 이미지에서 하나의 픽셀을 기준으로 설명하겠다. 하나의 픽셀이 upsampling 되어 16개의 픽셀이 되므로, 하나의 픽셀이 
    a b c d
    e f g h
    i j k l
    m n o p 
    와 같이 upsampling 되었다고 가정하고, 위의 a~p를 이용해 해당 코드가 어떤 픽셀들을 나타내는지 설명하겠다.
    */
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
    for (int j = 0; j < height; j++) {//128*128로 downsampling 한 픽셀들을 먼저 넣어주기. (a 픽셀 자리)
        for (int i = 0; i < width; i++) {
            Y2[(4 * j) * width2 + 4 * i] = Y1[j * width + i];
        }
    }
    for (int j = 0; j < height; j++) {//위에서 a 자리에 있는 값들은 다 넣어주었으므로, 좌우에 이웃한 a들의 평균을 이용해 c의 자리 계산.
        for (int i = 0; i < width; i++) {
            if (i == width - 1) {
                Y2[(4 * j) * width2 + 4 * i + 2] = Y1[j * width + i];//맨 끝의 열은 오른쪽에 a가 없으므로, 그냥 현재의 a값을 c 자리에 넣어준다.
            }
            else {
                Y2[(4 * j) * width2 + 4 * i + 2] = Y1[j * width + i] * 0.5 + Y1[j * width + i + 1] * 0.5;
            }
        }
    }

    for (int j = 0; j < height; j++) {//위,아래 이웃한 a들의 평균을 이용해 i 자리의 픽셀값 계산.
        for (int i = 0; i < width; i++) {
            if (j == height - 1) {
                Y2[(4 * j + 2) * width2 + 4 * i] = Y1[j * width + i];//맨 아래 행은 더 이상 a값이 없으므로, 그냥 현재의 a값을 i 자리에 넣어준다.
            }
            else {
                Y2[(4 * j + 2) * width2 + 4 * i] = Y1[j * width + i] * 0.5 + Y1[(j + 1) * width + i] * 0.5;
            }
        }
    }

    for (int j = 0; j < height; j++) {//양 옆의 i들의 평균을 이용해 k 자리의 픽셀 값 계산 
        for (int i = 0; i < width; i++) {
            if (i == width - 1) {
                Y2[(4 * j + 2) * width2 + 4 * i + 2] = Y1[j * width + i];//위의 이유와 같이 boundary processing 한 것.
            }
            else {
                Y2[(4 * j + 2) * width2 + 4 * i + 2] = Y2[(4 * j + 2) * width2 + 4 * i] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * (i + 1)] * 0.5;
            }
        }
    }
    for (int j = 0; j < height; j++) {// 위의 b,d 자리의 픽셀 계산.(가로로 이웃한 픽셀들을 이용)-b,d 자리 모두 가장 가까운 a와 c의 평균을 이용해 픽셀 값을 계산한다.
        for (int i = 0; i < width; i++) {
            if (i == width - 1) {// 위의 이유와 같이 boundary processing 한 것.
                Y2[(4 * j) * width2 + 4 * i + 1] = Y1[j * width + i];//b 자리 boundary processing
                Y2[(4 * j) * width2 + 4 * i + 3] = Y1[j * width + i];//d 자리 boundary processing
            }
            else {
                Y2[(4 * j) * width2 + 4 * i + 1] = Y1[j * width + i] * 0.5 + Y2[(4 * j) * width2 + 4 * i + 2] * 0.5;//b 자리
                Y2[(4 * j) * width2 + 4 * i + 3] = Y1[j * width + i + 1] * 0.5 + Y2[(4 * j) * width2 + 4 * i + 2] * 0.5;//d 자리
            }
        }
    }
    for (int j = 0; j < height; j++) {//위의 j,l 자리의 픽셀을 계산.(가로로 이웃한 픽셀들을 이용)- j,l 자리 모두 가까운 i,k 평균을 이용해 픽셀 값을 계산한다.
        for (int i = 0; i < width; i++) {
            if (i == width - 1) {
                Y2[(4 * j + 2) * width2 + 4 * i + 1] = Y1[j * width + i];//j 자리 boundary processing
                Y2[(4 * j + 2) * width2 + 4 * i + 3] = Y1[j * width + i];//l 자리 boundary processing
            }
            else {
                Y2[(4 * j + 2) * width2 + 4 * i + 1] = Y2[(4 * j + 2) * width2 + 4 * i] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * i + 2] * 0.5;//j자리 
                Y2[(4 * j + 2) * width2 + 4 * i + 3] = Y2[(4 * j + 2) * width2 + 4 * i + 2] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * (i + 1)] * 0.5;//l 자리 
            }
        }
    }

    for (int j = 0; j < height; j++) {//위와 아래의 이웃한 i,a값의 평균을 이용해 (세로로 이웃한 픽셀들을 이용) e,m 자리의 픽셀값을 계산.
        for (int i = 0; i < width; i++) {
            if (j == height - 1) {
                Y2[(4 * j + 1) * width2 + 4 * i] = Y1[j * width + i];//e자리 boundary processing
                Y2[(4 * j + 3) * width2 + 4 * i] = Y1[j * width + i];//m자리 boundary processing
            }
            else {
                Y2[(4 * j + 1) * width2 + 4 * i] = Y1[j * width + i] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * i] * 0.5;//e자리
                Y2[(4 * j + 3) * width2 + 4 * i] = Y1[(j + 1) * width + i] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * i] * 0.5;//m자리
            }
        }
    }


    for (int j = 0; j < height; j++) {//남은 f g h,n,o,p 자리의 픽셀들을 계산.
        for (int i = 0; i < width; i++) {
            if (j == height - 1) {//순서대로 f,g,h,n,o,p 자리 boundary processing
                Y2[(4 * j + 1) * width2 + 4 * i + 1] = Y1[j * width + i];
                Y2[(4 * j + 1) * width2 + 4 * i + 2] = Y1[j * width + i];
                Y2[(4 * j + 1) * width2 + 4 * i + 3] = Y1[j * width + i];
                Y2[(4 * j + 3) * width2 + 4 * i + 1] = Y1[j * width + i];
                Y2[(4 * j + 3) * width2 + 4 * i + 2] = Y1[j * width + i];
                Y2[(4 * j + 3) * width2 + 4 * i + 3] = Y1[j * width + i];

            }
            else {
                Y2[(4 * j + 1) * width2 + 4 * i + 1] = Y2[(4 * j) * width2 + 4 * i + 1] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * i + 1] * 0.5;//위 아래로 이웃한 b,j의 평균을 이용해 f 자리의 픽셀값 계산.
                Y2[(4 * j + 1) * width2 + 4 * i + 2] = Y2[(4 * j) * width2 + 4 * i + 2] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * i + 2] * 0.5;//위 아래로 이웃한 c,k의 평균을 이용해 g 자리의 픽셀값 계산.
                Y2[(4 * j + 1) * width2 + 4 * i + 3] = Y2[(4 * j) * width2 + 4 * i + 3] * 0.5 + Y2[(4 * j + 2) * width2 + 4 * i + 3] * 0.5;//위 아래로 이웃한 d,l의 평균을 이용해 h 자리의 픽셀값 계산.
                Y2[(4 * j + 3) * width2 + 4 * i + 1] = Y2[(4 * j + 2) * width2 + 4 * i + 1] * 0.5 + Y2[(4 * (j + 1)) * width2 + 4 * i + 1] * 0.5;//위 아래로 이웃한 j,b의 평균을 이용해 n 자리의 픽셀값 계산.
                Y2[(4 * j + 3) * width2 + 4 * i + 2] = Y2[(4 * j + 2) * width2 + 4 * i + 2] * 0.5 + Y2[(4 * (j + 1)) * width2 + 4 * i + 2] * 0.5;//위 아래로 이웃한 k,c의 평균을 이용해 o 자리의 픽셀값 계산.
                Y2[(4 * j + 3) * width2 + 4 * i + 3] = Y2[(4 * j + 2) * width2 + 4 * i + 3] * 0.5 + Y2[(4 * (j + 1)) * width2 + 4 * i + 3] * 0.5;//위 아래로 이웃한 l,d의 평균을 이용해 p 자리의 픽셀값 계산.
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for (int j = 0; j < height2; j++) {
        for (int i = 0; i < width2; i++) {
            outputImg[j * stride2 + 3 * i + 2] = (unsigned char)(Y2[j * width2 + i] > 255 ? 255 : (Y2[j * width2 + i] < 0 ? 0 : Y2[j * width2 + i]));
            outputImg[j * stride2 + 3 * i + 1] = (unsigned char)(Y2[j * width2 + i] > 255 ? 255 : (Y2[j * width2 + i] < 0 ? 0 : Y2[j * width2 + i]));
            outputImg[j * stride2 + 3 * i + 0] = (unsigned char)(Y2[j * width2 + i] > 255 ? 255 : (Y2[j * width2 + i] < 0 ? 0 : Y2[j * width2 + i]));
        }
    }

    FILE* outputFile = fopen("OutputImg.bmp", "wb");
    bmpInfo.biWidth = width2;
    bmpInfo.biHeight = height2;
    bmpInfo.biSizeImage = size2;
    bmpFile.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size2;
    fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
    fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
    fwrite(outputImg, sizeof(unsigned char), size2, outputFile);

    free(outputImg);
    free(inputImg);
    fclose(inputFile);
    fclose(outputFile);
    return 0;
}