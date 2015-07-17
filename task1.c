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
	}
	modulo = (accReal*accReal) + (accImaginary*accImaginary);  //Real(acc)^2 + Imag(acc)^2	
	
	return modulo;
	
}

int main() {
	float ans = Timing_Synchronization(R, coef);
}
