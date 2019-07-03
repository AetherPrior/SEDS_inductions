struct processUsage
{
	float ram_usage;
	std::string name;
};
class loggerObject
{
	private:
	int num_processes;
	public:
	float cpu_use_percentage;
	//unsigned long mem_used;
	processUsage p_array[10];
	void GetCpuUsage()
	{
	std::ifstream my_file("/proc/stat");
	float _time[8];
	char placeholder[4];
	my_file>>placeholder;
	for(int i=0;i<8;i++)
	{
		my_file>>_time[i];
	}
	cpu_use_percentage = 100 - _time[3]*100/( _time[0]+
													_time[1]+
													_time[2]+
													_time[3]+
													_time[4]+
													_time[5]+
													_time[6]);

	my_file.close();
	
	}
	/* 
	void GetUsedRam()
	{
		std::ifstream my_file;
		unsigned long int mem_tot,mem_free;
	std::string token;
    my_file.open("/proc/meminfo");
    while(my_file >> token) {
        if(token == "MemTotal:") {
            my_file>> mem_tot;
        }
		else if(token == "MemFree:")
		{
			my_file>> mem_free;
		}
	}
	my_file.close();
	mem_used = mem_tot-mem_free;
	
	}*/
	void GetMaxMemProcesses()
	{
	FILE *RAM_processes = popen("ps -a -o %mem,args --sort -rss | sed -n 2,11p","r");
	char *buf = new char[101];
	size_t buf_size = 100;
	num_processes=0;
	while(getline(&buf,&buf_size,RAM_processes) && num_processes<10)
	{
		std::string placeholderstring(buf);
	 	std::istringstream iss(placeholderstring);
		iss>>p_array[num_processes].ram_usage
		>>p_array[num_processes].name;
		num_processes++;
	}
	
	fclose(RAM_processes);
		}
	void Output()
	{
			std::cout<<cpu_use_percentage<<"\n";
			//std::cout<<mem_used<<"KiB"<<std::endl;
			for(int j =0;j<num_processes;j++)
	{
		std::cout<<p_array[j].ram_usage<<"\t"<<p_array[j].name<<"\n";
	}
	}
};
