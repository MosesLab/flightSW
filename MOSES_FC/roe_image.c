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

/*the xml file is incrementally stored in RAM using these variables. An xml snippet is the part of the xml file that is associated with one image*/
int xml_index = 0; // index for storing how many xml entries have been saved
char * xml_snips[1000]; // array for storing xml snippets, room for 1000 images per program run.
int xml_snips_sz[1000]; // array for storing the amount of bytes in each xml snippet

void constructEmpty() {

}

void constructImage(roeimage_t * image, int* psize, char channels, int pbitpix) {
    int i;
    for (i = 0; i < 4; i++) {
        image->size[i] = 0;
    }
    init(image, psize, channels, pbitpix);
}

void init(roeimage_t * image, int *psize, char channels, int pbitpix) {
    /* Set image values*/
    image->bitpix = pbitpix;
    setData(image, psize, channels);

    image->width = 0;
    image->height = 0;

    image->filename = "ddmmyyhhmmss.roe";
    image->name = "default";
    image->date = "2014-06-13";
    image->time = "00:00:00";
    image->origin = "Montana State University";
    image->instrument = "MOSES";
    image->observer = "MOSES lab";
    image->object = "Sun";
    image->duration = 0;
}

/* Set the data for this image*/
void setData(roeimage_t * image, int *psize, char pchannels) {
    image->channels = pchannels; // set the channel information

    image->data = malloc(SIZE_DS_BUFFER * 4); // allocate space for 4-channel images

}

void createXML() {
    /*Append will create a new file if one does not exist*/
    /*If a file does exist, this function will open it and */
    /*then close it immediately*/
    FILE *checkxml;
    checkxml = fopen(CATALOG, "a");
    fclose(checkxml);
}

void writeToFile(roeimage_t * image) {

    int i;
    int rc;
    //    char msg[255];
    int linecount = 0;
    int newfile = 0;
    char xmlarr[10000][200];
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
    } else {
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
    if ((int) image->channels & CH0)
        strncat(schannels, "0", 1);
    if ((int) image->channels & CH1)
        strncat(schannels, "1", 1);
    if ((int) image->channels & CH2)
        strncat(schannels, "2", 1);
    if ((int) image->channels & CH3)
        strncat(schannels, "3", 1);

    if (newfile) {
        /*Write XML declaration if new file*/
        fprintf(outxml, "<?xml version=\"1.0\" encoding=\"ASCII\" standalone=\"yes\"?>\n");
        fprintf(outxml, "<CATALOG>\n");
    }

    /*allocate buffer for xml file snippet to send through telemetry*/
    unsigned int xml_sz = 0;
    char * xmlbuf = malloc(700 * sizeof (char)); // Each xml snippet is approximately 597 bytes

    /*write xml data to buffer*/
    xml_sz += sprintf(xmlbuf + xml_sz, "<ROEIMAGE>\n");
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<FILENAME>%s</FILENAME>\n", image->filename);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<NAME>%s</NAME>\n", image->name);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<BITPIX>%d</BITPIX>\n", image->bitpix);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<WIDTH>%d</WIDTH>\n", image->width);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<HEIGHT>%d</HEIGHT>\n", image->height);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<DATE>%s</DATE>\n", image->date);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<TIME>%s</TIME>\n", image->time);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<ORIGIN>%s</ORIGIN>\n", image->origin);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<INSTRUMENT>%s</INSTRUMENT>\n", image->instrument);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<OBSERVER>%s</OBSERVER>\n", image->observer);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<OBJECT>%s</OBJECT>\n", image->object);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<DURATION>%d</DURATION>\n", image->duration);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<CHANNELS>%s</CHANNELS>\n", schannels);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<PIXELS>%d</PIXELS>\n", image->total_size);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<ROE_TEMP_UPPER>%3.2f</ROE_TEMP_UPPER>\n", image->roe_temp_up);
    xml_sz += sprintf(xmlbuf + xml_sz, "\t<ROE_TEMP_LOWER>%3.2f</ROE_TEMP_LOWER>\n", image->roe_temp_low);
    for (i = 0; i < 4; i++) {
        xml_sz += sprintf(xmlbuf + xml_sz, "\t<CHANNEL_SIZE ch=\"%d\">%dpix</CHANNEL_SIZE>\n", i, image->size[i]);
    }
    xml_sz += sprintf(xmlbuf + xml_sz, "</ROEIMAGE>\n");

    /*save xml snippet buffer in roeimage struct to pass to telem*/
    xml_snips[xml_index] = xmlbuf;
    xml_snips_sz[xml_index] = xml_sz;
    xml_index++;

    /*save xml snippet to disk*/
    rc = fwrite(xmlbuf, sizeof (char), xml_sz, outxml);

    /*write closing xml statement*/
    fprintf(outxml, "</CATALOG>\n");

    fclose(outxml);

    /*Write Image Data*/
    FILE *dataOut;
    dataOut = fopen(image->filename, "w");

    /*write image to library buffer*/
    rc = fwrite(image->data, sizeof (short), image->total_size, dataOut);
    if (rc < image->total_size) {
        record(strerror(ferror(dataOut)));
    }

    /*flush library buffer*/
    rc = fflush(dataOut);
    if (rc != 0) {
        record("Error flushing science data buffer!");
    }

    fclose(dataOut);

}

