#undef TRACE_SYSTEM
#define TRACE_SYSTEM fuse

#if !defined(_TRACE_FUSE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_FUSE_H

#include <linux/types.h>
#include <linux/tracepoint.h>

TRACE_EVENT(queue_lengths,

	TP_PROTO(long long int bg_length, long long int pending_length, long long int processing_length),

	TP_ARGS(bg_length, pending_length, processing_length),

	TP_STRUCT__entry(
		__field(long long int, bg_length)
		__field(long long int, pending_length)
		__field(long long int, processing_length)
	),

	TP_fast_assign(
		__entry->bg_length		= bg_length;
		__entry->pending_length		= pending_length;
		__entry->processing_length	= processing_length;
	),

	TP_printk("BG Length : %lld Pending Length : %lld Processing Length : %lld",
		__entry->bg_length, __entry->pending_length, __entry->processing_length)
);

TRACE_EVENT(fuse_read_difference,

	TP_PROTO(unsigned long long int inode, long int time_diff),

	TP_ARGS(inode, time_diff),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
		__field(long int, time_diff)
	),

	TP_fast_assign(
		__entry->inode		= inode;
		__entry->time_diff	= time_diff;
	),

	TP_printk("Fuse Read on inode : %llu time diff : %ld", __entry->inode, __entry->time_diff)
);


TRACE_EVENT(fuse_read_iter_start,

	TP_PROTO(unsigned long long int inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
	),

	TP_fast_assign(
		__entry->inode		= inode;
	),

	TP_printk("Fuse Read Iter start on inode : %llu", __entry->inode)
);

TRACE_EVENT(fuse_read_iter_end,

	TP_PROTO(unsigned long long int inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
	),

	TP_fast_assign(
		__entry->inode		= inode;
	),

	TP_printk("Fuse Read Iter end on inode : %llu", __entry->inode)
);

TRACE_EVENT(fuse_dev_read_start,

	TP_PROTO(unsigned long long int inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
	),

	TP_fast_assign(
		__entry->inode		= inode;
	),

	TP_printk("Fuse Dev Read start on inode : %llu", __entry->inode)
);

TRACE_EVENT(fuse_dev_write_end,

	TP_PROTO(unsigned long long int inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
	),

	TP_fast_assign(
		__entry->inode		= inode;
	),

	TP_printk("Fuse Dev Write end on inode : %llu", __entry->inode)
);

TRACE_EVENT(fuse_read_request_create,

	TP_PROTO(unsigned long long int inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
	),

	TP_fast_assign(
		__entry->inode		= inode;
	),

	TP_printk("Fuse Read Request created on inode : %llu", __entry->inode)
);

TRACE_EVENT(fuse_write_request_create,

        TP_PROTO(unsigned long long int inode, unsigned long long int time_val),

        TP_ARGS(inode, time_val),

        TP_STRUCT__entry(
                __field(unsigned long long int, inode)
                __field(unsigned long long int, time_val)
        ),

        TP_fast_assign(
                __entry->inode		= inode;
                __entry->time_val	= time_val;
        ),

        TP_printk("Fuse Write Request created on inode : %llu Time : %llu", __entry->inode, __entry->time_val)
);

TRACE_EVENT(bg_queue_difference,

	TP_PROTO(unsigned long long int inode, long int time_diff),

	TP_ARGS(inode, time_diff),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
		__field(long int, time_diff)
	),

	TP_fast_assign(
		__entry->inode		= inode;
		__entry->time_diff	= time_diff;
	),

	TP_printk("Bg Queue on inode : %llu time diff : %ld", __entry->inode, __entry->time_diff)
);

TRACE_EVENT(pending_queue_difference,

	TP_PROTO(unsigned long long int inode, long int time_diff),

	TP_ARGS(inode, time_diff),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
		__field(long int, time_diff)
	),

	TP_fast_assign(
		__entry->inode		= inode;
		__entry->time_diff	= time_diff;
	),

	TP_printk("Pending Queue on inode : %llu time diff : %ld", __entry->inode, __entry->time_diff)
);

TRACE_EVENT(processing_queue_difference,

	TP_PROTO(unsigned long long int inode, long int time_diff),

	TP_ARGS(inode, time_diff),

	TP_STRUCT__entry(
		__field(unsigned long long int, inode)
		__field(long int, time_diff)
	),

	TP_fast_assign(
		__entry->inode		= inode;
		__entry->time_diff	= time_diff;
	),

	TP_printk("Processing Queue on inode : %llu time diff : %ld", __entry->inode, __entry->time_diff)
);

TRACE_EVENT(fuse_write_begin_page_alloc_start,

        TP_PROTO(pgoff_t index, unsigned len),

        TP_ARGS(index, len),

        TP_STRUCT__entry(
                __field(pgoff_t, index)
		__field(unsigned, len)
        ),

        TP_fast_assign(
                __entry->index 	= index;
		__entry->len	= len;
        ),

        TP_printk("Page index %lu len : %u ",
                __entry->index, __entry->len)
);

TRACE_EVENT(fuse_write_begin_page_alloc_end,

        TP_PROTO(pgoff_t index, unsigned len),

        TP_ARGS(index, len),

        TP_STRUCT__entry(
                __field(pgoff_t, index)
                __field(unsigned, len)
        ),

        TP_fast_assign(
                __entry->index  = index;
                __entry->len    = len;
        ),

        TP_printk("Page index %lu len : %u ",
                __entry->index, __entry->len)
);

TRACE_EVENT(fuse_file_write_iter_begin,

	TP_PROTO(unsigned long long int inode, unsigned long long int time_val),

	TP_ARGS(inode, time_val),

	TP_STRUCT__entry(
		__field(unsigned long long, inode)
		__field(unsigned long long, time_val)
	),

	TP_fast_assign(
		__entry->inode		= inode;
		__entry->time_val	= time_val;
	),

	TP_printk("Fuse Write Iter begin on inode : %llu Time : %llu", __entry->inode, __entry->time_val)
);


TRACE_EVENT(fuse_file_write_pages_begin,

	TP_PROTO(unsigned long long dummy),

	TP_ARGS(dummy),

	TP_STRUCT__entry(
		__field(unsigned long long, dummy)
	),

	TP_fast_assign(
		__entry->dummy = dummy;
	),

	TP_printk("Fuse write pages begin : %llu(dummy)", __entry->dummy)
);

TRACE_EVENT(fuse_file_write_pages_flush_amount,

	TP_PROTO(unsigned num_pages),

	TP_ARGS(num_pages),

	TP_STRUCT__entry(
		__field(unsigned, num_pages)
	),

	TP_fast_assign(
		__entry->num_pages = num_pages;
	),

	TP_printk("Pages packed in single Write Request : %u", __entry->num_pages)
);

TRACE_EVENT(fuse_file_write_pages_end,

	TP_PROTO(unsigned long long dummy),

	TP_ARGS(dummy),

	TP_STRUCT__entry(
		__field(unsigned long long, dummy)
	),

	TP_fast_assign(
		__entry->dummy = dummy;
	),

	TP_printk("Fuse write pages end : %llu(dummy)", __entry->dummy)
);

TRACE_EVENT(fuse_file_read_pages_begin,

	TP_PROTO(unsigned long long dummy),

	TP_ARGS(dummy),

	TP_STRUCT__entry(
		__field(unsigned long long, dummy)
	),

	TP_fast_assign(
		__entry->dummy = dummy;
	),

	TP_printk("Fuse read pages begin : %llu(dummy)", __entry->dummy)
);

TRACE_EVENT(fuse_file_read_pages_req_amount,

	TP_PROTO(unsigned num_pages),

	TP_ARGS(num_pages),

	TP_STRUCT__entry(
		__field(unsigned, num_pages)
	),

	TP_fast_assign(
		__entry->num_pages = num_pages;
	),

	TP_printk("Pages packed in single Read Request : %u", __entry->num_pages)
);

TRACE_EVENT(fuse_file_read_pages_end,

	TP_PROTO(unsigned long long dummy),

	TP_ARGS(dummy),

	TP_STRUCT__entry(
		__field(unsigned long long, dummy)
	),

	TP_fast_assign(
		__entry->dummy = dummy;
	),

	TP_printk("Fuse read pages end : %llu(dummy)", __entry->dummy)
);

TRACE_EVENT(fuse_file_write_iter_end, 

        TP_PROTO(unsigned long long int inode, unsigned long long int time_val),

        TP_ARGS(inode, time_val),

        TP_STRUCT__entry(
                __field(unsigned long long int, inode)
                __field(unsigned long long int, time_val)
        ),

        TP_fast_assign(
                __entry->inode		= inode;
                __entry->time_val	= time_val;
        ),

        TP_printk("Fuse Write Iter end on Inode : %llu Time : %llu", __entry->inode, __entry->time_val)
);

TRACE_EVENT(fuse_fill_write_page_alloc_start,

        TP_PROTO(pgoff_t index, unsigned len),

        TP_ARGS(index, len),

        TP_STRUCT__entry(
                __field(pgoff_t, index)
                __field(unsigned, len)
        ),

        TP_fast_assign(
                __entry->index  = index;
                __entry->len    = len;
        ),

        TP_printk("Page index %lu len : %u ",
                __entry->index, __entry->len)
);

TRACE_EVENT(fuse_fill_write_page_alloc_end,

        TP_PROTO(pgoff_t index, unsigned len),

        TP_ARGS(index, len),

        TP_STRUCT__entry(
                __field(pgoff_t, index)
                __field(unsigned, len)
        ),

        TP_fast_assign(
                __entry->index  = index;
                __entry->len    = len;
        ),

        TP_printk("Page index %lu len : %u ",
                __entry->index, __entry->len)
);

#endif /* _TRACE_FUSE_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
