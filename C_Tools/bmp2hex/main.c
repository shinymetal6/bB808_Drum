#include <stdio.h>
#include <stdlib.h>
#define BUF_SIZE    1024*1024*4
#define BUF_SEGSIZE    16384
unsigned char  bmpfile[BUF_SIZE];
unsigned char  bmp_segfile[10][16384];
FILE*   fpin;
FILE*   fpout;

int readbmp(char *filename)
{
int bmp_len;
    fpin = fopen (filename, "r");
    if ( fpin == NULL )
    {
        printf( "Unable to open %s for read\n",filename ) ;
        return -1;
    }
    bmp_len = fread(bmpfile, 1, BUF_SIZE, fpin);
    fclose(fpin);
    return bmp_len;
}

int convert_to_c(char *filename,char *arrayname,int bmp_len)
{
int i,k=0;
    fpout = fopen(filename, "w");
    if ( fpout == NULL )
    {
        printf( "Unable to open %s for write\n",filename ) ;
        return -1;
    }
    fprintf(fpout,"#include \"main.h\"\n");
    fprintf(fpout,"__attribute__((section(\".table\"))) const uint8_t %s[%d] = \n",arrayname,bmp_len);
    fprintf(fpout,"{\n");
    printf("%s has %d length\n",arrayname,bmp_len);
    for(i=0;i<bmp_len;i++, k++)
    {
        if ( k == 8)
        {
            k=0;
            fprintf(fpout,"\n");
        }
        fprintf(fpout,"0x%02x,",bmpfile[i]);
        //printf("\n");
    }
    fprintf(fpout,"\n};\n");
    fclose(fpout);
    return 0;
}
char bfilenames[10][16] = {
    "blue0",
    "blue1",
    "blue2",
    "blue3",
    "blue4",
    "blue5",
    "blue6",
    "blue7",
    "blue8",
    "blue9",
};

char gfilenames[10][16] = {
    "green0",
    "green1",
    "green2",
    "green3",
    "green4",
    "green5",
    "green6",
    "green7",
    "green8",
    "green9",
};

char rfilenames[10][16] = {
    "red0",
    "red1",
    "red2",
    "red3",
    "red4",
    "red5",
    "red6",
    "red7",
    "red8",
    "red9",
};

int readbmpArray(int val)
{
int bmp_len , i;
char filename[128];
int     offset=0;

    for(i=0;i<10;i++)
    {
        switch(val)
        {
            case    0   :   sprintf(filename,"../../Images/%s.bmp",bfilenames[i]);break;
            case    1   :   sprintf(filename,"../../Images/%s.bmp",gfilenames[i]);break;
            case    2   :   sprintf(filename,"../../Images/%s.bmp",rfilenames[i]);break;
            default     :   sprintf(filename,"../../Images/%s.bmp",bfilenames[i]);break;
        }
        fpin = fopen (filename, "r");
        if ( fpin == NULL )
        {
            printf( "Unable to open %s for read\n",filename ) ;
            return -1;
        }
        bmp_len = fread(&bmp_segfile[i], 1, BUF_SEGSIZE, fpin);

        fclose(fpin);
        offset += bmp_len;
    }
    return bmp_len;
}

int convert_to_c_array(char *filename,char *array_name,int bmp_len)
{
int i,k=0,j,d;
    d = 0;
    fpout = fopen(filename, "w");
    if ( fpout == NULL )
    {
        printf( "Unable to open %s for write\n",filename ) ;
        return -1;
    }
    printf( "Creating %s\n",array_name ) ;

    fprintf(fpout,"#include \"main.h\"\n");
    fprintf(fpout,"#define DIGIT_X    20\n");
    fprintf(fpout,"#define DIGIT_Y    36\n");
    fprintf(fpout,"__attribute__((section(\".table\"))) const uint8_t %s[10][%d] = \n",array_name,bmp_len);
    fprintf(fpout,"     {\n");
    for(j=0;j<10;j++)
    {
        fprintf(fpout,"     {\n");
        for(i=0;i<bmp_len;i++, k++)
        {
            if ( k == 8)
            {
                k=0;
                fprintf(fpout,"\n");
            }
            if ( k == 0 )
                fprintf(fpout,"         0x%02x,",bmp_segfile[j][i]);
            else
                fprintf(fpout,"0x%02x,",bmp_segfile[j][i]);
        }
        fprintf(fpout,"     \n},\n");
        d += bmp_len;
    }
    fprintf(fpout,"\n};\n");
    fclose(fpout);
    return 0;
}

#define BLUE     0
#define GREEN    1
#define RED      2

int main()
{
int bmp_len;

    bmp_len = readbmp ("../../Images/usbdrive.bmp");
    convert_to_c("../../Core/Src/bB808/Images/usbdrive.c","usbdrive",bmp_len);
    bmp_len = readbmp ("../../Images/beat.bmp");
    convert_to_c("../../Core/Src/bB808/Images/beat.c","beat",bmp_len);
    bmp_len = readbmpArray(BLUE);
    convert_to_c_array("../../Core/Src/bB808/Images/blue_digits.c","blue_digits",bmp_len);
    bmp_len = readbmpArray(GREEN);
    convert_to_c_array("../../Core/Src/bB808/Images/green_digits.c","green_digits",bmp_len);
    bmp_len = readbmpArray(RED);
    convert_to_c_array("../../Core/Src/bB808/Images/red_digits.c","red_digits",bmp_len);
    return 0;
}
