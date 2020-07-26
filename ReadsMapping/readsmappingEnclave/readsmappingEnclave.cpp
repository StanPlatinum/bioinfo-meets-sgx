#include "readsmappingEnclave_t.h"

#include "sgx_trts.h"
#include "sgx_tseal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sgx_tae_service.h>

#include <string>

#include "nw.h"
using namespace std;

static const uint16_t seeds_length = 22;
static const uint16_t overlap = 5;

char input_file[3000];
char reads[] = "TGCCACCTCTGTCCTCCCGCCTCTCTCTCACCTCCCGGTCCTCCCGCCTCTCACTGTCTGTCTCTTTGTTTCCAAGCGAACTGGCCAATGAGATCCCCGTGTGAGTAG";
char seed[22];
int inputfilesize = 0;

void readfiletobuffer(void)
{
	char *pos = input_file;  
	
    const int read_count = 3000;
    int ret_openfile;
	openfile(&ret_openfile);
	//MYDEBUG(2);
	if(ret_openfile == -1)     // all Idx file loaded
	{
		return; // load font file failed.
	}
	
	int ret_loadfont;
	loadFontFromFile(&ret_loadfont, pos);
	//MYDEBUG(ret_loadfont);
	
	inputfilesize = ret_loadfont;
	//MYDEBUG(inputfilesize);
}

uint32_t fib(uint32_t pn)
{
	//readfiletobuffer();
	
	//MYDEBUG(inputfilesize);
	int len = strlen(reads);
	/*
	TRACE(13, (uint64_t)reads);
	TRACE(13, (uint64_t)(&reads[2999]));
	TRACE(13, (uint64_t)&fib);
	TRACE(13, (uint64_t)&strlen);	
	TRACE(13, (uint64_t)&strncpy);
	
	TRACE(13, (uint64_t)input_file);	
	TRACE(13, (uint64_t)reads);
	TRACE(13, (uint64_t)seed);
	*/
	
	int i = 0;
	while(i < len - seeds_length + overlap)
	{
		//TRACE(13, (uint64_t)&overlap);
		//MYDEBUG(overlap);
		strncpy(seed, reads+i, seeds_length);
		char *pos = strstr(input_file, seed);
		
		i += seeds_length - overlap;
		
		
		//if(pos != NULL)
		{
			char tmpstr[120];
			
			//strncpy(tmpstr, pos-i, strlen(reads));
			string str1(reads);
			string str2(tmpstr);
			string seq_1_al;
			string seq_2_al;  // aligned result
			int score;
			nw(str1, str2, seq_1_al, seq_2_al, score, false); 
		}
		break;
	}
	
	return 1;
	//return (strstr(str1, str6)-str1);
}
