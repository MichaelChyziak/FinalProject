#include "compact_coeff.h"
#include "compact_R.h"
#pragma ARM

unsigned int Timing_Synchronization(int *Samples, int *Coeff) {
	int accReal, accImaginary, i, sample, samplesReal, samplesImaginary, coeffReal, coeffImaginary, accDummy1, accDummy2;
	__asm {
		MOV accReal, #0 //initializing accReal
		MOV accImaginary, #0 //initializing accImaginary
		MOV i, #0 //initializing loop counter
		firstLoop:
		LDR sample, [Samples, i, LSL #2] //load Samples[i] into temp variable.
		MOV samplesReal, sample, ASR #16 //get real part of sample
		MOV samplesImaginary, sample, LSL #16 
		MOV samplesImaginary, samplesImaginary, ASR #16 //get imaginary part of sample
		LDR sample, [Coeff, i, LSL #2] //load Coeff[i] into temp variable
		MOV coeffReal, sample, ASR #16 //get real part of coefficient
		MOV coeffImaginary, sample, LSL #16 
		MOV coeffImaginary, coeffImaginary, ASR #16 //get imaginary part of coefficient
		MUL accDummy1, samplesReal, coeffReal 
		MUL accDummy2, samplesImaginary, coeffImaginary
		ADD accReal, accReal, accDummy1, ASR #16
		SUB accReal, accReal, accDummy2, ASR #16 //preceding four lines compute accReal
		MUL accDummy1, samplesReal, coeffImaginary
		MUL accDummy2, samplesImaginary, coeffReal
		ADD accImaginary, accImaginary, accDummy1, ASR #16
		ADD accImaginary, accImaginary, accDummy2, ASR #16 //preceding four lines compute accImaginary
		
		//unrolled loop, operationally the same as above
		ADD i, i, #1
		LDR sample, [Samples, i, LSL #2]
		MOV samplesReal, sample, ASR #16
		MOV samplesImaginary, sample, LSL #16
		MOV samplesImaginary, samplesImaginary, ASR #16
		LDR sample, [Coeff, i, LSL #2]
		MOV coeffReal, sample, ASR #16
		MOV coeffImaginary, sample, LSL #16
		MOV coeffImaginary, coeffImaginary, ASR #16
		MUL accDummy1, samplesReal, coeffReal
		MUL accDummy2, samplesImaginary, coeffImaginary
		ADD accReal, accReal, accDummy1, ASR #16
		SUB accReal, accReal, accDummy2, ASR #16
		MUL accDummy1, samplesReal, coeffImaginary
		MUL accDummy2, samplesImaginary, coeffReal
		ADD accImaginary, accImaginary, accDummy1, ASR #16
		ADD accImaginary, accImaginary, accDummy2, ASR #16
		ADD i, i, #1
		
		//for statement
		CMP i, #256
		BNE firstLoop 
		MUL accDummy1, accReal, accReal
		MLA accDummy2, accImaginary, accImaginary, accDummy1 //compute the modulo of (accReal,accImaginary)
	}
	return accDummy2; //return modulo
}

const unsigned int DetThr=0x69000;
unsigned int mod_out;
int i,j,sample[256];
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
		};
	}
}  

