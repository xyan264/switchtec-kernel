/*
 * Microsemi Switchtec PCIe Driver
 * Copyright (c) 2016, Microsemi Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#ifndef SWITCHTEC_H
#define SWITCHTEC_H

#include <linux/pci.h>
#include <linux/cdev.h>

#define MICROSEMI_VENDOR_ID         0x11f8
#define MICROSEMI_NTB_CLASSCODE     0x068000
#define MICROSEMI_MGMT_CLASSCODE    0x058000

#define SWITCHTEC_MRPC_PAYLOAD_SIZE 1024

enum {
	SWITCHTEC_GAS_MRPC_OFFSET       = 0x0000,
	SWITCHTEC_GAS_TOP_CFG_OFFSET    = 0x1000,
	SWITCHTEC_GAS_SW_EVENT_OFFSET   = 0x1800,
	SWITCHTEC_GAS_SYS_INFO_OFFSET   = 0x2000,
	SWITCHTEC_GAS_FLASH_INFO_OFFSET = 0x2200,
	SWITCHTEC_GAS_PART_CFG_OFFSET   = 0x4000,
	SWITCHTEC_GAS_NTB_OFFSET        = 0x10000,
	SWITCHTEC_GAS_PFF_CSR_OFFSET    = 0x134000,
};

struct mrpc_regs {
	u8 input_data[SWITCHTEC_MRPC_PAYLOAD_SIZE];
	u8 output_data[SWITCHTEC_MRPC_PAYLOAD_SIZE];
	u32 cmd;
	u32 status;
	u32 ret_value;
} __packed;

enum mrpc_status {
	SWITCHTEC_MRPC_STATUS_INPROGRESS = 1,
	SWITCHTEC_MRPC_STATUS_DONE = 2,
	SWITCHTEC_MRPC_STATUS_ERROR = 0xFF,
	SWITCHTEC_MRPC_STATUS_INTERRUPTED = 0x100,
};

struct sw_event_regs {
	u64 event_report_ctrl;
	u64 reserved1;
	u64 part_event_bitmap;
	u64 reserved2;
	u32 global_summary;
	u32 reserved3[3];
	u32 stack_error_event_hdr;
	u32 stack_error_event_data;
	u32 reserved4[4];
	u32 ppu_error_event_hdr;
	u32 ppu_error_event_data;
	u32 reserved5[4];
	u32 isp_error_event_hdr;
	u32 isp_error_event_data;
	u32 reserved6[34];
	u32 twi_mrpc_comp_hdr;
	u32 twi_mrpc_comp_data;
	u32 reserved7[4];
	u32 twi_mrpc_comp_async_hdr;
	u32 twi_mrpc_comp_async_data;
	u32 reserved8[4];
	u32 cli_mrpc_cmp_hdr;
	u32 cli_mrpc_cmp_data;
	u32 reserved9[4];
	u32 cli_mrpc_comp_async_hdr;
	u32 cli_mrpc_comp_async_data;
	u32 reserved10[4];
	u32 gpio_interrupt_hdr;
	u32 gpio_interrupt_data;
	u32 reserved11[4];
} __packed;

struct sys_info_regs {
	u32 device_id;
	u32 device_version;
	u32 firmware_version;
	u32 reserved1;
	u32 vendor_table_revision;
	u32 table_format_version;
	u32 partition_id;
	u32 cfg_file_fmt_version;
	u32 reserved2[58];
	char vendor_id[8];
	char product_id[16];
	char product_revision[4];
	char component_vendor[8];
	u16 component_id;
	u8 component_revision;
} __packed;

struct flash_info_regs {
	uint32_t flash_part_map_upd_idx;

	struct active_partition_info {
		uint32_t address;
		uint32_t build_version;
		uint32_t build_string;
	} active_img;

	struct active_partition_info active_cfg;
	struct active_partition_info inactive_img;
	struct active_partition_info inactive_cfg;

	uint32_t flash_length;

	struct partition_info {
		uint32_t address;
		uint32_t length;
	} cfg0;

	struct partition_info cfg1;
	struct partition_info img0;
	struct partition_info img1;
	struct partition_info nvlog;
	struct partition_info vendor[8];
};

struct ntb_info_regs {
	u8  partition_count;
	u8  partition_id;
	u16 reserved1;
	u64 ep_map;
	u16 requester_id;
} __packed;

struct part_cfg_regs {
	u32 status;
	u32 state;
	u32 port_cnt;
	u32 usp_port_mode;
	u32 usp_pff_inst_id;
	u32 vep_pff_inst_id;
	u32 dsp_inst_id[47];
	u32 reserved1[11];
	u16 vep_vector_number;
	u16 usp_vector_number;
	u32 port_event_bitmap;
	u32 reserved2[3];
	u32 part_event_summary;
	u32 reserved3[3];
	u32 part_reset_hdr;
	u32 part_reset_data[5];
	u32 mrpc_comp_hdr;
	u32 mrpc_comp_data[5];
	u32 mrpc_comp_async_hdr;
	u32 mrpc_comp_async_data[5];
	u32 dyn_binding_hdr;
	u32 dyn_binding_data[5];
	u32 reserved4[159];
} __packed;

enum {
	SWITCHTEC_PART_CFG_EVENT_RESET = 1 << 0,
	SWITCHTEC_PART_CFG_EVENT_MRPC_CMP = 1 << 1,
	SWITCHTEC_PART_CFG_EVENT_MRPC_ASYNC_CMP = 1 << 2,
	SWITCHTEC_PART_CFG_EVENT_DYN_PART_CMP = 1 << 3,
};

struct pff_csr_regs {
	u16 vendor_id;
	u16 device_id;
	u32 pci_cfg_header[15];
	u32 pci_cap_region[48];
	u32 pcie_cap_region[448];
	u32 indirect_gas_window[128];
	u32 indirect_gas_window_off;
	u32 reserved[127];
	u32 port_event_summary;
	u32 reserved2[3];
	u32 aer_in_p2p_hdr;
	u32 aer_in_p2p_data[5];
	u32 aer_in_vep_hdr;
	u32 aer_in_vep_data[5];
	u32 dpc_hdr;
	u32 dpc_data[5];
	u32 cts_hdr;
	u32 cts_data[5];
	u32 reserved3[6];
	u32 hotplug_hdr;
	u32 hotplug_data[5];
	u32 ier_hdr;
	u32 ier_data[5];
	u32 threshold_hdr;
	u32 threshold_data[5];
	u32 power_mgmt_hdr;
	u32 power_mgmt_data[5];
	u32 tlp_throttling_hdr;
	u32 tlp_throttling_data[5];
	u32 force_speed_hdr;
	u32 force_speed_data[5];
	u32 credit_timeout_hdr;
	u32 credit_timeout_data[5];
	u32 link_state_hdr;
	u32 link_state_data[5];
	u32 reserved4[174];
} __packed;

struct switchtec_dev {
	struct pci_dev *pdev;
	struct msix_entry msix[4];
	struct device dev;
	struct cdev cdev;
	unsigned int event_irq;

	void __iomem *mmio;
	struct mrpc_regs __iomem *mmio_mrpc;
	struct sw_event_regs __iomem *mmio_sw_event;
	struct sys_info_regs __iomem *mmio_sys_info;
	struct flash_info_regs __iomem *mmio_flash_info;
	struct ntb_info_regs __iomem *mmio_ntb;
	struct part_cfg_regs __iomem *mmio_part_cfg;
	struct pff_csr_regs __iomem *mmio_pff_csr;

	/*
	 * The mrpc mutex must be held when accessing the other
	 * mrpc fields
	 */
	struct mutex mrpc_mutex;
	struct list_head mrpc_queue;
	int mrpc_busy;
	struct work_struct mrpc_work;
	struct delayed_work mrpc_timeout;
};

#endif
