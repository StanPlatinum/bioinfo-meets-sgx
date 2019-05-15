#include "readsmappingEnclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */

#include <errno.h>
#include <string.h> /* for memcpy etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)


typedef struct ms_fib_t {
	uint32_t ms_retval;
	uint32_t ms_pn;
} ms_fib_t;

typedef struct ms_loadFontFromFile_t {
	int ms_retval;
	char* ms_pFileBuffer;
} ms_loadFontFromFile_t;

typedef struct ms_openfile_t {
	int ms_retval;
} ms_openfile_t;

typedef struct ms_MYDEBUG_t {
	int ms_out;
} ms_MYDEBUG_t;

typedef struct ms_TRACE_t {
	uint32_t ms_type;
	uint32_t ms_addr;
} ms_TRACE_t;

static sgx_status_t SGX_CDECL sgx_fib(void* pms)
{
	ms_fib_t* ms = SGX_CAST(ms_fib_t*, pms);
	sgx_status_t status = SGX_SUCCESS;

	CHECK_REF_POINTER(pms, sizeof(ms_fib_t));

	ms->ms_retval = fib(ms->ms_pn);


	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[1];
} g_ecall_table = {
	1,
	{
		{(void*)(uintptr_t)sgx_fib, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[4][1];
} g_dyn_entry_table = {
	4,
	{
		{0, },
		{0, },
		{0, },
		{0, },
	}
};


sgx_status_t SGX_CDECL loadFontFromFile(int* retval, char* pFileBuffer)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_pFileBuffer = 3000;

	ms_loadFontFromFile_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_loadFontFromFile_t);
	void *__tmp = NULL;

	ocalloc_size += (pFileBuffer != NULL && sgx_is_within_enclave(pFileBuffer, _len_pFileBuffer)) ? _len_pFileBuffer : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_loadFontFromFile_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_loadFontFromFile_t));

	if (pFileBuffer != NULL && sgx_is_within_enclave(pFileBuffer, _len_pFileBuffer)) {
		ms->ms_pFileBuffer = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_pFileBuffer);
		memset(ms->ms_pFileBuffer, 0, _len_pFileBuffer);
	} else if (pFileBuffer == NULL) {
		ms->ms_pFileBuffer = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(0, ms);

	if (retval) *retval = ms->ms_retval;
	if (pFileBuffer) memcpy((void*)pFileBuffer, ms->ms_pFileBuffer, _len_pFileBuffer);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL openfile(int* retval)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_openfile_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_openfile_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_openfile_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_openfile_t));

	status = sgx_ocall(1, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL MYDEBUG(int out)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_MYDEBUG_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_MYDEBUG_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_MYDEBUG_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_MYDEBUG_t));

	ms->ms_out = out;
	status = sgx_ocall(2, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL TRACE(uint32_t type, uint32_t addr)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_TRACE_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_TRACE_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_TRACE_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_TRACE_t));

	ms->ms_type = type;
	ms->ms_addr = addr;
	status = sgx_ocall(3, ms);


	sgx_ocfree();
	return status;
}

