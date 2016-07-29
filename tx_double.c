#include <stdio.h>
#include <string.h>
#include <complex.h>
#include "fftw.h"
#include <sys/time.h>
#include "usrp_lib.h"

#define FFT_SIZE 64
#define FFT_CP_SIZE 16

double complex long_sync_store[64] = {
 0.156 + -0.000j ,  0.005 + 0.120j ,  0.040 + -0.111j ,  -0.097 + -0.083j ,  0.021 + 0.028j ,  -0.060 + 0.088j ,  -0.115 + -0.055j ,  0.038 + 0.106j , 
	 0.098 + -0.026j ,  -0.053 + -0.004j ,  0.001 + -0.115j ,  0.137 + 0.047j ,  0.024 + -0.059j ,  -0.059 + 0.015j ,  -0.022 + 0.161j ,  -0.119 + 0.004j , 
	 0.062 + -0.062j ,  -0.037 + -0.098j ,  -0.057 + 0.039j ,  0.131 + -0.065j ,  0.082 + 0.092j ,  -0.070 + -0.014j ,  -0.060 + 0.081j ,  0.056 + 0.022j , 
	 -0.035 + -0.151j ,  0.122 + 0.017j ,  -0.127 + -0.021j ,  -0.075 + 0.074j ,  -0.003 + 0.054j ,  0.092 + -0.115j ,  0.092 + 0.106j ,  -0.012 + -0.098j , 
	 -0.156 + 0.000j ,  -0.012 + 0.098j ,  0.092 + -0.106j ,  0.092 + 0.115j ,  -0.003 + -0.054j ,  -0.075 + -0.074j ,  -0.127 + 0.021j ,  0.122 + -0.017j , 
	 -0.035 + 0.151j ,  0.056 + -0.022j ,  -0.060 + -0.081j ,  -0.070 + 0.014j ,  0.082 + -0.092j ,  0.131 + 0.065j ,  -0.057 + -0.039j ,  -0.037 + 0.098j , 
	 0.062 + 0.062j ,  -0.119 + -0.004j ,  -0.022 + -0.161j ,  -0.059 + -0.015j ,  0.024 + 0.059j ,  0.137 + -0.047j ,  0.001 + 0.115j ,  -0.053 + 0.004j , 
	 0.098 + 0.026j ,  0.038 + -0.106j ,  -0.115 + 0.055j ,  -0.060 + -0.088j ,  0.021 + -0.028j ,  -0.097 + 0.083j ,  0.040 + 0.111j ,  0.005 + -0.120j , 
};

int long_sync_corr(complex double *receive_data, int receive_data_length, int corr_length)
{
	int index = 0;
	complex double corr_result = 0.0;
	complex double corr_power = 0.0;

	double arr[65536] = {0.0};
	while(index < receive_data_length)
	{
		corr_result = 0.0;
		corr_power = 0.0;
		int corr_index;
		for(corr_index = 0; corr_index < corr_length; corr_index++)
		{
			corr_result += receive_data[index + corr_index] * conj(long_sync_store[corr_index]);
			corr_power += long_sync_store[corr_index] * conj(long_sync_store[corr_index]);
		}
		double an = cabs(corr_result);
		double complex an_result = an/corr_power;

		arr[index] = creal(an_result);
		//printf("[long] an result index %d result %0.6f\n",index, arr[index]);
		index++;
	}

	double max[3] = {0.0};
	int max_index[3] = {0};

	int tmp_index = 0;
	for(tmp_index; tmp_index < receive_data_length; tmp_index++)
	{
		if(arr[tmp_index] >= max[2])
		{
			max[1] = max[2];
			max_index[1] = max_index[2];
			max[0] = max[1];
			max_index[0] = max_index[1];
			max[2] = arr[tmp_index];
			max_index[2] = tmp_index;
		}
		else if(arr[tmp_index] >= max[1])
		{
			max[0] = max[1];
			max_index[0] = max_index[1];
			max[1] = arr[tmp_index];
			max_index[1] = tmp_index;
		}
		else if(arr[tmp_index] > max[0])
		{
			max[0] = arr[tmp_index];
			max_index[0] = tmp_index;
		}
	}

		for(tmp_index = 0; tmp_index < 3; tmp_index++)
		{
			printf("[long]   index: %d  corr_result:  %0.6f \n", max_index[tmp_index],max[tmp_index] );

		}

		int tmp;
		if(max_index[0] > max_index[1]){tmp = max_index[0]; max_index[0] = max_index[1]; max_index[1] = tmp;};
		if(max_index[0] > max_index[2]){tmp = max_index[0]; max_index[0] = max_index[2]; max_index[2] = tmp;};
		if(max_index[1] > max_index[2]){tmp = max_index[1]; max_index[1] = max_index[2]; max_index[2] = tmp;};

		if( (max_index[2] - max_index[1]) == 64)
		{
			printf("index1  %d  index2  %d \n", max_index[2],max_index[1]);
			return (max_index[2] + 64);
		}
		else if ( (max_index[1] - max_index[0]) == 64)
		{
			printf("index1  %d  index2  %d \n", max_index[1],max_index[0]);
			return (max_index[1] + 64);
		}

		return 0;

}

void short_sync_corr(complex double *receive_data, int receive_data_length, int corr_length)
{
	int index = 0;
	complex double corr_result = 0.0;
	complex double corr_power = 0.0;
	int continus_corr = 0;

	while(index < receive_data_length)
	{
		corr_result = 0.0;
		corr_power = 0.0;
		int corr_index;
		for(corr_index = 0; corr_index < corr_length; corr_index++)
		{
			corr_result += receive_data[index + corr_index] * conj(receive_data[index + corr_index + 16]);
			corr_power += receive_data[index + corr_index + 16] * conj(receive_data[index + corr_index + 16]);
		}
		double complex an_result = cabs(corr_result)/corr_power;

		if(cabs(an_result) > 0.80 )
		{
			printf("[short]    index: %d  corr_result: ( %0.2f + %0.2fj )  %0.4f \n", index+16, creal(an_result), cimag(an_result), cabs(an_result));
			continus_corr++;

			if(continus_corr == 3)
			{
				continus_corr = 0;
				printf("Find the short peak index [%d] !........ \n", index+16);

				int long_index = long_sync_corr(&receive_data[index+16], 384, 64);
				if ( long_index >= 64 )
				{
					printf("[LONG] the start samp %d \n", long_index + index + 16);
					int tmp_index;
					for(tmp_index = 0; tmp_index < 4; tmp_index++)
					{
						printf("[ %0.6f + %0.6f ]  \n", creal(receive_data[long_index+index+16+tmp_index]), cimag(receive_data[long_index+index+16+tmp_index]));
					}

					index += 320;
				}

			}

		}
		else
		{
			if(continus_corr > 0)
				continus_corr = 0;
		}


		index++;
	}

}



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
		+1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0, 1.4719601443879746+1.4719601443879746j, 0.0, 0.0, 0.0,
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

	complex double tx_data[64] = {
		0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j,
		0.5+0.5j,0.5+0.5j,0.0+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.0j,0.5+0.5j,0.5-0.5j,
		0.5+0.5j,0.5+0.5j,0.0+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.0j,0.5+0.5j,0.5-0.5j,
		0.5-0.5j,0.5-0.5j,0.0+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.0j,0.5+0.5j,0.5-0.5j,
		0.5+0.5j,0.5-0.5j,0.0+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j,
		0.5+0.5j,0.5-0.5j,0.0+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j,
		0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j,
		0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j,0.5+0.5j,0.5+0.5j,0.5+0.5j,0.5-0.5j
	};

	complex double phy_sync_ofdm[400] = {0.0};

	fftw_plan fftwp = fftw_create_plan(FFT_SIZE, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_one(fftwp, (fftw_complex *)&short_sync[0], (fftw_complex *)&phy_sync_ofdm[FFT_CP_SIZE]);
	memcpy(&phy_sync_ofdm[0], &phy_sync_ofdm[64], FFT_CP_SIZE * sizeof(complex double));
	memcpy(&phy_sync_ofdm[80], &phy_sync_ofdm[0], 80* sizeof(complex double));

	fftw_one(fftwp, (fftw_complex *)&long_sync_1[0], (fftw_complex *)&phy_sync_ofdm[176]);
	memcpy(&phy_sync_ofdm[160], &phy_sync_ofdm[224],  FFT_CP_SIZE * sizeof(complex double));

	fftw_one(fftwp, (fftw_complex *)&long_sync_2[0], (fftw_complex *)&phy_sync_ofdm[256]);
	memcpy(&phy_sync_ofdm[240], &phy_sync_ofdm[304],  FFT_CP_SIZE * sizeof(complex double));

	fftw_one(fftwp, (fftw_complex *)&tx_data[0], (fftw_complex *)&phy_sync_ofdm[336]);
	memcpy(&phy_sync_ofdm[320], &phy_sync_ofdm[384],  FFT_CP_SIZE * sizeof(complex double));

	fftw_destroy_plan(fftwp);
	
	int i;
	for(i = 0; i < 400; i++)
	{
		phy_sync_ofdm[i] /= 64.0;
	}

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
		printf("( %0.3f + %0.3fj ) ", creal(phy_sync_ofdm[index]), cimag(phy_sync_ofdm[index]));
		if((index + 1)%8 == 0)
			printf("\n");
	}
	printf("\n");
	printf("the ifft result of short sync : \n");
	for(index; index < 160; index++)
	{
		printf("( %0.3f + %0.3fj ) ", creal(phy_sync_ofdm[index]), cimag(phy_sync_ofdm[index]));
		if((index + 1)%8 == 0)
			printf("\n");
	}
	printf("\n");

	printf("the ifft result of long sync : \n");
	for(index; index < 320; index++)
	{
		printf("( %0.3f + %0.3fj ) ", creal(phy_sync_ofdm[index]), cimag(phy_sync_ofdm[index]));
		if((index + 1)%8 == 0)
			printf("\n");
	}
	printf("\n");

	printf("the ifft result of data : \n");
	for(index; index < 400; index++)
	{
		printf("( %0.3f + %0.3fj ) ", creal(phy_sync_ofdm[index]), cimag(phy_sync_ofdm[index]));
		if((index + 1)%8 == 0)
			printf("\n");
	}
	printf("\n");
#endif

	complex double phy_sync_test[65536] = {0 + 0j};
	memcpy(&phy_sync_test[100], &phy_sync_ofdm[0], 400 * sizeof(complex double));
	short_sync_corr(phy_sync_test, 2016, 48);
	//long_sync_corr(phy_sync_test, 2016, 64);

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

	init_usrp(40e6, 20e6, 2.444e9, 60);
	create_tx_stream();
	while(1)
	{
		usrp_send((double *)phy_sync_test, 4096, 65536);
	}

}
