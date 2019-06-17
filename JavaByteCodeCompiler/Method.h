
#ifndef METHOD_H
#define METHOD_H

#include <vector>
#include <iostream>

class Method
{
private:
	unsigned short accessFlags;
	std::string name;
	std::string descriptor;
	unsigned short maxStack;
	unsigned short maxLocals;
	std::vector<uint8_t> byteCodes;
	bool isVoid;
public:
	Method(unsigned short accessFlags, std::string name, std::string descriptor, unsigned short maxStack, unsigned short maxLocals, std::vector<uint8_t> byteCodes) :
		accessFlags(accessFlags), name(name), descriptor(descriptor), maxStack(maxStack), maxLocals(maxLocals), byteCodes(byteCodes), isVoid(true)
	{
	}
	~Method();
	std::string getName() const;
	std::string getDescriptor();
	//returns maxLocals
	unsigned short getMaxLocals();
	//returns the number of bytes in byteCodes
	int getByteCodeSize();
	//get the byte with the given index
	uint8_t getByte(int index);
	//get two bytes at given index and returns uint16 in big endian
	int16_t getTwoByte(int index);
	//get four bytes at given index and returns uint32 in big endian
	int32_t getFourByte(int index);
	//get/set isVoid
	bool getIsVoid();
	void setIsVoid(bool b);
};
#endif