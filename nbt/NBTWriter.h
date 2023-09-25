#ifndef NBT_WRITER_H
#define NBT_WRITER_H

#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
#define TwinStackSize 128
namespace NBT{
	const char idEnd=0;
	const char idByte=1;
	const char idShort=2;
	const char idInt=3;
	const char idLong=4;
	const char idFloat=5;
	const char idDouble=6;
	const char idByteArray=7;
	const char idString=8;
	const char idList=9;
	const char idCompound=10;
	const char idIntArray=11;
	const char idLongArray=12;


    template <typename T>
    void IE2BE(T &Val)
    {
        if (sizeof(T)<=1)return;
        char *s=(char*)&Val;
        for(short i=0;i*2+1<sizeof(T);i++)
            swap(s[i],s[sizeof(T)-1-i]);
    }

    template <typename T>
    T IE2BE(T *Val)
    {
        if (sizeof(T)<=1)return *Val;

        T Res=*Val;
        char *s=(char*)&Res;
        for(short i=0;i*2+1<sizeof(T);i++)
            swap(s[i],s[sizeof(T)-1-i]);
        return Res;
    }


bool isSysBE();

void disp(const char*Str);


class NBTWriter
{
	private:
		//Vars
		bool isOpen;
		bool isBE;
		fstream *File;
		unsigned long long ByteCount;
		short top;
		char CLA[TwinStackSize];
		int Size[TwinStackSize];
		//StackFun
		void pop();
		void push(char typeId,int size);
		bool isEmpty();
		bool isFull();
		char readType();
		char readSize();
		//WriterFun
		void elementWritten();
		void endList();
		int writeEnd();
		bool typeMatch(char typeId);
		//AutoFiller
		int emergencyFill();
	public:
		//Construct&deConstruct
		NBTWriter(const char*path);
		~NBTWriter();
		//Vars
		bool allowEmergencyFill;
		//WriterFun
		bool isInList();
		bool isInCompound();
		unsigned long long close();
		bool isListFinished();
		char CurrentType();
		//WriteAbstractTags
		template <typename T>
		int writeSingleTag(char typeId,const char*Name,T value);
		//int writeArrayHead(char typeId,const char*Name,int arraySize);
		//WriteSpecialTags
		int writeCompound(const char*Name);
		int writeListHead(const char*Name,char typeId,int listSize);
		int endCompound();
		int writeString(const char*Name,const char*value);
		//WriteRealSingleTags
		int writeByte(const char*Name,char value);
		int writeShort(const char*Name,short value);
		int writeInt(const char*Name,int value);
		int writeLong(const char*Name,long long value);
		int writeFloat(const char*Name,float value);
		int writeDouble(const char*Name,double value);
		//WriteArrayHeads
		int writeLongArrayHead(const char*Name,int arraySize);
		int writeByteArrayHead(const char*Name,int arraySize);
		int writeIntArrayHead(const char*Name,int arraySize);
};


template <typename T>
int NBTWriter::writeSingleTag(char typeId,const char*Name,T value)
{
	if (!isOpen){disp("�ļ�û�д򿪣�����д��");return 0;}
	int ThisCount=0;short realNameL=strlen(Name),writeNameL=realNameL;
	if(!isBE)
	{
		IE2BE(writeNameL);IE2BE(value);//value����Ҫ��ȡ��ֻ��Ҫд�� 
	}
	
	
	if (isInCompound())//д��Ϊ������Tag 
	{
		File->write(&typeId,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeNameL,sizeof(short));ThisCount+=sizeof(short);
		File->write(Name,realNameL);ThisCount+=realNameL;
		File->write((char*)&value,sizeof(T));ThisCount+=sizeof(T);
	}
	
	if (isInList()&&typeMatch(typeId))//д��Ϊ�б��е�tag 
	{
		File->write((char*)&value,sizeof(T));ThisCount+=sizeof(T);
		elementWritten();
		
	}
	ByteCount+=ThisCount;
	if(!ThisCount)disp("writeSingleTag����д��ʧ��"); 
	return ThisCount;
}


//NameSpace NBT ends here
}

#endif