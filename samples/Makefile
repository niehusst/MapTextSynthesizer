library : text_synthesizer_py.cpp
	g++ -shared text_synthesizer_py.cpp -I/usr/include/python2.7 `pkg-config --cflags --libs mtsynth` -lpthread -lboost_python -lpython2.7 -fPIC -o mtsynth.so
