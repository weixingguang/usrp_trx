#include <uhd/usrp/multi_usrp.hpp>
#include<stdlib.h>
using namespace std;

int main(int argc, char *argv[])
{
	cout<<"UHD TX"<<endl;
	string args = ""; //usrp args;

	uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(args);
	size_t chan = usrp->get_tx_num_channels();
	
	//set the clock_reference
	string ref = "internal"; 
	usrp->set_clock_source(ref);

	//usrp->set_tx_subdev_spec();

	//set and get the tx_rate
	double rate = 5e6;
	usrp->set_tx_rate(rate);
	usrp->get_tx_rate(rate);

	//set and get the freq
	double freq = 2.6e9;
	uhd::tune_request_t tune_request(freq);
	tune_request.args = uhd::device_addr_t("mode_n=integer");
	usrp->set_tx_freq(tune_request, 0);
	usrp->get_tx_freq(0);
	
	//set and get the rf gain
	double gain = 1;
	usrp->set_tx_gain(gain,0);
	usrp->get_tx_gain(0);

	//set and get the analog frontend filter bandwidth
	double bw = 1e6;
	usrp->set_tx_bandwidth(bw,0);
	usrp->get_tx_bandwidth(0);

	//set the antenna
	string ant;
	usrp->set_tx_antenna(ant,0);

	//create a transmit streamer
	string cpu_format, wire_format;
	uhd::stream_args_t stream_args(cpu_format,wire_format);
	uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);

	uhd::tx_metadata_t md;
	md.start_of_burst = false;
	md.end_of_burst = false;

	int length = 1e6;
	char *buff = (char *)malloc(length * sizeof(char));
	for(int i = 0; i < length; i++)
		buff[i] = i%2 == 0 ? '0' : '1';
	md.end_of_burst = '\0';
	tx_stream->send(buff,length,md);
		

	return 0;
}
