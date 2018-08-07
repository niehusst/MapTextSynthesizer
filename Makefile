# Compile the shared library
libmtsynth.so:
	$(MAKE) -C samples libmtsynth.so

# Compile the static library
libmtsynth.a:
	$(MAKE) $@ -C samples libmtsynth.a

# Compile a sample C++ synthesizer program 
cpp_sample:
	$(MAKE) $@ -C samples cpp_sample

# Compile a sample C++ synthesizer program with static library 
cpp_sample_static:
	$(MAKE) $@ -C samples cpp_sample_static

# Compile a Python sample


# Prevent errors from occuring if a file were named 'clean'
.PHONY: clean

# Clean rule for getting rid of stray files
clean:
	$(MAKE) -C samples clean
	rm -f *~ core*
	rm -rf bin

