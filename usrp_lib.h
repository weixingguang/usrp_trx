#ifndef _INCLUDE_USRP_H_
#define _INCLUDE_USRP_H_
void init_usrp(double clock_rate, double tx_samp_rate, double tx_freq, double tx_gain);
void create_tx_stream();
void usrp_send(double *data, int length);
#endif
