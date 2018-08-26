BONUS_FLAGS=
producer.o : producer.cpp
	g++ ${BONUS_FLAGS} -c -I../../../include/mtsynth $^

consumer.o : consumer.c 
	gcc ${BONUS_FLAGS} -c -fPIC $^

prod_cons.o : prod_cons.c
	gcc ${BONUS_FLAGS} -c -fPIC $^

base.o : base.c
	gcc ${BONUS_FLAGS} -c $^

master.o : master.c
	gcc ${BONUS_FLAGS} -c -fPIC $^

ipc_cleanup.o : ipc_cleanup.c
	gcc ${BONUS_FLAGS} -c -fPIC $^

producer : producer.o ../../../bin/libmtsynth.a prod_cons.o
	g++ ${BONUS_FLAGS} $^ -o producer `pkg-config --cflags --libs pangocairo glib-2.0 opencv`

base : prod_cons.o base.o
	gcc ${BONUS_FLAGS} $^ -o base

master : prod_cons.o consumer.o master.o
	gcc -c -fPIC ${BONUS_FLAGS} $^ -o master

ipc_cleanup : prod_cons.o ipc_cleanup.o
	gcc ${BONUS_FLAGS} $^ -o ipc_cleanup

all : producer.o consumer.o prod_cons.o base.o master.o ipc_cleanup.o \
      producer base ipc_cleanup

clean :
	rm -f ./*.o
	rm -f ./*~
	rm -f ./producer ./ipc_cleanup ./base
