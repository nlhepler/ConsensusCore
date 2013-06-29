# Overview:

ConsensusCore is a C++ library containing several consensus- and
variant- calling algorithms used in circular consensus sequencing
(CCS) analysis and in genomic consensus/variant analyses. Several of
the algorithms are based on generic multiple sequence alignment
methods, while the Quiver method is based on a PacBio-specific error
model.  ConsensusCore exposes language bindings to C# and Python,
enabling integration into the Pipeline and secondary analysis
workflows. **Note that ConsensusCore is a library, and does not contain
any driver programs.** Most users will be interested in installing
``GenomicConsensus``, which contains the ``variantCaller.py`` program.
See [installation instructions](https://github.com/PacificBiosciences/GenomicConsensus/blob/master/doc/HowToQuiver.rst).


## Installing in your python environment

To install in your virtualenv (or your global python environment, if
you wish), run

    $ python setup.py install --swig=/path/to/swig_executable --boost=/boost/root


## Installing without python environment

If you want to play with just the C++ library or the C# bindings, you
can build like so:

    $ make              # build C++ library ...
    $ make tests        # ... and test it.
    $ make csharp       # build CSharp bindings
    $ make csharp-demo  # run a C# demo


## Documentation

The best place to get started is the BrownBag2012 presentation under
the doc/ folder.  Then, to see how to write code using ConsensusCore,
take a look at quiver/demo.py under the (separate) GenomicConsensus
project.


[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/936a243e6d9d77202b4c6428a255ac6f "gi
