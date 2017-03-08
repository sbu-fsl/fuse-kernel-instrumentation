#undef TRACE_SYSTEM
#define TRACE_SYSTEM fuse

#if !defined(_TRACE_FUSE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_FUSE_H

#include <linux/types.h>
#include <linux/tracepoint.h>

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
