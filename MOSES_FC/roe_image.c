/**************************************************
 * Author: David Keltgen                           *
 * Company: Montana State University: MOSES LAB    *
 * File Name: roeImage.c                           *
 * Date:  June 12 2014                             *
 * Description: The roeImage stucture will contain *
 *              all of the information about an    *
 *              image captured from the ROE camera.*
 *              This include the actual image data *
 *              as well as the metadata for the    *
 *              image.                             *
 **************************************************/
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <fcntl.h>
#include <errno.h>
#include "roe_image.h"
//#include "defs.h"

#define CH0		0x1	//0x8
#define CH1		0x2	//0x4
#define CH2		0x4	//0x2
#define CH3		0x8	//0x1

void constructEmpty() {

}

void constructImage(short** pdata, int* psize, char channels, int pbitpix) {
    int i;
    for (i = 0; i < 4; i++)
        image.size[i] = 0;
    init(pdata, psize, channels, pbitpix);
}

void init(short** pdata, int *psize, char channels, int pbitpix) {
    /* Set image values*/
    image.bitpix = pbitpix;
    setData(pdata, psize, channels);

    image.width = 0;
    image.height = 0;

    image.filename = "ddmmyyhhmmss.roe";
    image.name = "default";
    image.date = "2014-06-13";
    image.time = "00:00:00";
    image.origin = "Montana State University";
    image.instrument = "MOSES";
    image.observer = "MOSES lab";
    image.object = "Sun";
    image.duration = 0;
}

/* Set the data for this image*/
void setData(short **pdata, int *psize, char pchannels) {
    int i, j;

    image.channels = pchannels; // set the channel information

    for (i = 0; i < 4; i++) {
        /*Delete preexisting data*/

        image.size[i] = psize[i]; //set the correct size
        if (psize[i] != 0) {
            image.data[i] = (short *) malloc(2200000 * sizeof (short));
            /*Copy the data*/
            memcpy((char *) image.data[i], (char *) pdata[i], image.size[i]); //copy data
        }


    }
}

void writeToFile(char* file, char* catalog) {
    int i;
    char* msg;
    int newfile = 0;

    /*Check to see if file exists or not*/
    FILE *checkxml;
    if ((checkxml = fopen(catalog, "r")) == NULL) {
        printf("newfile = 1\n");
        newfile = 1;
    } else {
        fclose(checkxml);
    }

/*Write to XML File, if file does not exist, it will create one
  If file exists, it will append to it*/
FILE *outxml;
outxml = fopen(catalog, "a");
int foundline = 0;
char line[100];
printf("new file: %d\n", newfile);
if (!newfile) {
       /* Set the cursor to before </CATALOG>*/
}


/*construct the channels string*/
char schannels[5] = "";
if ((int) image.channels & CH0)
    strncat(schannels, "0", 1);
if ((int) image.channels & CH1)
    strncat(schannels, "1", 1);
if ((int) image.channels & CH2)
    strncat(schannels, "2", 1);
if ((int) image.channels & CH3)
    strncat(schannels, "3", 1);

if (newfile) {
    /*Write XML delcaration if new file*/
    fprintf(outxml, "<?xml version=\"1.0\" encoding=\"ASCII\" standalone=\"yes\"?>\n");
    fprintf(outxml, "<CATALOG>\n");
    newfile = 0;
}

fprintf(outxml, "<ROEIMAGE>\n");
fprintf(outxml, "\t<FILENAME>%s</FILENAME>\n", image.filename);
fprintf(outxml, "\t<NAME>%s</NAME>\n", image.name);
fprintf(outxml, "\t<BITPIX>%d</BITPIX>\n", image.bitpix);
fprintf(outxml, "\t<WIDTH>%d</WIDTH>\n", image.width);
fprintf(outxml, "\t<HEIGHT>%d</HEIGHT>\n", image.height);
fprintf(outxml, "\t<DATE>%s</DATE>\n", image.date);
fprintf(outxml, "\t<TIME>%s</TIME>\n", image.time);
fprintf(outxml, "\t<ORIGIN>%s</ORIGIN>\n", image.origin);
fprintf(outxml, "\t<INSTRUMENT>%s</INSTRUMENT>\n", image.instrument);
fprintf(outxml, "\t<OBSERVER>%s</OBSERVER>\n", image.observer);
fprintf(outxml, "\t<OBJECT>%s</OBJECT>\n", image.object);
fprintf(outxml, "\t<DURATION>%d</DURATION>\n", image.duration);
fprintf(outxml, "\t<CHANNELS>%s</CHANNELS>\n", schannels);

for (i = 0; i < 4; i++) {
    fprintf(outxml, "\t<CHANNEL_SIZE ch=\"%d\">%dpix</CHANNEL_SIZE>\n", i, image.size[i]);
}

fprintf(outxml, "</ROEIMAGE>\n");
if(newfile)
{
fprintf(outxml, "</CATALOG>\n");
}
fclose(outxml);

/*Write Image Data*/
FILE *dataOut;
dataOut = fopen(file, "w");
for (i = 0; i < 4; i++) {
    if (image.channels & (char) (1 << i))
        fwrite(image.data[i], sizeof (short), image.size[i], dataOut);
}
fclose(dataOut);

}

/*void pointToEnd(FILE* a)
{
    char line[100];
        

    }
    printf("string not found!\n");
    return;
}*/
