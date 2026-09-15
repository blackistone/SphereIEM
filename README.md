# SphereIEM
The file you probably need is ballers2.maxpat

Files for sphere control of IEM Ambisonics suite\
\
Experimental\
Takes two HAPP arcade trackballs as controllers\
IDs\
Left x: 0\
Left y: 1\
Right x: 2\
Right y: 3\
\
Serial output 9600bps\
format ID, INT_16 - ie, "2,52342"(\n terminator)\ 
\
Max takes serial in and sends for a couple IEM encoders\
Main working one is StereoEncoder in which\
Left x -> Azimuth\
Left y -> Elevation\
Right x -> Roll\
Right y -> Width\
\
OSC on port 1234
