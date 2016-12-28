//
//  Golomb.cpp
//  CAV_T2
//
//  Created by Pedro Costa on 11/11/14.
//  Copyright (c) 2014 pmec. All rights reserved.
//

#include "Golomb.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>

Golomb::Golomb(BitStream *bs, int m) {
	this->bs = bs;
	this->m = m;

	this->b = (int)ceil(log2(m));
	this->l = pow(2, b) - m;
	this->h = m - l;
}

Golomb::~Golomb() {

}

GolombEncoder::GolombEncoder(BitStream *bs, int m) : Golomb(bs, m) {

}

GolombEncoder::~GolombEncoder() {

}

void GolombEncoder::encode(int e) {

	unsigned int q, r;

	q = e / m;
	r = e % m;

	/*
     Check if m is efficitent
        if yes      . Rice code
        otherwise   . Golomb code
	 */
	if (r < l){
		bs->writeNBits(r, b - 1);
	} else {
		bs->writeNBits(r + l, b);
	}

	for (unsigned int i = 0; i < q; i++) {
		bs->writeBit(1);
	}

	bs->writeBit(0);
}

GolombDecoder::GolombDecoder(BitStream *bs, int m) : Golomb(bs, m) {

}

GolombDecoder::~GolombDecoder() {

}

int GolombDecoder::decode() {
	unsigned int q = 0, r, t;
	int x;

	x = bs->readNBits(b - 1);
	if(x == -1)
		return -1;

	if((unsigned)x < l){
		r = x;
	}
	else{
		int y = bs->readNBits(1);
		if(y == -1)
			return -1;
		x = ( x << 1 ) | y;
		r = x - l;
	}

	int tmp;
	while ((tmp = bs->readBit() == 1)) {
		q ++;
	}

	if(tmp == -1)
		return -1;

	t = q * m + r;

	//std::cout << "Residue: " << t << "\n";
	return t;
}
