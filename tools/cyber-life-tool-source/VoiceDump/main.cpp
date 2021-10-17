#include <string>
#include <stdio.h>

int main(int argc, char* argv[])
{
	if (argc == 1)
		return -1;
	std::string filename = argv[1];
	FILE* f;
	f = fopen(argv[1],"rb");
	printf("TAG \"%s\"\n",filename.c_str());
	int looper;
	printf("\"DefaultLanguage\"\n");
	for(looper=0;looper<32;looper++)
	{
		unsigned int name;
		fread(&name,4,1,f);
		if (looper < 4)
			name = 0;
		if (name == 0)
			printf("\"\"\n");
		else
		{
			char temp[5] = "xxxx";
			temp[0] = name & 255;
			name = name >> 8;
			temp[1] = name & 255;
			name = name >> 8;
			temp[2] = name & 255;
			name = name >> 8;
			temp[3] = name & 255;
			printf("\"%s\"\n",temp);
		}
		fread(&name,4,1,f);
		printf("\"%d\"\n",name);
	}

	printf("TAG \"DefaultLanguage\"\n");
	for(int i=0;i<3;i++)
		for(int j=0;j<27;j++)
		{
			unsigned int value;
			fread(&value,4,1,f);
			printf("\"%8x\"\n",value);
		}
	fclose(f);
	return 0;
}

