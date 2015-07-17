int Timing_Synchronization(int *Samples, int *Coeff) {
	
	int accReal = 0;
	int accImaginary = 0;
	int modulo = 0;
	int i;
	

	for (i = 0; i < 256; i++) {
		
		int samplesReal = Samples[i] >> 16;  
		int coeffReal = Coeff[i] >> 16;   
		int samplesImaginary = (Samples[i] << 16) >> 16;   
		int coeffImaginary = (Coeff[i] << 16) >> 16;   
		//because (a+ib)*(c+id) = (ac - db) + (ad + bc)i
		accReal += ((samplesReal*coeffReal) - (samplesImaginary*coeffImaginary)) >> 16;
		accImaginary += ((samplesReal*coeffImaginary) + (samplesImaginary*coeffReal)) >> 16;
		
	}
	modulo = (accReal*accReal) + (accImaginary*accImaginary);  //Real(acc)^2 + Imag(acc)^2
	
	
	
	return modulo;
}
