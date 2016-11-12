all:
	g++ test_predictor.cpp -o tp.o -lsndfile
	g++ test_reverse_predictor.cpp -o tr.o -lsndfile
	g++ test_predictor2.cpp -o tp2.o -lsndfile	
	g++ test_reverse_predictor2.cpp -o tr2.o -lsndfile
	g++ test_predictor3.cpp -o tp3.o -lsndfile	
	g++ test_reverse_predictor3.cpp -o tr3.o -lsndfile
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
