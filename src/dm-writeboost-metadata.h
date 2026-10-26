/*
 * This file is part of dm-writeboost
 * Copyright (C) 2012-2026 Akira Hayakawa <ruby.wktk@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef DM_WRITEBOOST_METADATA_H
#define DM_WRITEBOOST_METADATA_H

/*----------------------------------------------------------------------------*/

struct segment_header *
get_segment_header_by_id(struct wb_device *, u64 segment_id);
struct rambuffer *get_rambuffer_by_id(struct wb_device *wb, u64 id);
sector_t calc_mb_start_sector(struct wb_device *, struct segment_header *,
			      u32 mb_idx);
u8 mb_idx_inseg(struct wb_device *, u32 mb_idx);
struct segment_header *mb_to_seg(struct wb_device *, struct metablock *);
bool is_on_buffer(struct wb_device *, u32 mb_idx);

/*----------------------------------------------------------------------------*/

/*
 * The caller owns both arrays, their lifetime, and synchronization. No operation
 * allocates memory. Sectors must be aligned to eight sectors (one 4 KiB block),
 * and entry indices must be less than nr_entries.
 */
int wb_map_init(struct wb_map *map, struct wb_map_entry *entries,
		u32 nr_entries, u32 *heads, u32 nr_buckets);
u32 wb_map_lookup(const struct wb_map *map, u64 sector);

/* Removing an unlinked entry is harmless and preserves its sector. */
void wb_map_remove(struct wb_map *map, u32 idx);

/*
 * Any other entry for this sector must already have been removed. Re-registering
 * the same index is allowed, and moves it to the front of its bucket. An index
 * previously linked under a different sector is unlinked before its key changes.
 */
void wb_map_insert(struct wb_map *map, u32 idx, u64 sector);

struct lookup_key {
	sector_t sector;
};

struct ht_head;
struct ht_head *ht_get_head(struct wb_device *, struct lookup_key *);
struct metablock *ht_lookup(struct wb_device *,
			    struct ht_head *, struct lookup_key *);
void ht_register(struct wb_device *, struct ht_head *,
		 struct metablock *, struct lookup_key *);
void ht_del(struct wb_device *, struct metablock *);
void discard_caches_inseg(struct wb_device *, struct segment_header *);

/*----------------------------------------------------------------------------*/

void prepare_segment_header_device(void *rambuffer, struct wb_device *,
				   struct segment_header *src);
u32 calc_checksum(void *rambuffer, u8 length);

/*----------------------------------------------------------------------------*/

int try_alloc_writeback_ios(struct wb_device *, size_t nr_batch, gfp_t gfp);

/*----------------------------------------------------------------------------*/

int resume_cache(struct wb_device *);
void free_cache(struct wb_device *);

/*----------------------------------------------------------------------------*/

#endif
