
#include "NBTWriter.h"

#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
#define TwinStackSize 128
namespace NBT{


bool isSysBE()
{
	short S=1;char *temp=(char*)&S;
	if(temp[0]==0&&temp[1]==1)
	return true;
	return false;
}

void disp(const char*Str)
{

	return;
}


NBTWriter::NBTWriter(const char*path)
{
	allowEmergencyFill=true;
	isBE=isSysBE();
	ByteCount=0;
	File=new fstream(path,ios::out|ios::binary);
		char temp[3]={10,0,0};
		File->write(temp,3);ByteCount+=3;
	isOpen=true;
	for(top=0;top<TwinStackSize;top++)
	{
		CLA[top]=114;
		Size[top]=114514;
	}

	top=-1;

}

NBTWriter::~NBTWriter()
{
	if(isOpen)close();
	delete File;
	return;
}

unsigned long long NBTWriter::close()
{
	if(isOpen)
	{
		if(!isEmpty())emergencyFill();

	File->write(&idEnd,1);ByteCount+=1;
	File->close();}
	return ByteCount;
}

bool NBTWriter::isEmpty()
{
	return (top==-1);
}

bool NBTWriter::isFull()
{
	return top>=TwinStackSize;
}

bool NBTWriter::isListFinished()
{
	return (Size[top]<=0);
}

char NBTWriter::readType()
{
	return CLA[top];
}

bool NBTWriter::isInCompound()
{
	return isEmpty()||(readType()==0);
}

bool NBTWriter::isInList()
{
	return !isInCompound();
}

bool NBTWriter::typeMatch(char typeId)
{
	return(readType()==typeId);
}

void NBTWriter::endList()
{
	if(isInList()&&isListFinished())
	{
		pop();
		disp("������һ���б�");
		elementWritten();
	}
	return;
}

void NBTWriter::pop()
{
	if(!isEmpty()){
	top--;
	disp("popped");}
	else
	disp("popʧ�ܣ���ջ���ܼ���pop");
	return;
}

void NBTWriter::push(char typeId,int size)
{
	if(!isFull())
	{
		top++;
		CLA[top]=typeId;
		Size[top]=size;
	}
	else
	disp("pushʧ�ܣ���ջ���ܼ���push");
	return;
}

void NBTWriter:: elementWritten()
{
	if(isInList()&&!isListFinished())
	Size[top]--;
	if(isListFinished())
	endList();
	return;
}

int NBTWriter::writeEnd()
{
	File->write(&idEnd,1);
	return 1;
}

int NBTWriter::writeCompound(const char*Name)
{
	if (!isOpen)return 0;
	int ThisCount=0;short realNameL=strlen(Name),writeNameL=realNameL;
	if(!isBE)
	{
		IE2BE(writeNameL);
	}
	if(isInCompound())
	{
		File->write(&idCompound,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeNameL,sizeof(short));ThisCount+=sizeof(short);
		File->write(Name,realNameL);ThisCount+=realNameL;
		push(idEnd,0);
		ByteCount+=ThisCount;
		return ThisCount;
	}

	if (isInList()&&typeMatch(idCompound))//д��Ϊ�б��е�tag
	{
		//writeNothing
		push(idEnd,0);
		ByteCount+=ThisCount;
		return ThisCount;
	}

	ByteCount+=ThisCount;
	return ThisCount;

}

int NBTWriter::endCompound()
{
	if(!isOpen)return 0;
	int ThisCount=0;
	if(isInCompound())
	{
		ThisCount+=writeEnd();
		pop();//This pop means end a Compound
		elementWritten();//���pop֮���Ƿ���List��
		ByteCount+=ThisCount;
		disp("������һ���ļ���");
		return ThisCount;
	}
	disp("�������ﲻ�ܽ����ļ���");
	return ThisCount;
}

int NBTWriter::writeListHead(const char*Name,char TypeId,int listSize)
{
	if(!isOpen){disp("ʧ�ܣ��ļ�δ��");return 0;}
	int ThisCount=0;short realNameL=strlen(Name),writeNameL=realNameL;
	int writeListSize=listSize;//listSize->readListSize
	if(!isBE){IE2BE(writeNameL);IE2BE(writeListSize);}

	if(isInCompound())
	{
		File->write(&idList,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeNameL,sizeof(short));ThisCount+=sizeof(short);
		File->write(Name,realNameL);ThisCount+=realNameL;
		File->write(&TypeId,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeListSize,sizeof(int));ThisCount+=sizeof(int);
		push(TypeId,listSize);
		disp("�ɹ����ļ����д���List");
		ByteCount+=ThisCount;
		if(listSize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}

	if(isInList()&&typeMatch(idList))
	{
		File->write(&TypeId,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeListSize,sizeof(int));ThisCount+=sizeof(int);
		push(TypeId,listSize);
		disp("�ɹ���List�д���List");
		ByteCount+=ThisCount;
		if(listSize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}
	disp("ʧ�ܣ�δ�ܴ���List");
	return ThisCount;

}

int NBTWriter::writeByte(const char*Name,char value)
{
	return writeSingleTag(idByte,Name,value);
}

int NBTWriter::writeShort(const char*Name,short value)
{
	return writeSingleTag(idShort,Name,value);
}

int NBTWriter::writeInt(const char*Name,int value)
{
	return writeSingleTag(idInt,Name,value);
}

int NBTWriter::writeLong(const char*Name,long long value)
{
	return writeSingleTag(idLong,Name,value);
}

int NBTWriter::writeFloat(const char*Name,float value)
{
	return writeSingleTag(idFloat,Name,value);
}

int NBTWriter::writeDouble(const char*Name,double value)
{
	return writeSingleTag(idDouble,Name,value);
}

int NBTWriter::writeLongArrayHead(const char*Name,int arraySize)
{
	if(!isOpen){disp("ʧ�ܣ��ļ�δ��");return 0;}
	int ThisCount=0;short realNameL=strlen(Name),writeNameL=realNameL;
	int writeArraySize=arraySize;//arraSize->readArraySize
	if(!isBE){IE2BE(writeNameL);IE2BE(writeArraySize);}

	if(isInCompound())
	{
		File->write(&idLongArray,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeNameL,sizeof(short));ThisCount+=sizeof(short);
		File->write(Name,realNameL);ThisCount+=realNameL;
		//File->write(&idLong,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeArraySize,sizeof(int));ThisCount+=sizeof(int);
		push(idLong,arraySize);
		disp("�ɹ����ļ����д���LongArray");
		ByteCount+=ThisCount;
		if(arraySize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}

	if(isInList()&&typeMatch(idLongArray))
	{
		//File->write(&idLong,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeArraySize,sizeof(int));ThisCount+=sizeof(int);
		push(idLong,arraySize);
		disp("�ɹ���List�д���LongArray");
		ByteCount+=ThisCount;
		if(arraySize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}
	disp("ʧ�ܣ�δ�ܴ���LongArray");
	return ThisCount;
}

int NBTWriter::writeByteArrayHead(const char*Name,int arraySize)
{
	if(!isOpen){disp("ʧ�ܣ��ļ�δ��");return 0;}
	int ThisCount=0;short realNameL=strlen(Name),writeNameL=realNameL;
	int writeArraySize=arraySize;//arraSize->readArraySize
	if(!isBE){IE2BE(writeNameL);IE2BE(writeArraySize);}

	if(isInCompound())
	{
		File->write(&idByteArray,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeNameL,sizeof(short));ThisCount+=sizeof(short);
		File->write(Name,realNameL);ThisCount+=realNameL;
		//File->write(&idLong,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeArraySize,sizeof(int));ThisCount+=sizeof(int);
		push(idByte,arraySize);
		disp("�ɹ����ļ����д���ByteArray");
		ByteCount+=ThisCount;
		if(arraySize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}

	if(isInList()&&typeMatch(idByteArray))
	{
		//File->write(&idLong,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeArraySize,sizeof(int));ThisCount+=sizeof(int);
		push(idByte,arraySize);
		disp("�ɹ���List�д���ByteArray");
		ByteCount+=ThisCount;
		if(arraySize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}
	disp("ʧ�ܣ�δ�ܴ���ByteArray");
	return ThisCount;
}

int NBTWriter::writeIntArrayHead(const char*Name,int arraySize)
{
	if(!isOpen){disp("ʧ�ܣ��ļ�δ��");return 0;}
	int ThisCount=0;short realNameL=strlen(Name),writeNameL=realNameL;
	int writeArraySize=arraySize;//arraySize->readArraySize
	if(!isBE){IE2BE(writeNameL);IE2BE(writeArraySize);}

	if(isInCompound())
	{
		File->write(&idIntArray,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeNameL,sizeof(short));ThisCount+=sizeof(short);
		File->write(Name,realNameL);ThisCount+=realNameL;

		File->write((char*)&writeArraySize,sizeof(int));ThisCount+=sizeof(int);
		push(idInt,arraySize);
		disp("�ɹ����ļ����д���IntArray");
		ByteCount+=ThisCount;
		if(arraySize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}

	if(isInList()&&typeMatch(idIntArray))
	{
		//File->write(&idLong,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeArraySize,sizeof(int));ThisCount+=sizeof(int);
		push(idInt,arraySize);
		disp("�ɹ���List�д���IntArray");
		ByteCount+=ThisCount;
		if(arraySize==0)elementWritten();//��ʼд����б��ͬ�����д����б�
		return ThisCount;
	}
	disp("ʧ�ܣ�δ�ܴ���IntArray");
	return ThisCount;
}

int NBTWriter::writeString(const char*Name,const char*value)
{
	if(!isOpen){disp("ʧ�ܣ��ļ�δ��");return 0;}
	int ThisCount=0;
	short realNameL=strlen(Name),writeNameL=realNameL;
	short realValL=strlen(value),writeValL=realValL;
	if(!isBE){IE2BE(writeNameL);IE2BE(writeValL);}

	if(isInCompound())
	{
		File->write(&idString,sizeof(char));ThisCount+=sizeof(char);
		File->write((char*)&writeNameL,sizeof(short));ThisCount+=sizeof(short);
		File->write(Name,realNameL);ThisCount+=realNameL;
		File->write((char*)&writeValL,sizeof(short));ThisCount+=sizeof(short);
		File->write(value,realValL);ThisCount+=realValL;
		disp("�ɹ����ļ����д���String");
		ByteCount+=ThisCount;
		elementWritten();
		return ThisCount;
	}

	if(isInList()&&typeMatch(idString))
	{
		File->write((char*)&writeValL,sizeof(short));ThisCount+=sizeof(short);
		File->write(value,realValL);ThisCount+=realValL;
		disp("�ɹ���List�д���String");
		ByteCount+=ThisCount;
		elementWritten();
		return ThisCount;
	}
	disp("ʧ�ܣ�δ�ܴ���String");
	return ThisCount;
}

char NBTWriter::CurrentType()
{
	return readType();
}

int NBTWriter::emergencyFill()
{
	if(!allowEmergencyFill)return 0;
	if(isEmpty())return 0;
	int ThisCount=0;
	while(!isEmpty())
	{
		if(isInCompound()){ThisCount+=endCompound();continue;}
		//���������Ȼ��List�����б���
		switch (readType())
		{
			case idEnd:

				continue;
			case idByte:

				ThisCount+=writeByte("autoByte",114);
				continue;
			case idShort:

				ThisCount+=writeShort("autoShort",514);
				continue;
			case idInt:

				ThisCount+=writeInt("autoInt",114514);
				continue;
			case idLong:

				ThisCount+=writeLong("autoLong",1919810);
				continue;
			case idFloat:

				ThisCount+=writeFloat("autoFloat",114.514f);
				continue;
			case idDouble:

				ThisCount+=writeDouble("autoDouble",1919810.114514);
				continue;
			case idByteArray:

				ThisCount+=writeByteArrayHead("autoByteArray",1);
				continue;
			case idString:

				ThisCount+=writeString("autoString","FuckYUUUUUUUUUUUUUUUUU!");
				continue;
			case idList:

				ThisCount+=writeListHead("autoList",NBT::idInt,1);
				continue;
			case idCompound:

				ThisCount+=writeCompound("autoCompound");
				continue;
			case idIntArray:

				ThisCount+=writeIntArrayHead("autoIntArray",1);
				continue;
			case idLongArray:

				ThisCount+=writeLongArrayHead("autoLongArray",1);
				continue;
			default:
				continue;
		}
	}
	ThisCount+=writeString("TokiNoBug'sWarning","There's sth wrong with ur NBTWriter, the file format is completed automatically instead of manually.");
	disp("������ȫ��ʽ�ɹ�");
	return ThisCount;
}

//NameSpace NBT ends here
}


