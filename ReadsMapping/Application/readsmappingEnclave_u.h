#ifndef READSMAPPINGENCLAVE_U_H__
#define READSMAPPINGENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_satus_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

int SGX_UBRIDGE(SGX_NOCONVENTION, loadFontFromFile, (char* pFileBuffer));
int SGX_UBRIDGE(SGX_NOCONVENTION, openfile, ());
void SGX_UBRIDGE(SGX_NOCONVENTION, MYDEBUG, (int out));
void SGX_UBRIDGE(SGX_NOCONVENTION, TRACE, (uint32_t type, uint32_t addr));

sgx_status_t fib(sgx_enclave_id_t eid, uint32_t* retval, uint32_t pn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
