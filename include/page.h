#ifndef PAGE_H
#define PAGE_H

#include <mm_types.h>
#include <asm/mm.h>
#include <bitops.h>

extern struct page *mem_map;

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE-1)&PAGE_MASK)
#define PAGE_ALIGN_UP(addr)  PAGE_ALIGN(addr)
#define PAGE_ALIGN_DOWN(addr) (addr & PAGE_MASK)
#define pfn_to_page(pfn) (mem_map + ((pfn) - ARCH_PFN_OFFSET))
#define page_to_pfn(page) ((unsigned long)(page - mem_map) + ARCH_PFN_OFFSET)

#define page_to_nid(page) (page->nid)

#define page_to_address(page) __va(page_to_pfn(page) << PAGE_SHIFT)
#define virt_to_page(addr)  (pfn_to_page(__pa(addr) >> PAGE_SHIFT))

#define PFN_UP(x) (((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x)  (x >> PAGE_SHIFT)

static inline int get_order(unsigned long size)
{
	int order;

	size--;
	size >>= PAGE_SHIFT;
	order = fls64(size);

	return order;
}

static inline struct zone *page_zone(const struct page *page)
{
	return &NODE_DATA(page_to_nid(page))->node_zones[page->zone_id];
}

static inline void set_page_count(struct page *page, int v)
{
	/*Todo : using atomic_set */
	page->refcount = v;
}

void __free_pages(struct page *page, unsigned int order);
unsigned long memblock_free_all(void);
void memblock_free_pages(struct page *page, unsigned int order);
struct page *alloc_pages(unsigned int order);
void show_buddyinfo(void);
void free_area_init_node(int nid, unsigned long node_start_pfn,
		unsigned long *zone_size);
unsigned long get_free_pages(unsigned int order);
unsigned long get_free_page(void);
void free_pages(unsigned long addr, unsigned int order);
void free_page(unsigned long addr);

#endif /*PAGE_H*/
