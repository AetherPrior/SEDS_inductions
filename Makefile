seds_make: SEDS_inductions_file.cpp
	g++ SEDS_inductions_file.cpp -lcurl -lcurlpp -ljsoncpp -lpthread -o SEDS
