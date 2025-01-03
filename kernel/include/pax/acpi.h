#ifndef _RSDP_H
#define _RSDP_H

// #include <stdbool.h>
// #include <stddef.h>
// #include <sys/io.h>
// #include <sys/types.h>

// #define FIELD_OFFSET(s, name) (offsetof(s, name) / sizeof(void *))

// enum acpi_version_t { ACPI_VERSION_1 = 0, ACPI_VERSION_2_TO_6 = 2 };

// struct rsdp_t {
//   struct rsdp_v1_t *v1;
//   struct rsdp_v2_t *v2;
// };

// struct rsdp_v1_t {
//   u8 signature[8];
//   u8 checksum;
//   u8 oemid[6];
//   u8 revision;
//   u32 rsdt_addr;
// } __attribute__((packed));

// struct rsdp_v2_t {
//   u32 len;
//   u64 xsdt_addr;
//   u8 extended_checksum;
//   u8 reserved[3];
// } __attribute__((packed));

// enum acpi_profile_t {
//   ACPI_PROFILE_UNSPECIFIED = 0,
//   ACPI_PROFILE_DESKTOP = 1,
//   ACPI_PROFILE_MOBILE = 2,
//   ACPI_PROFILE_WORKSTATION = 3,
//   ACPI_PROFILE_ENTERPRISE_SERVER = 4,
//   ACPI_PROFILE_SOHO_SERVER = 5,
//   ACPI_PROFILE_APPLICANCE_PC = 6,
//   ACPI_PROFILE_PERFORMANCE_SERVER = 7,
//   ACPI_PROFILE_TABLET = 8
// };

// struct __attribute__((packed)) rsdt_t {
//   u8 signature[4];
//   u32 length;
//   u8 revision;
//   u8 checksum;
//   u8 oemid[6];
//   u64 oem_tableid;
//   u32 oem_revision;
//   u8 creator_id[4];
//   u32 creator_revision;
//   u32 *entries;
// };

// struct fadt_t {
//   u8 signature[4];
//   u32 length;
//   u8 fadt_major_version;
//   u8 checksum;
//   u8 oemid[6];
//   u64 oem_table_id;
//   u32 oem_revision;
//   u32 creator_id;
//   u32 creator_revision;
//   u32 firmware_ctrl;
//   u32 dsdt;
//   u8 _reserved0;
//   u8 preferred_pm_profile;
//   u16 sci_int;
//   u32 smi_cmd;
//   u8 acpi_enable;
//   u8 acpi_disable;
//   u8 s4bios_req;
//   u8 pstate_cnt;
//   u32 pm1a_evt_blk;
//   u32 bm1b_evt_blk;
//   u32 bm1a_cnt_blk;
//   u32 pm1b_cnt_blk;
//   u32 pm2_cnt_blk;
//   u32 pm_tmr_blk;
//   u32 gpe0_blk;
//   u32 gpe1_blk;
//   u8 pm1_evt_len;
//   u8 pm1_cnt_len;
//   u8 pm2_cnt_len;
//   u8 pm_tmr_len;
//   u8 gpe0_blk_len;
//   u8 gpe1_blk_len;
//   u8 gpe1_base;
//   u8 cst_cnt;
//   u16 p_lvl2_lat;
//   u16 p_lvl3_lat;
//   u16 flush_size;
//   u16 flush_stride;
//   u8 duty_offset;
//   u8 duty_width;
//   u8 day_alrm;
//   u8 mon_alrm;
//   u8 century;
//   u16 iapc_boot_arch;
//   u8 _reserved1;
//   u32 flags;
//   u8 reset_reg[12];
//   u8 reset_value;
//   u16 arm_boot_arch;
//   u8 fadt_minor_version;
//   u64 x_firmware_ctrl;
//   u64 x_dsdt;
//   u8 x_pm1a_evt_blk[12];
//   u8 xpm1b_evt_blk[12];
//   u8 xpm1a_cnt_blk[12];
//   u8 xpm1b_cnt_blk[12];
//   u8 xpm2_cnt_blk[12];
//   u8 x_pm_tmr_blk[12];
//   u8 x_gpe0_blk[12];
//   u8 x_gpe1_blk[12];
//   u8 sleep_control_reg[12];
//   u8 sleep_status_reg[12];
//   u64 hypervisor_vendor_identity;
// } __attribute__((packed));

// struct dsdt_t {
//   u8 signature[4];
//   u32 length;
//   u8 revision;
//   u8 checksum;
//   u8 oemid[6];
//   u64 oem_table_id;
//   u32 oem_revision;
//   u32 creator_id;
//   u32 creator_revision;
//   u8 *definition_block;
// } __attribute__((packed));

// struct fadt_t {
//   u8 signature[4];
//   u32 length;
//   u8 version;
//   u8 checksum;
//   u8 oemid[6];
//   u64 oem_table_id;
//   u32 oem_revision;
//   u32 creator_id;
//   u32 creator_revision;
//   u32 firmware_ctrl;
//   u32 dsdt;
//   u8 __reserved;
//   u8 preferred_pm_profile;  // acpi_profile_t
//   u16 sci_int;
//   u32 smi_cmd;
//   u8 acpi_enable;
//   u8 acpi_disable;
//   u8 s4bios_req;
//   u8 pstate_cnt;
//   u32 pm1a_evt_blk;
//   u32 pm1b_evt_blk;
//   u32 pm1a_cnt_blk;
//   u32 pm1b_cnt_blk;
//   u32 pm2_cnt_blk;
//   u32 pm_tmr_blk;
//   u32 gpe0_blk;
//   u32 gpe1_blk;
//   u8 pm1_evt_len;
//   u8 pm1_cnt_len;
//   u8 pm2_cnt_len;
//   u8 pm_tmr_len;
//   u8 gpeo_blk_len;
//   u8 gpe1_blk_len;
//   u8 gpe1_base;
//   u8 cst_cnt;
//   u16 p_lvl2_lat;
//   u16 p_lvl3_lat;
// } __attribute__((packed));

/*
        Find "RSD PTR" signature on 16 byte boundary in a given memory
   region
*/

/*
        Locate the RSDP at either:
        1. First KB of the EBDA located at 0x40E
        2. The region 0xE0000-0xFFFFF
*/
// bool acpi_init(void);
// struct rsdt_t *acpi_get_rsdp(void);
// bool acpi_set_mode(struct fadt_t *fadt);

#endif
