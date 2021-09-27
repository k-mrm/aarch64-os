#ifndef AARCH64_OS_EXT2_H
#define AARCH64_OS_EXT2_H

#include "aarch64-os.h"

struct bootblock {
  ;
};

struct superblock {
  u32 s_inodes_count;
  u32 s_blocks_count;
  u32 s_r_blocks_count;
  u32 s_free_blocks_count;
  u32 s_free_inodes_count;
  u32 s_first_data_block;
  u32 s_log_block_size;
  u32 s_log_frag_size;
  u32 s_blocks_per_group;
  u32 s_frags_per_group;
  u32 s_inodes_per_group;
  u32 s_mtime;
  u32 s_wtime;
  u16 s_mnt_count;
  u16 s_max_mnt_count;
  u16 s_magic;
  u16 s_state;
  u16 s_errors;
  u16 s_minor_rev_level;
  u32 s_lastcheck;
  u32 s_checkinterval;
  u32 s_creator_os;
  u32 s_rev_level;
  u16 s_def_resuid;
  u16 s_def_resgid;

  u32 s_first_ino;
  u16 s_inode_size;
  u16 s_block_group_nr;
  u32 s_feature_compat;
  u32 s_feature_incompat;
  u32 s_feature_ro_compat;
  u64 s_uuid[2];
  char s_volume_name[16];
  char s_last_mounted[64];
  u32 s_algo_bitmap;

  u8 s_prealloc_blocks;
  u8 s_prealloc_dir_blocks;
  u16 _align1;

  u64 s_journal_uuid[2];
  u32 s_journal_inum;
  u32 s_journal_dev;
  u32 s_last_orphan;

  u32 s_hash_seed[4];
  u8 s_def_hash_version;
  u8 _padding[3];

  u32 s_default_mount_options;
  u32 s_first_meta_bg;
  char _unused[760];
};

enum filetype {
  F_FILE,
  F_DIR,
};

struct inode {
  enum filetype type;
  int refcnt;
  int nbyte;
  struct blk *blk;
};

#endif
