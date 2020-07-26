#ifndef READSMAPPINGENCLAVE_U_H__
#define READSMAPPINGENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOADFONTFROMFILE_DEFINED__
#define LOADFONTFROMFILE_DEFINED__
int SGX_UBRIDGE(SGX_NOCONVENTION, loadFontFromFile, (char* pFileBuffer));
#endif
#ifndef OPENFILE_DEFINED__
#define OPENFILE_DEFINED__
int SGX_UBRIDGE(SGX_NOCONVENTION, openfile, (void));
#endif
#ifndef MYDEBUG_DEFINED__
#define MYDEBUG_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, MYDEBUG, (int out));
#endif
#ifndef TRACE_DEFINED__
#define TRACE_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, TRACE, (uint32_t type, uint32_t addr));
#endif

sgx_status_t fib(sgx_enclave_id_t eid, uint32_t* retval, uint32_t pn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
