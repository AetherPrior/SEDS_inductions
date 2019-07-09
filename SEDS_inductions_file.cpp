#include"include_src.h"
#include"classes.cpp"
void logger(loggerObject &data_logger)
{
	data_logger.GetCpuUsage();
	data_logger.GetMaxMemProcesses();
}
int makeGetRequest(std::string website_url,std::string usr_string)
{
	curlpp::Easy get_requester;
	std::ofstream outfile("Get_request.json");
	Json::Value request_json;
	std::string OK_message = std::string("No entries received from identifier ")
							+usr_string
							+std::string(" so far. You may use this identifier if you wish.");

	get_requester.setOpt(new curlpp::options::Url(website_url.c_str()));
	get_requester.setOpt(new curlpp::options::WriteStream( &outfile ) );
	get_requester.perform();
	outfile.close();
	
	std::ifstream infile("Get_request.json");
	infile >> request_json;
	long http_code = curlpp::infos::ResponseCode::get(get_requester);
	try{std::cout<<std::string(request_json["Message"].asCString())<<std::endl;}
	catch (Json::LogicError e)
	{
		std::cout<<e.what()<<std::endl;
		std::cout<<"Choose another Identifier. Maybe this is already taken"<<std::endl;
		return 1;
	}

	if(http_code != 200)
	{
		std::cout<<"\nRequest failed with error code: "<<http_code<<std::endl;
		return -1;
		}
	
	else if( std::string(request_json["Message"].asCString()) != OK_message)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}
void sendPost(std::string website_url)
{
	const char *url = website_url.c_str();
	std::cout<<url<<std::endl;
	std::ifstream logfile("./logger.json",std::ios::ate|std::ios::binary);
	unsigned int size;
	char *buf;

	if(!logger)
	{
		std::cerr<<"Unable to open file\n";
		logfile.close();
	}

	else
	{
	 	size = logfile.tellg(); //find the size of the logfile, without trailing eof	
		logfile.close();
	 	logfile.open("./logger.json",std::ios::binary);
	 	buf = new char[size];
	 	logfile.read(buf,size);
		//buf[size-1] = '\0';
		/*
		std::ofstream log2file("./logger3.json",std::ios::binary);
		log2file.write(buf,size);
		log2file.close();
	 	*/
		logfile.close();
	}
	try{
		cURLpp::initialize(CURL_GLOBAL_ALL);
		curlpp::Easy post_json;
		std::list<std::string> header;
    		header.push_back("Content-Type: application/json");

		post_json.setOpt(cURLpp::Options::Url(url));
		//http://jsonplaceholder.typicode.com/posts
		
		post_json.setOpt(new cURLpp::Options::HttpHeader(header));
		post_json.setOpt(new curlpp::options::PostFields(buf));
		post_json.setOpt(new curlpp::Options::WriteStream(&std::cout));
		post_json.perform();
		//std::cout<<buf<<std::endl;
		long http_code = curlpp::infos::ResponseCode::get(post_json);
		if(http_code == 200)
		{
			std::cout<<"\nRequest made successfully"<<std::endl;
		}
		else
		{
			std::cout<<"\nRequest failed with error code: "<<http_code<<std::endl;
		}
	}
	catch(curlpp::LogicError e)
	{
		std::cerr<<std::endl<<e.what()<<std::endl;
	}
	catch(curlpp::RuntimeError r)
	{
		std::cerr<<std::endl<<r.what()<<std::endl;
	}
}

void jsonify(loggerObject data_logger[10],std::string append_name)
{
	Json::Value val;
	for(int num_second =0;num_second<10;num_second++)
	{
		val["name"] = append_name.c_str();
		val["data_points"][num_second]["cpu_usage"] = data_logger[num_second].cpu_use_percentage;
		for(int num_process =0;num_process<10;num_process++)
		{
			val["data_points"][num_second]["processes"][num_process]["name"] = data_logger[num_second].p_array[num_process].name;
			val["data_points"][num_second]["processes"][num_process]["ram_usage"] = data_logger[num_second].p_array[num_process].ram_usage;
		}
	}
	std::ofstream logfile("logger.json",std::ios::out);
	
	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	writer->write(val, &logfile);
	std::cout << std::endl;
	
	/*
	std::ofstream logfile2("logger2.json");	
	Json::StyledStreamWriter s;
	s.write(logfile2,val);
	logfile2.close();
	*/

	logfile.close();
}
int main(int argc, char **argv)
{
	
	if(argc != 2)
	{
		std::cout<<"USAGE: ./SEDS <Identifier>"<<std::endl;
		return 0;
	}
	std::string website_url = std::string("http://fathomless-thicket-66026.herokuapp.com/");
	std::string append_name = std::string(argv[1]);
	website_url = website_url + append_name;
	//Firstly, check if the username is taken
	int get_result = makeGetRequest(website_url,append_name);
	if(get_result != 0)
	{
		std::cout<<"\nInvalid Username or get request failed"<<std::endl;
		return 0;
	}
	loggerObject data_logger[10];
	std::vector<std::future<void>> result; //global var for async to work
	
	for(int seconds = 0;seconds<=60;seconds++)
	{
		std::cout<<seconds<<std::endl;
		auto start = std::chrono::high_resolution_clock::now();
		
		logger(data_logger[seconds%10]);
		//data_logger[seconds%10].Output();
		if(seconds%10 == 0 && seconds != 0)
		{
			jsonify(data_logger,append_name);
			result.push_back(std::async(std::launch::async,sendPost,website_url));
			std::cout<<"posting request:"<<seconds/10<<std::endl;
		}
	
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long mu_sec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		usleep(1000000 - mu_sec);
	
	}
	
	return 0;
}
