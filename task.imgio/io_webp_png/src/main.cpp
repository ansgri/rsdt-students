#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <libwebp/webp/decode.h>
int main()
{
	FILE * pFile;
	long lSize;
	uint8_t *buffer;
	size_t result;
	
	pFile = fopen ( "1.webp" , "rb" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	buffer = (uint8_t*) malloc (sizeof(uint8_t)*lSize);
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

	// copy the file into the buffer:
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

	/* the whole file is now loaded in the memory buffer. */

	// terminate
	fclose (pFile);
	free (buffer);
	return 0;
}
