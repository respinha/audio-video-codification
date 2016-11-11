all:
	g++ test_predictor.cpp -o tp.o -lsndfile
	g++ test_reverse_predictor.cpp -o tr.o -lsndfile
	g++ encoder.cpp -o e.o
	rm metadata
	rm out*
	rm encoded
readbit:
	g++ readbits.cpp -o r.o
	rm encoded
writebit:
	g++ writebits.cpp -o w.o
	rm out*
encode:
	g++ encoder.cpp -o e.o
	rm encoded1
	rm out*
