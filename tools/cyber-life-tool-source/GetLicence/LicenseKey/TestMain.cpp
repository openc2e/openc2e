// Description: Test for license key generation and validation
// Author: Rich Smith
// Date: 23/10/98


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CTL_License.h"

#define BUFFER_SIZE 128

void main(int argc, char *argv[])
	{
	char lh_RealName[BUFFER_SIZE] = "";
	char lh_Temp[BUFFER_SIZE] = "";
	char lh_LicenseKey[BUFFER_SIZE] = "";
	long lh_ProductCode = 0;
	long lh_SerialNumber = 0;
	int loop = 0;
	//
	//  license.exe "user name" product_code serial_number
	//	Process arguments:
	if (argc != 4)
		{
		printf("Usage: Licence 'User Name' ProductCode SerialNumber\n");
		return;
		}

	strcpy(lh_RealName, argv[1]);
	lh_ProductCode = atol(argv[2]);
	lh_SerialNumber = atol(argv[3]);

	CTL_GenerateKey(lh_LicenseKey, lh_RealName, lh_ProductCode, lh_SerialNumber);

	printf("\nLicense issued in the name: "
		"[%s] P-Code: [%ld] SN#: [%ld] Key: [%s]\n",
		lh_RealName, lh_ProductCode, lh_SerialNumber,lh_LicenseKey);


	// Validate the key using simple test

	if (CTL_ValidKey(lh_RealName, lh_ProductCode, lh_LicenseKey)) {
		printf("SimpleTest Key valid\n\n");
	}
	else {
		printf("SimpleTest Key invalid\n\n");
	}

	// Validate the key using Registry info
	// Assumes that CAOS has been installed - reflected with entries in the 
	// registry

	LONG argn = 1;
	LPSTR registry_location = "Software\\CyberLife Technology\\CAOS\\1.0";

	if (CTL_ValidKeyInRegistry(NULL, &argn, registry_location)) {
		printf("RegistryTest Key valid\n\n");
	}
	else {
		printf("RegistryTest Key invalid\n\n");
	}

	LONG product_no = 102;

	if (CTL_ValidKeyInRegistryApp(product_no, registry_location)) {
		printf("RegistryTestApp Key valid\n\n");
	}
	else {
		printf("RegistryTestApp Key invalid\n\n");
	}

	return;
	}


