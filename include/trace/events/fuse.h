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

	TP_PROTO(unsigned long long io_count),

	TP_ARGS(io_count),

	TP_STRUCT__entry(
		__field(unsigned long long, io_count)
	),

	TP_fast_assign(
		__entry->io_count = io_count;
	),

	TP_printk("iteration count : %llu ", __entry->io_count)
);

TRACE_EVENT(fuse_file_write_iter_end, 

        TP_PROTO(unsigned long long io_count),

        TP_ARGS(io_count),

        TP_STRUCT__entry(
                __field(unsigned long long, io_count)
        ),

        TP_fast_assign(
                __entry->io_count = io_count;
        ),

        TP_printk("iteration count : %llu ", __entry->io_count)
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
