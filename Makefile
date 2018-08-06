# Compile the shared library
libmtsynth.so:
	$(MAKE) -C samples 

# Compile the static library
libmtsynth.a:
	$(MAKE) $@ -C samples 

# Compile a sample C++ synthesizer program 
cpp_sample:
	$(MAKE) $@ -C samples

# Compile a sample C++ synthesizer program with static library 
cpp_sample_static:
	$(MAKE) $@ -C samples

# Prevent errors from occuring if a file were named 'clean'
.PHONY: clean

# Clean rule for getting rid of stray object files
clean:
	rm -f src/*.o *~ core* src/*~ samples/*.o samples/*~ samples/a.out
	rm -rf bin
