# fstest
Filesystem integrity checker

Fstest creates files of size 2^random with a random name and
content. After a while it reads back the file, verifies the content
and deletes them. File creation and deletion is scheduled to keep the
filesystem around a set percentage of full (default 90%). Files are
placed in a directory structure that grows deeper and fuller the more
files are present keeping the number of files in each directory
moderate. Every now and then (default 900 seconds) a statistic is
printed showing the amount of work done and speed of the FS.

Note: At 90% full fragmentation of the filesystem usually becomes an
issue and it is expected that speed drops over time. This should give
the lesser used code paths a good workout too.

    ./fstest -h|--help         - show help.
    ./fstest [options] [<dir>] - run filesystem test in <dir> [.]

    Options:
     -p|--percent <percent> - goal percentage used of filesystem [90].
     -i|--min-size <i>      - use minimum size 2^i [20].
     -a|--max-size <i>      - use maximum size 2^i [35].
     -t|--stats-time <i>    - report stats at most every i seconds [900].
