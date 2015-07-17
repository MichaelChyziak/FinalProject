
#include "compact_coeff.h"
#include "compact_R.h"

float Timing_Synchronization(int *Samples, int *Coeff) { //t = 2.85090367
	float accReal = 0;
	float accImaginary = 0;
	float modulo = 0;
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
			accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
			accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);
		}

/*
float Timing_Synchronization(int *Samples, int *Coeff) {  //NEW VERSION, TESTING (code = 2424), t = 3.12778733 from 3.21363758 (limit 63), t = 3.08199808 (limit 127), 
	//3.08675667(limit 149), 3.08229642(limit 134), 3.08204117(limit 131), 3.08198392(limit 128), 3.08199158(limit 129). THERFORE LIMIT 128 IS BEST
	float accReal = 0;
	float accImaginary = 0;
	float modulo = 0;
	int i;
	int limit = 256;
	if (Samples[129] == 0 && Samples[128] == 0) limit = 128;  //Can we assume 2 samples cannot be 0 right beside each other? ASK FABIO
	for (i = 0; i < limit; i++) {
		int samplesReal = Samples[i] >> 16;  
		int coeffReal = Coeff[i] >> 16;  
		int samplesImaginary = (Samples[i] << 16) >> 16; 
		int coeffImaginary = (Coeff[i] << 16) >> 16;
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
		accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);
*/

/*  (ORIGINAL)
float Timing_Synchronization(int *Samples, int *Coeff) {
	float accReal = 0;
	float accImaginary = 0;
	float modulo = 0;
	int i;
	for (i = 0; i < 256; i++) {
		int samplesReal = Samples[i] >> 16;  
		int coeffReal = Coeff[i] >> 16;  
		int samplesImaginary = (Samples[i] << 16) >> 16; 
		int coeffImaginary = (Coeff[i] << 16) >> 16;
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
		accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);
		*/
		
		/*
		//unroll (each one adds about 100 bytes), time goes from 3.21363758 to 3.11453125 overall
		samplesReal = Samples[++i] >> 16;  
		coeffReal = Coeff[i] >> 16;  
		samplesImaginary = (Samples[i] << 16) >> 16; 
		coeffImaginary = (Coeff[i] << 16) >> 16;
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += (samplesReal*coeffReal) - (samplesImaginary*coeffImaginary);
		accImaginary += (samplesReal*coeffImaginary) + (samplesImaginary*coeffReal);
		*/
		
	}
	modulo = (accReal*accReal) + (accImaginary*accImaginary);  //Real(acc)^2 + Imag(acc)^2	
	
	return modulo;
	
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

