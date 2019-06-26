#include<curlpp/cURLpp.hpp>
#include<curlpp/Easy.hpp>
#include<curlpp/Exception.hpp>
#include<curlpp/Options.hpp>
#include<iostream>
#include<fstream>
#include<list>
#include<string>
#include<unistd.h>
#include<vector>
#include<sstream>
struct processUsage
{
	int PID;
	std::string TTY;
	std::string TIME;
	std::string CMD;
};
//todo, make a log struct that logs everything in general
//use a JSON parser to do the dirty work
//send the JSON to the http 
void logger()
{
	std::ifstream myfile("/proc/stat");
	float _time[8];
	char placeholder[4];
	myfile>>placeholder;
	for(int i=0;i<8;i++)
	{
		myfile>>_time[i];
	}
	float cpu_use_percentage = 100 - _time[3]*100/( _time[0]+
													_time[1]+
													_time[2]+
													_time[3]+
													_time[4]+
													_time[5]+
													_time[6]);
	std::cout<<cpu_use_percentage<<"\n";
	myfile.close();
	
	unsigned long int memtot,memfree;
	std::string token;
    myfile.open("/proc/meminfo");
    while(myfile >> token) {
        if(token == "MemTotal:") {
            myfile >> memtot;
        }
		else if(token == "MemFree:")
		{
			myfile >> memfree;
		}
	}
	myfile.close();
	std::cout<<memtot-memfree<<"KiB"<<std::endl;
	
	
	FILE *RAM_processes = popen("ps -A --sort -rss | sed -n 2,11p","r");
	processUsage p_array[10];
	char *buf = new char[100];
	size_t bufsize = 90;
	int i=0;
	while(getline(&buf,&bufsize,RAM_processes) && i<10)
	{
		std::string placeholderstring(buf);
	 	std::istringstream iss(placeholderstring);
		iss>>p_array[i].PID>>p_array[i].TTY>>p_array[i].TIME>>p_array[i].CMD;
		i++;
	}
	for(int j =0;j<i;j++)
	{
		std::cout<<p_array[j].CMD<<"\n";
	}
	fclose(RAM_processes);
}
int main(int argc, char **argv)
{
	for(int i =0;i<10;i++){
	usleep(1000000);
	logger();
	}
	//double cpu_pc  = (cputime_final-cputime_init)/(uptime_final-uptime_init);
	//std::cout<<cpu_pc<<std::endl;

	//now that it has been logged, time to post.
	try{
	cURLpp::initialize(CURL_GLOBAL_ALL);
	curlpp::Easy easyhandle;
	
	std::list<std::string> header;
    header.push_back("Content-Type: application/json");


	easyhandle.setOpt(cURLpp::Options::Url("http://pastebin.org"));
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
	return 0;
}
