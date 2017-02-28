/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2008  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "fuse_i.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#define FUSE_CTL_SUPER_MAGIC 0x65735543

#define MAX_SIZE 1536

/*
 * This is non-NULL when the single instance of the control filesystem
 * exists.  Protected by fuse_mutex
 */
static struct super_block *fuse_control_sb;

static struct fuse_conn *fuse_ctl_file_conn_get(struct file *file)
{
	struct fuse_conn *fc;
	mutex_lock(&fuse_mutex);
	fc = file_inode(file)->i_private;
	if (fc)
		fc = fuse_conn_get(fc);
	mutex_unlock(&fuse_mutex);
	return fc;
}

static ssize_t fuse_conn_abort_write(struct file *file, const char __user *buf,
				     size_t count, loff_t *ppos)
{
	struct fuse_conn *fc = fuse_ctl_file_conn_get(file);
	if (fc) {
		fuse_abort_conn(fc);
		fuse_conn_put(fc);
	}
	return count;
}

static ssize_t fuse_conn_waiting_read(struct file *file, char __user *buf,
				      size_t len, loff_t *ppos)
{
	char tmp[32];
	size_t size;

	if (!*ppos) {
		long value;
		struct fuse_conn *fc = fuse_ctl_file_conn_get(file);
		if (!fc)
			return 0;

		value = atomic_read(&fc->num_waiting);
		file->private_data = (void *)value;
		fuse_conn_put(fc);
	}
	size = sprintf(tmp, "%ld\n", (long)file->private_data);
	return simple_read_from_buffer(buf, len, ppos, tmp, size);
}

static ssize_t fuse_conn_limit_read(struct file *file, char __user *buf,
				    size_t len, loff_t *ppos, unsigned val)
{
	char tmp[32];
        size_t size = sprintf(tmp, "%u\n", val);

        return simple_read_from_buffer(buf, len, ppos, tmp, size);
}

static ssize_t fuse_conn_limit_reads(struct file *file, char __user *buf,
                                    size_t len, loff_t *ppos, unsigned long long int val1, 
					unsigned long long int val2, unsigned long long int val3, 
					unsigned long long int val4, unsigned long long int val5,
					unsigned long long int val6, unsigned long long int val7,
					unsigned long long int val8, unsigned long long int val9)
{
        char tmp[128];
        size_t size = sprintf(tmp, "%llu\n%llu\n%llu\n%llu\n%llu\n%llu\n%llu\n%llu\n%llu\n", val1, val2, val3, val4, val5, val6, val7, val8, val9);

        return simple_read_from_buffer(buf, len, ppos, tmp, size);
}

static ssize_t fuse_conn_limit_write(struct file *file, const char __user *buf,
				     size_t count, loff_t *ppos, unsigned *val,
				     unsigned global_limit)
{
	unsigned long t;
	unsigned limit = (1 << 16) - 1;
	int err;

	if (*ppos)
		return -EINVAL;

	err = kstrtoul_from_user(buf, count, 0, &t);
	if (err)
		return err;

	if (!capable(CAP_SYS_ADMIN))
		limit = min(limit, global_limit);

	if (t > limit)
		return -EINVAL;

	*val = t;

	return count;
}

static ssize_t fuse_conn_max_background_read(struct file *file,
					     char __user *buf, size_t len,
					     loff_t *ppos)
{
	struct fuse_conn *fc;
	unsigned val;

	fc = fuse_ctl_file_conn_get(file);
	if (!fc)
		return 0;

	val = fc->max_background;
	fuse_conn_put(fc);

	return fuse_conn_limit_read(file, buf, len, ppos, val);
}

static ssize_t fuse_conn_max_background_write(struct file *file,
					      const char __user *buf,
					      size_t count, loff_t *ppos)
{
	unsigned uninitialized_var(val);
	ssize_t ret;

	ret = fuse_conn_limit_write(file, buf, count, ppos, &val,
				    max_user_bgreq);
	if (ret > 0) {
		struct fuse_conn *fc = fuse_ctl_file_conn_get(file);
		if (fc) {
			fc->max_background = val;
			fuse_conn_put(fc);
		}
	}

	return ret;
}

static ssize_t fuse_conn_congestion_threshold_read(struct file *file,
						   char __user *buf, size_t len,
						   loff_t *ppos)
{
	struct fuse_conn *fc;
	unsigned val;

	fc = fuse_ctl_file_conn_get(file);
	if (!fc)
		return 0;

	val = fc->congestion_threshold;
	fuse_conn_put(fc);

	return fuse_conn_limit_read(file, buf, len, ppos, val);
}

static ssize_t fuse_conn_congestion_threshold_write(struct file *file,
						    const char __user *buf,
						    size_t count, loff_t *ppos)
{
	unsigned uninitialized_var(val);
	ssize_t ret;

	ret = fuse_conn_limit_write(file, buf, count, ppos, &val,
				    max_user_congthresh);
	if (ret > 0) {
		struct fuse_conn *fc = fuse_ctl_file_conn_get(file);
		if (fc) {
			fc->congestion_threshold = val;
			fuse_conn_put(fc);
		}
	}

	return ret;
}

static ssize_t fuse_conn_background_queue_request_timing_read(struct file *file,
                                                   char __user *buf, size_t len,
                                                   loff_t *ppos)
{
        struct fuse_conn *fc;
	char tmp[1536], line[256], number[64];
	int available = 1536, i, j, starting_index = -1, count = 0, ret;
	size_t size;

	fc = fuse_ctl_file_conn_get(file);
        if (!fc)
                return 0;

	spin_lock(&fc->lock);
	if (*ppos == 0) {
                starting_index = 1;
        } else {
                line[0] = '\0';
                for (i = 1; i < 46; i++) {
                        for (j = 0; j < 33; j++) {
                                size = sprintf(number, "%llu ", fc->req_type_bg[i][j]);
                                strcat(line, number);
                        }
                        size = strlen(line);
                        if (count < *ppos)
                                count = count + (size + 1);
                        else {
                                starting_index = i;
                                goto out1;
                        }
                        line[0] = '\0';
                }
        }
out1 :
        if (starting_index == -1) {
		spin_unlock(&fc->lock);
		fuse_conn_put(fc);
                return 0;
	}
        tmp[0] = '\0';
        line[0] = '\0';
        for (i = starting_index; i < 46; i++) {
                for (j = 0; j < 33; j++) {
                        sprintf(number, "%llu ", fc->req_type_bg[i][j]);
                        strcat(line, number);
                }
                size = strlen(line);
                if (size+1 <= available) {
                        strcat(tmp, line);
                        available = available - size;
                        strcat(tmp, "\n");
                        available = available - 1;
                } else {
                        goto out;
                }
                line[0] = '\0';
        }

out:
	spin_unlock(&fc->lock);
        fuse_conn_put(fc);
	count = strlen(tmp);
	ret = copy_to_user(buf, tmp, count);
	if (ret == count)
		return -EFAULT;
	count -= ret;
	*ppos = *ppos + count;
	return count;
}

static ssize_t fuse_conn_pending_queue_request_timing_read(struct file *file,
                                                   char __user *buf, size_t len,
                                                   loff_t *ppos)
{
        struct fuse_conn *fc;
        char tmp[1536], line[256], number[64];
        int available = 1536, i, j, starting_index = -1, count = 0, ret;
        size_t size;

        fc = fuse_ctl_file_conn_get(file);
        if (!fc)
                return 0;

	spin_lock(&fc->lock);
        if (*ppos == 0) {
                starting_index = 1;
        } else {
                line[0] = '\0';
                for (i = 1; i < 46; i++) {
                        for (j = 0; j < 33; j++) {
                                size = sprintf(number, "%llu ", fc->req_type_pending[i][j]);
                                strcat(line, number);
                        }
                        size = strlen(line);
                        if (count < *ppos)
                                count = count + (size + 1);
                        else {
                                starting_index = i;
                                goto out1;
                        }
                        line[0] = '\0';
                }
        }
out1 :
        if (starting_index == -1) {
		spin_unlock(&fc->lock);
                fuse_conn_put(fc);
                return 0;
        }
        tmp[0] = '\0';
        line[0] = '\0';
        for (i = starting_index; i < 46; i++) {
                for (j = 0; j < 33; j++) {
                        sprintf(number, "%llu ", fc->req_type_pending[i][j]);
                        strcat(line, number);
                }
                size = strlen(line);
                if (size+1 <= available) {
                        strcat(tmp, line);
                        available = available - size;
                        strcat(tmp, "\n");
                        available = available - 1;
                } else {
                        goto out;
                }
                line[0] = '\0';
        }

out:
	spin_unlock(&fc->lock);
        fuse_conn_put(fc);
        count = strlen(tmp);
        ret = copy_to_user(buf, tmp, count);
        if (ret == count)
                return -EFAULT;
        count -= ret;
        *ppos = *ppos + count;
        return count;
}

static ssize_t fuse_conn_processing_queue_request_timing_read(struct file *file,
                                                   char __user *buf, size_t len,
                                                   loff_t *ppos)
{
        struct fuse_conn *fc;
        char tmp[1536], line[256], number[64];
        int available = 1536, i, j, starting_index = -1, count = 0, ret;
        size_t size;

        fc = fuse_ctl_file_conn_get(file);
        if (!fc)
                return 0;

	spin_lock(&fc->lock);
        if (*ppos == 0) {
                starting_index = 1;
        } else {
                line[0] = '\0';
                for (i = 1; i < 46; i++) {
                        for (j = 0; j < 33; j++) {
                                size = sprintf(number, "%llu ", fc->req_type_processing[i][j]);
                                strcat(line, number);
                        }
                        size = strlen(line);
                        if (count < *ppos)
                                count = count + (size + 1);
                        else {
                                starting_index = i;
                                goto out1;
                        }
                        line[0] = '\0';
                }
        }
out1 :
        if (starting_index == -1) {
		spin_unlock(&fc->lock);
                fuse_conn_put(fc);
                return 0;
        }
        tmp[0] = '\0';
        line[0] = '\0';
        for (i = starting_index; i < 46; i++) {
                for (j = 0; j < 33; j++) {
                        sprintf(number, "%llu ", fc->req_type_processing[i][j]);
                        strcat(line, number);
                }
                size = strlen(line);
                if (size+1 <= available) {
                        strcat(tmp, line);
                        available = available - size;
                        strcat(tmp, "\n");
                        available = available - 1;
                } else {
                        goto out;
                }
                line[0] = '\0';
        }

out:
	spin_unlock(&fc->lock);
        fuse_conn_put(fc);
        count = strlen(tmp);
        ret = copy_to_user(buf, tmp, count);
        if (ret == count)
                return -EFAULT;
        count -= ret;
        *ppos = *ppos + count;
        return count;
}

/*No locks*/
static ssize_t fuse_conn_writeback_req_sizes_read(struct file *file,
						char __user *buf, size_t len,
						loff_t *ppos)
{
	struct fuse_conn *fc;
	char tmp[MAX_SIZE], number[10];
	int available = MAX_SIZE, length, i, count = 0, ret, starting_index = -1;
	size_t size;

//	printk("Inside fuse_conn_writeback_req_sizes_read\n");
	fc = fuse_ctl_file_conn_get(file);
	if (!fc)
                return 0;

	tmp[0] = '\0';
	length = fc->req_sizes_len;
	if (*ppos == 0) {
		for (i = 0; i < length; i++) {
			size = sprintf(number, "%u", fc->req_sizes[i]);
			if ((size+1) < available) {
				strcat(tmp, number);
				available = available-size;
				strcat(tmp, "\n");
				available = available - 1;
			} else {
//				printk("Index broken at : %d\n", i);
				goto out;
			}
		}
	} else {
		for (i = 0; i < length; i++) {
			size = sprintf(number, "%u", fc->req_sizes[i]);
			if (count < *ppos) {
				count = count + size + 1;
			} else {
				starting_index = i;
				break;
			}
		}
//		printk("starting index : %d\n", starting_index);
		if (starting_index != -1) {
			for (i = starting_index; i < length; i++) {
				size = sprintf(number, "%u", fc->req_sizes[i]);
				if ((size+1) < available) {
					strcat(tmp, number);
					available = available-size;
					strcat(tmp, "\n");
					available = available - 1;
				} else {
					goto out;
				}
			}
		} else {
			fuse_conn_put(fc);
			return 0;
		}
	}
out: 
	fuse_conn_put(fc);
	count = strlen(tmp);
	if (count == 0)
		return count;
//	printk("Total count of bytes copying to user space : %d\n", count);
	ret = copy_to_user(buf, tmp, count);
	if (ret == count)
                return -EFAULT;
	count -= ret;
        *ppos = *ppos + count;
        return count;
}

/*
static unsigned long long int list_count(struct list_head *head) {
	struct list_head *pln;
	unsigned long long int numItems = 0;
	
	list_for_each(pln, head) {
		numItems++;
	}
	return numItems;
}
*/

static ssize_t fuse_conn_queue_lengths_read(struct file *file,
					char __user *buf, size_t len,
					loff_t *ppos)
{
	struct fuse_conn *fc;
	unsigned long long int bg_entered, bg_removed, bg_max, pending_entered, pending_removed, pending_max, processing_entered, processing_removed, processing_max;
	
	fc = fuse_ctl_file_conn_get(file);
	spin_lock(&fc->lock);
	bg_entered = fc->bg_entered;
	bg_removed = fc->bg_removed;
	bg_max = fc->max_bg_count;
	fc->bg_entered = 0;
	fc->bg_removed = 0;
	fc->max_bg_count = 0;

	pending_entered = fc->pending_entered;
	pending_removed = fc->pending_removed;
	pending_max = fc->max_pending_count;
	fc->pending_entered = 0;
	fc->pending_removed = 0;
	fc->max_pending_count = 0;

	processing_entered = fc->processing_entered;
	processing_removed = fc->processing_removed;
	processing_max = fc->max_processing_count;
	fc->processing_entered = 0;
	fc->processing_removed = 0;
	fc->max_processing_count = 0;

	spin_unlock(&fc->lock);
	fuse_conn_put(fc);
	return fuse_conn_limit_reads(file, buf, len, ppos, bg_entered, bg_removed, bg_max, pending_entered, pending_removed, pending_max, processing_entered, processing_removed, processing_max);
}

static const struct file_operations fuse_ctl_abort_ops = {
	.open = nonseekable_open,
	.write = fuse_conn_abort_write,
	.llseek = no_llseek,
};

static const struct file_operations fuse_ctl_waiting_ops = {
	.open = nonseekable_open,
	.read = fuse_conn_waiting_read,
	.llseek = no_llseek,
};

static const struct file_operations fuse_conn_max_background_ops = {
	.open = nonseekable_open,
	.read = fuse_conn_max_background_read,
	.write = fuse_conn_max_background_write,
	.llseek = no_llseek,
};

static const struct file_operations fuse_conn_congestion_threshold_ops = {
	.open = nonseekable_open,
	.read = fuse_conn_congestion_threshold_read,
	.write = fuse_conn_congestion_threshold_write,
	.llseek = no_llseek,
};

static const struct file_operations fuse_conn_background_queue_requests_timings_ops = {
	.open = nonseekable_open,
	.read = fuse_conn_background_queue_request_timing_read,
	.write = NULL,
	.llseek = no_llseek,
};

static const struct file_operations fuse_conn_pending_queue_requests_timings_ops = {
        .open = nonseekable_open,
        .read = fuse_conn_pending_queue_request_timing_read,
        .write = NULL,
        .llseek = no_llseek,
};

static const struct file_operations fuse_conn_processing_queue_requests_timings_ops = {
        .open = nonseekable_open,
        .read = fuse_conn_processing_queue_request_timing_read,
        .write = NULL,
        .llseek = no_llseek,
};

static const struct file_operations fuse_conn_queue_lengths_ops = {
        .open = nonseekable_open,
        .read = fuse_conn_queue_lengths_read,
        .write = NULL,
        .llseek = no_llseek,
};

static const struct file_operations fuse_conn_writeback_req_sizes_ops = {
        .open = nonseekable_open,
        .read = fuse_conn_writeback_req_sizes_read,
        .write = NULL,
        .llseek = no_llseek,
};

static struct dentry *fuse_ctl_add_dentry(struct dentry *parent,
					  struct fuse_conn *fc,
					  const char *name,
					  int mode, int nlink,
					  const struct inode_operations *iop,
					  const struct file_operations *fop)
{
	struct dentry *dentry;
	struct inode *inode;
	BUG_ON(fc->ctl_ndents >= FUSE_CTL_NUM_DENTRIES);
	dentry = d_alloc_name(parent, name);
	if (!dentry)
		return NULL;

	fc->ctl_dentry[fc->ctl_ndents++] = dentry;
	inode = new_inode(fuse_control_sb);
	if (!inode)
		return NULL;

	inode->i_ino = get_next_ino();
	inode->i_mode = mode;
	inode->i_uid = fc->user_id;
	inode->i_gid = fc->group_id;
	inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
	/* setting ->i_op to NULL is not allowed */
	if (iop)
		inode->i_op = iop;
	inode->i_fop = fop;
	set_nlink(inode, nlink);
	inode->i_private = fc;
	d_add(dentry, inode);
	return dentry;
}

/*
 * Add a connection to the control filesystem (if it exists).  Caller
 * must hold fuse_mutex
 */
int fuse_ctl_add_conn(struct fuse_conn *fc)
{
	struct dentry *parent;
	char name[32];

	if (!fuse_control_sb)
		return 0;

	parent = fuse_control_sb->s_root;
	inc_nlink(d_inode(parent));
	sprintf(name, "%u", fc->dev);
	parent = fuse_ctl_add_dentry(parent, fc, name, S_IFDIR | 0500, 2,
				     &simple_dir_inode_operations,
				     &simple_dir_operations);
	if (!parent)
		goto err;

	if (!fuse_ctl_add_dentry(parent, fc, "waiting", S_IFREG | 0400, 1,
				 NULL, &fuse_ctl_waiting_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "abort", S_IFREG | 0200, 1,
				 NULL, &fuse_ctl_abort_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "max_background", S_IFREG | 0600,
				 1, NULL, &fuse_conn_max_background_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "congestion_threshold",
				 S_IFREG | 0600, 1, NULL,
				 &fuse_conn_congestion_threshold_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "background_queue_requests_timings",
                                 S_IFREG | 0600, 1, NULL,
                                 &fuse_conn_background_queue_requests_timings_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "pending_queue_requests_timings",
                                 S_IFREG | 0600, 1, NULL,
                                 &fuse_conn_pending_queue_requests_timings_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "processing_queue_requests_timings",
                                 S_IFREG | 0600, 1, NULL,
                                 &fuse_conn_processing_queue_requests_timings_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "queue_lengths",
                                 S_IFREG | 0600, 1, NULL,
                                 &fuse_conn_queue_lengths_ops) ||
	    !fuse_ctl_add_dentry(parent, fc, "writeback_req_sizes",
                                 S_IFREG | 0600, 1, NULL,
                                 &fuse_conn_writeback_req_sizes_ops))
		goto err;

	return 0;

 err:
	fuse_ctl_remove_conn(fc);
	return -ENOMEM;
}

/*
 * Remove a connection from the control filesystem (if it exists).
 * Caller must hold fuse_mutex
 */
void fuse_ctl_remove_conn(struct fuse_conn *fc)
{
	int i;

	if (!fuse_control_sb)
		return;

	for (i = fc->ctl_ndents - 1; i >= 0; i--) {
		struct dentry *dentry = fc->ctl_dentry[i];
		d_inode(dentry)->i_private = NULL;
		d_drop(dentry);
		dput(dentry);
	}
	drop_nlink(d_inode(fuse_control_sb->s_root));
}

static int fuse_ctl_fill_super(struct super_block *sb, void *data, int silent)
{
	struct tree_descr empty_descr = {""};
	struct fuse_conn *fc;
	int err;

	err = simple_fill_super(sb, FUSE_CTL_SUPER_MAGIC, &empty_descr);
	if (err)
		return err;

	mutex_lock(&fuse_mutex);
	BUG_ON(fuse_control_sb);
	fuse_control_sb = sb;
	list_for_each_entry(fc, &fuse_conn_list, entry) {
		err = fuse_ctl_add_conn(fc);
		if (err) {
			fuse_control_sb = NULL;
			mutex_unlock(&fuse_mutex);
			return err;
		}
	}
	mutex_unlock(&fuse_mutex);

	return 0;
}

static struct dentry *fuse_ctl_mount(struct file_system_type *fs_type,
			int flags, const char *dev_name, void *raw_data)
{
	return mount_single(fs_type, flags, raw_data, fuse_ctl_fill_super);
}

static void fuse_ctl_kill_sb(struct super_block *sb)
{
	struct fuse_conn *fc;

	mutex_lock(&fuse_mutex);
	fuse_control_sb = NULL;
	list_for_each_entry(fc, &fuse_conn_list, entry)
		fc->ctl_ndents = 0;
	mutex_unlock(&fuse_mutex);

	kill_litter_super(sb);
}

static struct file_system_type fuse_ctl_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "fusectl",
	.mount		= fuse_ctl_mount,
	.kill_sb	= fuse_ctl_kill_sb,
};
MODULE_ALIAS_FS("fusectl");

int __init fuse_ctl_init(void)
{
	return register_filesystem(&fuse_ctl_fs_type);
}

void __exit fuse_ctl_cleanup(void)
{
	unregister_filesystem(&fuse_ctl_fs_type);
}
