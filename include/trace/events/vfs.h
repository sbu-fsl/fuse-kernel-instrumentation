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

TRACE_EVENT(vfs_write_start,

        TP_PROTO(unsigned long long int inode, unsigned long long int time_val),

        TP_ARGS(inode, time_val),

        TP_STRUCT__entry(
                __field(unsigned long long int, inode)
                __field(unsigned long long int, time_val)	
        ),

        TP_fast_assign(
                __entry->inode          = inode;
		__entry->time_val	= time_val;
        ),

        TP_printk("VFS write start on inode : %llu Time : %llu", __entry->inode, __entry->time_val)
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

TRACE_EVENT(vfs_write_end,

        TP_PROTO(unsigned long long int inode, unsigned long long int time_val),

        TP_ARGS(inode, time_val),

        TP_STRUCT__entry(
                __field(unsigned long long int, inode)
                __field(unsigned long long int, time_val)
        ),

        TP_fast_assign(
                __entry->inode          = inode;
                __entry->time_val       = time_val;
        ),

        TP_printk("VFS write end on inode : %llu Time : %llu", __entry->inode, __entry->time_val)
);

#endif /* _TRACE_VFS_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
