Ananke
======
A C++ based graph for manipulating and connecting various audio node processors to generate audio output. Built with the JUCE framework. 

The project aims to create a modular synthesis environment to allow rapid prototyping of ideas.

Wishlist:
Extensive DSP library, possibly incorporate 3rd party libs or audio languages? Csound?
An elegant solution to supplying and manipulating control data
 - Possibly include a form of timeline component
 - Extensive use of macro control to manipulate multiple parameters
Graph needs to deal well with many visual components i.e not a rats nest of connections
 - Possibly have a grid based graph instead of free roaming, lock connections to tracks instead of direct lines
 - Split the graph into audio and control data routing. Multilayered graph?
Allow mixed time and frequency domain processing
- Possibly have a sub graph when moving to the frequency domain
- Graph must cope with audio latency well
