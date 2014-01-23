/****************************************************************
*Author: Roy Smart						*
*1/21/14							*
*MOSES LAB							*
*								*
*hkup connects to a serial device and reads raw data from the	*
*serial device. hkup then strips the start byte, stop byte and	*
*parity bits from the data and encloses the data in the packet	*
*structure to add to the packet queue connected to control.c	*
****************************************************************/

#include "hkup.h"

void hkup(){
    init_serial_connection();
    
    buildLookupTable();
    
    while(TRUE);
}

void init_serial_connection(){
	/*Open serial device for reading*/
        fup = open(HKUP, O_RDONLY | O_NOCTTY);
        if (fup < 0)
        {
                perror(HKUP);
                exit(-1);
        }
        /*save current serial port settings*/
        tcgetattr(fup, &oldtio_up);

        /*clear struct for new port settings*/
        bzero(&newtio_up, sizeof(newtio_up));

        /*set flags for non-canonical serial connection*/
        newtio_up.c_cflag |= UPBAUD | CS8 | CSTOPB | HUPCL | CREAD | CLOCAL;
        newtio_up.c_cflag &= ~(PARENB | PARODD);
        newtio_up.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY | IMAXBEL);
        newtio_up.c_iflag |= ISTRIP;
        newtio_up.c_oflag &= ~OPOST;
        newtio_up.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN);

        /*set non-canonical attributes*/
        newtio_up.c_cc[VTIME] = 1;
        newtio_up.c_cc[VMIN] = 255;

        tcflush(fup, TCIFLUSH);
        tcsetattr(fup ,TCSANOW, &newtio_up);

}

packet readPacket(int fd){
    
    
}

char* readData(int fd, int len){
    read(fd, buf, len);
}
