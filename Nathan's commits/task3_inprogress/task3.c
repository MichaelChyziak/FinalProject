#include "compact_coeff.h"
#include "compact_R.h"
#include "task1.h"

const float DetThr=1e+20;
float mod_out;
int i,j,sample[256];
int main()
{
	for(j=0;j<256;j++) sample[i]=0; // intialization, not necessary on a simulator, may be needed on a board
	// Parsing one by one all input samples. Every time we add a sample in slot 1,
	// we shift all samples of one unit and we remove the sample in slot 256, then we run the FIR again
		for(i=0;i<3394;i++) {
		for(j=0;j<255;j++) sample[j+1]=sample[i];sample[0]=R[i]; 
		mod_out=Timing_Synchronization(sample,coef);
		
		if (mod_out>=DetThr)
		{
			// BOOOOOOOONG!!!!! Synchronization Achieved!!!
			// Hint -> Place a Breakpoint here for performance measurements!
			return(i); //whenever the peak is found the system quits.
			// (in the simulation data the peak is located at i=277)
		};
	};
}; 
