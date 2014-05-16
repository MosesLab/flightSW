
#include "packet.h"

/*Builds a packet out of provided values*/
Packet * constructPacket(char* type, char* subtype, char* data){
    int dataSize = strlen(data);  //find length of data string
    char dataLength[2];            //allocate buffer for char representation of length
    itoah(dataSize, dataLength, 2);  //convert length from int to string
    
    /*allocate space for packet*/
    Packet* p;
    if((p = (Packet*) malloc(sizeof(Packet))) == NULL){
        puts("malloc failed to allocate packet");
    }
    
//    char* timestamp;
//    if((timestamp = (char*) malloc(sizeof(char) * 7)) == NULL){
//        puts("malloc failed to allocate timestamp");
//    }
//    getCurrentTime(timestamp);
//    
//    /*copy memory into new packet*/
//    memcpy(p->timeStamp, timestamp, 6);
    getCurrentTime(p->timeStamp);
    //memcpy(p->type, type, 1);
    p->type[0] = type[0];
    memcpy(p->subtype, subtype, 3);
    memcpy(p->dataLength, dataLength, 2);
    memcpy(p->data, data, dataSize);
    p->checksum[0] = 0;
    p->status = GOOD_PACKET;
    p->dataSize = dataSize;
    
    return p;
}

void getCurrentTime(char* result){
    	char timeString[30];
    	time_t curTime = time(NULL);
    	struct tm *broken = localtime(&curTime);
    	strftime(timeString,30,"%H%M%S",broken); //format time
    	memcpy(result,timeString,7);
}
/*converts ascii hex to integer value*/
inline int ahtoi(char * aHex, int len) {
    int sum = 0; //Every character is translated to an integer and is then shifted by powers of 16 depending on its position
    int i;
    for (i = len - 1; i >= 0; i--)
        sum += (int) (((aHex[i] - 48 > 16) ? aHex[i] - 55 : aHex[i] - 48) * pow(16.0, len - (i + 1)));
    return sum;
}

/*converts integers to ascii hex value*/
inline void itoah(int dec, char * aHex, int len) {
    char * ahlt = "0123456789ABCDEF"; // ascii-hex lookup table
    aHex[len] = '\0';
    int i;
    for (i = len - 1; i >= 0; i--) {
        aHex[i] = ahlt[dec >> 4 * (len - (i + 1)) & 0xF];
    }
}

/**/
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
    char parityByte = encode(STARTBYTE); //this variable is XORed with all bytes to complete rectangle code

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


    for (i = 0; i < p->dataSize; i++) {
        parityByte ^= encode(p->data[i]);
    }

    parityByte = decode(parityByte);
    return parityByte;


//    for (i = 0; i < p->dataSize; i++) {
//        parityByte ^= encode(p->data[i]);
//    }
//
//
//
//    printf("%c\n", parityByte);
//    return parityByte;
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
        perror(HKDOWN);
        exit(-1);
    }
    /*save current serial port settings*/
    tcgetattr(fd, &oldtio_up);

    /*clear struct for new port settings*/
    bzero(&newtio_up, sizeof (newtio_up));

    /*set flags for non-canonical serial connection*/
    newtio_up.c_cflag |= DOWNBAUD | CS8 | CSTOPB | HUPCL | CLOCAL;
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
    p->status = TRUE;
    char temp;
    char * error = "";
    
    volatile int continue_read = FALSE;    
    int input;
    
    while (continue_read == FALSE && ts_alive == TRUE) {
        input = input_timeout(fd, 1);   //Wait until interrupt or timeout 
        
        volatile int clearBuffer = FALSE;
        if(input > 0){
            readData(fd, &temp, 1);
            if(temp == STARTBYTE) clearBuffer = TRUE;
        }
        if (clearBuffer) {
            ioctl(fd, FIONREAD);
            continue_read = TRUE;
//            readData(fd, &temp, 1);
//            while (temp != STARTBIT) {
//                error += temp;
//                readData(fd, &temp, 1);
//            }
//            if (error != "") {
//                //printf("Bad Packet Data\n");
//            }
            tempValid = readData(fd, p->timeStamp, 6);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) printf("Bad Timestamp\n");

            tempValid = readData(fd, p->type, 1);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) printf("Bad type\n");

            tempValid = readData(fd, p->subtype, 3);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) printf("Bad subtype\n");

            tempValid = readData(fd, p->dataLength, 2);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) printf("Bad data length\n");
            p->dataSize = ahtoi(p->dataLength, 2);      //calculate data size to find how many bytes to read

            tempValid = readData(fd, p->data, p->dataSize);
            p->status = p->status & tempValid;
            if (tempValid != TRUE) printf("Bad data\n");

            readData(fd, p->checksum, 1);
            readData(fd, &temp, 1);

            while (temp != ENDBYTE) {
                readData(fd, &temp, 1);
            }
            tempValid = (p->checksum[0] == calcCheckSum(p));
            p->status = p->status & tempValid;
            if (tempValid != TRUE) printf("Bad checksum %d\n", p->status);
            ioctl(fd, FIONREAD);
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
            printf("Bad packet Encoding\n");
        }
    }

    data[len] = '\0';
    return result;
}

void sendPacket(Packet * p, int fd){
    p->checksum[0] = calcCheckSum(p);
    char start = STARTBYTE;
    char end = ENDBYTE;
    char eof = 0x04;
    
    sendData(&start, 1, fd);
    sendData(p->timeStamp, 6, fd);
    sendData(p->type, 1, fd);
    sendData(p->subtype, 3, fd);
    sendData(p->dataLength, 2, fd);
    sendData(p->checksum, 1, fd);
    sendData(&end, 1, fd);
    sendData(&eof, 1, fd);
}

void sendData(char * data, int size, int fd){
    int i = 0;
    char temp;
    while(i < size){
        temp = encode(data[i]);
	if(write(fd, &temp, 1) < 1) printf("Write Error:%c\n",temp);
        i++;
    }
}
int input_timeout(int filedes, unsigned int seconds) {
    fd_set set;
    struct timeval timeout;

    /*initialize the file descriptor set for select function*/
    FD_ZERO(&set);
    FD_SET(filedes, &set);

    /*initialize timout data structure for select function*/
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    /*select returns 0 if timeout, 1 if input data is available, -1 if error*/
    return TEMP_FAILURE_RETRY(select(FD_SETSIZE, &set, NULL, NULL, &timeout));
}
