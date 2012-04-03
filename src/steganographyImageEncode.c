/*
 ============================================================================
 Name        : steganographyImage.c
 Author      : Aditya
 Version     : 1.0
		Copyright 2012 Aditya Arun <aditya@aditya-ThinkPad-SL400>
		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 2 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
		MA 02110-1301, USA.

 Description : Image steganography using OpenCV Library.
 The code takes in two images image to hide (source) and the image in which to hide (dest)
 The image in which the image is supposed to be hidden should have 8 times + 48 pixels
 The code picks up each pixel value from source, extracts one bit from it and adds it to the LSB
 of the destination. Since the max value an intensity level can take is 255 (assuming 8 bit), 1 
 source pixel will take 8 pixels to hide.
 The first 48 pixels in dest store the height and width information of the source image 
 ============================================================================
 */

#include <stdio.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <stdlib.h>

#define EXTRACT_BIT(n,i) ((n&(1<<i))?1:0)

int main(int argc, char **argv)
{
	IplImage *input; // input image
	IplImage *hide; // image to hide
	IplImage *output; // output image

	uchar *data;
	uchar *inputData;
	uchar *hideData;

	int messageChar;
	int i=0, j=0, dataPos = 48;
	int height, width, nchannels;
	int hideHeight, hideWidth, hideNchannels;

	if(argc < 3)
	{
		printf("Usage : %s <input image filepath> <image to hide>\n", argv[0]);
		exit(0);
	}

	input = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
	hide = cvLoadImage(argv[2], CV_LOAD_IMAGE_COLOR);

	//input image height and width
	height = input->height;
	width = input->width;
	nchannels = input->nChannels;
	/*
	 *Use 24 bits (1 bit per pixel) to save hide image height and width in output image
	 */

	hideHeight = hide->height;
	hideWidth = hide->width;
	hideNchannels = hide->nChannels;

	if(height*width < (hideHeight*hideWidth*8+48))
	{
		printf("Please input source image of larger size\n");
		exit(0);
	}

	if(nchannels != 3 || hideNchannels != 3)
	{
		printf("Sorry currently only support for RGB images\n");
		exit(0);
	}

	printf("Hiding image of size %d X %d in image of size %d X %d\n", hideHeight, hideWidth, height, width);

	//make an empty image with the same width and height as that of input image
	output = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U,3);

	data = (uchar *)output->imageData;
	inputData = (uchar *)input->imageData;
	hideData = (uchar *)hide->imageData;

	//save the height and width in output image 24 pixels for height and 24 pixels for width
	//save height
	for(i=0; i<24; i++)
	{
		data[i] = inputData[i] & 254;
		data[i] = data[i] + EXTRACT_BIT(hideHeight, i);
	}
	//save width
	for(i=0; i<24; i++)
	{
		data[i+24] = inputData[i+24] & 254;
		data[i+24] = data[i+24] + EXTRACT_BIT(hideWidth, i);
	}

	//start saving the image from the 48th pixel
	for(i=0; i<hideHeight*hideWidth*3; i++)
	{
		messageChar = hideData[i];
		//printf("%d\n", messageChar);
		for(j=0; j<8; j++)
		{
			data[dataPos] = inputData[dataPos] & 254;
			data[dataPos] = data[dataPos] + EXTRACT_BIT(messageChar, j);
			//printf("%d\t",data[dataPos]);
			dataPos++;
		}
		//printf("\n");
	}
	//hiding image completes

	//copy the rest of the image to the output image
	for(j=dataPos; j<height*width*3; j++)
	{
		data[j] = inputData[j];
	}

	//save the image in bmp or png format because they are lossless compression
	cvSaveImage("stegHideImage.png",output,0);

	cvNamedWindow("Original Image", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Hidden Message Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Original Image", input);
	cvShowImage("Hidden Message Image", output);

	cvWaitKey(0);

	cvReleaseImage(&input);
	cvReleaseImage(&output);
	cvReleaseImage(&hide);
	cvDestroyWindow("Original Image");
	cvDestroyWindow("Hidden Message Image");

	return 0;
}

