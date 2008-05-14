/*
 * Inode based directory notification for Linux
 *
 * Copyright (C) 2005 John McCutchan
 */

#ifndef _LOCAL_INOTIFY_H
#define _LOCAL_INOTIFY_H

#include <linux/types.h>

/*
 * struct inotify_event - structure read from the inotify device for each event
 *
 * When you are watching a directory, you will receive the filename for events
 * such as IN_CREATE, IN_DELETE, IN_OPEN, IN_CLOSE, ..., relative to the wd.
 */
struct inotify_event {
	__s32		wd;		/* watch descriptor */
	__u32		mask;		/* watch mask */
	__u32		cookie;		/* cookie to synchronize two events */
	__u32		len;		/* length (including nulls) of name */
	char		name[0];	/* stub for possible name */
};

#include "local_inotify-masks.h"

#ifdef __KERNEL__

#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/config.h>

#ifdef CONFIG_INOTIFY

extern void inotify_inode_queue_event(struct inode *, __u32, __u32,
				      const char *);
extern void inotify_dentry_parent_queue_event(struct dentry *, __u32, __u32,
					      const char *);
extern void inotify_unmount_inodes(struct list_head *);
extern void inotify_inode_is_dead(struct inode *);
extern u32 inotify_get_cookie(void);

#else

static inline void inotify_inode_queue_event(struct inode *inode,
					     __u32 mask, __u32 cookie,
					     const char *filename)
{
}

static inline void inotify_dentry_parent_queue_event(struct dentry *dentry,
						     __u32 mask, __u32 cookie,
						     const char *filename)
{
}

static inline void inotify_unmount_inodes(struct list_head *list)
{
}

static inline void inotify_inode_is_dead(struct inode *inode)
{
}

static inline u32 inotify_get_cookie(void)
{
	return 0;
}

#endif	/* CONFIG_INOTIFY */

#endif	/* __KERNEL __ */

#endif	/* _LINUX_INOTIFY_H */
