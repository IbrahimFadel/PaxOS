#include <kernel/acpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int acpiCheckHeader(unsigned int *ptr, char *sig);
static char calculate_checksum(u8 *buf, size_t size);
static struct rsdp_v1_t *acpi_check_rsdp_addr(struct rsdp_v1_t *addr);
static struct rsdp_v1_t *acpi_get_rsdp_in_region(uintptr_t start,
                                                 uintptr_t end);
static bool acpi_validate_header(void *buf, const char *sig);

static char calculate_checksum(u8 *buf, size_t size) {
  u64 sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum += *buf;
    buf++;
  }
  return (char)sum;
}

static struct rsdp_v1_t *acpi_check_rsdp_addr(struct rsdp_v1_t *addr) {
  if (memcmp("RSD PTR ", addr, 8) != 0) return NULL;
  if (calculate_checksum((u8 *)addr, sizeof(struct rsdp_v1_t)) == 0) {
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
  return NULL;
}

struct rsdt_t *acpi_get_rsdp(void) {
  struct rsdp_v1_t *rsdp = NULL;
  rsdp = acpi_get_rsdp_in_region(0x000E0000, 0x00100000);
  if (rsdp == NULL) {
    const uintptr_t ebda = (0x40e * 0x10) & 0x000FFFFF;
    rsdp = acpi_get_rsdp_in_region(ebda, ebda + 1024);
  }
  if (rsdp == NULL) return NULL;
  if (rsdp->revision != ACPI_VERSION_2_TO_6)
    return (struct rsdt_t *)rsdp->rsdt_addr;
  struct rsdp_v2_t *v2_data = (struct rsdp_v2_t *)rsdp + 8;
  if (calculate_checksum((u8 *)rsdp, sizeof(struct rsdp_v1_t) +
                                         sizeof(struct rsdp_v2_t)) == 0) {
    return (struct rsdt_t *)(u32)v2_data->xsdt_addr;
  } else {
    return NULL;
  }
}

/*

"RSD PTR "
  ||
 RsdtAddress pointer at offset 16
  ||
  \/
"RSDT"
  ||
 pointer at offset 36 + 4 * n (check the target for the sig "FACP" to get the
right n)
  ||
  \/
"FACP"
  ||
  ||=====\
  ||	||
  ||	PM1a_CNT_BLK; offset: 64	(see section 4.7.3.2)
  ||	PM1b_CNT_BLK; offset: 68
  ||		||
  ||		\/
  ||		SLP_TYPx; bit 10-12
  ||		SLP_EN;	  bit 13
  ||
 DSDT pointer at offset 40
  ||
  \/
"DSDT"	(export the \_S5 object somehow.)

*/

bool acpi_init(void) {
  struct rsdt_t *rsdt = acpi_get_rsdp();
  if (rsdt == NULL ||
      acpiCheckHeader((unsigned int *)rsdt->signature, "RSDT") == -1)
    return false;

  printf("Found RSDT\n");

  struct fadt_t *fadt = NULL;
  u32 rsdt_len = *((u32 *)rsdt + 1);
  int n = (rsdt_len - 36) / sizeof(u32);
  u32 *entries = (u32 *)rsdt + FIELD_OFFSET(struct rsdt_t, entries);
  for (int i = 0; i < n; i++) {
    uintptr_t entry = *entries;
    char *signature = (char *)signature;
    if (acpiCheckHeader((unsigned int *)signature, "FACP") == 0) {
      fadt = (struct fadt_t *)entry;
      break;
    }
    entries += 1;
  }

  printf("Found FACP\n");

  // printf("%c%c%c%c\n", *(u8 *)fadt, *((u8 *)fadt + 1), *((u8 *)fadt + 2),
  //        *((u8 *)fadt + 3));
  // struct dsdt_t *dsdt = (struct dsdt_t *)fadt->dsdt;
  struct dsdt_t *dsdt =
      (struct dsdt_t *)(fadt + FIELD_OFFSET(struct fadt_t, dsdt));
  if (acpiCheckHeader((unsigned int *)dsdt, "DSDT") == -1) return false;

  printf("Found DSDT\n");

  u8 *ptr = (u8 *)dsdt;
  u32 length = *(ptr + 4);
  printf("%c%c%c%c\n", ptr[0], ptr[1], ptr[2], ptr[3]);
  printf("%d\n", length);
  // u8 *s5_addr = NULL;
  // // u32 length = *(u32 *)(dsdt + FIELD_OFFSET(struct dsdt_t, length));
  // u32 length = dsdt->length - 36;
  // printf("%d\n", length);
  // // u8 *definition_block =
  // //     (u8 *)dsdt + FIELD_OFFSET(struct dsdt_t, definition_block);
  // u8 *definition_block = (u8 *)dsdt->definition_block;
  // printf("%c%c%c%c\n", definition_block[0], definition_block[1],
  //        definition_block[2], definition_block[3]);
  // for (size_t i = 0; i < length; i++) {
  //   void *s5_ptr = (void *)definition_block[i];
  //   // printf("%c%c%c%c\n")
  //   if (memcmp(s5_ptr, "_S5_", 4) == 0) {
  //     s5_addr = s5_ptr;
  //     break;
  //   }
  // }
  // if (s5_addr == NULL) return false;

  printf("Found S5 definition\n");

  // char code[1024];
  // memcpy(code, dsdt->definition_block, 1024);
  // printf("%s\n", code);

  return true;
}

// int initAcpi(void) {
//   unsigned int *ptr = acpiGetRSDPtr();

//   // check if address is correct  ( if acpi is available on this pc )
//   if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0) {
//     // the RSDT contains an unknown number of pointers to acpi tables
//     int entrys = *(ptr + 1);
//     entrys = (entrys - 36) / 4;
//     ptr += 36 / 4;
//     // skip header information

//     while (0 < entrys--) {
//       // check if the desired table is reached
//       if (acpiCheckHeader((unsigned int *)*ptr, "FACP") == 0) {
//         entrys = -2;
//         struct FACP *facp = (struct FACP *)*ptr;
//         if (acpiCheckHeader((unsigned int *)facp->DSDT, "DSDT") == 0) {
//           // search the \_S5 package in the DSDT
//           char *S5Addr = (char *)facp->DSDT + 36;
//           // skip header 					int dsdtLength =
//           *(facp->DSDT + 1) - 36;
//           while (0 < dsdtLength--) {
//             if (memcmp(S5Addr, "_S5_", 4) == 0) break;
//             S5Addr++;
//           }
//           // check if \_S5 was found
//           if (dsdtLength > 0) {
//             // check for valid AML structure
//             if ((*(S5Addr - 1) == 0x08 ||
//                  (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) &&
//                 *(S5Addr + 4) == 0x12) {
//               S5Addr += 5;
//               S5Addr +=
//                   ((*S5Addr & 0xC0) >> 6) + 2;  // calculate PkgLength size

//               if (*S5Addr == 0x0A) S5Addr++;
//               // skip byteprefix
//               SLP_TYPa = *(S5Addr) << 10;
//               S5Addr++;

//               if (*S5Addr == 0x0A) S5Addr++;
//               // skip byteprefix
//               SLP_TYPb = *(S5Addr) << 10;

//               SMI_CMD = facp->SMI_CMD;

//               ACPI_ENABLE = facp->ACPI_ENABLE;
//               ACPI_DISABLE = facp->ACPI_DISABLE;

//               PM1a_CNT = facp->PM1a_CNT_BLK;
//               PM1b_CNT = facp->PM1b_CNT_BLK;

//               PM1_CNT_LEN = facp->PM1_CNT_LEN;

//               SLP_EN = 1 << 13;
//               SCI_EN = 1;

//               return 0;
//             } else {
//                                                         wrstr("\\_S5 parse
// error.\n");
//             }
//           } else {
//             wrstr("\\_S5 not present.\n");
//           }
//         } else {
//           wrstr("DSDT invalid.\n");
//         }
//       }
//       ptr++;
//     }
//     wrstr("no valid FACP present.\n");
//   } else {
//     wrstr("no acpi.\n");
//   }

//   return -1;
// }

int acpiCheckHeader(unsigned int *ptr, char *sig) {
  if (memcmp(ptr, sig, 4) == 0) {
    char *checkPtr = (char *)ptr;
    int len = *(ptr + 1);
    char check = 0;
    while (0 < len--) {
      check += *checkPtr;
      checkPtr++;
    }
    if (check == 0) return 0;
  }
  return -1;
}

// static bool acpi_validate_header(void *buf, const char *sig) {
//   u8 *bptr = (u8 *)buf;
//   printf("test: %c%c%c%c\n", bptr[0], bptr[1], bptr[2], bptr[3]);
//   if (memcmp(buf, sig, 4) != 0) return false;
//   printf("Validating: %c%c%c%c\n", sig[0], sig[1], sig[2], sig[3]);
//   // u8 *bptr = (u8 *)buf;
//   printf("%d\n", *(bptr + 4));
//   size_t size = (size_t)*(bptr + 4);
//   return calculate_checksum((u8 *)buf, size) == 0;
// }

// bool acpi_set_mode(struct fadt_t *fadt) {
//   if (fadt->smi_cmd != 0) return false;
//   if (fadt->acpi_enable != 0 || fadt->acpi_disable != 0) return false;

//   printf("Char1: %c\n", fadt->signature[0]);

//   unsigned short reading = inw(fadt->pm1a_cnt_blk);
//   printf("Port addr: %d\nRead: %d\n", fadt->pm1a_cnt_blk, reading);
//   if (reading & 1 == 0) return false;

//   outb(fadt->smi_cmd, fadt->acpi_enable);

//   return false;
// }

// bool acpi_enable(struct fadt_t *fadt) {
//   if (!fadt) return false;
//   // if (inw((unsigned int)facp->pm1a_cnt_blk) == 0) {
//   // }
//   return false;
// }

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