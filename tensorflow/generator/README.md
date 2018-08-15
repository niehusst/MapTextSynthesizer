# Generator function

These files allow for simple streaming of data from the MapTextSynthesizer into a Tensorflow program.  

`data_synth.py` includes the `data_generator` function, which can be used directly as a generator function from which to derive a `tf.data.Dataset` object using `tf.data.Dataset.from_generator`.

Example of this use case can be seen in the `get_dataset` function of `maptextsynth.py`.

`maptextsynth.py` is structured specifically for use by `pipeline.py` in the following branch <fill-in-with-final-repo-location>.