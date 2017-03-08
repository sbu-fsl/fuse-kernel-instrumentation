#undef TRACE_SYSTEM
#define TRACE_SYSTEM writeback

#if !defined(_TRACE_WRITEBACK_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_WRITEBACK_H

#include <linux/tracepoint.h>
#include <linux/backing-dev.h>
#include <linux/writeback.h>

#define show_inode_state(state)					\
	__print_flags(state, "|",				\
		{I_DIRTY_SYNC,		"I_DIRTY_SYNC"},	\
		{I_DIRTY_DATASYNC,	"I_DIRTY_DATASYNC"},	\
		{I_DIRTY_PAGES,		"I_DIRTY_PAGES"},	\
		{I_NEW,			"I_NEW"},		\
		{I_WILL_FREE,		"I_WILL_FREE"},		\
		{I_FREEING,		"I_FREEING"},		\
		{I_CLEAR,		"I_CLEAR"},		\
		{I_SYNC,		"I_SYNC"},		\
		{I_DIRTY_TIME,		"I_DIRTY_TIME"},	\
		{I_DIRTY_TIME_EXPIRED,	"I_DIRTY_TIME_EXPIRED"}, \
		{I_REFERENCED,		"I_REFERENCED"}		\
	)

/* enums need to be exported to user space */
#undef EM
#undef EMe
#define EM(a,b) 	TRACE_DEFINE_ENUM(a);
#define EMe(a,b)	TRACE_DEFINE_ENUM(a);

#define WB_WORK_REASON							\
	EM( WB_REASON_BACKGROUND,		"background")		\
	EM( WB_REASON_TRY_TO_FREE_PAGES,	"try_to_free_pages")	\
	EM( WB_REASON_SYNC,			"sync")			\
	EM( WB_REASON_PERIODIC,			"periodic")		\
	EM( WB_REASON_LAPTOP_TIMER,		"laptop_timer")		\
	EM( WB_REASON_FREE_MORE_MEM,		"free_more_memory")	\
	EM( WB_REASON_FS_FREE_SPACE,		"fs_free_space")	\
	EMe(WB_REASON_FORKER_THREAD,		"forker_thread")

WB_WORK_REASON

/*
 * Now redefine the EM() and EMe() macros to map the enums to the strings
 * that will be printed in the output.
 */
#undef EM
#undef EMe
#define EM(a,b)		{ a, b },
#define EMe(a,b)	{ a, b }

struct wb_writeback_work;

TRACE_EVENT(writeback_dirty_page,

	TP_PROTO(struct page *page, struct address_space *mapping),

	TP_ARGS(page, mapping),

	TP_STRUCT__entry (
		__array(char, name, 32)
		__field(unsigned long, ino)
		__field(pgoff_t, index)
	),

	TP_fast_assign(
		strncpy(__entry->name,
			mapping ? dev_name(inode_to_bdi(mapping->host)->dev) : "(unknown)", 32);
		__entry->ino = mapping ? mapping->host->i_ino : 0;
		__entry->index = page->index;
	),

	TP_printk("bdi %s: ino=%lu index=%lu",
		__entry->name,
		__entry->ino,
		__entry->index
	)
);

DECLARE_EVENT_CLASS(writeback_dirty_inode_template,

	TP_PROTO(struct inode *inode, int flags),

	TP_ARGS(inode, flags),

	TP_STRUCT__entry (
		__array(char, name, 32)
		__field(unsigned long, ino)
		__field(unsigned long, state)
		__field(unsigned long, flags)
	),

	TP_fast_assign(
		struct backing_dev_info *bdi = inode_to_bdi(inode);

		/* may be called for files on pseudo FSes w/ unregistered bdi */
		strncpy(__entry->name,
			bdi->dev ? dev_name(bdi->dev) : "(unknown)", 32);
		__entry->ino		= inode->i_ino;
		__entry->state		= inode->i_state;
		__entry->flags		= flags;
	),

	TP_printk("bdi %s: ino=%lu state=%s flags=%s",
		__entry->name,
		__entry->ino,
		show_inode_state(__entry->state),
		show_inode_state(__entry->flags)
	)
);

DEFINE_EVENT(writeback_dirty_inode_template, writeback_mark_inode_dirty,

	TP_PROTO(struct inode *inode, int flags),

	TP_ARGS(inode, flags)
);

DEFINE_EVENT(writeback_dirty_inode_template, writeback_dirty_inode_start,

	TP_PROTO(struct inode *inode, int flags),

	TP_ARGS(inode, flags)
);

DEFINE_EVENT(writeback_dirty_inode_template, writeback_dirty_inode,

	TP_PROTO(struct inode *inode, int flags),

	TP_ARGS(inode, flags)
);

DECLARE_EVENT_CLASS(writeback_write_inode_template,

	TP_PROTO(struct inode *inode, struct writeback_control *wbc),

	TP_ARGS(inode, wbc),

	TP_STRUCT__entry (
		__array(char, name, 32)
		__field(unsigned long, ino)
		__field(int, sync_mode)
	),

	TP_fast_assign(
		strncpy(__entry->name,
			dev_name(inode_to_bdi(inode)->dev), 32);
		__entry->ino		= inode->i_ino;
		__entry->sync_mode	= wbc->sync_mode;
	),

	TP_printk("bdi %s: ino=%lu sync_mode=%d",
		__entry->name,
		__entry->ino,
		__entry->sync_mode
	)
);

DEFINE_EVENT(writeback_write_inode_template, writeback_write_inode_start,

	TP_PROTO(struct inode *inode, struct writeback_control *wbc),

	TP_ARGS(inode, wbc)
);

DEFINE_EVENT(writeback_write_inode_template, writeback_write_inode,

	TP_PROTO(struct inode *inode, struct writeback_control *wbc),

	TP_ARGS(inode, wbc)
);

DECLARE_EVENT_CLASS(writeback_work_class,
	TP_PROTO(struct backing_dev_info *bdi, struct wb_writeback_work *work),
	TP_ARGS(bdi, work),
	TP_STRUCT__entry(
		__array(char, name, 32)
		__field(long, nr_pages)
		__field(dev_t, sb_dev)
		__field(int, sync_mode)
		__field(int, for_kupdate)
		__field(int, range_cyclic)
		__field(int, for_background)
		__field(int, reason)
	),
	TP_fast_assign(
		strncpy(__entry->name,
			bdi->dev ? dev_name(bdi->dev) : "(unknown)", 32);
		__entry->nr_pages = work->nr_pages;
		__entry->sb_dev = work->sb ? work->sb->s_dev : 0;
		__entry->sync_mode = work->sync_mode;
		__entry->for_kupdate = work->for_kupdate;
		__entry->range_cyclic = work->range_cyclic;
		__entry->for_background	= work->for_background;
		__entry->reason = work->reason;
	),
	TP_printk("bdi %s: sb_dev %d:%d nr_pages=%ld sync_mode=%d "
		  "kupdate=%d range_cyclic=%d background=%d reason=%s",
		  __entry->name,
		  MAJOR(__entry->sb_dev), MINOR(__entry->sb_dev),
		  __entry->nr_pages,
		  __entry->sync_mode,
		  __entry->for_kupdate,
		  __entry->range_cyclic,
		  __entry->for_background,
		  __print_symbolic(__entry->reason, WB_WORK_REASON)
	)
);
#define DEFINE_WRITEBACK_WORK_EVENT(name) \
DEFINE_EVENT(writeback_work_class, name, \
	TP_PROTO(struct backing_dev_info *bdi, struct wb_writeback_work *work), \
	TP_ARGS(bdi, work))
DEFINE_WRITEBACK_WORK_EVENT(writeback_queue);
DEFINE_WRITEBACK_WORK_EVENT(writeback_exec);
DEFINE_WRITEBACK_WORK_EVENT(writeback_start);
DEFINE_WRITEBACK_WORK_EVENT(writeback_written);
DEFINE_WRITEBACK_WORK_EVENT(writeback_wait);

TRACE_EVENT(writeback_bdi_over_bground_thresh_end,
        TP_PROTO(int option, bool value, 
		 long nr_dirty, long global_bg_thres,
		 long bdi_dirty, long bdi_bg_thresh),
        TP_ARGS(option, value, nr_dirty, global_bg_thres, bdi_dirty, bdi_bg_thresh),
        TP_STRUCT__entry(
                __field(int,           	option)
		__field(bool,	       	value)
		__field(long, 		nr_dirty)
		__field(long,		global_bg_thres)
		__field(long, 		bdi_dirty)
		__field(long, 		bdi_bg_thresh) 
        ),
        TP_fast_assign(
                __entry->option         = option;
		__entry->value		= value;
		__entry->nr_dirty	= nr_dirty;
		__entry->global_bg_thres= global_bg_thres;
		__entry->bdi_dirty	= bdi_dirty;
		__entry->bdi_bg_thresh	= bdi_bg_thresh;
		
        ),
        TP_printk("option : %d value : %d nr_dirty : %lu G_bg_thresh : %lu BDI_REC : %lu BDI_bg_thresh : %lu", 
		__entry->option, __entry->value, __entry->nr_dirty, __entry->global_bg_thres, __entry->bdi_dirty, __entry->bdi_bg_thresh)
);

TRACE_EVENT(writeback_bdi_over_bground_thresh_start,
        TP_PROTO(long pages_written),
        TP_ARGS(pages_written),
        TP_STRUCT__entry(
                __field(long,           pages)
        ),
        TP_fast_assign(
                __entry->pages          = pages_written;
        ),
        TP_printk("%ld", __entry->pages)
);

TRACE_EVENT(wb_check_background_flush_end,
        TP_PROTO(long pages_written),
        TP_ARGS(pages_written),
        TP_STRUCT__entry(
                __field(long,           pages)
        ),
        TP_fast_assign(
                __entry->pages          = pages_written;
        ),
        TP_printk("%ld", __entry->pages)
);


TRACE_EVENT(wb_check_background_flush_start,
        TP_PROTO(long pages_written),
        TP_ARGS(pages_written),
        TP_STRUCT__entry(
                __field(long,           pages)
        ),
        TP_fast_assign(
                __entry->pages          = pages_written;
        ),
        TP_printk("%ld", __entry->pages)
);

TRACE_EVENT(writeback_pages_before_written,
        TP_PROTO(long pages_written),
        TP_ARGS(pages_written),
        TP_STRUCT__entry(
                __field(long,           pages)
        ),
        TP_fast_assign(
                __entry->pages          = pages_written;
        ),
        TP_printk("%ld", __entry->pages)
);

TRACE_EVENT(writeback_pages_written,
	TP_PROTO(long pages_written),
	TP_ARGS(pages_written),
	TP_STRUCT__entry(
		__field(long,		pages)
	),
	TP_fast_assign(
		__entry->pages		= pages_written;
	),
	TP_printk("%ld", __entry->pages)
);

DECLARE_EVENT_CLASS(writeback_class,
	TP_PROTO(struct backing_dev_info *bdi),
	TP_ARGS(bdi),
	TP_STRUCT__entry(
		__array(char, name, 32)
	),
	TP_fast_assign(
		strncpy(__entry->name, dev_name(bdi->dev), 32);
	),
	TP_printk("bdi %s",
		  __entry->name
	)
);
#define DEFINE_WRITEBACK_EVENT(name) \
DEFINE_EVENT(writeback_class, name, \
	TP_PROTO(struct backing_dev_info *bdi), \
	TP_ARGS(bdi))

DEFINE_WRITEBACK_EVENT(writeback_nowork);
DEFINE_WRITEBACK_EVENT(writeback_wake_background);
DEFINE_WRITEBACK_EVENT(writeback_bdi_register);

DECLARE_EVENT_CLASS(wbc_class,
	TP_PROTO(struct writeback_control *wbc, struct backing_dev_info *bdi),
	TP_ARGS(wbc, bdi),
	TP_STRUCT__entry(
		__array(char, name, 32)
		__field(long, nr_to_write)
		__field(long, pages_skipped)
		__field(int, sync_mode)
		__field(int, for_kupdate)
		__field(int, for_background)
		__field(int, for_reclaim)
		__field(int, range_cyclic)
		__field(long, range_start)
		__field(long, range_end)
	),

	TP_fast_assign(
		strncpy(__entry->name, dev_name(bdi->dev), 32);
		__entry->nr_to_write	= wbc->nr_to_write;
		__entry->pages_skipped	= wbc->pages_skipped;
		__entry->sync_mode	= wbc->sync_mode;
		__entry->for_kupdate	= wbc->for_kupdate;
		__entry->for_background	= wbc->for_background;
		__entry->for_reclaim	= wbc->for_reclaim;
		__entry->range_cyclic	= wbc->range_cyclic;
		__entry->range_start	= (long)wbc->range_start;
		__entry->range_end	= (long)wbc->range_end;
	),

	TP_printk("bdi %s: towrt=%ld skip=%ld mode=%d kupd=%d "
		"bgrd=%d reclm=%d cyclic=%d "
		"start=0x%lx end=0x%lx",
		__entry->name,
		__entry->nr_to_write,
		__entry->pages_skipped,
		__entry->sync_mode,
		__entry->for_kupdate,
		__entry->for_background,
		__entry->for_reclaim,
		__entry->range_cyclic,
		__entry->range_start,
		__entry->range_end)
)

#define DEFINE_WBC_EVENT(name) \
DEFINE_EVENT(wbc_class, name, \
	TP_PROTO(struct writeback_control *wbc, struct backing_dev_info *bdi), \
	TP_ARGS(wbc, bdi))
DEFINE_WBC_EVENT(wbc_writepage);

TRACE_EVENT(writeback_queue_io,
	TP_PROTO(struct bdi_writeback *wb,
		 struct wb_writeback_work *work,
		 int moved),
	TP_ARGS(wb, work, moved),
	TP_STRUCT__entry(
		__array(char,		name, 32)
		__field(unsigned long,	older)
		__field(long,		age)
		__field(int,		moved)
		__field(int,		reason)
	),
	TP_fast_assign(
		unsigned long *older_than_this = work->older_than_this;
		strncpy(__entry->name, dev_name(wb->bdi->dev), 32);
		__entry->older	= older_than_this ?  *older_than_this : 0;
		__entry->age	= older_than_this ?
				  (jiffies - *older_than_this) * 1000 / HZ : -1;
		__entry->moved	= moved;
		__entry->reason	= work->reason;
	),
	TP_printk("bdi %s: older=%lu age=%ld enqueue=%d reason=%s",
		__entry->name,
		__entry->older,	/* older_than_this in jiffies */
		__entry->age,	/* older_than_this in relative milliseconds */
		__entry->moved,
		__print_symbolic(__entry->reason, WB_WORK_REASON)
	)
);

TRACE_EVENT(global_dirty_state,

	TP_PROTO(unsigned long background_thresh,
		 unsigned long dirty_thresh,
		 unsigned long avail_memory
	),

	TP_ARGS(background_thresh,
		dirty_thresh,
		avail_memory
	),

	TP_STRUCT__entry(
		__field(unsigned long,	nr_dirty)
		__field(unsigned long,	nr_writeback)
		__field(unsigned long,	nr_unstable)
		__field(unsigned long,	background_thresh)
		__field(unsigned long,	dirty_thresh)
		__field(unsigned long, 	avail_memory)
		__field(unsigned long,	dirty_limit)
		__field(unsigned long,	nr_dirtied)
		__field(unsigned long,	nr_written)
	),

	TP_fast_assign(
		__entry->nr_dirty	= global_page_state(NR_FILE_DIRTY);
		__entry->nr_writeback	= global_page_state(NR_WRITEBACK);
		__entry->nr_unstable	= global_page_state(NR_UNSTABLE_NFS);
		__entry->nr_dirtied	= global_page_state(NR_DIRTIED);
		__entry->nr_written	= global_page_state(NR_WRITTEN);
		__entry->background_thresh = background_thresh;
		__entry->dirty_thresh	= dirty_thresh;
		__entry->avail_memory	= avail_memory;
		__entry->dirty_limit = global_dirty_limit;
	),

	TP_printk("dirty=%lu writeback=%lu unstable=%lu "
		  "bg_thresh=%lu thresh=%lu avail_mem=%lu "
		  "limit=%lu " "dirtied=%lu written=%lu",
		  __entry->nr_dirty,
		  __entry->nr_writeback,
		  __entry->nr_unstable,
		  __entry->background_thresh,
		  __entry->dirty_thresh,
		  __entry->avail_memory,
		  __entry->dirty_limit,
		  __entry->nr_dirtied,
		  __entry->nr_written
	)
);

#define KBps(x)			((x) << (PAGE_SHIFT - 10))

TRACE_EVENT(bdi_dirty_ratelimit,

	TP_PROTO(struct backing_dev_info *bdi,
		 unsigned long dirty_rate,
		 unsigned long task_ratelimit),

	TP_ARGS(bdi, dirty_rate, task_ratelimit),

	TP_STRUCT__entry(
		__array(char,		bdi, 32)
		__field(unsigned long,	write_bw)
		__field(unsigned long,	avg_write_bw)
		__field(unsigned long,	dirty_rate)
		__field(unsigned long,	dirty_ratelimit)
		__field(unsigned long,	task_ratelimit)
		__field(unsigned long,	balanced_dirty_ratelimit)
	),

	TP_fast_assign(
		strlcpy(__entry->bdi, dev_name(bdi->dev), 32);
		__entry->write_bw	= KBps(bdi->write_bandwidth);
		__entry->avg_write_bw	= KBps(bdi->avg_write_bandwidth);
		__entry->dirty_rate	= KBps(dirty_rate);
		__entry->dirty_ratelimit = KBps(bdi->dirty_ratelimit);
		__entry->task_ratelimit	= KBps(task_ratelimit);
		__entry->balanced_dirty_ratelimit =
					  KBps(bdi->balanced_dirty_ratelimit);
	),

	TP_printk("bdi %s: "
		  "write_bw=%lu awrite_bw=%lu dirty_rate=%lu "
		  "dirty_ratelimit=%lu task_ratelimit=%lu "
		  "balanced_dirty_ratelimit=%lu",
		  __entry->bdi,
		  __entry->write_bw,		/* write bandwidth */
		  __entry->avg_write_bw,	/* avg write bandwidth */
		  __entry->dirty_rate,		/* bdi dirty rate */
		  __entry->dirty_ratelimit,	/* base ratelimit */
		  __entry->task_ratelimit, /* ratelimit with position control */
		  __entry->balanced_dirty_ratelimit /* the balanced ratelimit */
	)
);

TRACE_EVENT(bdi_dirty_limits,

	TP_PROTO(int option,
		 unsigned long dirty_thresh,
		 unsigned long background_thresh,
		 unsigned long bdi_dirty,
		 unsigned long bdi_reclaimable,
		 unsigned long bdi_writeback,
		 unsigned long bdi_thresh,
		 unsigned long bdi_bg_thresh),

	TP_ARGS(option, dirty_thresh, background_thresh, bdi_dirty, bdi_reclaimable, bdi_writeback, bdi_thresh, bdi_bg_thresh),

	TP_STRUCT__entry(
		__field(int, option)
		__field(unsigned long, dirty_thresh)
		__field(unsigned long, background_thresh)
		__field(unsigned long, bdi_dirty)
		__field(unsigned long, bdi_reclaimable)
		__field(unsigned long, bdi_writeback)
		__field(unsigned long, bdi_thresh)
		__field(unsigned long, bdi_bg_thresh)	
	),

	TP_fast_assign(
		__entry->option			= option;
		__entry->dirty_thresh		= dirty_thresh;
		__entry->background_thresh	= background_thresh;
		__entry->bdi_dirty		= bdi_dirty;
		__entry->bdi_reclaimable	= bdi_reclaimable;
		__entry->bdi_writeback		= bdi_writeback;
		__entry->bdi_thresh		= bdi_thresh;
		__entry->bdi_bg_thresh		= bdi_bg_thresh;
	),

	TP_printk("option=%d dirty_thresh=%lu bg_thresh=%lu "
		  "bdi_dirty=%lu bdi_reclaimable=%lu bdi_writeback=%lu bdi_thresh=%lu bdi_bg_thresh=%lu",
		  __entry->option,
		  __entry->dirty_thresh,	/* Global Dirty threshold (20% of available memory) */
		  __entry->background_thresh,	/* Global background threshold (10% of available memory) */	
		  __entry->bdi_dirty,		/* BDI Dirty pages = BDI_RECLAIMABLE */
		  __entry->bdi_reclaimable,
		  __entry->bdi_writeback,
		  __entry->bdi_thresh,		/* BDI Threshold (similar to Global Dirty Threshold) calculated using min and max ratio's of bdi*/
		  __entry->bdi_bg_thresh	/* BDI Background Threshold (similar to Global Background Threshold) calculated : bdi_thresh*(dirty_thresh/background_thresh) */
	)
);

TRACE_EVENT(bdi_dirty_fuselimts,

	TP_PROTO(unsigned long dirty_passed,
		 unsigned long long kernel_gave,
		 long numerator,
		 long denominator,
		 unsigned long min_dirty,
		 unsigned long max_dirty,
		 unsigned long long final_dirty),

	TP_ARGS(dirty_passed, kernel_gave, numerator, denominator, min_dirty, max_dirty, final_dirty),

	TP_STRUCT__entry(
		__field(unsigned long, dirty_passed)
		__field(unsigned long long, kernel_gave)
		__field(long, numerator)
		__field(long, denominator)
		__field(unsigned long, min_dirty)
		__field(unsigned long, max_dirty)
		__field(unsigned long long, final_dirty)
	),

	TP_fast_assign(
		__entry->dirty_passed	= dirty_passed;
		__entry->kernel_gave	= kernel_gave;
		__entry->numerator	= numerator;
		__entry->denominator	= denominator;
		__entry->min_dirty	= min_dirty;
		__entry->max_dirty	= max_dirty;
		__entry->final_dirty	= final_dirty;
	),

	TP_printk("dirty passed : %lu "
		  "kernel value : %llu numerator : %ld denominator : %ld min_dirty : %lu "
		  "max_dirty : %lu final_dirty : %llu",
		  __entry->dirty_passed,	/* Dirty threshold passed */
		  __entry->kernel_gave,		/* Kernel calculated value */
		  __entry->numerator,
		  __entry->denominator,
		  __entry->min_dirty,		/* Min dirty threshold */
		  __entry->max_dirty,		/* Max dirty threshold */
		  __entry->final_dirty		/* FInal dirty threshold value */
	)
);

TRACE_EVENT(writeback_chunk_size,

	TP_PROTO(long pages),

	TP_ARGS(pages),

	TP_STRUCT__entry(
		__field(long, pages)
	),

	TP_fast_assign(
		__entry->pages	= pages;
	),

	TP_printk("chunk size pages : %ld",
		 __entry->pages
	)
);

TRACE_EVENT(writeback_chunk_size_compare,

	TP_PROTO(long bandwidth, 
		long global_dirty_scope, 
		long pages1,
		long work_nr_pages,
		long pages2,
		long pages2_min_wb_pages,
		long min_wb_pages,
		long pages3),

	TP_ARGS(bandwidth, global_dirty_scope, pages1, 
		work_nr_pages, pages2, pages2_min_wb_pages, 
		min_wb_pages, pages3),

	TP_STRUCT__entry(
                __field(long, bandwidth)
		__field(long, global_dirty_scope)
		__field(long, pages1)
		__field(long, work_nr_pages)
		__field(long, pages2)
		__field(long, pages2_min_wb_pages)
		__field(long, min_wb_pages)
		__field(long, pages3)
        ),

	TP_fast_assign(
                __entry->bandwidth		= bandwidth;
		__entry->global_dirty_scope	= global_dirty_scope;
		__entry->pages1			= pages1;
		__entry->work_nr_pages		= work_nr_pages;
		__entry->pages2			= pages2;
		__entry->pages2_min_wb_pages	= pages2_min_wb_pages;
		__entry->min_wb_pages		= min_wb_pages;
		__entry->pages3			= pages3;
        ),

	TP_printk("Min(bdi_bandwidth/2 : %ld, global_dirty_lim/dirty_scope : %ld) : %ld "
		  "Min(pages1 : %ld, work_nr_pages : %ld) : %ld "
		  "Round_down(pages2+min_wb_pages : %ld, min_wb_pages : %ld) : %ld ",
		  __entry->bandwidth,
		  __entry->global_dirty_scope,
		  __entry->pages1, __entry->pages1,
		  __entry->work_nr_pages,
		  __entry->pages2,
		  __entry->pages2_min_wb_pages,
		  __entry->min_wb_pages,
		  __entry->pages3
        )
);

TRACE_EVENT(balance_dirty_pages_details,

	TP_PROTO(unsigned long nr_dirty, unsigned long nr_reclaimed),

	TP_ARGS(nr_dirty, nr_reclaimed),

	TP_STRUCT__entry(
		__field(unsigned long, nr_dirty)
		__field(unsigned long, nr_reclaimed)
	),

	TP_fast_assign(
		__entry->nr_dirty	= nr_dirty;
		__entry->nr_reclaimed	= nr_reclaimed;
	),

	TP_printk("nr_dirty : %lu nr_reclaimed : %lu ",
		  __entry->nr_dirty, __entry->nr_reclaimed
	)
);

TRACE_EVENT(write_cache_pages_return,

	TP_PROTO(int ret_val),

	TP_ARGS(ret_val),

	TP_STRUCT__entry(
		__field(int, ret_val)
	),

	TP_fast_assign(
		__entry->ret_val = ret_val;
	),

	TP_printk("Ret val : %d ", __entry->ret_val)
);

TRACE_EVENT(balance_dirty_pages_ratelimited,

        TP_PROTO(int curr_dirtied,
                 int curr_dirtied_pause,
                 int ratelimit),

        TP_ARGS(curr_dirtied, curr_dirtied_pause, ratelimit),

        TP_STRUCT__entry(
                __field(int, curr_dirtied)
                __field(int, curr_dirtied_pause)
                __field(int, ratelimit)
        ),

        TP_fast_assign(
                __entry->curr_dirtied   	= curr_dirtied;
                __entry->curr_dirtied_pause     = curr_dirtied_pause;
                __entry->ratelimit    		= ratelimit;
        ),

        TP_printk("current dirtied : %d "
                  "current dirtied pause : %d rate limit : %d",
                  __entry->curr_dirtied,        	
                  __entry->curr_dirtied_pause,         
                  __entry->ratelimit
        )
);

TRACE_EVENT(wb_do_writeback,

        TP_PROTO(long wrote,
                 long wrote_old_data,
                 long wrote_background_data),

        TP_ARGS(wrote, wrote_old_data, wrote_background_data),

        TP_STRUCT__entry(
                __field(long, wrote)
                __field(long, wrote_old_data)
                __field(long, wrote_background_data)
        ),

        TP_fast_assign(
                __entry->wrote			= wrote;
                __entry->wrote_old_data		= wrote_old_data;
                __entry->wrote_background_data	= wrote_background_data;
        ),

        TP_printk("wrote : %ld "
                  "wrote_old_data : %ld wrote_background_data : %ld",
                  __entry->wrote,
                  __entry->wrote_old_data,
                  __entry->wrote_background_data
        )
);

TRACE_EVENT(balance_dirty_pages_pause_start,

	TP_PROTO(long pause),

	TP_ARGS(pause),

	TP_STRUCT__entry(	
		__field(long, pause)
	),

	TP_fast_assign(
		__entry->pause = pause;
	),

	TP_printk("pause : %ld", __entry->pause)
);

TRACE_EVENT(balance_dirty_pages_pause_end,

        TP_PROTO(long pause),

        TP_ARGS(pause),

        TP_STRUCT__entry(       
                __field(long, pause)
        ),

        TP_fast_assign(
		__entry->pause = pause;
        ),

        TP_printk("pause : %ld", __entry->pause)
);

TRACE_EVENT(balance_dirty_pages_debug,

        TP_PROTO(int option,
		unsigned long nr_reclaimable,
		unsigned long nr_dirty,
		unsigned long dirty_thresh,
                unsigned long bg_thresh,
		unsigned long bdi_dirty,
                unsigned long bdi_bg_thresh,
		unsigned long bdi_thresh,
                unsigned long bdi_dirty_ratelimit,
		unsigned long pos_ratio,
                unsigned long task_ratelimit,
		long min_pause,
                long max_pause,
                long pause,
                int nr_dirtied_pause),

        TP_ARGS(option, nr_reclaimable, nr_dirty, dirty_thresh, bg_thresh, bdi_dirty, bdi_bg_thresh, bdi_thresh, bdi_dirty_ratelimit, pos_ratio, task_ratelimit, min_pause, max_pause, pause, nr_dirtied_pause),

        TP_STRUCT__entry(
                __field(int, option)
		__field(unsigned long, nr_reclaimable)
		__field(unsigned long, nr_dirty)
		__field(unsigned long, dirty_thresh)
		__field(unsigned long, bg_thresh)
		__field(unsigned long, bdi_dirty)
		__field(unsigned long, bdi_bg_thresh)
		__field(unsigned long, bdi_thresh)
		__field(unsigned long, bdi_dirty_ratelimit)
		__field(unsigned long, pos_ratio)
		__field(unsigned long, task_ratelimit)
		__field(long, min_pause)
		__field(long, max_pause)
		__field(long, pause)
		__field(int, nr_dirtied_pause)
        ),

        TP_fast_assign(
                __entry->option 		= option;
		__entry->nr_reclaimable 	= nr_reclaimable;
		__entry->nr_dirty		= nr_dirty;
		__entry->dirty_thresh		= dirty_thresh;
		__entry->bg_thresh		= bg_thresh;
		__entry->bdi_dirty		= bdi_dirty;
		__entry->bdi_bg_thresh		= bdi_bg_thresh;
		__entry->bdi_thresh		= bdi_thresh;
		__entry->bdi_dirty_ratelimit	= bdi_dirty_ratelimit;
		__entry->pos_ratio		= pos_ratio;
		__entry->task_ratelimit		= task_ratelimit;
		__entry->min_pause		= min_pause;
		__entry->max_pause		= max_pause;
		__entry->pause			= pause;
		__entry->nr_dirtied_pause	= nr_dirtied_pause;
        ),

        TP_printk("option : %d nr_reclaimable : %lu nr_dirty : %lu "
		  "dirty_thresh : %lu bg_thresh : %lu bdi_dirty : %lu "
		  "bdi_bg_thresh : %lu bdi_thresh : %lu bdi_dirty_ratelimit : %lu "
		  "pos_ratio : %lu task_ratelimit : %lu min_pause : %ld "
		  "max_pause : %ld pause : %ld nr_dirtied_pause : %d ", 
		  __entry->option, 
		  __entry->nr_reclaimable, 
		  __entry->nr_dirty, 
		  __entry->dirty_thresh,
		  __entry->bg_thresh,
		  __entry->bdi_dirty,
		  __entry->bdi_bg_thresh,
       		  __entry->bdi_thresh,
		  __entry->bdi_dirty_ratelimit,
		  __entry->pos_ratio,
		  __entry->task_ratelimit,
		  __entry->min_pause,
		  __entry->max_pause,
		  __entry->pause,
		  __entry->nr_dirtied_pause
		)
);

TRACE_EVENT(balance_dirty_pages,

	TP_PROTO(struct backing_dev_info *bdi,
		 unsigned long thresh,
		 unsigned long bg_thresh,
		 unsigned long dirty,
		 unsigned long bdi_thresh,
		 unsigned long bdi_dirty,
		 unsigned long dirty_ratelimit,
		 unsigned long task_ratelimit,
		 unsigned long dirtied,
		 unsigned long period,
		 long pause,
		 unsigned long start_time),

	TP_ARGS(bdi, thresh, bg_thresh, dirty, bdi_thresh, bdi_dirty,
		dirty_ratelimit, task_ratelimit,
		dirtied, period, pause, start_time),

	TP_STRUCT__entry(
		__array(	 char,	bdi, 32)
		__field(unsigned long,	limit)
		__field(unsigned long,	setpoint)
		__field(unsigned long,	dirty)
		__field(unsigned long,	bdi_setpoint)
		__field(unsigned long,	bdi_dirty)
		__field(unsigned long,	dirty_ratelimit)
		__field(unsigned long,	task_ratelimit)
		__field(unsigned int,	dirtied)
		__field(unsigned int,	dirtied_pause)
		__field(unsigned long,	paused)
		__field(	 long,	pause)
		__field(unsigned long,	period)
		__field(	 long,	think)
	),

	TP_fast_assign(
		unsigned long freerun = (thresh + bg_thresh) / 2;
		strlcpy(__entry->bdi, dev_name(bdi->dev), 32);

		__entry->limit		= global_dirty_limit;
		__entry->setpoint	= (global_dirty_limit + freerun) / 2;
		__entry->dirty		= dirty;
		__entry->bdi_setpoint	= __entry->setpoint *
						bdi_thresh / (thresh + 1);
		__entry->bdi_dirty	= bdi_dirty;
		__entry->dirty_ratelimit = KBps(dirty_ratelimit);
		__entry->task_ratelimit	= KBps(task_ratelimit);
		__entry->dirtied	= dirtied;
		__entry->dirtied_pause	= current->nr_dirtied_pause;
		__entry->think		= current->dirty_paused_when == 0 ? 0 :
			 (long)(jiffies - current->dirty_paused_when) * 1000/HZ;
		__entry->period		= period * 1000 / HZ;
		__entry->pause		= pause * 1000 / HZ;
		__entry->paused		= (jiffies - start_time) * 1000 / HZ;
	),


	TP_printk("bdi %s: "
		  "limit=%lu setpoint=%lu dirty=%lu "
		  "bdi_setpoint=%lu bdi_dirty=%lu "
		  "dirty_ratelimit=%lu task_ratelimit=%lu "
		  "dirtied=%u dirtied_pause=%u "
		  "paused=%lu pause=%ld period=%lu think=%ld",
		  __entry->bdi,
		  __entry->limit,
		  __entry->setpoint,
		  __entry->dirty,
		  __entry->bdi_setpoint,
		  __entry->bdi_dirty,
		  __entry->dirty_ratelimit,
		  __entry->task_ratelimit,
		  __entry->dirtied,
		  __entry->dirtied_pause,
		  __entry->paused,	/* ms */
		  __entry->pause,	/* ms */
		  __entry->period,	/* ms */
		  __entry->think	/* ms */
	  )
);

TRACE_EVENT(writeback_sb_inodes_requeue,

	TP_PROTO(struct inode *inode),
	TP_ARGS(inode),

	TP_STRUCT__entry(
		__array(char, name, 32)
		__field(unsigned long, ino)
		__field(unsigned long, state)
		__field(unsigned long, dirtied_when)
	),

	TP_fast_assign(
		strncpy(__entry->name,
		        dev_name(inode_to_bdi(inode)->dev), 32);
		__entry->ino		= inode->i_ino;
		__entry->state		= inode->i_state;
		__entry->dirtied_when	= inode->dirtied_when;
	),

	TP_printk("bdi %s: ino=%lu state=%s dirtied_when=%lu age=%lu",
		  __entry->name,
		  __entry->ino,
		  show_inode_state(__entry->state),
		  __entry->dirtied_when,
		  (jiffies - __entry->dirtied_when) / HZ
	)
);

DECLARE_EVENT_CLASS(writeback_congest_waited_template,

	TP_PROTO(unsigned int usec_timeout, unsigned int usec_delayed),

	TP_ARGS(usec_timeout, usec_delayed),

	TP_STRUCT__entry(
		__field(	unsigned int,	usec_timeout	)
		__field(	unsigned int,	usec_delayed	)
	),

	TP_fast_assign(
		__entry->usec_timeout	= usec_timeout;
		__entry->usec_delayed	= usec_delayed;
	),

	TP_printk("usec_timeout=%u usec_delayed=%u",
			__entry->usec_timeout,
			__entry->usec_delayed)
);

DEFINE_EVENT(writeback_congest_waited_template, writeback_congestion_wait,

	TP_PROTO(unsigned int usec_timeout, unsigned int usec_delayed),

	TP_ARGS(usec_timeout, usec_delayed)
);

DEFINE_EVENT(writeback_congest_waited_template, writeback_wait_iff_congested,

	TP_PROTO(unsigned int usec_timeout, unsigned int usec_delayed),

	TP_ARGS(usec_timeout, usec_delayed)
);

DECLARE_EVENT_CLASS(writeback_single_inode_template,

	TP_PROTO(struct inode *inode,
		 struct writeback_control *wbc,
		 unsigned long nr_to_write
	),

	TP_ARGS(inode, wbc, nr_to_write),

	TP_STRUCT__entry(
		__array(char, name, 32)
		__field(unsigned long, ino)
		__field(unsigned long, state)
		__field(unsigned long, dirtied_when)
		__field(unsigned long, writeback_index)
		__field(long, nr_to_write)
		__field(unsigned long, wrote)
	),

	TP_fast_assign(
		strncpy(__entry->name,
			dev_name(inode_to_bdi(inode)->dev), 32);
		__entry->ino		= inode->i_ino;
		__entry->state		= inode->i_state;
		__entry->dirtied_when	= inode->dirtied_when;
		__entry->writeback_index = inode->i_mapping->writeback_index;
		__entry->nr_to_write	= nr_to_write;
		__entry->wrote		= nr_to_write - wbc->nr_to_write;
	),

	TP_printk("bdi %s: ino=%lu state=%s dirtied_when=%lu age=%lu "
		  "index=%lu to_write=%ld wrote=%lu",
		  __entry->name,
		  __entry->ino,
		  show_inode_state(__entry->state),
		  __entry->dirtied_when,
		  (jiffies - __entry->dirtied_when) / HZ,
		  __entry->writeback_index,
		  __entry->nr_to_write,
		  __entry->wrote
	)
);

DEFINE_EVENT(writeback_single_inode_template, writeback_single_inode_start,
	TP_PROTO(struct inode *inode,
		 struct writeback_control *wbc,
		 unsigned long nr_to_write),
	TP_ARGS(inode, wbc, nr_to_write)
);

DEFINE_EVENT(writeback_single_inode_template, writeback_single_inode,
	TP_PROTO(struct inode *inode,
		 struct writeback_control *wbc,
		 unsigned long nr_to_write),
	TP_ARGS(inode, wbc, nr_to_write)
);

DECLARE_EVENT_CLASS(writeback_lazytime_template,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(unsigned long,	ino			)
		__field(unsigned long,	state			)
		__field(	__u16, mode			)
		__field(unsigned long, dirtied_when		)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->state	= inode->i_state;
		__entry->mode	= inode->i_mode;
		__entry->dirtied_when = inode->dirtied_when;
	),

	TP_printk("dev %d,%d ino %lu dirtied %lu state %s mode 0%o",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->ino, __entry->dirtied_when,
		  show_inode_state(__entry->state), __entry->mode)
);

DEFINE_EVENT(writeback_lazytime_template, writeback_lazytime,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
);

DEFINE_EVENT(writeback_lazytime_template, writeback_lazytime_iput,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
);

DEFINE_EVENT(writeback_lazytime_template, writeback_dirty_inode_enqueue,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
);

#endif /* _TRACE_WRITEBACK_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
