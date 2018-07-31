# Compile the shared library
libmtsynth.so:
	$(MAKE) -C samples 

# Compile a sample C++ synthesizer program 
sample:
	$(MAKE) $@ -C samples

# Prevent errors from occuring if a file were named 'clean'
.PHONY: clean

# Clean rule for getting rid of stray object files
clean:
	rm -f src/*.o *~ core* src/*~ samples/*.o samples/*~
	rm -rf bin
