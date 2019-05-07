// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cs50.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames
    int n = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    //n between 1 and 100
    if (n < 0 || n > 100)
    {
        printf("number must be between 1 and 100\n");
        return 5;
    }

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf, bf2;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    bf2 = bf;

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi, bi2;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    bi2 = bi;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    bi2.biWidth = bi.biWidth * n;
    bi2.biHeight = bi.biHeight * n;

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int newpadding = (4 - (bi2.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // make new size
    bi2.biSizeImage = ((sizeof(RGBTRIPLE) * bi2.biWidth) + newpadding) * abs(bi2.biHeight);
    bf2.bfSize = bi2.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf2, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi2, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int k = 0; k < n; k++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                for (int l = 0; l < n; l++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            // then add it back (to demonstrate how)
            for (int m = 0; m < newpadding; m++)
            {
                fputc(0x00, outptr);
            }
            if (k < n - 1)
            {
                fseek(inptr, -bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
            }

        }
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
