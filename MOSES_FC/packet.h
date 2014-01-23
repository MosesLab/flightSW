/****************************************/
/*Author: Roy Smart			*/
/*1/21/14				*/
/*MOSES LAB				*/
/*					*/
/*packet struct encapsulates attributes */
/*of serial command packets		*/
/****************************************/



const char start = '%';
const char end = '^';
char lookupTable[128];	//this lookup table is global, but since it is only written to once, it does not need to be locked


typedef struct {
	char timeStamp[6];
	char type;
	char subtype[3];
	char dataLength[2];
	char data[255];
	char checksum;

        void * next;
        
	int dataSize;

} packet;

char encode(char dataByte) {return lookupTable[dataByte];}
char decode(char dataByte) {return (dataByte & 0x7F);}


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
