#include "compact_coeff.h" //coeff
#include <stdlib.h>

/**
 * @param *Samples must be an array of 256 integers
 * @param *Coeff must be an array of 256 integers
 * Calculates the modulo between the Samples and Coeff value with the assumption that overflow does not have to be taken
 * into account for the operation.
 * @return the float value of the modulo operation between Samples and Coeff
*/
float Timing_Synchronization_float(int *Samples, int *Coeff) {
	//initialize values
	float accReal = 0;
	float accImaginary = 0;
	int i = 0;
	for (; i < 256; i++) { //for all 256 samples
		int samplesReal, coeffReal, samplesImaginary, coeffImaginary, coeff; //temporary variables
		int sample = Samples[i];
		samplesReal = sample >> 16; //gets real value of the samples (top 16 bits)
		samplesImaginary = (sample << 16) >> 16;  //gets imaginary value of the samples (bottom 16 bits)
		coeff = Coeff[i];
		coeffReal = coeff >> 16; //gets real value of the coefficient (top 16 bits)
		coeffImaginary = (coeff << 16) >> 16; //get imaginary value of the coefficient (bottom 16 bits)
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
		accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);

		//unrolling loop once and thus redo-ing the same calculations again
		i++;
		sample = Samples[i];
		samplesReal = sample >> 16;
		samplesImaginary = (sample << 16) >> 16;
		coeff = Coeff[i];
		coeffReal = coeff >> 16;
		coeffImaginary = (coeff << 16) >> 16;
		accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
		accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);
}
	return (accReal*accReal) + (accImaginary*accImaginary);  //MODULO = Real(acc)^2 + Imag(acc)^2
}


int Timing_Synchronization_int(int *Samples, int *Coeff) {
	int accReal = 0;
	int accImaginary = 0;
	int i;
	for (i = 0; i < 256; i++) {
		int samplesReal, coeffReal, samplesImaginary, coeffImaginary, coeff;
		int sample = Samples[i];
		samplesReal = sample >> 16;
		samplesImaginary = (sample << 16) >> 16;
		coeff = Coeff[i];
		coeffReal = coeff >> 16;
		coeffImaginary = (coeff << 16) >> 16;
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += ((samplesReal*coeffReal)>>16) - ((samplesImaginary*coeffImaginary) >> 16);
		accImaginary += ((samplesReal*coeffImaginary)>>16) + ((samplesImaginary*coeffReal) >> 16);

		i = i+1;
		sample = Samples[i];
		samplesReal = sample >> 16;
		samplesImaginary = (sample << 16) >> 16;
		coeff = Coeff[i];
		coeffReal = coeff >> 16;
		coeffImaginary = (coeff << 16) >> 16;
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += ((samplesReal*coeffReal)>>16) - ((samplesImaginary*coeffImaginary) >> 16);
		accImaginary += ((samplesReal*coeffImaginary)>>16) + ((samplesImaginary*coeffReal) >> 16);
}
	return (accReal*accReal) + (accImaginary*accImaginary);  //Real(acc)^2 + Imag(acc)^2
}


int Timing_Synchronization_inline(int *Samples, int *Coeff) {
	//hoping samples is in r0 and coeff is in r1 because r0-r3 reserved for functions
	//accReal, accImaginary, i, samplesReal, samplesImaginary, coeffReal, coeffImaginary  are from 4-10 respectively. accDummy1/sample, accDummy2 are r2 and r3
	//at the end when storing in dummy2, make it r0 (only at this time tho)
	int modulo = 0;
	asm("MOV r4, #0");
	asm("MOV r5, #0");
	asm("MOV r6, #0");
	asm("firstLoop:");
	asm("LDR r2, [r0, r6, LSL #2]");
	asm("MOV r7, r2, ASR #16");
	asm("MOV r8, r2, LSL #16");
	asm("MOV r8, r8, ASR #16");
	asm("LDR r2, [r1, r6, LSL #2]");
	asm("MOV r9, r2, ASR #16");
	asm("MOV r10, r2, LSL #16");
	asm("MOV r10, r10, ASR #16");
	asm("MUL r2, r7, r9");
	asm("MUL r3, r8, r10");
	asm("ADD r4, r4, r2, ASR #16");
	asm("SUB r4, r4, r3, ASR #16");
	asm("MUL r2, r7, r10");
	asm("MUL r3, r8, r9");
	asm("ADD r5, r5, r2, ASR #16");
	asm("ADD r5, r5, r3, ASR #16");


	//unrolled second time
	asm("ADD r6, r6, #1");
	asm("LDR r2, [r0, r6, LSL #2]");
	asm("MOV r7, r2, ASR #16");
	asm("MOV r8, r2, LSL #16");
	asm("MOV r8, r8, ASR #16");
	asm("LDR r2, [r1, r6, LSL #2]");
	asm("MOV r9, r2, ASR #16");
	asm("MOV r10, r2, LSL #16");
	asm("MOV r10, r10, ASR #16");
	asm("MUL r2, r7, r9");
	asm("MUL r3, r8, r10");
	asm("ADD r4, r4, r2, ASR #16");
	asm("SUB r4, r4, r3, ASR #16");
	asm("MUL r2, r7, r10");
	asm("MUL r3, r8, r9");
	asm("ADD r5, r5, r2, ASR #16");
	asm("ADD r5, r5, r3, ASR #16");
	asm("ADD r6, r6, #1");

	//for statement
	asm("CMP r6, #256");
	asm("BNE firstLoop");
	asm("MUL r2, r4, r4");
	asm("MLA %0, r5, r5, r2": "=r"(modulo));
	return modulo;
}


#define BUTTON			(int*)0x41200000
#define LED				(int*)0x41210000
#define SWITCH			(int*)0x41220000
#define BUTTON_CONTROL	(int*)(BUTTON+4)
#define LED_CONTROL 	(int*)(LED+4)
#define SWITCH_CONTROL	(int*)(SWITCH+4)

float DetThr_float= 0;
float mod_out_float;
int DetThr_int = 0;
int mod_out_int;

int i,j,sample[256];



int synch[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int synch_location = 0;


int main(void) {
	srand(1);
	*BUTTON_CONTROL = 0xff; //button as input
	*LED_CONTROL = 0x0; //led as output
	*SWITCH_CONTROL = 0xff; //switch as input
	*LED = 0x0;//turn all LEDs off
	while(1) {
		if (*BUTTON == 0x2) {
			float max_float = 0;
			int max_int = 0;
			*LED = 0x0;//turn all LEDs off
			for(j=0;j<256;j++) sample[j]=0; // intialization, not necessary on a simulator, may be needed on a board
			// Parsing one by one all input samples. Every time we add a sample in slot 1,
			// we shift all samples of one unit and we remove the sample in slot 256, then we run the FIR again
			for(i=0;i<10000;i++) {
				for(j=255;j>0;j--) sample[j]=sample[j-1];sample[0]=rand()>>8;
				mod_out_float=Timing_Synchronization_float(sample,coef);
				mod_out_int = Timing_Synchronization_int(sample, coef);
				if (mod_out_float > max_float)
				{
					max_float = mod_out_float;
				}
				if (mod_out_int > max_int)
				{
					max_int = mod_out_int;
				}
			}
			DetThr_float = 0.9 * max_float;
			DetThr_int = 0.9 * max_int;
			*LED = 0x1;
		}

		else if (*BUTTON == 0x1) {
			//float version
			if (*SWITCH == 0x1) {
				synch_location = 0;
				for (i=0;i<20;i++) {
					synch[i] = 0;
				}
				*LED = 0x0;//turn all LEDs off
				for(j=0;j<256;j++) sample[j]=0; // intialization, not necessary on a simulator, may be needed on a board
				// Parsing one by one all input samples. Every time we add a sample in slot 1,
				// we shift all samples of one unit and we remove the sample in slot 256, then we run the FIR again
				for(i=0;i<40000;i++) {
					for(j=255;j>0;j--) sample[j]=sample[j-1];sample[0]=rand()>>8;
					mod_out_float=Timing_Synchronization_float(sample,coef);
					if (mod_out_float>=DetThr_float)
					{
						//TURN LED 8 for float version
						*LED = 0x80;
						if (synch_location != 20) {
							synch[synch_location] = i;
							synch_location++;
						}

					};
				}
			}

			//int version
			else if (*SWITCH == 0x2) {
				synch_location = 0;
				for (i=0;i<20;i++) {
					synch[i] = 0;
				}
				*LED = 0x0;//turn all LEDs off
				for(j=0;j<256;j++) sample[j]=0; // intialization, not necessary on a simulator, may be needed on a board
				// Parsing one by one all input samples. Every time we add a sample in slot 1,
				// we shift all samples of one unit and we remove the sample in slot 256, then we run the FIR again
				for(i=0;i<40000;i++) {
					for(j=255;j>0;j--) sample[j]=sample[j-1];sample[0]=rand()>>8;
					mod_out_int=Timing_Synchronization_int(sample,coef);
					if (mod_out_int>=DetThr_int)
					{
						//TURN LED 4 for int version
						*LED = 0x8;
						if (synch_location != 20) {
							synch[synch_location] = i;
							synch_location++;
						}
					};
				}
			}

			//inline version
			else if (*SWITCH == 0x4) {
				synch_location = 0;
				for (i=0;i<20;i++) {
					synch[i] = 0;
				}
				*LED = 0x0;//turn all LEDs off
				for(j=0;j<256;j++) sample[j]=0; // intialization, not necessary on a simulator, may be needed on a board
				// Parsing one by one all input samples. Every time we add a sample in slot 1,
				// we shift all samples of one unit and we remove the sample in slot 256, then we run the FIR again
				for(i=0;i<40000;i++) {
					for(j=255;j>0;j--) sample[j]=sample[j-1];sample[0]=rand()>>8;
					mod_out_int=Timing_Synchronization_inline(sample,coef);
					if (mod_out_int>=DetThr_int)
					{
						//TURN LED 2 for inline version
						*LED = 0x2;
						if (synch_location != 20) {
							synch[synch_location] = i;
							synch_location++;
						}
					};
				}
			}

			//error version
			else if (*SWITCH != 0x0) {
				int flashing;
				int flash;
				while (1) {
					for (flashing = 0; flashing < 15; flashing++) {
						for (flash = 0; flash < 200000; flash++) {
							*LED = 0xff;
						}
						for (flash = 0; flash < 200000; flash++) {
							*LED = 0x0;
						}
					}
					if (*SWITCH == 0x0 || *SWITCH == 0x1 || *SWITCH == 0x2 || *SWITCH == 0x4) {
						break;
					}
				}
			}

			//no switches on version
			else {
				*LED = 0x0;//turn all LEDs off
			}
		}
	}
}
