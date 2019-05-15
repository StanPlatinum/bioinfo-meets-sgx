#ifndef READSMAPPINGENCLAVE_T_H__
#define READSMAPPINGENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


uint32_t fib(uint32_t pn);

sgx_status_t SGX_CDECL loadFontFromFile(int* retval, char* pFileBuffer);
sgx_status_t SGX_CDECL openfile(int* retval);
sgx_status_t SGX_CDECL MYDEBUG(int out);
sgx_status_t SGX_CDECL TRACE(uint32_t type, uint32_t addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
