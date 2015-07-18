#include "compact_coeff.h"
#include "compact_R.h"


const float DetThr=1e+15;
float mod_out;
int i,j,sample[256];

int Timing_Synchronization(int *Samples, int *Coeff);

/***
problem, if Timing_Synchronization works on ints, mod_out is never larger than 2147483647
which is way smaller than 1e+15 so the program never ends.  So either we need to reduce
the detection threshold, or find another solution.

Maybe unsigned? Ask Fabio.

*/
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

int Timing_Synchronization(int *Samples, int *Coeff) { //t = 2.85090367
	int accReal = 0;
	int accImaginary = 0;
	int modulo = 0;
	int i;
	int limit = 256;
	if (Samples[129] == 0 && Samples[128] == 0) limit = 128;  //Can we assume 2 samples cannot be 0 right beside each other? ASK FABIO
	for (i = 0; i < limit; i++) {
		int sample = Samples[i];
		if (sample != 0) {

			int samplesReal = sample >> 16;
			int coeffReal = Coeff[i] >> 16;
			int samplesImaginary = (sample << 16) >> 16;
			int coeffImaginary = (Coeff[i] << 16) >> 16;
			//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
			accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
			accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);
		}
		sample = Samples[++i];
		if (sample != 0) {

			int samplesReal = sample >> 16;
			int coeffReal = Coeff[i] >> 16;
			int samplesImaginary = (sample << 16) >> 16;
			int coeffImaginary = (Coeff[i] << 16) >> 16;
			//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
			accReal += ((samplesReal*coeffReal) - (samplesImaginary*coeffImaginary)) >> 16;
			accImaginary += ((samplesReal*coeffImaginary) + (samplesImaginary*coeffReal)) >> 16;
		}
	}
	modulo = (accReal*accReal) + (accImaginary*accImaginary);  //Real(acc)^2 + Imag(acc)^2

	return modulo; //now executes in 0.00011767 sec using int


}
