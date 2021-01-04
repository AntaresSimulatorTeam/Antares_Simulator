
#ifndef MEASURE_TIME_H_
#define MEASURE_TIME_H_

class measure_time
{
public:
	measure_time() :
		conv_optim_1(0.),
		optim_1(0.),
		conv_optim_2(0.),
		optim_2(0.)
	{}


public:
	double conv_optim_1;
	double optim_1;
	double conv_optim_2;
	double optim_2;

};

#endif  // MEASURE_TIME_H_