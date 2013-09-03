all : co



co : main.cpp schedule.cpp
	g++ -g -Wall -o $@ $^ -I./

clean : 
	rm co	
