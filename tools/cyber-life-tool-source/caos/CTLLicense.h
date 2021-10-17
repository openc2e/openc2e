// Description: Methods to generate and validate a license key
// Author: Rich Smith
// Date: 23/10/98
// Details:
// License Key Format:
//
// PPP-NNNNC-SSSSC-C
//   PPP = Product Code
//   NNNN = Username ASCII's summed, inverted, in hex (C == Checkdigit)
//   SSSS = Serial Number, inverted, in hex (C == Checkdigit)
//   CC = Check Digit on NOT'd SSSS, then checksum (byte, anded 0xff)
//     -- Checksum and Checkdigit calculation: sum all digits, MOD 26, as alphabet letter

#ifndef CTL_KEY_H
#define CTL_KEY_H

#include <WTYPES.H>

// Visual Basic and C calling convention
#define PREFIX_CVB __stdcall

// Generate a license key (PPP-NNNNC-SSSSC-C)
void PREFIX_CVB CTL_GenerateKey(char* lk_KeyBuffer,    // Out
							 char* lk_RealName,     // In
					 		 long lk_ProductCode,   // In
							 long lk_SerialNumber); // In


// Return true/false if valid/invalid license key supplied
bool PREFIX_CVB CTL_ValidKey(char* user_name, // In
						  long product_no, // In
						  char* user_key); // In

// Return 1/0 if valid/invalid license key supplied.
// Calling convention here is to be compatible with InstallShield.
// Usage example within InstallShield:
//
//	svRegistryAddr = "Software\\CyberLife Technology\\CAOS\\1.0";
// 	szDLL      = "CTLKey.dll";
//  szFunction = "CTL_VALID_KEY_IN_REGISTRY";
//  lvArgNum    = 1;
//
// 	nResult = CallDLLFx (szDLL, szFunction, lvArgNum, svRegistryAddr);
//
// Note that CallDLLFx calls the function below

LONG  PREFIX_CVB  CTL_ValidKeyInRegistry(HWND /*hwnd*/, LPLONG lpIValue, LPSTR lpszValue);

// return true/false if valid/invalid license key supplied.
// Calling convention here is for application
// Usage example within ProjectApp::InitInstance():
//
//	char*  registry_location = "Software\\CyberLife Technology\\CAOS\\1.0";
//	LONG product_no = 102;
//
//	if (!CTL_ValidKeyInRegistryApp(product_no, registry_location)) {
//		return false;
//	}


bool PREFIX_CVB CTL_ValidKeyInRegistryApp(LONG product_no, char* registry_location);
	
#endif // CTL_KEY_H

