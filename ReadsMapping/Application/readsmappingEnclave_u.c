#include "readsmappingEnclave_u.h"
#include <errno.h>

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

static sgx_status_t SGX_CDECL readsmappingEnclave_loadFontFromFile(void* pms)
{
	ms_loadFontFromFile_t* ms = SGX_CAST(ms_loadFontFromFile_t*, pms);
	ms->ms_retval = loadFontFromFile(ms->ms_pFileBuffer);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL readsmappingEnclave_openfile(void* pms)
{
	ms_openfile_t* ms = SGX_CAST(ms_openfile_t*, pms);
	ms->ms_retval = openfile();

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL readsmappingEnclave_MYDEBUG(void* pms)
{
	ms_MYDEBUG_t* ms = SGX_CAST(ms_MYDEBUG_t*, pms);
	MYDEBUG(ms->ms_out);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL readsmappingEnclave_TRACE(void* pms)
{
	ms_TRACE_t* ms = SGX_CAST(ms_TRACE_t*, pms);
	TRACE(ms->ms_type, ms->ms_addr);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[4];
} ocall_table_readsmappingEnclave = {
	4,
	{
		(void*)readsmappingEnclave_loadFontFromFile,
		(void*)readsmappingEnclave_openfile,
		(void*)readsmappingEnclave_MYDEBUG,
		(void*)readsmappingEnclave_TRACE,
	}
};
sgx_status_t fib(sgx_enclave_id_t eid, uint32_t* retval, uint32_t pn)
{
	sgx_status_t status;
	ms_fib_t ms;
	ms.ms_pn = pn;
	status = sgx_ecall(eid, 0, &ocall_table_readsmappingEnclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

