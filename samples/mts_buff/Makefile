library : textsynthinterface.cpp
	g++ -shared textsynthinterface.cpp -I/usr/include/python2.7 -I../../include/mtsynth -lpthread -lboost_python -lpython2.7 -fPIC -o maptextsynth_interface.so

example : simple_module.cpp
	g++ -shared simple_module.cpp -lboost_python -lpython2.7 -fPIC -o hello_ext.so

mts_faster : textsynthinterface.cpp
	g++ textsynthinterface.cpp `pkg-config --cflags --libs mtsynth` -shared -fPIC -lpthread -o libmts_faster.so
