/*
 *  Created by Bertram Bourdrez on Mon Jun  7 2004.
 *  Copyright (c) 2004 Bertram Bourdrez. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include <stdio.h>
#include <netinet/in.h>
#include <stdint.h>

#define swapEndianLong(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                   (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                   (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                   (((uint32_t)(A) & 0x000000ff) << 24))

int main(int argc, char ** argv) {
	size_t len = 0;
	int little, big, line = 1;
	char endianness;
	FILE * fptr;
	char * buf = (char *) malloc(1024);
	if(argc != 3) {
		fprintf(stderr, "Usage: %s [b|l] inputfile\n", argv[0]);
		exit(1);
	}
	endianness = tolower(argv[1][0]);
	if(strlen(argv[1]) != 1 || endianness != 'b' && endianness != 'l') {
		fprintf(stderr, "Invalid byte order specification!\n");
		exit(1);
	}
	fptr = fopen(argv[2], "r");
	if(! fptr) {
		perror("fopen");
		exit(1);
	}
	while(getline(&buf, &len, fptr) != -1) {
		if(strlen(buf) != 4) {
			if(! (strlen(buf) == 5 && buf[4] == '\n')) {
				fprintf(stderr, "Length of line %i is %i, not 4!\n", line, strlen(buf));
				exit(1);
			}
		}
		little = htonl(*((unsigned int *)buf));
		big = swapEndianLong(little);
		printf("%lx\n", (endianness == 'b' ? big : little));
		line++;
	}
	return 0;
}
