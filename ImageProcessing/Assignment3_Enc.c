#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<windows.h>

int main(int argc, char* argv[]) {
    BITMAPFILEHEADER bmpFile;
    BITMAPINFOHEADER bmpInfo;
    FILE* inputFile = NULL;
    inputFile = fopen("AICenterY.bmp", "rb");
    fread(&bmpFile, sizeof(BITMAPFILEHEADER), 1, inputFile);
    fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, inputFile);

    int width = bmpInfo.biWidth;
    int height = bmpInfo.biHeight;
    int size = bmpInfo.biSizeImage;
    int bitCnt = bmpInfo.biBitCount;
    int stride = (((bitCnt / 8) * width) + 3) / 4 * 4;
    printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

    
    unsigned char* inputImg = NULL, * outputImg = NULL;
    inputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
    outputImg = (unsigned char*)calloc(size, sizeof(unsigned char));
    double* Y1 = (double*)calloc(width * height, sizeof(double));
    int* Y2 = (int*)calloc(width * height, sizeof(int));
    fread(inputImg, sizeof(unsigned char), size, inputFile);
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Y1[j * width + i] = inputImg[j * stride + 3 * i];
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    FILE* fp;//fixed-length code 3bit로 구현함.(binarization ppt 5쪽에 주어진 binary code 그대로 사용.)
    int e;
    fp = fopen("bitstream.txt", "w"); //픽셀값을 읽어들여 fixed-length code를 bitstream에 넣어야 하므로, 쓰기 모드로 열어준다.
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (i == 0) {
                e = Y1[j * width + i] - 128;
                e /= 5;
                if (e < -3) { // e값이  -3보다 작을 경우, reconstruction할 때 -4로 적용시킴.
                    fprintf(fp, "%s", "000");
                    e = -4;
                }
                else if (e == -3) {
                    fprintf(fp, "%s", "001");
                }
                else if (e == -2) {
                    fprintf(fp, "%s", "010");
                }
                else if (e == -1) {
                    fprintf(fp, "%s", "011");
                }
                else if (e == 0) {
                    fprintf(fp, "%s", "100");
                }
                else if (e == 1) {
                    fprintf(fp, "%s", "101");
                }
                else if (e == 2) {
                    fprintf(fp, "%s", "110");
                }
                else {// e가 2보다 클 경우, "111" 작성.
                    fprintf(fp, "%s", "111");
                    e = 3;// e>2 의 경우 reconstruction 할 때 3으로 적용시킴.
                }
                e *= 5;
                Y2[j * width + i] = 128 + e;
            }
            else {
                e = Y1[j * width + i] - Y2[j * width + i - 1];
                e /= 5;
                if (e < -3) {
                    fprintf(fp, "%s", "000");
                    e = -4;// 위의 주석과 마찬가지 이유로 e=-4
                }
                else if (e == -3) {
                    fprintf(fp, "%s", "001");
                }
                else if (e == -2) {
                    fprintf(fp, "%s", "010");
                }
                else if (e == -1) {
                    fprintf(fp, "%s", "011");
                }
                else if (e == 0) {
                    fprintf(fp, "%s", "100");
                }
                else if (e == 1) {
                    fprintf(fp, "%s", "101");
                }
                else if (e == 2) {
                    fprintf(fp, "%s", "110");
                }
                else {
                    fprintf(fp, "%s", "111");
                    e = 3;//위와 마찬가지 이유로 e=3
                }
                e *= 5;
                Y2[j * width + i] = Y2[j * width + i - 1] + e;
            }

        }
    }
    fclose(fp);



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            outputImg[j * stride + 3 * i + 2] = (unsigned char)(Y2[j * width + i] > 255 ? 255 : (Y2[j * width + i] < 0 ? 0 : Y2[j * width + i]));
            outputImg[j * stride + 3 * i + 1] = (unsigned char)(Y2[j * width + i] > 255 ? 255 : (Y2[j * width + i] < 0 ? 0 : Y2[j * width + i]));
            outputImg[j * stride + 3 * i + 0] = (unsigned char)(Y2[j * width + i] > 255 ? 255 : (Y2[j * width + i] < 0 ? 0 : Y2[j * width + i]));
        }
    }
    FILE* outputFile = fopen("reconEncY.bmp", "wb");
    fwrite(&bmpFile, sizeof(BITMAPFILEHEADER), 1, outputFile);
    fwrite(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, outputFile);
    fwrite(outputImg, sizeof(unsigned char), size, outputFile);

    free(outputImg);
    free(inputImg);
    fclose(inputFile);
    fclose(outputFile);
    return 0;
}