
#include "packet.h"


inline int ahtoi(char * aHex, int len){
    int sum = 0; //Every character is translated to an integer and is then shifted by
	for(int i = len - 1; i >= 0; i--) //powers of 16 depending on its position
		sum += int( ((aHex[i]-48> 16)?aHex[i]-55:aHex[i]-48)*pow(16.0,len-(i+1)) );
	return sum;
}

inline char * itoah(int dec, int len){
    char aHex[len];
    char * ahlt = "0123456789ABCDEF"; // ascii-hex lookup table
    aHex[len] = '\0';
    for (int i = len - 1; i >= 0; i--){
        aHex[i] = ahlt[dec >> 4 * (len - (i + 1)) & 0xF];
    }
    return aHex;
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

char calcCheckSum(packet p){
	char parityByte = encode(start);	//this variable is XORed with all bytes to complete rectangle code

	int i;
	for(i = 0; i < 6; i++){
		parityByte ^= encode(p.timeStamp[i]);
	}

	parityByte ^= p.type;	//this is incorrect according to documentation
	//parityByte ^= encode(type);	//this is correct

	for(i = 0; i < 3; i++){
		parityByte ^= encode(p.subtype[i]);
	}

	for(i = 0; i < 2; i++){
		parityByte ^= encode(p.dataLength[i]);
	}

	for(i = 0; i < p.dataSize; i++){
		parityByte ^= encode(p.data[i]);
	}

	return parityByte;
}


