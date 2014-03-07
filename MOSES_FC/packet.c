
#include "packet.h"

inline int ahtoi(char * aHex, int len) {
    int sum = 0; //Every character is translated to an integer and is then shifted by powers of 16 depending on its position
    int i;
    for (i = len - 1; i >= 0; i--)
        sum += (int) (((aHex[i] - 48 > 16) ? aHex[i] - 55 : aHex[i] - 48) * pow(16.0, len - (i + 1)));
    return sum;
}

inline void itoah(int dec, char * aHex, int len) {
    char * ahlt = "0123456789ABCDEF"; // ascii-hex lookup table
    aHex[len] = '\0';
    int i;
    for (i = len - 1; i >= 0; i--) {
        aHex[i] = ahlt[dec >> 4 * (len - (i + 1)) & 0xF];
    }
}

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

char calcCheckSum(Packet * p) {
    char parityByte = encode(STARTBIT); //this variable is XORed with all bytes to complete rectangle code

    int i;
    for (i = 0; i < 6; i++) {
        parityByte ^= encode(p->timeStamp[i]);
    }

    //parityByte ^= p->type[0];	//this is incorrect according to documentation
    parityByte ^= (p->type[0]); //this is correct

    for (i = 0; i < 3; i++) {
        parityByte ^= encode(p->subtype[i]);
    }

    for (i = 0; i < 2; i++) {
        parityByte ^= encode(p->dataLength[i]);
    }


	for(i = 0; i < p->dataSize; i++){
		parityByte ^= encode(p->data[i]);
	}
        
        parityByte = decode(parityByte);
        printf("%c\n", parityByte);
	return parityByte;
               

    for (i = 0; i < p->dataSize; i++) {
        parityByte ^= encode(p->data[i]);
    }



    printf("%c\n", parityByte);
    return parityByte;
}

int init_hkup_serial_connection() {
    /*Open serial device for reading*/
    int fd = open(HKUP, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        perror(HKUP);
        exit(-1);
    }
    /*save current serial port settings*/
    tcgetattr(fd, &oldtio_up);

    /*clear struct for new port settings*/
    bzero(&newtio_up, sizeof (newtio_up));

    /*set flags for non-canonical serial connection*/
    newtio_up.c_cflag |= UPBAUD | CS8 | CSTOPB | HUPCL | CREAD | CLOCAL;
    newtio_up.c_cflag &= ~(PARENB | PARODD);
    newtio_up.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY | IMAXBEL);
    //newtio_up.c_iflag |= ISTRIP;
    newtio_up.c_oflag &= ~OPOST;
    newtio_up.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN);

    /*set non-canonical attributes*/
    newtio_up.c_cc[VTIME] = 1;
    newtio_up.c_cc[VMIN] = 255;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio_up);

    return fd;
}

int init_hkdown_serial_connection() {
    /*Open serial device for reading*/
    int fd = open(HKDOWN, O_WRONLY | O_NOCTTY);
    if (fd < 0) {
        perror(HKUP);
        exit(-1);
    }
    /*save current serial port settings*/
    tcgetattr(fd, &oldtio_up);

    /*clear struct for new port settings*/
    bzero(&newtio_up, sizeof (newtio_up));

    /*set flags for non-canonical serial connection*/
    newtio_up.c_cflag |= UPBAUD | CS8 | CSTOPB | HUPCL | CLOCAL;
    newtio_up.c_cflag &= ~(PARENB | PARODD);
    newtio_up.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY | IMAXBEL);
    //newtio_up.c_iflag |= ISTRIP;
    newtio_up.c_oflag &= ~OPOST;
    newtio_up.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN);

    /*set non-canonical attributes*/
    newtio_up.c_cc[VTIME] = 1;
    newtio_up.c_cc[VMIN] = 255;

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio_up);

    return fd;
}

void readPacket(int fd, Packet * p) {
    int tempValid = TRUE;
    p->valid = TRUE;
    char temp;
    char * error = "";

    readData(fd, &temp, 1);
    while (temp != STARTBIT) {
        error += temp;
        readData(fd, &temp, 1);
    }
    if (error != "") {
        //printf("Bad Packet Data\n");
    }
    tempValid = readData(fd, p->timeStamp, 6);
    p->valid = p->valid & tempValid;
    if (tempValid != TRUE) printf("Bad Timestamp\n");

    tempValid = readData(fd, p->type, 1);
    p->valid = p->valid & tempValid;
    if (tempValid != TRUE) printf("Bad type\n");

    tempValid = readData(fd, p->subtype, 3);
    p->valid = p->valid & tempValid;
    if (tempValid != TRUE) printf("Bad subtype\n");

    tempValid = readData(fd, p->dataLength, 2);
    p->valid = p->valid & tempValid;
    if (tempValid != TRUE) printf("Bad data length\n");
    p->dataSize = ahtoi(p->dataLength, 2);

    tempValid = readData(fd, p->data, p->dataSize);
    p->valid = p->valid & tempValid;
    if (tempValid != TRUE) printf("Bad data\n");

    readData(fd, p->checksum, 1);
    readData(fd, &temp, 1);

    while (temp != ENDBIT) {
        readData(fd, &temp, 1);
    }
    tempValid = (p->checksum[0] == calcCheckSum(p));
    p->valid = p->valid & tempValid;
    if (tempValid != TRUE) printf("Bad checksum %d\n", p->valid);

    //return p;
}

/*readData returns an array if successful or 0 if an error occurred*/
int readData(int fd, char * data, int len) {
    char temp;
    int available = 0;
    int result = TRUE;
    
    /*change file pointed to fd_set for select*/
    fd_set set;
    FD_ZERO (&set);
    FD_SET(fd, &set);

    /*create timestructure for select function*/
    struct timeval timeout;
    timeout.tv_sec = 2;
    while (ts_alive) {
        available = select(fd+1, &set, NULL, NULL, &timeout);   //Use select to be able to exit, and not hang on read()
        if(available) { //If select returns true, read the data
            int rsz = read(fd, data, len);
            while (rsz < len) {
                rsz += read(fd, data + rsz, len - rsz);
            }

            int i;
            for (i = 0; i < len; i++) {
                temp = data[i];
                data[i] = decode(temp);

                if (temp != encode(data[i])) {
                    result = FALSE;
                    printf("Bad packet Encoding");
                }
            }
        }
    }
    data[len] = '\0';
    return result;
}
