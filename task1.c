#include "compact_coeff.h" //coeff
#include "compact_R.h"  //samples

/*
@param *Samples must be an array of 256 integers
@param *Coeff must be an array of 256 integers
Calculates the modulo between the Samples and Coeff value with the assumption that overflow does not have to be taken
into account for the operation.
@return the float value of the modulo operation between Samples and Coeff
*/
float Timing_Synchronization(int *Samples, int *Coeff) {
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

/*
Testing for float Timing_Synchronization
*/
int main() {
	float ans = Timing_Synchronization(R, coef); //tests the first 256 values of R(sampels) with first 256 values 
							//of coeff
	return 0; //-->put breakpoint here
}

