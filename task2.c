#include "compact_coeff.h"
#include "compact_R.h"

float Timing_Synchronization(int *Samples, int *Coeff) {
	float accReal = 0;
	float accImaginary = 0;
	int i;
	for (i = 0; i < 256; i++) {//for all 256 samples
		int samplesReal, coeffReal, samplesImaginary, coeffImaginary, coeff;  //temporary variables
		int sample = Samples[i]; //gets real value of the samples (top 16 bits)
		samplesReal = sample >> 16;  //gets imaginary value of the samples (bottom 16 bits)   
		samplesImaginary = (sample << 16) >> 16; 
		coeff = Coeff[i];
		coeffReal = coeff >> 16; //gets real value of the coefficient (top 16 bits)
		coeffImaginary = (coeff << 16) >> 16;//get imaginary value of the coefficient (bottom 16 bits)
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
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
		accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);
}
	
	return (accReal*accReal) + (accImaginary*accImaginary);  //MODULO = Real(acc)^2 + Imag(acc)^2
	
}


const float DetThr=1e+15;
float mod_out;
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

