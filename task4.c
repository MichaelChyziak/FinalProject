
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
			//NZ - I've confirmed the number, assembly inlining gives this result, so far.
		};
	}
}


//Somethin isn't right with the assembly. Need to think about it: gives peak at i = 3 - clearly wrong.
int Timing_Synchronization(int *Samples, int *Coeff) {
	/*NZ: This now executes the full function in ~0.00023883 sec +/- with assembly inline
	To Do: re-write the for loop in assembly
					(this may require reorganizing variables.*/

	
	int accReal; //= 0;
	int accImaginary; //= 0;
	int limit;// = 256;  //possibly hard code this value somewhere
	int i;
	
	//just playing with syntax as I go
	//note: found out that LDR <variable>, <immediate> is not supported, must use MOV instead
	__asm{MOV accReal, #0
				MOV accImaginary, #0
				MOV limit, #256 //possibly hard coding this
				MOV i, #0}
				
	
	//if (Samples[129] == 0 && Samples[128] == 0) limit = 128;  
	//commented out this line because I just want to keep the assembly simple for now
	//Can we assume 2 samples cannot be 0 right beside each other? ASK FABIO
				
	for (i = 0; i < limit; i++) { //haven't decided how I want to handle the for loop yet
		int samplesReal, coeffReal, samplesImaginary, coeffImaginary;
		int sample = Samples[i];
		if (sample != 0) {
			
//			samplesReal = sample >> 16;   
//			samplesImaginary = (sample << 16) >> 16; 
//			coeffReal = Coeff[i] >> 16; 
//			coeffImaginary = (Coeff[i] << 16) >> 16;
			
			/*this code needs optimizing, code is smaller when not using assembly
				I'm going to see about combining the shifts for the imaginary terms,
				apparently ASR/ASL can be used instead of mov (preferred synonyms).*/
			
			__asm{MOV samplesReal, sample, ASR #16
						MOV samplesImaginary, sample, ASL #16
						ASR samplesImaginary, samplesImaginary, #16 
						MOV coeffReal, Coeff[i], ASR #16
						MOV coeffImaginary, Coeff[i], ASL #16
						ASR coeffImaginary, coeffImaginary, #16 }
			
			//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
			//accReal += ((samplesReal*coeffReal) - (samplesImaginary*coeffImaginary)) >> 16;
			//accImaginary += ((samplesReal*coeffImaginary) + (samplesImaginary*coeffReal)) >> 16;
						
			__asm{//accReal - this is not implemented as a MLA in task 3 either, this is the only way
						//I could think to do it, another way might be to use 2's complement of the second term
						// and add them -> then use MLA.
						MUL R11, samplesReal, coeffReal
						MUL R12, samplesImaginary, coeffImaginary //I noticed here that the compiler assigns R14,
						//even though its the link register. We can't access this with inline assembly - probably 
						//why the compiler is much better at optimizing.
						SUB R11, R11, R12, ASR #16
						ADD accReal, accReal, R11
				
						//accImaginary
						MUL R11, samplesReal, coeffImaginary
						MLA R12, samplesImaginary, coeffReal, R11
						ASR R12, #16}
						
						
		}
			
		sample = Samples[++i];
		if (sample != 0) {
			
//			samplesReal = sample >> 16;  
//			samplesImaginary = (sample << 16) >> 16; 
//			coeffReal = Coeff[i] >> 16;  
//			coeffImaginary = (Coeff[i] << 16) >> 16;
			
			//see note above
			__asm{MOV samplesReal, sample, ASR #16
						MOV samplesImaginary, sample, ASL #16
						ASR samplesImaginary, samplesImaginary, #16 
						MOV coeffReal, Coeff[i], ASR #16
						MOV coeffImaginary, Coeff[i], ASL #16
						ASR coeffImaginary, coeffImaginary, #16 }
			
			//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
			//accReal += ((samplesReal*coeffReal) - (samplesImaginary*coeffImaginary)) >> 16;
			//accImaginary += ((samplesReal*coeffImaginary) + (samplesImaginary*coeffReal)) >> 16;
						
			__asm{//see notes above
						MUL R11, samplesReal, coeffReal
						MUL R12, samplesImaginary, coeffImaginary
						SUB R11, R11, R12, ASR #16
						ADD accReal, accReal, R11
				
						//accImaginary
						MUL R11, samplesReal, coeffImaginary
						MLA R12, samplesImaginary, coeffReal, R11
						ASR R12, #16}
		}
}
	return (accReal*accReal) + (accImaginary*accImaginary);  //Real(acc)^2 + Imag(acc)^2		
}
