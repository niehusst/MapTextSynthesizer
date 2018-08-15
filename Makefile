# A recursive makefile that calls make rules in samples/makefile

# Compile the shared library
shared:
	$(MAKE) -C samples libmtsynth.so

# Compile the static library
static:
	$(MAKE) -C samples libmtsynth.a

# Compile a sample C++ synthesizer program 
cpp_sample:
	$(MAKE) -C samples cpp_sample

# Compile a sample C++ synthesizer program with static library 
cpp_sample_static:
	$(MAKE) -C samples cpp_sample_static

# Compile a Python Ctypes sample, uses shared library
python_ctypes: tf_lib

# Compile program to list fonts available on the system
list_fonts:
	$(MAKE) -C samples list_fonts

# Compile shared library and MTS generator interface for use in TF
tf_lib:
	$(MAKE) -C tensorflow/generator lib

# Prevent errors from occuring if a file were named 'clean'
.PHONY: clean

# Clean rule for getting rid of stray files
clean:
	$(MAKE) -C samples clean
	$(MAKE) -C tensorflow/generator clean
	rm -f *~ core* \#*#
	rm -rf bin

