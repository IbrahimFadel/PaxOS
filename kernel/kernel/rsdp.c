#include <kernel/rsdp.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static const uintptr_t EBDA = (0x40e * 0x10) & 0x000FFFFF;
static const char *RSD_SIG = "RSD PTR ";

static char calculate_checksum(u8 *buf, size_t size) {
  u64 sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum += *buf;
    buf++;
  }
  return (char)sum;
}

static struct rsdp_v1_t *acpi_check_rsdp_addr(struct rsdp_v1_t *addr) {
  const size_t size = sizeof(struct rsdp_v1_t);
  if (memcmp(RSD_SIG, addr, size != 0)) return NULL;
  if (calculate_checksum((u8 *)addr, size) == 0) {
    return addr;
  } else {
    return NULL;
  }
}

static struct rsdp_v1_t *acpi_get_rsdp_in_region(uintptr_t start,
                                                 uintptr_t end) {
  struct rsdp_v1_t *rsdp = NULL;
  for (uintptr_t addr = start; addr < end; addr += 0x10 / sizeof(addr)) {
    rsdp = acpi_check_rsdp_addr((struct rsdp_v1_t *)addr);
    if (rsdp != NULL) return rsdp;
  }
  return rsdp;
}

struct facp_t *acpi_get_rsdp(void) {
  struct rsdp_v1_t *rsdp = NULL;
  rsdp = acpi_get_rsdp_in_region(0x000E0000, 0x00100000);
  if (rsdp == NULL) {
    rsdp = acpi_get_rsdp_in_region(EBDA, EBDA + 1024);
  }
  if (rsdp == NULL) return NULL;

  if (rsdp->revision != ACPI_VERSION_2_TO_6)
    return (struct facp_t *)rsdp->rsdt_addr;
  struct rsdp_v2_t *v2_data = (struct rsdp_v2_t *)rsdp + 8;
  if (calculate_checksum((u8 *)rsdp, sizeof(struct rsdp_v1_t) +
                                         sizeof(struct rsdp_v2_t)) == 0) {
    return (struct facp_t *)(u32)v2_data->xsdt_addr;
  } else {
    return NULL;
  }
}

static bool acpi_validate_header(void *buf, const char *sig) {
  if (memcmp(buf, sig, 4) != 0) return false;
  u8 *bptr = (u8 *)buf;
  size_t size = (size_t)bptr[1];
  return calculate_checksum((u8 *)buf, size) == 0;
}

bool acpi_enable(void) {}

// int acpiEnable(void)
// {
// 	// check if acpi is enabled
// 	if ( (inw((unsigned int) PM1a_CNT) &SCI_EN) == 0 )
// 	{
// 		// check if acpi can be enabled
// 		if (SMI_CMD != 0 && ACPI_ENABLE != 0)
// 		{
// 			outb((unsigned int) SMI_CMD, ACPI_ENABLE); // send acpi
// enable command
// 			// give 3 seconds time to enable acpi
// 			int i;
// 			for (i=0; i<300; i++ )
// 			{
// 				if ( (inw((unsigned int) PM1a_CNT) &SCI_EN) == 1
// ) 					break; sleep(10);
// 			}
// 			if (PM1b_CNT != 0)
// 				for (; i<300; i++ )
// 				{
// 					if ( (inw((unsigned int) PM1b_CNT)
// &SCI_EN) == 1 ) 						break;
// sleep(10);
// 				}
// 			if (i<300) {
// 				wrstr("enabled acpi.\n");
// 				return 0;
// 			} else {
// 				wrstr("couldn't enable acpi.\n");
// 				return -1;
// 			}
// 		} else {
// 			wrstr("no known way to enable acpi.\n");
// 			return -1;
// 		}
// 	} else {
// 		//wrstr("acpi was already enabled.\n");
// 		return 0;
// 	}
// }

// //
// // bytecode of the \_S5 object
// // -----------------------------------------
// //        | (optional) |    |    |    |
// // NameOP | \          | _  | S  | 5  | _
// // 08     | 5A         | 5F | 53 | 35 | 5F
// //
// //
// -----------------------------------------------------------------------------------------------------------
// //           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   )
// | ( Reserved   ) | (Reserved    )
// // PackageOP | PkgLength | NumElements  | byteprefix Num | byteprefix Num
// | byteprefix Num | byteprefix Num
// // 12        | 0A        | 04           | 0A         05  | 0A          05
// | 0A         05  | 0A         05
// //
// //----this-structure-was-also-seen----------------------
// // PackageOP | PkgLength | NumElements |
// // 12        | 06        | 04          | 00 00 00 00
// //
// // (Pkglength bit 6-7 encode additional PkgLength bytes [shouldn't be the
// case here])
// //
// int initAcpi(void)
// {
// 	unsigned int *ptr = acpiGetRSDPtr();

// 	// check if address is correct  ( if acpi is available on this pc )
// 	if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0)
// 	{
// 		// the RSDT contains an unknown number of pointers to acpi
// tables 		int entrys = *(ptr + 1); 		entrys =
// (entrys-36) /4; 		ptr += 36/4;
// // skip header information

// 		while (0<entrys--)
// 		{
// 			// check if the desired table is reached
// 			if (acpiCheckHeader((unsigned int *) *ptr, "FACP") == 0)
// 			{
// 				entrys = -2;
// 				struct FACP *facp = (struct FACP *) *ptr;
// 				if (acpiCheckHeader((unsigned int *) facp->DSDT,
// "DSDT") == 0)
// 				{
// 					// search the \_S5 package in the DSDT
// 					char *S5Addr = (char *) facp->DSDT +36;
// // skip header 					int dsdtLength =
// *(facp->DSDT+1) -36; 					while (0 <
// dsdtLength--)
// 					{
// 						if ( memcmp(S5Addr, "_S5_", 4)
// == 0) 							break;
// S5Addr++;
// 					}
// 					// check if \_S5 was found
// 					if (dsdtLength > 0)
// 					{
// 						// check for valid AML structure
// 						if ( ( *(S5Addr-1) == 0x08 || (
// *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 )
// 						{
// 							S5Addr += 5;
// 							S5Addr += ((*S5Addr
// &0xC0)>>6) +2;	// calculate PkgLength size

// 							if (*S5Addr == 0x0A)
// 								S5Addr++;
// // skip byteprefix
// SLP_TYPa = *(S5Addr)<<10; S5Addr++;

// 							if (*S5Addr == 0x0A)
// 								S5Addr++;
// // skip byteprefix
// SLP_TYPb = *(S5Addr)<<10;

// 							SMI_CMD = facp->SMI_CMD;

// 							ACPI_ENABLE =
// facp->ACPI_ENABLE; ACPI_DISABLE = facp->ACPI_DISABLE;

// 							PM1a_CNT =
// facp->PM1a_CNT_BLK;
// PM1b_CNT = facp->PM1b_CNT_BLK;

// 							PM1_CNT_LEN =
// facp->PM1_CNT_LEN;

// 							SLP_EN = 1<<13;
// 							SCI_EN = 1;

// 							return 0;
// 						} else {
// 							wrstr("\\_S5 parse
// error.\n");
// 						}
// 					} else {
// 						wrstr("\\_S5 not present.\n");
// 					}
// 				} else {
// 					wrstr("DSDT invalid.\n");
// 				}
// 			}
// 			ptr++;
// 		}
// 		wrstr("no valid FACP present.\n");
// 	} else {
// 		wrstr("no acpi.\n");
// 	}

// 	return -1;
// }

// void acpiPowerOff(void)
// {
// 	// SCI_EN is set to 1 if acpi shutdown is possible
// 	if (SCI_EN == 0)
// 		return;

// 	acpiEnable();

// 	// send the shutdown command
// 	outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
// 	if ( PM1b_CNT != 0 )
// 		outw((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );

// 	wrstr("acpi poweroff failed.\n");
// }