#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

namespace binbuff
{

class Buffer;

class Serializable
{
public:
	Serializable() = default;
	virtual ~Serializable() = default;


	virtual void serialize(Buffer &buffer) const = 0;
	virtual void deserialize(Buffer &buffer) = 0;
};

}
#endif
