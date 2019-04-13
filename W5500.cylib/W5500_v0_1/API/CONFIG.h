/**
* @file     `$INSTANCE_NAME`_CONFIG.h
* @version  3
* @brief    Configuration file.
*/

#ifndef `$INSTANCE_NAME`_CONFIG_H
#define `$INSTANCE_NAME`_CONFIG_H

#include "`$SPI_MASTER`.h"

/* So there's no symbol to know if the API is being built
 * for a psoc4, 5lp or 6, so i found out based on the spi headers */
#if defined (CY_SCB_`$SPI_MASTER`_H)
# if defined (`$SPI_MASTER`_CY_SCB_SPI_PDL_H)
#  define _PSOC6
# else
#  define _PSOC4_SCB
   #include "`$SPI_MASTER`_SPI_UART.h"
# endif
#else
# define _PSOC_UDB
#endif

#include "`$INSTANCE_NAME`_DEFS.h"

// Component version info
#define `$INSTANCE_NAME`_VERSION_MAJOR  `=$CY_MAJOR_VERSION`
#define `$INSTANCE_NAME`_VERSION_MINOR  `=$CY_MINOR_VERSION`
#define `$INSTANCE_NAME`_VERSION        `=$CY_MAJOR_VERSION`.`=$CY_MINOR_VERSION`

#define `$INSTANCE_NAME`_HW_RST         `$Hardware_RST`    

#endif /* `$INSTANCE_NAME`_CONFIG_H */

/* [] END OF FILE */
