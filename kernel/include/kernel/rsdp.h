#ifndef RSDP_H
#define RSDP_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

enum acpi_version_t { ACPI_VERSION_1 = 0, ACPI_VERSION_2_TO_6 = 2 };

struct rsdp_t {
  struct rsdp_v1_t *v1;
  struct rsdp_v2_t *v2;
};

struct rsdp_v1_t {
  char signature[8];
  u8 checksum;
  char oemid[6];
  u8 revision;
  u32 rsdt_addr;
} __attribute__((packed));

struct rsdp_v2_t {
  u32 len;
  u64 xsdt_addr;
  u8 extended_checksum;
  u8 reserved[3];
} __attribute__((packed));

enum acpi_profile_t {
  ACPI_PROFILE_UNSPECIFIED = 0,
  ACPI_PROFILE_DESKTOP = 1,
  ACPI_PROFILE_MOBILE = 2,
  ACPI_PROFILE_WORKSTATION = 3,
  ACPI_PROFILE_ENTERPRISE_SERVER = 4,
  ACPI_PROFILE_SOHO_SERVER = 5,
  ACPI_PROFILE_APPLICANCE_PC = 6,
  ACPI_PROFILE_PERFORMANCE_SERVER = 7,
  ACPI_PROFILE_TABLET = 8
};

struct facp_t {
  u8 signature[4];
  u32 length;
  u8 version;
  u8 checksum;
  u8 oemid[6];
  u64 oem_table_id;
  u32 oem_revision;
  u32 creator_id;
  u32 creator_revision;
  u32 firmware_ctrl;
  u32 dsdt;
  u8 __reserved;
  u8 preferred_pm_profile;  // acpi_profile_t
  u16 sci_int;
  u32 sci_cmd;
  u8 acpi_enable;
  u8 acpi_disable;
  u8 s4bios_req;
  u8 pstate_cnt;
  u32 pm1a_evt_blk;
  u32 pm1b_evt_blk;
  u32 pm1a_cnt_blk;
  u32 pm1b_cnt_blk;
  u32 pm2_cnt_blk;
  u32 pm_tmr_blk;
  u32 gpe0_blk;
  u32 gpe1_blk;
  u8 pm1_evt_len;
  u8 pm1_cnt_len;
  u8 pm2_cnt_len;
  u8 pm_tmr_len;
  u8 gpeo_blk_len;
  u8 gpe1_blk_len;
  u8 gpe1_base;
  u8 cst_cnt;
  u16 p_lvl2_lat;
  u16 p_lvl3_lat;
} __attribute__((packed));

/*
        Find "RSD PTR" signature on 16 byte boundary in a given memory region
*/
static char calculate_checksum(u8 *buf, size_t size);
static struct rsdp_v1_t *acpi_check_rsdp_addr(struct rsdp_v1_t *addr);
static struct rsdp_v1_t *acpi_get_rsdp_in_region(uintptr_t start,
                                                 uintptr_t end);
static bool acpi_validate_header(void *buf, const char *sig);

/*
        Locate the RSDP at either:
        1. First KB of the EBDA located at 0x40E
        2. The region 0xE0000-0xFFFFF
*/
struct facp_t *acpi_get_rsdp(void);

#endif
