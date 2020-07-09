#ifndef ASM_PGTABLE_H
#define ASM_PGTABLE_H

#include <asm/pgtable_hwdef.h>
#include <asm/pgtable_types.h>
#include <asm/mm.h>
#include <asm/barrier.h>

extern char idmap_pg_dir[];
extern char swapper_pg_dir[];
extern char init_pg_dir[];

/* 查找PGD索引 */
#define pgd_index(addr) (((addr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

/* 通过地址addr查找PGD的表项 */
#define pgd_offset_raw(pgd, addr) ((pgd_t *)(((pgd_t *)(pgd)) \
			+ pgd_index(addr)))

#define pgd_offset(mm, addr) (pgd_offset_raw((mm)->pgd, (addr)))

#define pgd_addr_end(addr, end)						\
({	unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK;	\
	(__boundary - 1 < (end) - 1) ? __boundary : (end);		\
})

#define pud_addr_end(addr, end)						\
({	unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;	\
	(__boundary - 1 < (end) - 1) ? __boundary : (end);		\
})

#define pmd_addr_end(addr, end)						\
({	unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;	\
	(__boundary - 1 < (end) - 1) ? __boundary : (end);		\
})

#define pgd_none(pgd) (!pgd_val(pgd))
#define pud_none(pud) (!pud_val(pud))
#define pmd_none(pmd) (!pmd_val(pmd))
#define ptd_none(ptd) (!ptd_val(ptd))

#define pmd_sect(pmd)	((pmd_val(pmd) & PMD_TYPE_MASK) == \
				 PMD_TYPE_SECT)
#define pud_sect(pud)	((pud_val(pud) & PUD_TYPE_MASK) == \
				 PUD_TYPE_SECT)

static inline unsigned long pgd_page_paddr(pgd_t pgd)
{
	return pgd_val(pgd) & PTE_ADDR_MASK;
}

#define pud_index(addr) ((addr) >> PUD_SHIFT & (PTRS_PER_PUD - 1))
#define pud_offset_phys(pgd, addr) ((pud_t *)((pgd_page_paddr(*(pgd)) + \
				pud_index(addr) * sizeof(pud_t))))
#define pud_offset_virt(pgd, addr) \
	((pud_t *)__va((unsigned long)pud_offset_phys(pgd, addr)))
#define pud_set_fixmap_offset(pgd, addr) \
	pud_set_fixmap((unsigned long)pud_offset_phys(pgd, addr))

static inline unsigned long pud_page_paddr(pud_t pud)
{
	return pud_val(pud) & PTE_ADDR_MASK;
}

#define pmd_index(addr) ((addr) >> PMD_SHIFT & (PTRS_PER_PMD - 1))
#define pmd_offset_phys(pud, addr) \
	((pmd_t *)(pud_page_paddr(*(pud)) + pmd_index(addr) * sizeof(pmd_t)))
#define pmd_offset_virt(pud, addr) \
	((pmd_t *)__va((unsigned long)pmd_offset_phys(pud, addr)))
#define pmd_set_fixmap_offset(pud, addr) \
	pmd_set_fixmap((unsigned long)pmd_offset_phys(pud, addr))

#define pmd_table(pmd)	((pmd_val(pmd) & PMD_TYPE_MASK) == \
			  PMD_TYPE_TABLE)
#define pmd_sect(pmd)	((pmd_val(pmd) & PMD_TYPE_MASK) == \
			  PMD_TYPE_SECT)

#define pfn_pmd(pfn, prot) (__pmd(((pfn) << PMD_SHIFT) | (prot)))

static inline unsigned long pmd_page_paddr(pmd_t pmd)
{
	return pmd_val(pmd) & PTE_ADDR_MASK;
}

#define pte_index(addr) (((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_phys(dir, addr) \
	((pte_t *)(pmd_page_paddr(*(dir)) + pte_index(addr) * sizeof(pte_t)))

#define pte_offset_virt(pmd, addr) \
	((pte_t *)__va((unsigned long)pte_offset_phys(pmd, addr)))

#define pte_set_fixmap_offset(pmd, addr) \
	pte_set_fixmap((unsigned long)pte_offset_phys(pmd, addr))

#define pfn_pte(pfn, prot)  (__pte(((pfn) << PAGE_SHIFT) | (prot)))

static inline void set_pgd(pgd_t *pgdp, pgd_t pgd)
{
	*pgdp = pgd;

	dsb(ishst);
}

static inline void set_pud(pud_t *pudp, pud_t pud)
{
	*pudp = pud;

	dsb(ishst);
}

static inline void set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	*pmdp = pmd;

	dsb(ishst);
}

static inline void set_pte(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
	dsb(ishst);
}

static inline unsigned long mk_sect_prot(unsigned long prot)
{
	return prot & ~PTE_TABLE_BIT;
}
#endif /*ASM_PGTABLE_H*/

