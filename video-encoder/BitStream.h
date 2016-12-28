#ifndef _BITSTREAM_
#define _BITSTREAM_

#include <fstream>

/* 
 *@class BitStream to read and write bits on a file.
 *@author Antonio J. R. Neves (an@ua.pt)
 */
class BitStream
{
	public:
		BitStream();
		/** Constructor 
			\param fileName The string containing the name of the encoded file.
			\param mode The string w or r. */
		BitStream(std::string fileName, std::string m);

		/** Destructor */ 
		~BitStream();

		/** Method to close bitStream */
		void close();

		/** Method to write a bit in the bitstream.
			\param value The bit to be written. */
		void writeBit(int value);

		/** Method to read a bit from the bitstream.
			\return 0 or 1 depending on the bit. */
		int readBit();

		/** Method to write several bits into the bitstream.
			\param nb The number of bits to write in the bitstream.
			\param value The value to be written*/
		void writeNBits(int value, int nb);

		/** Method to read several bits from the bitstream.
			\param nb The number of bits to read from the bitstream.
			\return The value contained in the n bits. */
		int readNBits(int nb);

		/** return count */
		inline int getCount(){return count;}

	private:
		/** Buffer to store the data. */
		unsigned char buffer;

		/** Position on the buffer where the bit is being accessed. */
		int pos;

		/** File pointer */
		std::fstream fp;

		/** Read or Write mode */
		int mode;
		
		/** Count bytes */
		int count;
};

#endif
