#include "sgx_urts.h"
#include "sgx_tseal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>

#include <time.h>
#define ONE_OVER_BILLION 1E-9
#define ONE_BILLION 1E9

#include "readsmappingEnclave_u.h"

char *ENCLAVE_DLL_FILE = "readsmappingEnclave.signed.so";

FILE *file; 
int openfile()
{
	file = fopen("genomeSeq.dat", "rb"); 
	//file = fopen("Ubuntu-M.ttf", "rb");
	if(!file)
	{	
		printf("Open file failed.\n");
		return -1;                   // I return an impossible value for a normal execution
	}
	printf("file opened.\n");
	return 0;
}

int loadFontFromFile(char *pFileBuffer)
{
	if(!file)     // open file failed, I assume that the file does not exist
	{
		printf("File not opened.\n");
//		file = fopen("SmallREF.fa.0.fm", "rb");
//		return -1;
	}
	int cnt = 0;
	
	if((cnt = fread(pFileBuffer, sizeof(char), 3000, file)) != 3000)
	{
		if(ferror(file))
		{
			printf("Error reading file.\n");
			fclose(file);
			exit(-1);
		}else if(feof(file))
		{
			printf("Reading file finished. Size: %d.\n", cnt);
		}
		return cnt;
	}

	return cnt;
}

void MYDEBUG(int out)
{
	printf("Running here: %d.\n", out);
}

void TRACE(uint32_t type, uint32_t addr)
{
	switch(type)
	{
	case 13:
		printf("address: %08x.\n", addr); break;
	case 0:
		printf("--- snpSet[n-1] address: %08x.\n", addr); break;
	case 1:
		printf("--- snpSet[0] address: %08x.\n", addr); break;
	case 2:
		printf("--- snpSet[n/2] address: %08x.\n", addr); break;
	case 3:
		printf("--- snpSet size: %d.\n", addr); break;
	case 4:
		printf("--- insert(...) address: %08x.\n", addr); break;
	case 5:
		printf("--- memcpy(...) address: %08x.\n", addr); break;
	case 6:
		printf("--- Charcot_marie_Tooth_Disease_Suspicion() address: %08x.\n", addr); break;
	case 7:
		printf("--- dpm_init() address: %08x.\n", addr); break;
	case 8:
		printf("--- nw_align() address: %08x.\n", addr); break;
	case 9:
		printf("--- F[][]: %08x.\n", addr); break;
	case 10:
		printf("--- T_bwt: %08x.\n", addr); break;
	case 11:
		printf("--- T_bwt->bitstring[..]: %08x.\n", addr); break;
	case 12:
		printf("--- T_bwt->occ: %08x.\n", addr); break;
	}
}

int main()
{
	printf("Main func called.\n");
	
	/* enclave setup. */
	sgx_enclave_id_t eid;
	sgx_status_t ret = SGX_ERROR_UNEXPECTED;
	sgx_launch_token_t token = {0};
	int updated = 0;

	ret = sgx_create_enclave(ENCLAVE_DLL_FILE, 1, &token, &updated, &eid, NULL);

	if(ret != SGX_SUCCESS)
	{
		printf("Create Enclave Failed. Error code: %d.\n", ret);
		return -1;
	}
	
	uint32_t result = 0;
	uint32_t pn = 1;
	struct timespec begin, end;
	
	int j, n = 1;
	clock_gettime(CLOCK_REALTIME, &begin);
	for(j = 0; j < n; j++)
	{
		fib(eid, &result, pn);
	}
	clock_gettime(CLOCK_REALTIME, &end);

   // Calculate time it took
	double accum = ( end.tv_sec - begin.tv_sec )*ONE_BILLION + ( end.tv_nsec - begin.tv_nsec );
	printf("Time: %lf\n", accum/(n+0.0));
	
	printf("result: %d\n", result);
	
	sgx_destroy_enclave(eid);
	return 0;
}
