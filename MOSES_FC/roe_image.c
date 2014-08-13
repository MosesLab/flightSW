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
#include "roe_image.h"

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
    int i;

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

void createXML()
{
    /*Append will create a new file if one does not exist*/
    /*If a file does exist, this function will open it and */
    /*then close it immediately*/
    FILE *checkxml;
    checkxml = fopen(CATALOG, "a");
    fclose(checkxml);
}


void writeToFile() {
 
    int i;
    int linecount = 0;
    int newfile = 0;
    char xmlarr[10000][200];
    char msg[255];
    char* line;
    size_t len = 0;
    ssize_t read;
    createXML();
    /*Check to see if file had data in it*/
    FILE *checkxml;
    checkxml = fopen(CATALOG, "r");
    fseek(checkxml, 0, SEEK_END);
    if (ftell(checkxml) == 0) {
        newfile = 1;
    }
    else {
        newfile = 0;
    }
    fclose(checkxml);
    FILE *readxml;
    readxml = fopen(CATALOG, "r");

    while ((read = getline(&line, &len, readxml)) != -1) {
        strncpy(xmlarr[linecount], line, 200);
        linecount++;
    }
    fclose(readxml);
    /*Write to XML File*/
    FILE * outxml;
    outxml = fopen(CATALOG, "r+");
    /*Write data from previous xml file*/
    for (i = 0; i < linecount; i++) {
        fwrite(xmlarr[i], sizeof (xmlarr[i][0]), strlen(xmlarr[i]), outxml);
    }
    
    if (!newfile) {
        /* Set the cursor to before </CATALOG>*/
        fseek(outxml, -11, SEEK_END);
    }

    /*construct the channels string*/
    char schannels[5] = "";
    if ((int) tempimage.channels & CH0)
        strncat(schannels, "0", 1);
    if ((int) tempimage.channels & CH1)
        strncat(schannels, "1", 1);
    if ((int) tempimage.channels & CH2)
        strncat(schannels, "2", 1);
    if ((int) tempimage.channels & CH3)
        strncat(schannels, "3", 1);
    
    if (newfile) {
        /*Write XML declaration if new file*/
        fprintf(outxml, "<?xml version=\"1.0\" encoding=\"ASCII\" standalone=\"yes\"?>\n");
        fprintf(outxml, "<CATALOG>\n");
    }
    record("Writing XML file\n");
    fprintf(outxml, "<ROEIMAGE>\n");
    fprintf(outxml, "\t<FILENAME>%s</FILENAME>\n", tempimage.filename);
    fprintf(outxml, "\t<NAME>%s</NAME>\n", tempimage.name);
    fprintf(outxml, "\t<BITPIX>%d</BITPIX>\n", tempimage.bitpix);
    fprintf(outxml, "\t<WIDTH>%d</WIDTH>\n", tempimage.width);
    fprintf(outxml, "\t<HEIGHT>%d</HEIGHT>\n", tempimage.height);
    fprintf(outxml, "\t<DATE>%s</DATE>\n", tempimage.date);
    fprintf(outxml, "\t<TIME>%s</TIME>\n", tempimage.time);
    fprintf(outxml, "\t<ORIGIN>%s</ORIGIN>\n", tempimage.origin);
    fprintf(outxml, "\t<INSTRUMENT>%s</INSTRUMENT>\n", tempimage.instrument);
    fprintf(outxml, "\t<OBSERVER>%s</OBSERVER>\n", tempimage.observer);
    fprintf(outxml, "\t<OBJECT>%s</OBJECT>\n", tempimage.object);
    fprintf(outxml, "\t<DURATION>%d</DURATION>\n", tempimage.duration);
    fprintf(outxml, "\t<CHANNELS>%s</CHANNELS>\n", schannels);
    for (i = 0; i < 4; i++) {
        fprintf(outxml, "\t<CHANNEL_SIZE ch=\"%d\">%dpix</CHANNEL_SIZE>\n", i, tempimage.size[i]);
    }
    fprintf(outxml, "</ROEIMAGE>\n");
    fprintf(outxml, "</CATALOG>\n");
    
    fclose(outxml);
   
    /*Write Image Data*/
    FILE *dataOut;
    dataOut = fopen(tempimage.filename, "w"); 
    
    for (i = 0; i < 4; i++) {
        record(msg);
        if (tempimage.channels & (char) (1 << i))
            fwrite(tempimage.data[i], sizeof (short), tempimage.size[i], dataOut);
    }
    fclose(dataOut);
    
   

}

