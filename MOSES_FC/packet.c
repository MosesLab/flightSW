
#include "packet.h"

/*Builds a packet out of provided values, returns packet pointer*/
packet_t* constructPacket(char* type, char* subtype, char* data) {
    int dataSize;
    if (data != NULL) {
        dataSize = strlen(data); //find length of data string
    } else {
        dataSize = 0;
    }
    char dataLength[2]; //allocate buffer for char representation of length
    itoah(dataSize, dataLength, 2); //convert length from int to string

    /*allocate space for packet*/
    packet_t* p;
    if ((p = (packet_t*) calloc(sizeof (packet_t), 1)) == NULL) {
        record("malloc failed to allocate packet");
    }

    getCurrentTime(p->timeStamp);
    //p->type[0] = type[0];
    memcpy(p->type, type, 2);
    memcpy(p->subtype, subtype, 4);
    memcpy(p->dataLength, dataLength, 3);
    p->dataSize = dataSize;
    if (data != NULL) {
        memcpy(p->data, data, dataSize + 1);
    } else {
        p->data[0] = '\0';
    }
    p->checksum[0] = calcCheckSum(p);
    p->checksum[1] = '\0';
    p->status = GOOD_PACKET;

    return p;
}

char encode(char dataByte) {
    return lookupTable[(int) dataByte];
}

char decode(char dataByte) {
    return (dataByte & 0x7F);
}

void getCurrentTime(char* result) {
    char timeString[30];
    time_t curTime = time(NULL);
    struct tm *broken = localtime(&curTime);
    strftime(timeString, 30, "%H%M%S", broken); //format time
    memcpy(result, timeString, 7);
}

/*records a packet to a log file*/
void recordPacket(packet_t* p) {
    char* pString = (char *) malloc(300 * sizeof (char));
    /*decode checksum so it doesn't print gibberish*/
    if (sprintf(pString, "%c%s%s%s%s%s%s%c\n", STARTBYTE, p->timeStamp, p->type, p->subtype, p->dataLength, p->data, p->checksum, ENDBYTE) == 0) {
        record("failed to record packet");
    }
    record(pString);
    free(pString); //clean up string after recording
}

/*BROKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DON'T USE, use strtol(3) instead*/
/*converts ascii hex to integer value*/
//inline int ahtoi(char * aHex, int len) {
//    int sum = 0; //Every character is translated to an integer and is then shifted by powers of 16 depending on its position
//    int i;
//    for (i = len - 1; i >= 0; i--)
//        sum += (int) (((aHex[i] - 48 > 16) ? aHex[i] - 55 : aHex[i] - 48) * pow(16.0, len - (i + 1)));
//    return sum;
//}

/*converts integers to ascii hex value*/
inline void itoah(int dec, char * aHex, int len) {
    char * ahlt = "0123456789ABCDEF"; // ascii-hex lookup table
    aHex[len] = '\0';
    int i;
    for (i = len - 1; i >= 0; i--) {
        aHex[i] = ahlt[dec >> 4 * (len - (i + 1)) & 0xF];
    }
}

/*build lookup table to encode parity bit for checksum calculation*/
void buildLookupTable() {
    int j;
    for (j = 0; j < 128; j++) {
        char sum = 0;

        int i;
        for (i = 0; i < 8; i++) {
            sum += (j << i) & 1; //calculate parity bit
        }
        lookupTable[j] = (sum % 2 == 0) ? j & 0x7F : j | 0x80; //inline if statement
    }
}

/*calculate checksum for HLP packets*/
char calcCheckSum(packet_t * p) {
    char parityByte = encode(STARTBYTE); //this variable is XORed with all bytes to complete rectangle code

    int i;
    for (i = 0; i < 6; i++) {
        parityByte ^= encode(p->timeStamp[i]);
    }

    parityByte ^= encode(p->type[0]);

    for (i = 0; i < 3; i++) {
        parityByte ^= encode(p->subtype[i]);
    }

    for (i = 0; i < 2; i++) {
        parityByte ^= encode(p->dataLength[i]);
    }


    for (i = 0; i < p->dataSize; i++) {
        parityByte ^= encode(p->data[i]);
    }

    parityByte = decode(parityByte); //decode because all chars are encoded before being sent
    return parityByte;
}

/*Open HLP up serial port for reading*/
int init_serial_connection(int hkup, char * serial_path) {
    struct termios oldtio, newtio; //structures for old device settings and new 

    /*Open serial device for reading*/
    int fd;
    if (hkup == TRUE) {
        fd = open(serial_path, O_RDONLY | O_NOCTTY);
    } else {
        fd = open(serial_path, O_WRONLY | O_NOCTTY);
    }
    if (fd < 0) {
        perror(serial_path);
        exit(-1);
    }
    /*save current serial port settings*/
    tcgetattr(fd, &oldtio);

    /*clear struct for new port settings*/
    bzero(&newtio, sizeof (newtio));

    /*set flags for non-canonical serial connection*/
    if (hkup) {
        newtio.c_cflag |= UPBAUD | CS8 | CSTOPB | HUPCL | CREAD | CLOCAL;
    } else {
        newtio.c_cflag |= DOWNBAUD | CS8 | CSTOPB | HUPCL | CLOCAL;
    }
    newtio.c_cflag &= ~(PARENB | PARODD);
    newtio.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY | IMAXBEL);
    //newtio_up.c_iflag |= ISTRIP;
    newtio.c_oflag &= ~OPOST;
    newtio.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN);

    /*set non-canonical attributes*/
    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 255;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    return fd;
}

void readPacket(int fd, packet_t * p) {
    int tempValid = TRUE;
    p->status = TRUE;
    char temp;
    //    char * error = "";

    int continue_read = FALSE;
    int input;

    while (continue_read == FALSE && ts_alive == TRUE) {
        input = input_timeout(fd, 10000, 0); //Wait until interrupt or timeout 
        //if(input==0) puts("select returned");
        volatile int clearBuffer = FALSE;
        if (input > 0) {
            readData(fd, &temp, 1);
            if (temp == STARTBYTE) clearBuffer = TRUE;
        }
        if (clearBuffer) {
            //            ioctl(fd, FIONREAD);
            continue_read = TRUE;
            record("\n");
            tempValid = readData(fd, p->timeStamp, 6);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) record("Bad Timestamp\n");
            //            record("Timestamp\n");

            tempValid = readData(fd, p->type, 1);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) record("Bad type\n");
            //            record("type\n");

            tempValid = readData(fd, p->subtype, 3);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) record("Bad subtype\n");
            //            record("subtype\n");

            tempValid = readData(fd, p->dataLength, 2);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) record("Bad data length\n");
            p->dataSize = strtol(p->dataLength, NULL, 16); //calculate data size to find how many bytes to read
            //            char msg[255];
            //            sprintf(msg, "data length %d\n", p->dataSize);
            //            record(msg);

            tempValid = readData(fd, p->data, p->dataSize);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) record("Bad data\n");
            //            record("data\n");

            readData(fd, p->checksum, 1);
            //            record("checksum\n");
            readData(fd, &temp, 1);
            //            record("endbyte\n");
            while (temp != ENDBYTE) {
                readData(fd, &temp, 1);
            }

            readData(fd, &temp, 1);
            while (temp != EOF) {
                readData(fd, &temp, 1);
            }

            char rx_checksum = calcCheckSum(p);
            tempValid = (p->checksum[0] == rx_checksum);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) {
                char msg[255];
                sprintf(msg, "Bad checksum: got %c, expected %c\n", p->checksum[0], rx_checksum);
                record(msg);
            }
            //            ioctl(fd, FIONREAD);
        }
    }

}

/*readData returns an array if successful or 0 if an error occurred*/
int readData(int fd, char * data, int len) {
    char temp;
    int result = TRUE;

    int rsz = read(fd, data, len);
    while (rsz < len) {
        rsz += read(fd, data + rsz, len - rsz);
    }

    int i;
    for (i = 0; i < len; i++) {
        temp = data[i];
        data[i] = decode(temp);


        if (temp != encode(data[i]) && temp != data[i]) {
            result = FALSE;
            record("Bad packet Encoding\n");
        }
    }

    data[len] = '\0';
    return result;
}

void sendPacket(packet_t * p, int fd) {

    char start = STARTBYTE;
    char end = ENDBYTE;
    char eof = 0x04;

    sendData(&start, 1, fd);
    sendData(p->timeStamp, 6, fd);
    sendData(p->type, 1, fd);
    sendData(p->subtype, 3, fd);
    sendData(p->dataLength, 2, fd);
    sendData(p->data, p->dataSize, fd);
    sendData(p->checksum, 1, fd);
    sendData(&end, 1, fd);
    sendData(&eof, 1, fd);
}

void sendData(char * data, int size, int fd) {
    int i = 0;
    char temp;
    while (i < size) {
        temp = encode(data[i]);
        if (write(fd, &temp, 1) < 1) record("Write Error: hlp_down\n");
        i++;
    }
}

int input_timeout(int filedes, unsigned int seconds, unsigned int micros) {
    fd_set set;
    struct timeval timeout;

    /*initialize the file descriptor set for select function*/
    FD_ZERO(&set);
    FD_SET(filedes, &set);

    /*initialize timout data structure for select function*/
    timeout.tv_sec = seconds;
    timeout.tv_usec = micros;

    /*select returns 0 if timeout, 1 if input data is available, -1 if error*/
    return TEMP_FAILURE_RETRY(select(FD_SETSIZE, &set, NULL, NULL, &timeout));
}
