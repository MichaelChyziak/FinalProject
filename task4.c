
#include "compact_coeff.h"
#include "compact_R.h"



#pragma ARM

const int DetThr=0x69000;
int mod_out;
int i,j,sample[256];

int Timing_Synchronization(int *Samples, int *Coeff);

int main()
{
	for(j=0;j<256;j++) sample[j]=0; // intialization, not necessary on a simulator, may be needed on a board
	// Parsing one by one all input samples. Every time we add a sample in slot 1,
	// we shift all samples of one unit and we remove the sample in slot 256, then we run the FIR again
	for(i=0;i<3394;i++) {
		for(j=255;j>0;j--) sample[j]=sample[j-1];sample[0]=R[i];
		mod_out=Timing_Synchronization(sample,coef);
		if (mod_out>=DetThr)
		{
			// BOOOOOOOONG!!!!! Synchronization Achieved!!!
			// Hint -> Place a Breakpoint here for performance measurements!
			return(i); //whenever the peak is found the system quits.
			// (in the simulation data the peak is located at i=715) 
			
			//NZ: Now goes to 758
		};
	}
}



int Timing_Synchronization(int *Samples, int *Coeff) {

	int accReal = 0;
	int accImaginary = 0;
	int limit = 256;
	int i=0;

	int samplesReal, coeffReal, samplesImaginary, coeffImaginary;
    //int sample = Samples[i];
  int temp1,temp2;



	//commented out this line because I just want to keep the assembly simpler for now -NZ
	//if (Samples[129] == 0 && Samples[128] == 0) limit = 128;
	//Can we assume 2 samples cannot be 0 right beside each other? ASK FABIO

    __asm{

		//if (sample != 0) {

        loop:

            CMP Samples[i], #0
            BEQ check_count

			//loading vars
						MOV samplesReal, Samples[i], ASR #16
            MOV samplesImaginary, Samples[i], LSL #16
            ASR samplesImaginary, samplesImaginary, #16
            MOV coeffReal, Coeff[i], ASR #16
            MOV coeffImaginary, Coeff[i], LSL #16
            ASR coeffImaginary, coeffImaginary, #16

			//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
			//accReal += ((samplesReal*coeffReal) - (samplesImaginary*coeffImaginary)) >> 16;
			//accImaginary += ((samplesReal*coeffImaginary) + (samplesImaginary*coeffReal)) >> 16;


						MUL temp1, samplesReal, coeffReal
            MUL temp2, samplesImaginary, coeffImaginary
            SUB temp1, temp1, temp2
            ADD accReal, accReal, temp1, ASR #16

						MUL temp1, samplesReal, coeffImaginary
            MLA accImaginary, samplesImaginary, coeffReal, temp1
            ASR accImaginary, accImaginary, #16


       //Unrolling the loop once.
            ADD i, i, #1
            CMP Samples[i], #0
            BEQ check_count
			
			//loading vars
						MOV samplesReal, Samples[i], ASR #16
            MOV samplesImaginary, Samples[i], LSL #16
            ASR samplesImaginary, samplesImaginary, #16
            MOV coeffReal, Coeff[i], ASR #16
            MOV coeffImaginary, Coeff[i], LSL #16
            ASR coeffImaginary, coeffImaginary, #16

			
			
			//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
			//accReal += ((samplesReal*coeffReal) - (samplesImaginary*coeffImaginary)) >> 16;
			//accImaginary += ((samplesReal*coeffImaginary) + (samplesImaginary*coeffReal)) >> 16;

						MUL temp1, samplesReal, coeffReal
            MUL temp2, samplesImaginary, coeffImaginary
            SUB temp1, temp1, temp2
            ADD accReal, accReal, temp1, ASR #16

						MUL temp1, samplesReal, coeffImaginary
            MLA accImaginary, samplesImaginary, coeffReal, temp1
            ASR accImaginary, accImaginary, #16

        check_count: //iterates the loop counter and checks it.
            ADD i, i, #1
            CMP i, limit
            BNE loop

		}


	return (accReal*accReal) + (accImaginary*accImaginary);  //Real(acc)^2 + Imag(acc)^2
}

