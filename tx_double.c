#include <stdio.h>
#include <string.h>
#include <complex.h>
#include "fftw.h"
#include <sys/time.h>
#include "usrp_lib.h"

#define FFT_SIZE 64
#define FFT_CP_SIZE 16


int main()
{
#ifdef _TIME_
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
#endif
	complex double short_sync[64] = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0, -1.4719601443879746-1.4719601443879746j, 0.0, 0.0, 0.0,
		1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0, -1.4719601443879746-1.4719601443879746j, 0.0, 0.0, 0.0,
		-1.4719601443879746-1.4719601443879746j, 0.0, 0.0, 0.0, 1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, -1.4719601443879746-1.4719601443879746j, 0.0, 0.0, 0.0,
		-1.4719601443879746-1.4719601443879746j, 0.0, 0.0, 0.0, 1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0,
		1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0, 1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0,
		1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};

	complex double long_sync_1[64] = {
		0, 0j, 0, 0j, 0, 0j, -1, 1j, -1, 1j, -1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, (-0-1j), 1, -1j, -1, 1j,
	 	0, -1j, 1, (-0-1j), 1, -1j, 1, 1j, -1, -1j, 1, (-0-1j), -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 0j, 0, 0j, 0, 0j
	};
	complex double long_sync_2[64] = {
		0, 0, 0, 0, 0, 0, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1,
	 	0, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 0, 0, 0, 0
	};

	complex double phy_sync_ofdm[320] = {0.0};

	fftw_plan fftwp = fftw_create_plan(FFT_SIZE, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_one(fftwp, (fftw_complex *)&short_sync[0], (fftw_complex *)&phy_sync_ofdm[FFT_CP_SIZE]);
	memcpy(&phy_sync_ofdm[0], &phy_sync_ofdm[64], FFT_CP_SIZE * sizeof(complex double));
	memcpy(&phy_sync_ofdm[80], &phy_sync_ofdm[0], 80* sizeof(complex double));

	fftw_one(fftwp, (fftw_complex *)&long_sync_1[0], (fftw_complex *)&phy_sync_ofdm[176]);
	memcpy(&phy_sync_ofdm[160], &phy_sync_ofdm[224],  FFT_CP_SIZE * sizeof(complex double));

	fftw_one(fftwp, (fftw_complex *)&long_sync_2[0], (fftw_complex *)&phy_sync_ofdm[256]);
	memcpy(&phy_sync_ofdm[240], &phy_sync_ofdm[304],  FFT_CP_SIZE * sizeof(complex double));
	fftw_destroy_plan(fftwp);

#ifdef _TIME_
	gettimeofday(&te, NULL);
	double timespan = (te.tv_sec - ts.tv_sec) * 1000.0 + (te.tv_usec - ts.tv_usec) / 1000.0;
	printf("timespan = %0.5f ms \n", timespan);
#endif

#ifdef _PRINTF_
	printf("the ifft result of short sync : \n");
	int index;
	for(index = 0; index < 80; index++)
	{
		printf("( %0.2f + %0.2fj ) ", creal(phy_sync_ofdm[index]), cimag(phy_sync_ofdm[index]));
		if((index + 1)%8 == 0)
			printf("\n");
	}
	printf("\n");
	printf("the ifft result of short sync : \n");
	for(index; index < 160; index++)
	{
		printf("( %0.2f + %0.2fj ) ", creal(phy_sync_ofdm[index]), cimag(phy_sync_ofdm[index]));
		if((index + 1)%8 == 0)
			printf("\n");
	}
	printf("\n");

	printf("the ifft result of long sync : \n");
	for(index; index < 320; index++)
	{
		printf("( %0.2f + %0.2fj ) ", creal(phy_sync_ofdm[index]), cimag(phy_sync_ofdm[index]));
		if((index + 1)%8 == 0)
			printf("\n");
	}
	printf("\n");
#endif

	complex double phy_sync_test[2048] = {0 + 0j};
	memcpy(&phy_sync_test[100], &phy_sync_ofdm[0], 320 * sizeof(complex double));
	memcpy(&phy_sync_test[1024], &phy_sync_ofdm[0], 320 * sizeof(complex double));
	complex double phy_sync_test_zero[1024] = {0 + 0j};
	phy_sync_test_zero[1023] = 0.0 + 0.01j;


	complex float pilots[127][4] = {
		{1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},
		{-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},
		{-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1},
		{1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},
		{1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},
		{1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},
		{-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},
		{1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},
		{-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},
		{-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1},
		{-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},
		{1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},
		{-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},
		{1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},   {-1, -1, -1, 1},
		{-1, -1, -1, 1}, {1, 1, 1, -1},   {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {1, 1, 1, -1},   {1, 1, 1, -1},   {1, 1, 1, -1},
		{-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}, {-1, -1, -1, 1}
	};
	int pilot_position[4] = {-21, -7, 7, 21};
	int data_position[49] = {
		-26, -25, -24, -23, -22, -20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -6, -5, -4, -3, -2, -1,
		1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26, 27
	};

//	double *tx = (double *)phy_sync_test;
//	printf("( %0.2f + %0.2fj ) ", creal(tx[2048]), cimag(tx[2048]));

	init_usrp(40e6, 20e6, 2.412e9, 30);
	create_tx_stream();
	while(1)
	{
		usrp_send((double *)phy_sync_test, 1024, 4096);
	}

}
