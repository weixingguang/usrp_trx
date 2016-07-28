void short_sync_corr(complex double *receive_data, int receive_data_length, int corr_length)
{
	int index = 0;
	complex double corr_result = 0.0;
	complex double corr_power = 0.0;

	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	while(index < receive_data_length)
	{
		corr_result = 0.0;
		corr_power = 0.0;
		int corr_index;
		for(corr_index = 0; corr_index < corr_length; corr_index++)
		{
			corr_result += receive_data[index + corr_index] * conj(receive_data[index + corr_index + 16]);
			//corr_power += receive_data[index + corr_index] * conj(receive_data[index + corr_index]);
			corr_power += receive_data[index + corr_index + 16] * conj(receive_data[index + corr_index + 16]);
		}
		double an = cabs(corr_result);
		double complex an_result = an/corr_power;

		if(cabs(an_result) > 0.75 )
			printf("[short]    index: %d  corr_result: ( %0.2f + %0.2fj )  %0.4f \n", index, creal(an/corr_power), cimag(an/corr_power), cabs(an_result));
		//printf("index: %d  corr_result: ( %0.2f + %0.2fj ) \n", index, creal(corr_result), cimag(corr_result));
		index++;
		//usleep(50000);
	}
	gettimeofday(&te, NULL);
	double timespan = (te.tv_sec - ts.tv_sec) * 1000.0 + (te.tv_usec - ts.tv_usec) / 1000.0;
	printf("[short] timespan = %0.5f ms \n", timespan);
}

double complex long_sync_store[64] = {
	10.00 -0.00j ,  0.33 + 7.70j ,  2.54 -7.11j ,  -6.20 -5.30j ,  1.35 + 1.78j ,  -3.83 + 5.61j ,  -7.37 -3.53j ,  2.45 + 6.79j , 
	6.24 -1.66j ,  -3.41 -0.26j ,  0.06 -7.36j ,  8.76 + 3.03j ,  1.57 -3.75j ,  -3.75 + 0.96j ,  -1.44 + 10.28j ,  -7.63 + 0.26j , 
	4.00 -4.00j ,  -2.36 -6.29j ,  -3.66 + 2.52j ,  8.40 -4.17j ,  5.26 + 5.91j ,  -4.45 -0.90j ,  -3.86 + 5.20j ,  3.61 + 1.40j , 
	-2.24 -9.66j ,  7.80 + 1.06j ,  -8.15 -1.31j ,  -4.80 + 4.74j ,  -0.18 + 3.44j ,  5.88 -7.37j ,  5.87 + 6.78j ,  -0.79 -6.25j , 
	-10.00 + 0.00j ,  -0.79 + 6.25j ,  5.87 -6.78j ,  5.88 + 7.37j ,  -0.18 -3.44j ,  -4.80 -4.74j ,  -8.15 + 1.31j ,  7.80 -1.06j , 
	-2.24 + 9.66j ,  3.61 -1.40j ,  -3.86 -5.20j ,  -4.45 + 0.90j ,  5.26 -5.91j ,  8.40 + 4.17j ,  -3.66 -2.52j ,  -2.36 + 6.29j , 
	4.00 + 4.00j ,  -7.63 -0.26j ,  -1.44 -10.28j ,  -3.75 -0.96j ,  1.57 + 3.75j ,  8.76 -3.03j ,  0.06 + 7.36j ,  -3.41 + 0.26j , 
	6.24 + 1.66j ,  2.45 -6.79j ,  -7.37 + 3.53j ,  -3.83 -5.61j ,  1.35 -1.78j ,  -6.20 + 5.30j ,  2.54 + 7.11j ,  0.33 -7.70j 
};

void long_sync_corr(complex double *receive_data, int receive_data_length, int corr_length)
{
	int index = 0;
	complex double corr_result = 0.0;
	complex double corr_power = 0.0;
	struct timeval ts, te;
	gettimeofday(&ts, NULL);
	while(index < receive_data_length)
	{
		corr_result = 0.0;
		corr_power = 0.0;
		int corr_index;
		for(corr_index = 0; corr_index < corr_length; corr_index++)
		{
			corr_result += receive_data[index + corr_index] * conj(long_sync_store[corr_index]);
			//corr_power += receive_data[index + corr_index] * conj(receive_data[index + corr_index]);
			corr_power += long_sync_store[corr_index] * conj(long_sync_store[corr_index]);
		}
		double an = cabs(corr_result);
		double complex an_result = an/corr_power;

		if(cabs(an_result) > 0.75)
			//printf("[long]   index: %d  corr_result: ( %0.2f + %0.2fj ) \n", index, creal(an/corr_power), cimag(an/corr_power));
		printf("index: %d  corr_result: ( %0.2f + %0.2fj ) \n", index, creal(an_result), cimag(an_result));

		index++;
	}
	gettimeofday(&te, NULL);
	double timespan = (te.tv_sec - ts.tv_sec) * 1000.0 + (te.tv_usec - ts.tv_usec) / 1000.0;
	printf("[long] timespan = %0.5f ms \n", timespan);
}

short_sync_corr(phy_sync_test, 930, 16);
long_sync_corr(phy_sync_test, 930, 64);

