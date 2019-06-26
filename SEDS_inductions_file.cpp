#include"include_src.h"
#include"classes.cpp"
//todo, make a log struct that logs everything in general
//use a JSON parser to do the dirty work
//send the JSON to the http 
void logger(loggerObject &data_logger)
{
	data_logger.GetCpuUsage();
	data_logger.GetUsedRam();
	data_logger.GetMaxMemProcesses();
}
void sendpost()
{
	try{
	cURLpp::initialize(CURL_GLOBAL_ALL);
	curlpp::Easy easyhandle;
	
	std::list<std::string> header;
    	header.push_back("Content-Type: application/json");


	easyhandle.setOpt(cURLpp::Options::Url("http://a/a"));
	easyhandle.setOpt(new cURLpp::Options::HttpHeader(header));
	easyhandle.setOpt(new curlpp::options::PostFields(("a)")));
	easyhandle.setOpt(new curlpp::Options::WriteStream(&std::cout));
	easyhandle.perform();
	}
	catch(curlpp::LogicError e)
	{
		std::cout<<e.what();
	}
	catch(curlpp::RuntimeError r)
	{
		std::cout<<r.what();
	}
	std::cout<<"posted"<<std::endl;
}
int main(int argc, char **argv)
{
	loggerObject data_logger[10];
	int i = 1;
	std::future<void> result;
	for(;;i++){
	auto start = std::chrono::high_resolution_clock::now();
	logger(data_logger[i%10]);
	if(i%10 == 0 && i != 0)
	{
		result = (std::async(std::launch::async,sendpost));
		std::cout<<"posting request:"<<i/9<<std::endl;
	}
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long mu_sec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	usleep(1000000 - mu_sec);
	std::cout<<i<<std::endl;
	if(i == 60)
	{
		break;
	}
	}
	//double cpu_pc  = (cputime_final-cputime_init)/(uptime_final-uptime_init);
	//std::cout<<cpu_pc<<std::endl;

	//now that it has been logged, time to post.
	
	return 0;
}
