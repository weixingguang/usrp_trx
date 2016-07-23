#include "uhd/usrp/multi_usrp.hpp"	
#include "usrp.h"

// [ transmit sync sequence ]
uhd::usrp::multi_usrp::sptr usrp;

void init_usrp(double clock_rate, double tx_samp_rate, double tx_freq, double tx_gain)
{
	std::string args = " ";
	usrp = uhd::usrp::multi_usrp::make(args);

	usrp->set_clock_source("internal");
	usrp->set_master_clock_rate(clock_rate);
	std::cout << "set the usrp clock rate " << usrp->get_master_clock_rate() << "Hz" << std::endl;

	usrp->set_tx_rate(tx_samp_rate);
	std::cout << "set the usrp tx sample rate " << usrp->get_tx_rate() << "Hz" << std::endl;

	usrp->set_tx_freq(tx_freq);
	std::cout << "set the usrp tx center frequency  " << usrp->get_tx_freq() << "Hz" << std::endl;

	usrp->set_tx_gain(tx_gain);
	std::cout << "set the usrp tx gain  " << usrp->get_tx_gain() << std::endl;
}

uhd::tx_streamer::sptr tx_stream;
void create_tx_stream()
{
	std::string cpu_format = "fc64";
	std::string wire_format = "sc16";
	uhd::stream_args_t stream_args(cpu_format, wire_format);
	tx_stream = usrp->get_tx_stream(stream_args);

}

void usrp_send(double *data, int length)
{
	uhd::tx_metadata_t md;
	md.start_of_burst = false;
	md.end_of_burst = false;
	tx_stream->send(data, length, md);
	std::cout << "TX data.........." << std::endl;
}

