# Compile all the source code (there is no executable!)
all:
		$(MAKE) -C src

# Compile a sample C++ synthesizer program 
sample:
		$(MAKE) -C samples

# Prevent errors from occuring if a file were named 'clean'
.PHONY: clean

# Clean rule for getting rid of stray object files
clean:
		rm -f src/*.o *~ core* src/*~ samples/*.o samples/*~
