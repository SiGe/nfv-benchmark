#ifndef _ZIPF_H_
#define _ZIPF_H_

// Source: http://www.csee.usf.edu/~kchriste/tools/genzipf.c
// To improve performance use binary search: 
//      https://stackoverflow.com/questions/9983239/how-to-generate-zipf-distributed-numbers-efficiently
//===========================================================================
//=  Function to generate Zipf (power law) distributed random variables     =
//=    - Input: alpha and N, regenerate forces regeneration of zipf-dist    =
//=    - Output: Returns with Zipf distributed random variable              =
//===========================================================================
int zipf(double alpha, int n, int regenerate);

//=========================================================================
//= Multiplicative LCG for generating uniform(0.0, 1.0) random numbers    =
//=   - x_n = 7^5*x_(n-1)mod(2^31 - 1)                                    =
//=   - With x seeded to 1 the 10000th x value should be 1043618065       =
//=   - From R. Jain, "The Art of Computer Systems Performance Analysis," =
//=     John Wiley & Sons, 1991. (Page 443, Figure 26.2)                  =
//=========================================================================
double rand_val(int seed);

#endif /* _ZIPF_H_ */
