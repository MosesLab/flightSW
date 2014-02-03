
#include "packet.h"


inline int ahtoi(char * aHex, int len){
    int sum = 0; //Every character is translated to an integer and is then shifted by powers of 16 depending on its position
    int i;
    for(i = len - 1; i >= 0; i--) 
		sum += (int)( ((aHex[i]-48> 16)?aHex[i]-55:aHex[i]-48)*pow(16.0,len-(i+1)) );
	return sum;
}

inline void  itoah(int dec, char * aHex, int len){
    char * ahlt = "0123456789ABCDEF"; // ascii-hex lookup table
    aHex[len] = '\0';
    int i;
    for (i = len - 1; i >= 0; i--){
        aHex[i] = ahlt[dec >> 4 * (len - (i + 1)) & 0xF];
    }
}
void buildLookupTable(){
	int j;
	for(j = 0; j < 128; j++){
		char sum = 0;

		int i;
		for(i = 0; i < 8; i++){
			sum += (j << i) & 1; 	//calculate parity bit
		}
		lookupTable[j] = (sum%2 == 0)?j & 0x7F:j | 0x80;	//inline if statement
	}
}

char calcCheckSum(Packet * p){
	char parityByte = encode(STARTBIT);	//this variable is XORed with all bytes to complete rectangle code

	int i;
	for(i = 0; i < 6; i++){
		parityByte ^= encode(p->timeStamp[i]);
	}

	//parityByte ^= p->type[0];	//this is incorrect according to documentation
	parityByte ^= (p->type[0]);	//this is correct

	for(i = 0; i < 3; i++){
		parityByte ^= encode(p->subtype[i]);
	}

	for(i = 0; i < 2; i++){
		parityByte ^= encode(p->dataLength[i]);
	}

	for(i = 0; i < p->dataSize; i++){
		parityByte ^= encode(p->data[i]);
	}
        
        
        printf("%c\n", parityByte);
	return parityByte;
}


