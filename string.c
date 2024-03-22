// SPDX-License-Identifier: GPL-2.0
/*
 * arch/loongarch/boot/compressed/string.c
 *
 * Very small subset of simple string routines
 */

typedef long size_t;

#define NULL 0

void *  memchr(const void *s, int c, size_t n)
{
	const unsigned char *p = s;
	while (n-- != 0) {
		if ((unsigned char)c == *p++) {
			return (void *)(p - 1);
		}
	}
	return NULL;
}

int  memcmp(const void *cs, const void *ct, size_t count)
{
	int res = 0;
	const unsigned char *su1, *su2;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--) {
		res = *su1 - *su2;
		if (res != 0)
			break;
	}
	return res;
}

void  *memset(void *s, int c, size_t n)
{
	int i;
	char *ss = s;

	for (i = 0; i < n; i++)
		ss[i] = c;
	return s;
}

void  *memcpy(void *dest, const void *src, size_t n)
{
	int i;
	const char *s = src;
	char *d = dest;

	for (i = 0; i < n; i++)
		d[i] = s[i];
	return dest;
}

void  *memmove(void *dest, const void *src, size_t n)
{
	int i;
	const char *s = src;
	char *d = dest;

	if (d < s) {
		for (i = 0; i < n; i++)
			d[i] = s[i];
	} else if (d > s) {
		for (i = n - 1; i >= 0; i--)
			d[i] = s[i];
	}

	return dest;
}

int  strcmp(const char *str1, const char *str2)
{
	int delta = 0;
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;

	while (*s1 || *s2) {
		delta = *s1 - *s2;
		if (delta)
			return delta;
		s1++;
		s2++;
	}
	return 0;
}

size_t  strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

size_t  strnlen(const char *s, size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

char *  strrchr(const char *s, int c)
{
	const char *last = NULL;
	do {
		if (*s == (char)c)
			last = s;
	} while (*s++);
	return (char *)last;
}

/*
 * These two variables specify the free mem region
 * that can be used for temporary malloc area
 */
unsigned long free_mem_ptr;
unsigned long free_mem_end_ptr;

/* The linker tells us where the image is. */
extern unsigned char __image_begin, __image_end;

/* debug interfaces  */
#ifdef CONFIG_DEBUG_ZBOOT
extern void puts(const char *s);
extern void puthex(unsigned long long val);
#else
#define puts(s) do {} while (0)
#define puthex(val) do {} while (0)
#endif

void decompress_kernel(unsigned long boot_heap_start, long kdump_reloc_offset)
{
	unsigned long zimage_start, zimage_size;


	zimage_start = (unsigned long)(&__image_begin);
	zimage_size = (unsigned long)(&__image_end) -
	    (unsigned long)(&__image_begin);

	puts("image at:     ");
	puthex(zimage_start);
	puts(" ");
	puthex(zimage_size + zimage_start);
	puts("\n");

	/* Display standard Linux/LoongArch boot prompt */
	puts("Copying kernel to load address ");
	puthex(KERNEL_LOAD_ADDRESS_ULL);
	puts("\n");

    memcpy((void *)KERNEL_LOAD_ADDRESS_ULL + kdump_reloc_offset, (const void *)zimage_start, zimage_size);
		   
	puts("Now, booting the kernel...\n");
}
