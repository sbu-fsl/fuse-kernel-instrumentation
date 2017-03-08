#undef TRACE_SYSTEM
#define TRACE_SYSTEM vfs

#if !defined(_TRACE_VFS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_VFS_H

#include <linux/types.h>
#include <linux/tracepoint.h>


TRACE_EVENT(vfs_read_start,

        TP_PROTO(unsigned long long int inode),

        TP_ARGS(inode),

        TP_STRUCT__entry(
                __field(unsigned long long int, inode)
        ),

        TP_fast_assign(
                __entry->inode          = inode;
        ),

        TP_printk("VFS read start on inode : %llu", __entry->inode)
);

TRACE_EVENT(vfs_read_end,

        TP_PROTO(unsigned long long int inode),

        TP_ARGS(inode),

        TP_STRUCT__entry(
                __field(unsigned long long int, inode)
        ),

        TP_fast_assign(
                __entry->inode          = inode;
        ),

        TP_printk("VFS read end on inode : %llu", __entry->inode)
);

#endif /* _TRACE_VFS_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
