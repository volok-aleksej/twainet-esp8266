#ifndef DATA_MESSAGE_H
#define DATA_MESSAGE_H

class Deserializer
{
public:
    virtual bool deserialize(char* data, int& len) const = 0;
};

class Serializer
{
public:
    virtual bool serialize(char* data, int len) = 0;
};

class NamedMessage
{
public:
    virtual ~NamedMessage(){};
    
    virtual const char* GetName()const = 0;
};

class DataMessage
	: public NamedMessage
	, public Deserializer
	, public Serializer
{
public:
	virtual ~DataMessage(){};

	virtual void onMessage(){};
	virtual bool serialize(char* data, int len){return false;}
	virtual bool deserialize(char* data, int& len) const{return false;}
};

#endif	// DATA_MESSAGE_H
