#include <stdio.h>
#include <stdlib.h>

#include "minmax.h"
#include "bmp.h"

#define filterWidth 3
#define filterHeight 3

int main(int argc, char* argv[])
{
    double filter[filterHeight][filterWidth] =
    {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    };
    double factor = 1.0/9.0;
    
    
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: %s infile outfile\n", argv[0]);
        return 1;
    }

    // remember filenames
    char* infile = argv[1];
    char* outfile = argv[2];
    
    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }
    
    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    
    // read infile's BITMAPFILEHEADER, BITMAPINFOHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    // ensure infile is a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // write outfile's BITMAPFILEHEADER, BITMAPINFOHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    //array for store image as array
    
    int w = bi.biWidth;
    int h = abs(bi.biHeight);
    int buff_size = w * h;
    
    RGBTRIPLE *image = (RGBTRIPLE *)malloc(buff_size * sizeof(RGBTRIPLE));
 
    
    //read img to bufer
    for (int x = 0; x < buff_size; x++)
        fread(&image[x], sizeof(RGBTRIPLE), 1, inptr);
    
    
    // apply filter
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            RGBTRIPLE tempPixel;
            tempPixel.rgbtRed = 0;
            tempPixel.rgbtGreen = 0;
            tempPixel.rgbtBlue = 0;
            
            for(int filterY = 0; filterY < filterWidth; filterY++)
            {
                for(int filterX = 0; filterX < filterHeight; filterX++)
                {
                    int imageX = (x - filterWidth / 2 + filterX + w) % w;
                    int imageY = (y - filterHeight / 2 + filterY + h) % h;
                    
                    tempPixel.rgbtRed += image[imageY * w + imageX].rgbtRed * (filter[filterY][filterX] *factor);
                    tempPixel.rgbtGreen += image[imageY * w + imageX].rgbtGreen * (filter[filterY][filterX]*factor);
                    tempPixel.rgbtBlue += image[imageY * w + imageX].rgbtBlue * (filter[filterY][filterX]*factor);
                }
            }
            
            image[y * w + x].rgbtRed = (int)tempPixel.rgbtRed;
            image[y * w + x].rgbtGreen = (int)tempPixel.rgbtGreen;
            image[y * w + x].rgbtBlue = (int)tempPixel.rgbtBlue;
        }
    }
    
    // write bufer to file
    for (int x = 0; x < buff_size; x++)
        fwrite(&image[x], sizeof(RGBTRIPLE), 1, outptr);
    
    
    
    // close files
    fclose(inptr);
    fclose(outptr);

    return 0;
}
