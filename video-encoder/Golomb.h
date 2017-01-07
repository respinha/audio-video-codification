//
//  Golomb.h
//  CAV_T2
//
//  Created by Pedro Costa on 11/11/14.
//  Copyright (c) 2014 pmec. All rights reserved.
//

#ifndef GOLOMB_H
#define GOLOMB_H

#include <stdio.h>
#include "BitStream.h"

class Golomb {
public:
	Golomb(BitStream *bs, int m);
	~Golomb();

	bool isEfficient(int x);

protected:
	BitStream *bs;
	unsigned int m, b, l, h;
};

class GolombEncoder : public Golomb {
public:
	GolombEncoder(BitStream *bs, int m);
	~GolombEncoder();

	void encode(int e);
protected:

};

class GolombDecoder : public Golomb {
public:
	GolombDecoder(BitStream *bs, int m);
	~GolombDecoder();

	int decode();
protected:

};

#endif
