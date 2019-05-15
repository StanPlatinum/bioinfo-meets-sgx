#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <asm/traps.h>
#include <asm/desc_defs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/context_tracking.h>     /* exception_enter(), ...       */


/*
 *  Copyright (C) 1995  Linus Torvalds
 *  Copyright (C) 2001, 2002 Andi Kleen, SuSE Labs.
 *  Copyright (C) 2008-2009, Red Hat Inc., Ingo Molnar
 */
#include <linux/sched.h>		/* test_thread_flag(), ...	*/
#include <linux/kdebug.h>		/* oops_begin/end, ...		*/
#include <linux/module.h>		/* search_exception_table	*/
#include <linux/bootmem.h>		/* max_low_pfn			*/
#include <linux/kprobes.h>		/* NOKPROBE_SYMBOL, ...		*/
#include <linux/mmiotrace.h>		/* kmmio_handler, ...		*/
#include <linux/perf_event.h>		/* perf_sw_event		*/
#include <linux/hugetlb.h>		/* hstate_index_to_shift	*/
#include <linux/prefetch.h>		/* prefetchw			*/
#include <linux/context_tracking.h>	/* exception_enter(), ...	*/
#include <linux/uaccess.h>		/* faulthandler_disabled()	*/

typedef void (*do_page_fault_handler_t)(struct pt_regs *, unsigned long);
extern void default_do_page_fault(struct pt_regs *regs, unsigned long error_code);
extern do_page_fault_handler_t do_page_fault_handler;

unsigned long count = 0;
unsigned long previous_instruction_fault = 0;
unsigned long p_previous_instruction_fault = 0;
unsigned long pp_previous_instruction_fault = 0;
unsigned long ppp_previous_instruction_fault = 0;
unsigned long pppp_previous_instruction_fault = 0;

unsigned long previous_data_fault = 0;
unsigned long p_previous_data_fault = 0;
unsigned long pp_previous_data_fault = 0;
unsigned long ppp_previous_data_fault = 0;
unsigned long pppp_previous_data_fault = 0;
bool flag = true;
bool flag_of_first = true;
unsigned long firstEnclaveInstruction = 0;
unsigned long count_of_function = 0;
bool flag_of_addr = false;

extern unsigned long enclave_baseaddr;	
extern unsigned long enclave_size;	

static inline void my_flush_tlb_singlepage(unsigned long addr)
{
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

static inline void my_flush_tlb_allpages(void)
{
    unsigned long val;
    asm volatile("mov %%cr3,%0\n\t" : "=r" (val), "=m" (__force_order));
    asm volatile("mov %0,%%cr3": : "r" (val), "m" (__force_order));
}

static void clear_enclavepage(void)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct page *page = NULL;
	struct mm_struct *mm = task->mm;
	
	for(addr = enclave_baseaddr; addr < enclave_baseaddr+enclave_size; addr += 0x1000)
	{
		pgd = pgd_offset(mm, addr);
		if (pgd_none(*pgd) || pgd_bad(*pgd))
		{
			continue;
		}

		pud = pud_offset(pgd, addr);
		if (pud_none(*pud) || pud_bad(*pud))
		{
			continue;
		}
		
		pmd = pmd_offset(pud, addr);
		if (pmd_none(*pmd) || pmd_bad(*pmd))
		{
			continue;
		}
		
		ptep = pte_offset_map(pmd, addr);
		if (!ptep)
		{
			continue;
		}
		
		pte = *ptep;
	
		//temp_pte = pte_set_flags(pte, _PAGE_SOFTW1);
		//if(pte_present(pte))
		{
			//down_write(&mm->mmap_sem);
			if((pte_flags(pte) & _PAGE_PRESENT))
			//if(pte_present(pte))
			{
				//printk("Cleared_codepage: %08x\n", addr);
				temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
				//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW1);
				//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW2);
				temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
				set_pte(ptep, temp_pte);
				my_flush_tlb_singlepage(addr);
			}
			//up_write(&mm->mmap_sem);
		}
		if(ptep) pte_unmap(ptep);
	}
}



static void clear_codepage(void)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct page *page = NULL;
	struct mm_struct *mm = task->mm;
	
	for(addr = mm->start_code; addr < mm->end_code; addr += 0x1000)
	//for(addr = 0x00000000LL; addr < 0xffffffffLL; addr += 0x1000)
	{
		pgd = pgd_offset(mm, addr);
		if (pgd_none(*pgd) || pgd_bad(*pgd))
		{
			continue;
		}

		pud = pud_offset(pgd, addr);
		if (pud_none(*pud) || pud_bad(*pud))
		{
			continue;
		}
		
		pmd = pmd_offset(pud, addr);
		if (pmd_none(*pmd) || pmd_bad(*pmd))
		{
			continue;
		}
		
		ptep = pte_offset_map(pmd, addr);
		if (!ptep)
		{
			continue;
		}
		
		pte = *ptep;
	
		//temp_pte = pte_set_flags(pte, _PAGE_SOFTW1);
		//if(pte_present(pte))
		{
			//down_write(&mm->mmap_sem);
			if((pte_flags(pte) & _PAGE_PRESENT))
			//if(pte_present(pte))
			{
				//printk("Cleared_codepage: %08x\n", addr);
				temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
				//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW1);
				//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW2);
				temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
				set_pte(ptep, temp_pte);
				my_flush_tlb_singlepage(addr);
			}
			//up_write(&mm->mmap_sem);
		}
		if(ptep) pte_unmap(ptep);
	}
}

static void clear_datapage(void)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct page *page = NULL;
	struct mm_struct *mm = task->mm;
	struct vm_area_struct *vma = mm->mmap;
	
	// addr >= mm->brk && 
	// vma = vma->vm_next; vma = vma->vm_next; skip code and heap
	struct vm_area_struct *vma2 = find_vma(mm, firstEnclaveInstruction);
	
	
	while(vma != NULL)
	{
		for(addr = vma->vm_start ; addr < vma->vm_end; addr += 0x1000)
		{
			if(addr < mm->end_code) continue;
			if((addr >= vma2->vm_start) && (addr < vma2->vm_end)) continue;
			pgd = pgd_offset(mm, addr);
			if (pgd_none(*pgd) || pgd_bad(*pgd))
			{
				continue;
			}

			pud = pud_offset(pgd, addr);
			if (pud_none(*pud) || pud_bad(*pud))
			{
				continue;
			}
		
			pmd = pmd_offset(pud, addr);
			if (pmd_none(*pmd) || pmd_bad(*pmd))
			{
				continue;
			}
		
			ptep = pte_offset_map(pmd, addr);
			if (!ptep)
			{
				continue;
			}
		
			pte = *ptep;
	
			//temp_pte = pte_set_flags(pte, _PAGE_SOFTW1);
			//if(pte_present(pte))
			{
				//down_write(&mm->mmap_sem);
				if((pte_flags(pte) & _PAGE_PRESENT))
				//if(pte_present(pte))
				{
					printk("Cleared page: %08x\n", addr);
					temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
					//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW1);
					//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW2);
					temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
					set_pte(ptep, temp_pte);
					my_flush_tlb_singlepage(addr);
				}
				//up_write(&mm->mmap_sem);
			}
			if(ptep) pte_unmap(ptep);
		}
		vma = vma->vm_next;
	}
}

static void clear_heapregion(void)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct page *page = NULL;
	struct mm_struct *mm = task->mm;
	
	for(addr = mm->start_brk; addr < mm->brk; addr += 0x1000)
	//for(addr = 0x00000000LL; addr < 0xffffffffLL; addr += 0x1000)
	{
		pgd = pgd_offset(mm, addr);
		if (pgd_none(*pgd) || pgd_bad(*pgd))
		{
			continue;
		}

		pud = pud_offset(pgd, addr);
		if (pud_none(*pud) || pud_bad(*pud))
		{
			continue;
		}
		
		pmd = pmd_offset(pud, addr);
		if (pmd_none(*pmd) || pmd_bad(*pmd))
		{
			continue;
		}
		
		ptep = pte_offset_map(pmd, addr);
		if (!ptep)
		{
			continue;
		}
		
		pte = *ptep;
	
		//temp_pte = pte_set_flags(pte, _PAGE_SOFTW1);
		//if(pte_present(pte))
		{
			//down_write(&mm->mmap_sem);
			if((pte_flags(pte) & _PAGE_PRESENT))
			//if(pte_present(pte))
			{
				//printk("Cleared_codepage: %08x\n", addr);
				temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
				//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW1);
				//temp_pte = pte_set_flags(temp_pte, _PAGE_SOFTW2);
				temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
				set_pte(ptep, temp_pte);
				my_flush_tlb_singlepage(addr);
			}
			//up_write(&mm->mmap_sem);
		}
		if(ptep) pte_unmap(ptep);
	}
}

static void clear_targetVM(unsigned long address)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct page *page = NULL;
	struct mm_struct *mm = task->mm;
	
	//struct vm_area_struct *vma = mm->mmap;
    
	struct vm_area_struct *vma = find_vma(mm, address);;
	//while(vma != NULL)
	{
		//for(addr = vma->vm_start; addr < vma->vm_end; addr += 0x1000)	
		addr = address;
		{
			pgd = pgd_offset(mm, addr);
			if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
			{
				pud = pud_offset(pgd, addr);
				if( !(pud_none(*pud) || pud_bad(*pud)) )
				{	
					pmd = pmd_offset(pud, addr);
					if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
					{
						ptep = pte_offset_map(pmd, addr);
						if (ptep)
						{
							pte = *ptep;
			
							
							if((pte_flags(pte) & _PAGE_PRESENT))
							//if(pte_present(pte))
							{
								//down_write(&mm->mmap_sem);
								//printk("Cleared_targetVM: %08x\n", addr);
								temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
								temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
								set_pte(ptep, temp_pte);
								my_flush_tlb_singlepage(addr);
								
								//up_write(&mm->mmap_sem);
							}
							if(ptep) pte_unmap(ptep);
							
						}
					}
				}
			}
		}
	//	vma = vma->vm_next;
	}
	//flush_cache_mm(mm);
	//flush_tlb_mm(mm);
	
}

static void clear_targetVM2(unsigned long address)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct page *page = NULL;
	struct mm_struct *mm = task->mm;
	
	struct vm_area_struct *vma = find_vma(mm, address);;
	{
		for(addr = vma->vm_start; addr < vma->vm_end; addr += 0x1000)	
		{
			pgd = pgd_offset(mm, addr);
			if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
			{
				pud = pud_offset(pgd, addr);
				if( !(pud_none(*pud) || pud_bad(*pud)) )
				{	
					pmd = pmd_offset(pud, addr);
					if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
					{
						ptep = pte_offset_map(pmd, addr);
						if (ptep)
						{
							pte = *ptep;
			
							
							if((pte_flags(pte) & _PAGE_PRESENT))
							//if(pte_present(pte))
							{
								//down_write(&mm->mmap_sem);
								//printk("Cleared_targetVM: %08x\n", addr);
								temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
								temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
								set_pte(ptep, temp_pte);
								my_flush_tlb_singlepage(addr);
								
								//up_write(&mm->mmap_sem);
							}
							if(ptep) pte_unmap(ptep);
							
						}
					}
				}
			}
		}
	}
}


static void clear_range(unsigned long start, unsigned long end)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct mm_struct *mm = task->mm;
	
	for(addr = start; addr < end; addr += 0x1000)	
	{
		pgd = pgd_offset(mm, addr);
		if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
		{
			pud = pud_offset(pgd, addr);
			if( !(pud_none(*pud) || pud_bad(*pud)) )
			{	
				pmd = pmd_offset(pud, addr);
				if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
				{
					ptep = pte_offset_map(pmd, addr);
					if (ptep)
					{
						pte = *ptep;
						
						if((pte_flags(pte) & _PAGE_PRESENT))
						//if(pte_present(pte))
						{
							//down_write(&mm->mmap_sem);
							//printk("Cleared_targetVM: %08x\n", addr);
							temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
							temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
							set_pte(ptep, temp_pte);
							my_flush_tlb_singlepage(addr);
							
							//up_write(&mm->mmap_sem);
						}
						if(ptep) pte_unmap(ptep);			
					}
				}
			}
		}
	}
}

static void clear_allpages(void)
{
	pgd_t *pgd;
	pte_t *ptep, pte, temp_pte;
	pud_t *pud;
	pmd_t *pmd;
	
	unsigned long addr;
	struct task_struct * task = current;
	struct page *page = NULL;
	struct mm_struct *mm = task->mm;
	
	struct vm_area_struct *vma = mm->mmap;
	
	// addr >= mm->brk && 
	// vma = vma->vm_next; vma = vma->vm_next; skip code and heap
	//struct vm_area_struct *vma = find_vma(mm, address);;
	while(vma != NULL)
	{
		for(addr = vma->vm_start ; addr < vma->vm_end; addr += 0x1000)
		{
			pgd = pgd_offset(mm, addr);
			if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
			{
				pud = pud_offset(pgd, addr);
				if( !(pud_none(*pud) || pud_bad(*pud)) )
				{	
					pmd = pmd_offset(pud, addr);
					if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
					{
						ptep = pte_offset_map(pmd, addr);
						if (ptep)
						{
							pte = *ptep;
			
							if(pte_present(pte))
							{
								//printk("clear %08x", addr);
								//if((pte_flags(pte) & _PAGE_PRESENT)) 
								{
									temp_pte = pte_clear_flags(pte, _PAGE_PRESENT);
									temp_pte = pte_set_flags(temp_pte, _PAGE_PROTNONE);
									set_pte(ptep, temp_pte);
									my_flush_tlb_singlepage(addr);
								}
							}
							if(ptep) pte_unmap(ptep);
						}
					}
				}
			}
		}
		vma = vma->vm_next;
	}
	
}


void my_do_page_fault(struct pt_regs* regs, unsigned long error_code){
    struct task_struct * task = current;
    struct mm_struct *mm = task->mm;
    
	unsigned long addr = read_cr2();  /* get the page-fault address */
	unsigned long instruct_addr = regs->ip;//GET_IP(regs);  
	struct vm_area_struct *vma = mm->mmap;
	unsigned long virtual_addr;
	
	pgd_t *pgd;
    pte_t *ptep, pte, temp_pte;
    pud_t *pud;
    pmd_t *pmd;
	
	if ( (strcmp(task->comm, "App") != 0 ) )      // not our process
	{
		default_do_page_fault(regs, error_code);
		return;
	}
	
	//
	//if(!flag_of_first) printk("My page fault: %016x %016x, error_code: %x\n", instruct_addr, addr, error_code);
	
	if( (instruct_addr & 0xfff != 0x722) || (addr & 0xfff != 0xfff)  ) clear_enclavepage();
	if((addr >= enclave_baseaddr) && (addr < enclave_baseaddr + enclave_size) && ((addr & 0xfff) != 0xfff)) 
	{
		printk("My page fault: %08x %08x, error_code: %x\n", instruct_addr, addr, error_code);
		//printk("\t %08x, %08x\n", enclave_baseaddr, enclave_baseaddr);
	}	
	if(  (instruct_addr != addr) && ((addr & 0xfff) == 0) && (error_code & 0x10) != 0)   // enclave instruction pages
	//if( (error_code & 0x10) != 0)   // instruction pages
	{	
		//printk("My page fault: %016x %016x, error_code: %x\n", instruct_addr, addr, error_code);	
		/*if( ((pp_previous_instruction_fault & 0xfffff) == 0x2000)  &&  ((p_previous_instruction_fault & 0xfffff) == 0x3000) &&  ((previous_instruction_fault & 0xfffff) != 0x2000) && ((addr & 0xfffff) == 0x3000)   )   //&& ((pp_previous_data_fault & 0xffffff) == 0x015000) //  &&     
		{
			printk("No.%d, %08x, %08x, %08x, %08x, %08x \n", count_of_function++, pppp_previous_instruction_fault, ppp_previous_instruction_fault, pp_previous_instruction_fault, p_previous_instruction_fault, previous_instruction_fault);
			printk("\t data address: %08x, %08x, %08x, %08x, %08x\n", pppp_previous_data_fault, ppp_previous_data_fault, pp_previous_data_fault, p_previous_data_fault, previous_data_fault);
			//if(count >= 15) 
		//		printk("No.%d, count: %d\n", count_of_function++, ++count);
		//	count = 0;
		}  //*/
		//count++;
		//if((addr & 0xffffff) == 0x2000)
		//{
		//	if(count >= 15) printk("No.%d, count: %d\n", count_of_function++, ++count);
		//	count = 0;
		//}
		clear_targetVM2(previous_instruction_fault);
		//clear_codepage();
		//clear_enclavepage();
		if(p_previous_instruction_fault == addr)
		{
			pgd = pgd_offset(mm, previous_instruction_fault);
			if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
			{
				pud = pud_offset(pgd, previous_instruction_fault);
				if( !(pud_none(*pud) || pud_bad(*pud)) )
				{	
					pmd = pmd_offset(pud, previous_instruction_fault);
					if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
					{
						ptep = pte_offset_map(pmd, previous_instruction_fault);
						if (ptep)
						{
							pte = *ptep;
					
							//down_write(&mm->mmap_sem);
							if(!(pte_flags(pte) & _PAGE_PRESENT) && (pte_flags(pte) & _PAGE_PROTNONE))
							{
								temp_pte = pte_set_flags(pte, _PAGE_PRESENT);
								//temp_pte = pte_clear_flags(temp_pte, _PAGE_PROTNONE);
								set_pte(ptep, temp_pte);
						
								my_flush_tlb_singlepage(previous_instruction_fault);
							}
							//up_write(&mm->mmap_sem);
							if(ptep) pte_unmap(ptep);
						}
					}
				}
			}
			
			/// enable access to data address, too
			pgd = pgd_offset(mm, previous_data_fault);
			if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
			{
				pud = pud_offset(pgd, previous_data_fault);
				if( !(pud_none(*pud) || pud_bad(*pud)) )
				{	
					pmd = pmd_offset(pud, previous_data_fault);
					if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
					{
						ptep = pte_offset_map(pmd, previous_data_fault);
						if (ptep)
						{
							pte = *ptep;
					
							// //down_write(&mm->mmap_sem);
							if(!(pte_flags(pte) & _PAGE_PRESENT) && (pte_flags(pte) & _PAGE_PROTNONE))
							{
								temp_pte = pte_set_flags(pte, _PAGE_PRESENT);
								//temp_pte = pte_clear_flags(temp_pte, _PAGE_PROTNONE);
								set_pte(ptep, temp_pte);
						
								my_flush_tlb_singlepage(previous_data_fault);
							}
							// //up_write(&mm->mmap_sem);
							if(ptep) pte_unmap(ptep);
						}
					}
				}
			}
		}
	   
		pgd = pgd_offset(mm, addr);
		if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
		{
			pud = pud_offset(pgd, addr);
			if( !(pud_none(*pud) || pud_bad(*pud)) )
			{	
				pmd = pmd_offset(pud, addr);
				if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
				{
					ptep = pte_offset_map(pmd, addr);
					if (ptep)
					{
						pte = *ptep;
					
						//down_write(&mm->mmap_sem);
						if(!(pte_flags(pte) & _PAGE_PRESENT) && (pte_flags(pte) & _PAGE_PROTNONE))
						{
							temp_pte = pte_set_flags(pte, _PAGE_PRESENT);
							//temp_pte = pte_clear_flags(temp_pte, _PAGE_PROTNONE);
							set_pte(ptep, temp_pte);
						
							my_flush_tlb_singlepage(addr);
						}
						//up_write(&mm->mmap_sem);
						if(ptep) pte_unmap(ptep);
					}
				}
			}
		}
		
		
		
		pppp_previous_instruction_fault = ppp_previous_instruction_fault;
		ppp_previous_instruction_fault = pp_previous_instruction_fault;
		pp_previous_instruction_fault = p_previous_instruction_fault;
		p_previous_instruction_fault = previous_instruction_fault;
		previous_instruction_fault = addr;
	}else if( ((addr & 0xfff) == 0) && (error_code & 0x10) == 0 )   // enclave data pages
	{
		//if( ((addr & 0xfff) == 0) ) printk("No.%d, My page fault: %016x %016x, error_code: %x\n", count_of_function++, instruct_addr, addr, error_code);
		//if( ((ppp_previous_data_fault & 0xfffff) == 0xfe000) && ((pp_previous_data_fault & 0xfffff) == 0xc1000) && ((p_previous_data_fault & 0xfffff) == 0xc7000) && ((previous_data_fault & 0xfffff) == 0xc1000)  && ((addr & 0xfffff) == 0x9d000) ) //&& ((previous_data_fault & 0xfffff) == 0xc1000) && ((p_previous_data_fault & 0xfffff) == 0x9d000)
		//if( ((p_previous_data_fault & 0xfffff) == 0xfe000) && ((addr & 0xfffff) == 0xc7000) )
		//if(  ((addr & 0xfffff) == 0x15000)  && ((p_previous_data_fault & 0xfffff) == 0xc7000)  )  //
		//if(addr == enclave_baseaddr + 0x21d000)
		//{
			//printk("No.%d **** data address: %08x, %08x, %08x, %08x, %08x, %08x\n", count++, pppp_previous_data_fault, ppp_previous_data_fault, pp_previous_data_fault, p_previous_data_fault, previous_data_fault, addr);
		//	printk("No.%d: %d\n", count++, count_of_function); count_of_function = 0;
		//}
		//if(addr == enclave_baseaddr+0x21d000) count_of_function++;
		//clear_enclavepage();
		clear_targetVM2(previous_data_fault);
		if(p_previous_data_fault == addr)
		{
			pgd = pgd_offset(mm, previous_data_fault);
			if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
			{
				pud = pud_offset(pgd, previous_data_fault);
				if( !(pud_none(*pud) || pud_bad(*pud)) )
				{	
					pmd = pmd_offset(pud, previous_data_fault);
					if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
					{
						ptep = pte_offset_map(pmd, previous_data_fault);
						if (ptep)
						{
							pte = *ptep;
					
							// //down_write(&mm->mmap_sem);
							if(!(pte_flags(pte) & _PAGE_PRESENT) && (pte_flags(pte) & _PAGE_PROTNONE))
							{
								temp_pte = pte_set_flags(pte, _PAGE_PRESENT);
								//temp_pte = pte_clear_flags(temp_pte, _PAGE_PROTNONE);
								set_pte(ptep, temp_pte);
						
								my_flush_tlb_singlepage(previous_data_fault);
							}
							// //up_write(&mm->mmap_sem);
							if(ptep) pte_unmap(ptep);
						}
					}
				}
			}
			
			
			/// enable access to instruction address, too
			pgd = pgd_offset(mm, previous_instruction_fault);
			if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
			{
				pud = pud_offset(pgd, previous_instruction_fault);
				if( !(pud_none(*pud) || pud_bad(*pud)) )
				{	
					pmd = pmd_offset(pud, previous_instruction_fault);
					if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
					{
						ptep = pte_offset_map(pmd, previous_instruction_fault);
						if (ptep)
						{
							pte = *ptep;
					
							// //down_write(&mm->mmap_sem);
							if(!(pte_flags(pte) & _PAGE_PRESENT) && (pte_flags(pte) & _PAGE_PROTNONE))
							{
								temp_pte = pte_set_flags(pte, _PAGE_PRESENT);
								//temp_pte = pte_clear_flags(temp_pte, _PAGE_PROTNONE);
								set_pte(ptep, temp_pte);
						
								my_flush_tlb_singlepage(previous_instruction_fault);
							}
							// //up_write(&mm->mmap_sem);
							if(ptep) pte_unmap(ptep);
						}
					}
				}
			}
			
			
			
		}
	   
		pgd = pgd_offset(mm, addr);
		if( !(pgd_none(*pgd) || pgd_bad(*pgd)) )
		{
			pud = pud_offset(pgd, addr);
			if( !(pud_none(*pud) || pud_bad(*pud)) )
			{	
				pmd = pmd_offset(pud, addr);
				if( !(pmd_none(*pmd) || pmd_bad(*pmd)) )
				{
					ptep = pte_offset_map(pmd, addr);
					if (ptep)
					{
						pte = *ptep;
					
						// //down_write(&mm->mmap_sem);
						if(!(pte_flags(pte) & _PAGE_PRESENT) && (pte_flags(pte) & _PAGE_PROTNONE))
						{
							temp_pte = pte_set_flags(pte, _PAGE_PRESENT);
							//temp_pte = pte_clear_flags(temp_pte, _PAGE_PROTNONE);
							set_pte(ptep, temp_pte);
						
							my_flush_tlb_singlepage(addr);
						}
						// //up_write(&mm->mmap_sem);
						if(ptep) pte_unmap(ptep);
					}
				}
			}
		}
		
		
		pppp_previous_data_fault = ppp_previous_data_fault;
		ppp_previous_data_fault = pp_previous_data_fault;
		pp_previous_data_fault = p_previous_data_fault;
		p_previous_data_fault = previous_data_fault;
		previous_data_fault = addr;	
	}

	default_do_page_fault(regs, error_code);
}

static int __init page_fault_init(void)
{
    printk("page_fault_init module called...\n");
	do_page_fault_handler = my_do_page_fault;
	
	//my_flush_tlb_allpages();
    return 0;
}

static void __exit page_fault_exit(void)
{
    printk("page_fault_exit module called...\n");
    do_page_fault_handler = default_do_page_fault;
    
    //my_flush_tlb_allpages();
}

module_init(page_fault_init);
module_exit(page_fault_exit);
MODULE_LICENSE("Dual BSD/GPL");
