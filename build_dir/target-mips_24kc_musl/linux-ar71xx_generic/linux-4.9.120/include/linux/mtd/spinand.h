/*
 *
 * Copyright (c) 2016-2017 Micron Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __LINUX_MTD_SPINAND_H
#define __LINUX_MTD_SPINAND_H

#include <linux/mutex.h>
#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/of.h>

/**
 * Standard SPI NAND flash commands
 */
#define SPINAND_CMD_RESET			0xff
#define SPINAND_CMD_GET_FEATURE			0x0f
#define SPINAND_CMD_SET_FEATURE			0x1f
#define SPINAND_CMD_PAGE_READ			0x13
#define SPINAND_CMD_READ_FROM_CACHE		0x03
#define SPINAND_CMD_READ_FROM_CACHE_FAST	0x0b
#define SPINAND_CMD_READ_FROM_CACHE_X2		0x3b
#define SPINAND_CMD_READ_FROM_CACHE_DUAL_IO	0xbb
#define SPINAND_CMD_READ_FROM_CACHE_X4		0x6b
#define SPINAND_CMD_READ_FROM_CACHE_QUAD_IO	0xeb
#define SPINAND_CMD_BLK_ERASE			0xd8
#define SPINAND_CMD_PROG_EXC			0x10
#define SPINAND_CMD_PROG_LOAD			0x02
#define SPINAND_CMD_PROG_LOAD_RDM_DATA		0x84
#define SPINAND_CMD_PROG_LOAD_X4		0x32
#define SPINAND_CMD_PROG_LOAD_RDM_DATA_X4	0x34
#define SPINAND_CMD_READ_ID			0x9f
#define SPINAND_CMD_WR_DISABLE			0x04
#define SPINAND_CMD_WR_ENABLE			0x06

/* feature register */
#define REG_BLOCK_LOCK		0xa0
#define REG_CFG			0xb0
#define REG_STATUS		0xc0

/* status register */
#define STATUS_OIP_MASK		BIT(0)
#define STATUS_CRBSY_MASK	BIT(7)
#define STATUS_READY		0
#define STATUS_BUSY		BIT(0)

#define STATUS_E_FAIL_MASK	BIT(2)
#define STATUS_E_FAIL		BIT(2)

#define STATUS_P_FAIL_MASK	BIT(3)
#define STATUS_P_FAIL		BIT(3)

/* configuration register */
#define CFG_ECC_MASK		BIT(4)
#define CFG_ECC_ENABLE		BIT(4)

/* block lock register */
#define BL_ALL_UNLOCKED		0X00

struct spinand_op;
struct spinand_device;
struct nand_device;

/**
 * struct nand_memory_organization - memory organization structure
 * @bits_per_cell: number of bits per NAND cell
 * @pagesize: page size
 * @oobsize: OOB area size
 * @pages_per_eraseblock: number of pages per eraseblock
 * @eraseblocks_per_die: number of eraseblocks per die
 * @ndies: number of dies
 */
struct nand_memory_organization {
	unsigned int bits_per_cell;
	unsigned int pagesize;
	unsigned int oobsize;
	unsigned int pages_per_eraseblock;
	unsigned int eraseblocks_per_lun;
	unsigned int planes_per_lun;
	unsigned int luns_per_target;
	unsigned int ntargets;
};

#define NAND_MEMORG(bpc, ps, os, ppe, epl, ppl, lpt, nt)	\
	{							\
		.bits_per_cell = (bpc),				\
		.pagesize = (ps),				\
		.oobsize = (os),				\
		.pages_per_eraseblock = (ppe),			\
		.eraseblocks_per_lun = (epl),			\
		.planes_per_lun = (ppl),			\
		.luns_per_target = (lpt),			\
		.ntargets = (nt),				\
	}

/**
 * struct nand_bbt - bad block table structure
 * @cache: in memory BBT cache
 */
struct nand_bbt {
	unsigned char *cache;
};

struct nand_row_converter {
	unsigned int lun_addr_shift;
	unsigned int eraseblock_addr_shift;
};

struct nand_pos {
	unsigned int target;
	unsigned int lun;
	unsigned int plane;
	unsigned int eraseblock;
	unsigned int page;
};

struct nand_page_io_req {
	struct nand_pos pos;
	unsigned int dataoffs;
	unsigned int datalen;
	union {
		const void *out;
		void *in;
	} databuf;
	unsigned int ooboffs;
	unsigned int ooblen;
	union {
		const void *out;
		void *in;
	} oobbuf;
};
/**
 * struct nand_ops - NAND operations
 * @erase: erase a specific block
 * @markbad: mark a specific block bad
 */
struct nand_ops {
	int (*erase)(struct nand_device *nand, const struct nand_pos *pos);
	int (*markbad)(struct nand_device *nand, const struct nand_pos *pos);
	bool (*isbad)(struct nand_device *nand, const struct nand_pos *pos);
};

struct nand_ecc_req {
	unsigned int strength;
	unsigned int step_size;
};

#define NAND_ECCREQ(str, stp) { .strength = (str), .step_size = (stp) }

struct nand_device{
	struct mtd_info mtd;
	struct nand_memory_organization memorg;
	struct nand_ecc_req eccreq;
	struct nand_row_converter rowconv;
	struct nand_bbt bbt;
	const struct nand_ops *ops;
};

#define SPINAND_MAX_ID_LEN	4

/**
 * struct spinand_id - SPI NAND id structure
 * @data: buffer containing the id bytes. Currently 4 bytes large, but can
 *	  be extended if required.
 * @len: ID length
 */
struct spinand_id {
	u8 data[SPINAND_MAX_ID_LEN];
	int len;
};

/**
 * struct spinand_controller_ops - SPI NAND controller operations
 * @exec_op: executute SPI NAND operation
 */
struct spinand_controller_ops {
	int (*exec_op)(struct spinand_device *spinand,
		       struct spinand_op *op);
};


/**
 * struct manufacurer_ops - SPI NAND manufacturer specified operations
 * @detect: detect SPI NAND device, should bot be NULL.
 *          ->detect() implementation for manufacturer A never sends
 *          any manufacturer specific SPI command to a SPI NAND from
 *          manufacturer B, so the proper way is to decode the raw id
 *          data in spinand->id.data first, if manufacture ID dismatch,
 *          return directly and let others to detect.
 * @init: initialize SPI NAND device.
 * @cleanup: clean SPI NAND device footprint.
 * @prepare_op: prepara read/write operation.
 */
struct spinand_manufacturer_ops {
	bool (*detect)(struct spinand_device *spinand);
	int (*init)(struct spinand_device *spinand);
	void (*cleanup)(struct spinand_device *spinand);
	void (*adjust_cache_op)(struct spinand_device *spinand,
				const struct nand_page_io_req *req,
				struct spinand_op *op);
};

/**
 * struct spinand_manufacturer - SPI NAND manufacturer instance
 * @id: manufacturer ID
 * @name: manufacturer name
 * @ops: point to manufacturer operations
 */
struct spinand_manufacturer {
	u8 id;
	char *name;
	const struct spinand_manufacturer_ops *ops;
};

extern const struct spinand_manufacturer micron_spinand_manufacturer;
extern const struct spinand_manufacturer etron_spinand_manufacturer;
extern const struct spinand_manufacturer paragon_spinand_manufacturer;
extern const struct spinand_manufacturer giga_spinand_manufacturer;

#define SPINAND_CAP_RD_X1	BIT(0)
#define SPINAND_CAP_RD_X2	BIT(1)
#define SPINAND_CAP_RD_X4	BIT(2)
#define SPINAND_CAP_RD_DUAL	BIT(3)
#define SPINAND_CAP_RD_QUAD	BIT(4)
#define SPINAND_CAP_WR_X1	BIT(5)
#define SPINAND_CAP_WR_X2	BIT(6)
#define SPINAND_CAP_WR_X4	BIT(7)
#define SPINAND_CAP_WR_DUAL	BIT(8)
#define SPINAND_CAP_WR_QUAD	BIT(9)

/**
 * struct spinand_controller - SPI NAND controller instance
 * @ops: point to controller operations
 * @caps: controller capabilities
 */
struct spinand_controller {
	struct spinand_controller_ops *ops;
	u32 caps;
};

/**
 * struct spinand_device - SPI NAND device instance
 * @base: NAND device instance
 * @bbp: internal bad block pattern descriptor
 * @lock: protection lock
 * @id: ID structure
 * @read_cache_op: Opcode of read from cache
 * @write_cache_op: Opcode of program load
 * @buf: buffer for read/write data
 * @oobbuf: buffer for read/write oob
 * @rw_mode: read/write mode of SPI NAND device
 * @controller: SPI NAND controller instance
 * @manufacturer: SPI NAND manufacturer instance, describe
 *                manufacturer related objects
 */
struct spinand_device {
	struct nand_device base;
	struct mutex lock;
	struct spinand_id id;
	u8 read_cache_op;
	u8 write_cache_op;
	u8 *buf;
	u8 *oobbuf;
	u32 rw_mode;
	struct {
		struct spinand_controller *controller;
		void *priv;
	} controller;
	struct {
		const struct spinand_manufacturer *manu;
		void *priv;
	} manufacturer;
};

/**
 * struct nand_io_iter - NAND I/O iterator
 * @req: current I/O request
 * @oobbytes_per_page: maximun oob bytes per page
 * @dataleft: remaining number of data bytes to read/write
 * @oobleft: remaining number of OOB bytes to read/write
 */
struct nand_io_iter {
	struct nand_page_io_req req;
	unsigned int oobbytes_per_page;
	unsigned int dataleft;
	unsigned int oobleft;
};

/**
 * mtd_to_nanddev - Get the NAND device attached to the MTD instance
 * @mtd: MTD instance
 *
 * Return: the NAND device embedding @mtd.
 */
static inline struct nand_device *mtd_to_nanddev(struct mtd_info *mtd)
{
	return container_of(mtd, struct nand_device, mtd);
}
/**
 * nanddev_to_mtd - Get the MTD device attached to a NAND device
 * @nand: NAND device
 *
 * Return: the MTD device embedded in @nand.
 */
static inline struct mtd_info *nanddev_to_mtd(struct nand_device *nand)
{
	return &nand->mtd;
}

/**
 * mtd_to_spinand - Get the SPI NAND device attached to the MTD instance
 * @mtd: MTD instance
 *
 * Returns the SPI NAND device attached to @mtd.
 */
static inline struct spinand_device *mtd_to_spinand(struct mtd_info *mtd)
{
	return container_of(mtd_to_nanddev(mtd), struct spinand_device, base);
}

/**
 * spinand_to_mtd - Get the MTD device attached to the SPI NAND device
 * @spinand: SPI NAND device
 *
 * Returns the MTD device attached to @spinand.
 */
static inline struct mtd_info *spinand_to_mtd(struct spinand_device *spinand)
{
	return nanddev_to_mtd(&spinand->base);
}

/**
 * nand_to_spinand - Get the SPI NAND device embedding an NAND object
 * @nand: NAND object
 *
 * Returns the SPI NAND device embedding @nand.
 */
static inline struct spinand_device *nand_to_spinand(struct nand_device *nand)
{
	return container_of(nand, struct spinand_device, base);
}

/**
 * spinand_to_nand - Get the NAND device embedded in a SPI NAND object
 * @spinand: SPI NAND device
 *
 * Returns the NAND device embedded in @spinand.
 */
static inline struct nand_device *
spinand_to_nand(struct spinand_device *spinand)
{
	return &spinand->base;
}

/**
 * nanddev_set_of_node - Attach a DT node to a NAND device
 * @nand: NAND device
 * @np: DT node
 *
 * Attach a DT node to a NAND device.
 */
static inline void nanddev_set_of_node(struct nand_device *nand,
				       struct device_node *np)
{
	mtd_set_of_node(&nand->mtd, np);
}

/**
 * spinand_set_of_node - Attach a DT node to a SPI NAND device
 * @spinand: SPI NAND device
 * @np: DT node
 *
 * Attach a DT node to a SPI NAND device.
 */
static inline void spinand_set_of_node(struct spinand_device *spinand,
				       struct device_node *np)
{
	nanddev_set_of_node(&spinand->base, np);
}

#define SPINAND_MAX_ADDR_LEN	4

/**
 * struct spinand_op - SPI NAND operation description
 * @cmd: opcode to send
 * @n_addr: address bytes
 * @addr_nbits: number of bit used to transfer address
 * @dummy_types: dummy bytes followed address
 * @addr: address or dummy bytes buffer
 * @n_tx: size of tx_buf
 * @tx_buf: data to be written
 * @n_rx: size of rx_buf
 * @rx_buf: data to be read
 * @data_nbits: number of bit used to transfer data
 */
struct spinand_op {
	u8 cmd;
	u8 n_addr;
	u8 addr_nbits;
	u8 dummy_bytes;
	u8 addr[SPINAND_MAX_ADDR_LEN];
	u32 n_tx;
	const u8 *tx_buf;
	u32 n_rx;
	u8 *rx_buf;
	u8 data_nbits;
};
/**
 * nanddev_neraseblocks - Get the total number of erasablocks
 * @nand: NAND device
 *
 * Return: the number of eraseblocks exposed by @nand.
 */
static inline unsigned int nanddev_neraseblocks(const struct nand_device *nand)
{
	return (u64)nand->memorg.luns_per_target *
	       nand->memorg.eraseblocks_per_lun *
	        nand->memorg.ntargets;
}

/* BBT related functions */
enum nand_bbt_block_status {
	NAND_BBT_BLOCK_STATUS_UNKNOWN,
	NAND_BBT_BLOCK_GOOD,
	NAND_BBT_BLOCK_WORN,
	NAND_BBT_BLOCK_RESERVED,
	NAND_BBT_BLOCK_FACTORY_BAD,
	NAND_BBT_BLOCK_NUM_STATUS,
};
int nanddev_bbt_init(struct nand_device *nand);
void nanddev_bbt_cleanup(struct nand_device *nand);
int nanddev_bbt_update(struct nand_device *nand);
int nanddev_bbt_get_block_status(const struct nand_device *nand,
				 unsigned int entry);
int nanddev_bbt_set_block_status(struct nand_device *nand, unsigned int entry,
				 enum nand_bbt_block_status status);

/* SPI NAND supported OP mode */
#define SPINAND_RD_X1		BIT(0)
#define SPINAND_RD_X2		BIT(1)
#define SPINAND_RD_X4		BIT(2)
#define SPINAND_RD_DUAL		BIT(3)
#define SPINAND_RD_QUAD		BIT(4)
#define SPINAND_WR_X1		BIT(5)
#define SPINAND_WR_X2		BIT(6)
#define SPINAND_WR_X4		BIT(7)
#define SPINAND_WR_DUAL		BIT(8)
#define SPINAND_WR_QUAD		BIT(9)

#define SPINAND_RD_COMMON	(SPINAND_RD_X1 | SPINAND_RD_X2 | \
				 SPINAND_RD_X4 | SPINAND_RD_DUAL | \
				 SPINAND_RD_QUAD)
#define SPINAND_WR_COMMON	(SPINAND_WR_X1 | SPINAND_WR_X4)
#define SPINAND_RW_COMMON	(SPINAND_RD_COMMON | SPINAND_WR_COMMON)

struct spinand_device *devm_spinand_alloc(struct device *dev);
int spinand_init(struct spinand_device *spinand, struct module *owner);
void spinand_cleanup(struct spinand_device *spinand);

/**
 * nanddev_page_size - Get NAND page size
 * @nand: NAND device
 *
 * Return: the page size.
 */
static inline size_t nanddev_page_size(const struct nand_device *nand)
{
	return nand->memorg.pagesize;
}

/**
 * nanddev_per_page_oobsize - Get NAND OOB size
 * @nand: NAND device
 *
 * Return: the OOB size.
 */
static inline unsigned int
nanddev_per_page_oobsize(const struct nand_device *nand)
{
	return nand->memorg.oobsize;
}

/**
 * nanddev_pages_per_eraseblock - Get the number of pages per eraseblock
 * @nand: NAND device
 *
 * Return: the number of pages per eraseblock.
 */
static inline unsigned int
nanddev_pages_per_eraseblock(const struct nand_device *nand)
{
	return nand->memorg.pages_per_eraseblock;
}

/**
 * nanddev_per_page_oobsize - Get NAND erase block size
 * @nand: NAND device
 *
 * Return: the eraseblock size.
 */
static inline size_t nanddev_eraseblock_size(const struct nand_device *nand)
{
	return nand->memorg.pagesize * nand->memorg.pages_per_eraseblock;
}

static inline u64 nanddev_target_size(const struct nand_device *nand)
{
	return (u64)nand->memorg.luns_per_target *
	       nand->memorg.eraseblocks_per_lun *
	       nand->memorg.pages_per_eraseblock *
	       nand->memorg.pagesize;
}

/**
 * nanddev_ntarget - Get the total of targets
 * @nand: NAND device
 *
 * Return: the number of dies exposed by @nand.
 */
static inline unsigned int nanddev_ntargets(const struct nand_device *nand)
{
	return nand->memorg.ntargets;
}

/**
 * nanddev_size - Get NAND size
 * @nand: NAND device
 *
 * Return: the total size exposed of @nand.
 */
static inline u64 nanddev_size(const struct nand_device *nand)
{
	return nanddev_target_size(nand) * nanddev_ntargets(nand);
}

/**
 * nanddev_get_memorg - Extract memory organization info from a NAND device
 * @nand: NAND device
 *
 * This can be used by the upper layer to fill the memorg info before calling
 * nanddev_init().
 *
 * Return: the memorg object embedded in the NAND device.
 */
static inline struct nand_memory_organization *
nanddev_get_memorg(struct nand_device *nand)
{
	return &nand->memorg;
}


static inline unsigned int nanddev_pos_to_row(struct nand_device *nand,
					      const struct nand_pos *pos)
{
	return (pos->lun << nand->rowconv.lun_addr_shift) |
	       (pos->eraseblock << nand->rowconv.eraseblock_addr_shift) |
	       pos->page;
}


static inline unsigned int nanddev_offs_to_pos(struct nand_device *nand,
					       loff_t offs,
					       struct nand_pos *pos)
{
	unsigned int pageoffs;
	u64 tmp = offs;

	pageoffs = do_div(tmp, nand->memorg.pagesize);
	pos->page = do_div(tmp, nand->memorg.pages_per_eraseblock);
	pos->eraseblock = do_div(tmp, nand->memorg.eraseblocks_per_lun);
	pos->plane = pos->eraseblock % nand->memorg.planes_per_lun;
	pos->lun = do_div(tmp, nand->memorg.luns_per_target);
	pos->target = tmp;

	return pageoffs;
}

static inline int nanddev_pos_cmp(const struct nand_pos *a,
				  const struct nand_pos *b)
{
	if (a->target != b->target)
		return a->target < b->target ? -1 : 1;

	if (a->lun != b->lun)
		return a->lun < b->lun ? -1 : 1;

	if (a->eraseblock != b->eraseblock)
		return a->eraseblock < b->eraseblock ? -1 : 1;

	if (a->page != b->page)
		return a->page < b->page ? -1 : 1;

	return 0;
}

static inline void nanddev_pos_next_target(struct nand_device *nand,
					   struct nand_pos *pos)
{
	pos->page = 0;
	pos->plane = 0;
	pos->eraseblock = 0;
	pos->lun = 0;
	pos->target++;
}

static inline void nanddev_pos_next_lun(struct nand_device *nand,
					struct nand_pos *pos)
{
	if (pos->lun >= nand->memorg.luns_per_target - 1)
		return nanddev_pos_next_target(nand, pos);

	pos->lun++;
	pos->page = 0;
	pos->plane = 0;
	pos->eraseblock = 0;
}

static inline void nanddev_pos_next_eraseblock(struct nand_device *nand,
					       struct nand_pos *pos)
{
	if (pos->eraseblock >= nand->memorg.eraseblocks_per_lun - 1)
		return nanddev_pos_next_lun(nand, pos);

	pos->eraseblock++;
	pos->page = 0;
	pos->plane = pos->eraseblock % nand->memorg.planes_per_lun;
}

static inline loff_t nanddev_pos_to_offs(struct nand_device *nand,
					 const struct nand_pos *pos)
{
	unsigned int npages;

	npages = pos->page +
		 ((pos->eraseblock +
		   (pos->lun +
		    (pos->target * nand->memorg.luns_per_target)) *
		   nand->memorg.eraseblocks_per_lun) *
		  nand->memorg.pages_per_eraseblock);

	return (loff_t)npages * nand->memorg.pagesize;
}

static inline void nanddev_pos_next_page(struct nand_device *nand,
					 struct nand_pos *pos)
{
	if (pos->page >= nand->memorg.pages_per_eraseblock - 1)
		return nanddev_pos_next_eraseblock(nand, pos);

	pos->page++;
}

/**
 * nand_io_iter_init - Initialize a NAND I/O iterator
 * @nand: NAND device
 * @offs: absolute offset
 * @req: MTD request
 * @iter: page iterator
 */
static inline void nanddev_io_iter_init(struct nand_device *nand,
					loff_t offs, struct mtd_oob_ops *req,
					struct nand_io_iter *iter)
{
	struct mtd_info *mtd = nanddev_to_mtd(nand);

	iter->req.dataoffs = nanddev_offs_to_pos(nand, offs, &iter->req.pos);
	iter->req.ooboffs = req->ooboffs;
	iter->oobbytes_per_page = mtd_oobavail(mtd, req);
	iter->dataleft = req->len;
	iter->oobleft = req->ooblen;
	iter->req.databuf.in = req->datbuf;
	iter->req.datalen = min_t(unsigned int,
				  nand->memorg.pagesize - iter->req.dataoffs,
				  iter->dataleft);
	iter->req.oobbuf.in = req->oobbuf;
	iter->req.ooblen = min_t(unsigned int,
				 iter->oobbytes_per_page - iter->req.ooboffs,
				 iter->oobleft);
}

/**
 * nand_io_iter_next_page - Move to the next page
 * @nand: NAND device
 * @iter: page iterator
 */
static inline void nanddev_io_iter_next_page(struct nand_device *nand,
					     struct nand_io_iter *iter)
{
	nanddev_pos_next_page(nand, &iter->req.pos);
	iter->dataleft -= iter->req.datalen;
	iter->req.databuf.in += iter->req.datalen;
	iter->oobleft -= iter->req.ooblen;
	iter->req.oobbuf.in += iter->req.ooblen;
	iter->req.dataoffs = 0;
	iter->req.ooboffs = 0;
	iter->req.datalen = min_t(unsigned int, nand->memorg.pagesize,
				  iter->dataleft);
	iter->req.ooblen = min_t(unsigned int, iter->oobbytes_per_page,
				 iter->oobleft);
}

/**
 * nand_io_iter_end - Should end iteration or not
 * @nand: NAND device
 * @iter: page iterator
 */
static inline bool nanddev_io_iter_end(struct nand_device *nand,
				       const struct nand_io_iter *iter)
{
	if (iter->dataleft || iter->oobleft)
		return false;

	return true;
}

/**
 * nand_io_for_each_page - Iterate over all NAND pages contained in an MTD I/O
 *			   request
 * @nand: NAND device
 * @start: start address to read/write
 * @req: MTD I/O request
 * @iter: page iterator
 */
#define nanddev_io_for_each_page(nand, start, req, iter)		\
	for (nanddev_io_iter_init(nand, start, req, iter);		\
	     !nanddev_io_iter_end(nand, iter);				\
	     nanddev_io_iter_next_page(nand, iter))

static inline unsigned int nanddev_bbt_pos_to_entry(struct nand_device *nand,
						    const struct nand_pos *pos)
{
	return pos->eraseblock;
}

static inline bool nanddev_bbt_is_initialized(struct nand_device *nand)
{
	return !!nand->bbt.cache;
}

int nanddev_init(struct nand_device *nand, const struct nand_ops *ops,
		 struct module *owner);
void nanddev_cleanup(struct nand_device *nand);
bool nanddev_isbad(struct nand_device *nand, const struct nand_pos *pos);
bool nanddev_isreserved(struct nand_device *nand, const struct nand_pos *pos);
int nanddev_erase(struct nand_device *nand, const struct nand_pos *pos);
int nanddev_markbad(struct nand_device *nand, const struct nand_pos *pos);

/* MTD -> NAND helper functions. */
int nanddev_mtd_erase(struct mtd_info *mtd, struct erase_info *einfo);


#endif /* __LINUX_MTD_SPINAND_H */
