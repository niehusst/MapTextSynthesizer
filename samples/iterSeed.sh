#!/bin/bash

#program to run MTS cpp sample and change the seed in the config file for each run

seed=1
while [ 1 == 1 ]; do
	echo "Press q to quite"
	read line
	if [ $line == "q" ] 
	then
		break
	fi
	#run mts
	echo $seed
	./mts_sample_shared
	#edit config file for next run giving seed as command line arg
	./edit_config $seed
	#update seed
	((seed++))
	#remove config.txt and rename newConfig.txt to config.txt
	rm config.txt
	mv newConfig.txt config.txt
done
