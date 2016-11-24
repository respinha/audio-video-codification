./bin/SpatialEncoder img.jpg encoded 8 > out1.txt
./bin/SpatialDecoder encoded 8 > out1.txt
vimdiff out1.txt out.txt
