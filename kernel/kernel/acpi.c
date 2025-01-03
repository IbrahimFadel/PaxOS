// #include <kernel/acpi.h>
// #include <string.h>
// #include <sys/types.h>

// struct RSDPtr {
//   u8 Signature[8];
//   u8 CheckSum;
//   u8 OemID[6];
//   u8 Revision;
//   u32 *RsdtAddress;
// };

// struct FACP {
//   u8 Signature[4];
//   u32 Length;
//   u8 unneded1[40 - 8];
//   u32 *DSDT;
//   u8 unneded2[48 - 44];
//   u32 *SMI_CMD;
//   u8 ACPI_ENABLE;
//   u8 ACPI_DISABLE;
//   u8 unneded3[64 - 54];
//   u32 *PM1a_CNT_BLK;
//   u32 *PM1b_CNT_BLK;
//   u8 unneded4[89 - 72];
//   u8 PM1_CNT_LEN;
// };

// static char calculate_checksum(u8 *buf, size_t size) {
//   u32 sum = 0;
//   for (size_t i = 0; i < size; i++) {
//     sum += *buf;
//     buf++;
//   }
//   return (char)sum;
// }

// static u32 *acpi_get_rsdt_addr(struct RSDPtr *rsdp) {
//   if (memcmp("RSD PTR ", rsdp, 8) != 0) return NULL;
//   if (calculate_checksum((u8 *)rsdp, sizeof(struct RSDPtr)) == 0) {
//     return (u32 *)rsdp->RsdtAddress;
//   }
//   return NULL;
// }

// static u32 *acpi_get_rsdp_in_region(uintptr_t start, uintptr_t end) {
//   u32 *rsdp = NULL;
//   for (uintptr_t addr = start; addr < end; addr += 0x10 / sizeof(addr)) {
//     rsdp = acpi_get_rsdt_addr((struct RSDPtr *)addr);
//     if (rsdp != NULL) return rsdp;
//   }
//   return NULL;
// }

// static u32 *acpi_get_rsdp(void) {
//   u32 *rsdp = acpi_get_rsdp_in_region(0x000E0000, 0x00100000);
//   if (rsdp != NULL) return rsdp;
//   uintptr_t ebda = *((short *)0x40E);
//   ebda = ebda * 0x10 & 0x000FFFFF;  // transform segment into linear address
//   rsdp = acpi_get_rsdp_in_region(ebda, ebda + 1024);
//   if (rsdp != NULL) return rsdp;
//   return NULL;
// }

// uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
//   *out_rsdp_address = (uacpi_phys_addr)acpi_get_rsdp();
// }

// uacpi_status uacpi_kernel_pci_device_open(uacpi_pci_address address,
//                                           uacpi_handle *out_handle);
// void uacpi_kernel_pci_device_close(uacpi_handle);

// uacpi_status uacpi_kernel_pci_read(uacpi_handle device, uacpi_size offset,
//                                    uacpi_u8 byte_width, uacpi_u64 *value);
// uacpi_status uacpi_kernel_pci_write(uacpi_handle device, uacpi_size offset,
//                                     uacpi_u8 byte_width, uacpi_u64 value);

// /*
//  * Map a SystemIO address at [base, base + len) and return a
//  kernel-implemented
//  * handle that can be used for reading and writing the IO range.
//  */
// uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len,
//                                  uacpi_handle *out_handle);
// void uacpi_kernel_io_unmap(uacpi_handle handle);

// // //
// // // here is the slighlty complicated ACPI poweroff code
// // //

// // #include <kernel/tty.h>
// // #include <stdbool.h>
// // #include <stddef.h>
// // #include <stdio.h>
// // #include <string.h>
// // #include <sys/io.h>
// // #include <sys/types.h>

// // u32 *SMI_CMD;
// // u8 ACPI_ENABLE;
// // u8 ACPI_DISABLE;
// // u32 *PM1a_CNT;
// // u32 *PM1b_CNT;
// // u16 SLP_TYPa;
// // u16 SLP_TYPb;
// // u16 SLP_EN;
// // u16 SCI_EN;
// // u8 PM1_CNT_LEN;

// // struct RSDPtr {
// //   u8 Signature[8];
// //   u8 CheckSum;
// //   u8 OemID[6];
// //   u8 Revision;
// //   u32 *RsdtAddress;
// // };

// // struct FACP {
// //   u8 Signature[4];
// //   u32 Length;
// //   u8 unneded1[40 - 8];
// //   u32 *DSDT;
// //   u8 unneded2[48 - 44];
// //   u32 *SMI_CMD;
// //   u8 ACPI_ENABLE;
// //   u8 ACPI_DISABLE;
// //   u8 unneded3[64 - 54];
// //   u32 *PM1a_CNT_BLK;
// //   u32 *PM1b_CNT_BLK;
// //   u8 unneded4[89 - 72];
// //   u8 PM1_CNT_LEN;
// // };

// // static char calculate_checksum(u8 *buf, size_t size) {
// //   u32 sum = 0;
// //   for (size_t i = 0; i < size; i++) {
// //     sum += *buf;
// //     buf++;
// //   }
// //   return (char)sum;
// // }

// // u32 *acpi_get_rsdt_addr(struct RSDPtr *rsdp) {
// //   if (memcmp("RSD PTR ", rsdp, 8) != 0) return NULL;
// //   if (calculate_checksum((u8 *)rsdp, sizeof(struct RSDPtr)) == 0) {
// //     return (u32 *)rsdp->RsdtAddress;
// //   }
// //   return NULL;
// // }

// // static u32 *acpi_get_rsdp_in_region(uintptr_t start, uintptr_t end) {
// //   u32 *rsdp = NULL;
// //   for (uintptr_t addr = start; addr < end; addr += 0x10 / sizeof(addr)) {
// //     rsdp = acpi_get_rsdt_addr((struct RSDPtr *)addr);
// //     if (rsdp != NULL) return rsdp;
// //   }
// //   return NULL;
// // }

// // u32 *acpi_get_rsdp(void) {
// //   u32 *rsdp = acpi_get_rsdp_in_region(0x000E0000, 0x00100000);
// //   if (rsdp != NULL) return rsdp;
// //   uintptr_t ebda = *((short *)0x40E);
// //   ebda = ebda * 0x10 & 0x000FFFFF;  // transform segment into linear
// address
// //   rsdp = acpi_get_rsdp_in_region(ebda, ebda + 1024);
// //   if (rsdp != NULL) return rsdp;
// //   return NULL;
// // }

// // int acpi_validate_header(u32 *ptr, const char *sig) {
// //   if (memcmp(ptr, sig, 4) != 0) return FAILURE;
// //   u32 len = *(ptr + 1);
// //   if (calculate_checksum((u8 *)ptr, (size_t)len) == 0) return SUCCESS;
// //   return FAILURE;
// // }

// // static bool acpi_is_enabled(void) {
// //   return (inw((unsigned int)PM1a_CNT) & SCI_EN) == 1;
// // }

// // static bool acpi_can_be_enabled(void) {
// //   return SMI_CMD != 0 && ACPI_ENABLE != 0;
// // }

// // int acpi_enable(void) {
// //   if (acpi_is_enabled()) return 0;
// //   if (!acpi_can_be_enabled()) return -1;

// //   outb((unsigned int)SMI_CMD, ACPI_ENABLE);  // Send enable cmd

// //   // give 3 seconds time to enable acpi
// //   int i;
// //   for (i = 0; i < 300; i++) {
// //     if ((inw((unsigned int)PM1a_CNT) & SCI_EN) == 1) break;
// //     // sleep(10);
// //   }
// //   if (PM1b_CNT != 0)
// //     for (; i < 300; i++) {
// //       if ((inw((unsigned int)PM1b_CNT) & SCI_EN) == 1) break;
// //       // sleep(10);
// //     }
// //   if (i < 300) {
// //     printf("enabled acpi.\n");
// //     return 0;
// //   } else {
// //     printf("couldn't enable acpi.\n");
// //     return -1;
// //   }
// // }

// // int initAcpi(void) {
// //   u32 *ptr = acpi_get_rsdp();

// //   // check if address is correct  ( if acpi is available on this pc )
// //   if (ptr == NULL || acpi_validate_header(ptr, "RSDT") == FAILURE) {
// //     printf("no acpi\n");
// //     return FAILURE;
// //   }

// //   // the RSDT contains an unknown number of pointers to acpi tables
// //   int entrys = *(ptr + 1);
// //   entrys = (entrys - 36) / 4;
// //   ptr += 36 / 4;  // skip header information

// //   for (int i = 0; i < entrys; i++) {
// //     // check if the desired table is reached
// //     if (acpi_validate_header((u32 *)*ptr, "FACP") == FAILURE) continue;
// //     i = entrys + 1;
// //     struct FACP *facp = (struct FACP *)*ptr;

// //     if (acpi_validate_header((u32 *)facp->DSDT, "DSDT") == FAILURE) {
// //       printf("DSDT invalid.\n");
// //       continue;
// //     }

// //     // search the \_S5 package in the DSDT
// //     char *S5Addr = (char *)facp->DSDT + 36;  // skip header

// //     // char aml_code[64];
// //     // memcpy(aml_code, S5Addr, 10);
// //     // aml_code[10] = '\0';

// //     int dsdtLength = *(facp->DSDT + 1) - 36;

// //     // char buf[7000];
// //     printf("%d\n", dsdtLength);
// //     // // memcpy(buf, S5Addr, dsdtLength);
// //     // buf[0] = 'a';
// //     // buf[1] = 'b';
// //     // buf[2] = 'c';
// //     // buf[3] = 'd';
// //     // buf[6800] = '\0';
// //     // // tty_writestring(buf);
// //     // printf("%d\n", buf[0]);
// //     // printf("%d\n", buf[1]);
// //     // printf("%d\n", buf[2]);
// //     // printf("%d\n", buf[3]);

// //     // printf("%s\n", buf);
// //     // char *b = S5Addr;
// //     // printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", b[0], b[1], b[2],
// b[3],
// //     // b[4],
// //     //        b[5], b[6], b[7], b[8], b[9], b[10], b[11], b[12], b[13],
// //     //        b[14], b[15]);

// //     printf("%d\n", (int)S5Addr);
// //     while (0 < dsdtLength--) {
// //       if (memcmp(S5Addr, "_S5_", 4) == 0) break;
// //       S5Addr++;
// //     }
// //     printf("%d\n", (int)S5Addr);
// //     printf("%c%c%c%c\n", (char)S5Addr - 1);

// //     // printf("%c%c%c%c%c%c%c%c\n", S5Addr[0], S5Addr[1], S5Addr[2],
// //     S5Addr[3],
// //     //        S5Addr[4], S5Addr[5], S5Addr[6], S5Addr[7]);

// //     // check if \_S5 was found
// //     if (dsdtLength <= 0) {
// //       printf("\\_S5 not present.\n");
// //       continue;
// //     }

// //     // check for valid AML structure
// //     bool valid_aml = (*(S5Addr - 1) == 0x08 ||
// //                       (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) &&
// //                      *(S5Addr + 4) == 0x12;
// //     if (!valid_aml) {
// //       printf("\\_S5 parse error.\n");
// //       continue;
// //     }

// //     S5Addr += 5;
// //     S5Addr += ((*S5Addr & 0xC0) >> 6) + 2;  // calculate PkgLength size

// //     if (*S5Addr == 0x0A) S5Addr++;  // skip u8prefix
// //     SLP_TYPa = *(S5Addr) << 10;
// //     S5Addr++;

// //     if (*S5Addr == 0x0A) S5Addr++;  // skip u8prefix
// //     SLP_TYPb = *(S5Addr) << 10;

// //     SMI_CMD = facp->SMI_CMD;

// //     ACPI_ENABLE = facp->ACPI_ENABLE;
// //     ACPI_DISABLE = facp->ACPI_DISABLE;

// //     PM1a_CNT = facp->PM1a_CNT_BLK;
// //     PM1b_CNT = facp->PM1b_CNT_BLK;

// //     PM1_CNT_LEN = facp->PM1_CNT_LEN;

// //     SLP_EN = 1 << 13;
// //     SCI_EN = 1;

// //     return 0;

// //     ptr++;
// //   }

// //   printf("no valid FACP present.\n");
// //   return FAILURE;
// // }

// // void acpiPowerOff(void) {
// //   // SCI_EN is set to 1 if acpi shutdown is possible
// //   if (SCI_EN == 0) return;

// //   acpi_enable();

// //   // send the shutdown command
// //   outw((unsigned int)PM1a_CNT, SLP_TYPa | SLP_EN);
// //   if (PM1b_CNT != 0) outw((unsigned int)PM1b_CNT, SLP_TYPb | SLP_EN);

// //   printf("acpi poweroff failed.\n");
// // }
