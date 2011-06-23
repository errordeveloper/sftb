#ifndef UTIL_H
#define UTIL_H

# define		PRINT(...) vpi_printf("%s: ",func) ; \
vpi_printf(__VA_ARGS__)
# ifndef DBG
# define		DBG 0
# warning "Disabling debugging by default."
# endif

# if DBG
# define		DEBUG(...) PRINT(__VA_ARGS__)
# else
# define		DEBUG(...)
# endif

# define		ERROR(n)   sf.exit = n ; goto EXIT ;

#define getb(val, bit) (((val & (1 << bit)) >> bit) == 1)
#define Cbit(val, bit)  (val = (val & ~(1 << bit)))
#define Sbit(val, bit)  (val = (val | (1 << bit)))

/* vpiRegArray is not defined in vpi_user.h !!! */

# define vpiRegArray	116
# define vpiNetArray	114

#endif
