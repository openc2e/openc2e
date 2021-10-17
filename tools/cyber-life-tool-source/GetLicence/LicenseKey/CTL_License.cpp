// Description: Methods to generate and validate a license key
// Author: Rich Smith
// Date: 23/10/98

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "CTL_License.h"
#include "FlexibleRegistry.h"

#define BUFFER_SIZE 128
// #define DEBUG_CTL_LICENSE

// Local (private) method prototypes
//------------------------------------------------------------------

// Return check digit
static char _CheckDigit(long);

// Return serial number given license key
static LONG _ExtractSerialNo(char* license_key);

// Return product number from registry
static LONG _ExtractProductNo(char* license_key);

// Extract numbers from license string
static LONG _ExtractNumber(char* license_key, int start, int length);

// Store product no for reference during application license key validation
static LONG _productNo;


//-------------------------------------------------------------------

LONG  PREFIX_CVB  CTL_ValidKeyInRegistry(HWND /*hwnd*/, LPLONG argn, LPSTR registry_location)
{
	assert(1 == *argn); // Only one argument - registry location

	LONG rc = 0; // Failure


	FlexibleRegistry registry;

	char user_data[BUFFER_SIZE];
	char license_data[BUFFER_SIZE];
	DWORD user_size = BUFFER_SIZE * sizeof(char);
	DWORD license_size = user_size;
	DWORD type; // ignored

	// Extract user, license from registry HKEY_LOCAL_MACHINE

	if (registry.GetValue(license_data, license_size, "License", registry_location, type, HKEY_LOCAL_MACHINE) &&
		registry.GetValue(user_data, user_size, "User", registry_location, type, HKEY_LOCAL_MACHINE)) {
		rc = 1; // Success
	}

	// Validate license key

	if (rc) { 
		_productNo = _ExtractProductNo(license_data);
		if (!CTL_ValidKey(user_data, _productNo, license_data)) {
			rc = 0; // Failure
		}
	}

	return rc;
}

//-------------------------------------------------------------------

bool PREFIX_CVB CTL_ValidKeyInRegistryApp(LONG product_no, char* registry_location)
{
	bool rc = false;
	LONG argn = 1;

	if (1 == CTL_ValidKeyInRegistry(NULL, &argn, registry_location) &&
		product_no == _productNo) {
		rc = true;
	}

	return rc;
}

//-------------------------------------------------------------------

void PREFIX_CVB CTL_GenerateKey(char* lk_KeyBuffer,   // Out
					 char* lk_RealName,    // In
					 long lk_ProductCode,  // In
					 long lk_SerialNumber) // In
{
	long lk_Multiplier;
	char lk_UserCD, lk_SerialCD, lk_CD1, lk_CD2;
	long lk_UserCode = 0x6768, lk_CheckSum = 0;
	//
	// Generate user-code:
	while (*lk_RealName)
		{
		lk_Multiplier = *lk_RealName++;
		lk_Multiplier -= 0x41;
		lk_UserCode += lk_Multiplier * 8;
		}

	lk_UserCode += (lk_ProductCode * 2);
	lk_UserCode = lk_UserCode >> 1;				// Fiddle with user-code
	
	//
	// Now generate check digits:
	lk_UserCD = _CheckDigit(lk_UserCode);
	lk_SerialCD = _CheckDigit(lk_SerialNumber);

	lk_CD1 = _CheckDigit(lk_SerialNumber ^ 0xffffffff);
	//
	// Checksum what we have so far:
	lk_UserCode = lk_UserCode % 10000;		// Modulo 10,000 for user-code
	sprintf(lk_KeyBuffer, "%03ld-%04ld%c-%04ld%c-%c",
		lk_ProductCode, lk_UserCode, lk_UserCD, lk_SerialNumber, lk_SerialCD, lk_CD1);
	while (*lk_KeyBuffer)
		lk_CheckSum += *lk_KeyBuffer++;
	lk_CD2 = _CheckDigit(lk_CheckSum);		// Fetch final check-digit

	*lk_KeyBuffer++ = lk_CD2;
	*lk_KeyBuffer = 0;						// Zero terminate
}

//-------------------------------------------------------------------

bool PREFIX_CVB CTL_ValidKey(char* user_name, // In
				  long product_no, // In
				  char* user_key)  // In
{
	// Extract serial number from user's license key
	long serial_no = _ExtractSerialNo(user_key);

	// Generate a new license key with supplied details
	char generated_key[BUFFER_SIZE];
	CTL_GenerateKey(generated_key, user_name, product_no, serial_no);

#ifdef DEBUG_CTL_LICENSE
	printf("User's name = %s, product number = %d\n", user_name, product_no);
	printf("User's key = %s\nGen key = %s\n", user_key, generated_key);
#endif

	// Compare user's key with generated key
	return (0 == strcmp(user_key, generated_key) ? true : false);
}

//-------------------------------------------------------------------

long _ExtractSerialNo(char* license_key)
{
	int start = 10;
	int length = 4;

	LONG serial_no = _ExtractNumber(license_key, start, length);

#ifdef DEBUG_CTL_LICENSE
	printf("Serial no = %d\n", serial_no);
#endif

	return serial_no;
}

//-------------------------------------------------------------------

char _CheckDigit(long cd_ValueIn)
{
	char cd_WorkBuffer[128], *cd_Buffer;
	char cd_ConverterBuffer[128];
	long cd_ReturnValue = 0;

	sprintf(cd_WorkBuffer, "%ld", cd_ValueIn);
	cd_Buffer = &cd_WorkBuffer[0];
	
	while (*cd_Buffer)
		{
		sprintf(cd_ConverterBuffer, "%c", *cd_Buffer++);

		cd_ReturnValue += (atol(cd_ConverterBuffer) * 2);
		}

	cd_ReturnValue = (cd_ReturnValue % 26);
	return (cd_ReturnValue + 0x41);
}

//-------------------------------------------------------------------

LONG _ExtractProductNo(char* license_key)
{
	int start = 0;
	int length = 3;

	LONG product_no = _ExtractNumber(license_key, start, length);

#ifdef DEBUG_CTL_LICENSE
	printf("Product no = %d\n", product_no);
#endif

	return product_no;
}

//-------------------------------------------------------------------

LONG _ExtractNumber(char* license_key, int start, int length)
{
	char temp[BUFFER_SIZE];

	// Skip to start position in license key
	license_key += start;

	// Copy next 'length' characters into serial string
	strncpy(temp, license_key, length);
	temp[length] = 0; // Null terminate

	return atol(temp);
}




