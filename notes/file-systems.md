# file systems

## toc

- fs structure
- fs implementation
- virtual fs
- allocations
  - contiguous
  - linked
  - indexed
    - inodes
- free space mgmt
- mem mapped files

## fs

### rationale

- apps need to store & retrieve info
  - programs can store limited amounts of info in their own addr spaces
  - storage capacity restricted to size of vram
    - far too small for several apps
      - airline reservations, banking, directory services, &c.
- info in addr space of procs is not persistent
  - when proc terminates, info is lost
  - for many apps, info must be retained for a much longer time - weeks, years, forever
- multiple procs often need to access parts of info at same time

### essential requirements for long-term storage

- store large amount of info
- info must survive proc termination
- multiple procs must be able to concurrently access info
- store info on disk/external media in units called files

### files are abstraction mechanism

- provide way to store info & read it back later
- do this in a way that shields user from knowing
  - how & where info stored on disk
  - how disks really work

### naming files

- important characteristic of abstraction mechanism
- strings 8-255 chars long
- most os support 2-part file names separated by period
  - last part referred to as file extension
    - conventions: easy to remember
    - enforced in some cases e.g. compiler

### files can be structured in many ways: unstructured sequence of bytes

- os doesn't know or care what is in file
  - max flexibility
- os doesn't help, but doesn't get in way
- meaning is imposed by programs
- most os support this

### file structure: sequence of records

- file is sequence of fixed-length records
- read operation returns 1 record
  - write operatin overwrites/appends 1 record
- 80-column punch card used to be dominant
  - files consisted of 80-char records

### file structure: tree of records

- get record with specific key
- os, not user, decides where to place new records

### dir & disk structure

- typically, there are millions of files within a computer
- storage device can be used in its entirety for a fs
- can also be partitioned
  - limit size of individual fs
  - put multiple fs tupes
  - set aside for swap space

### dirs used to organize files

- can be viewed as symbol table
- in many systems, dirs therefore are files
- supported operations
  - insert, delete, search, list, rename entries
  - traversal

### organization of dirs

- 1-level dir
- 2-level dir
- tree-structured dirs
- acyclic graph dirs
  - shared subdir

### mounting fs

- many systems have more than 1 disk - how to handle?
- solution 1: keep self-contained fs on each disk, keep them separate
- solution 2: allow a disk to be mounted in another disk's file tree

### checks performed during mounting

- os verified that device contains valid fs
- read device dir
  - make sure format is expected
- windows mounting
  - each device in separate namespace
  - letter followed by colon

## fs structure

### disks provide bulk of secondary storage

- disk can be rewritten in place
  - read, modify, then write back to same place
- disks can directly access any block of info
- i/o transfers between mem & disk performed in units of blocks

### 2 core design problems in fs

- defining how fs should look to user
- creating algorithms & data structures to map logical fs onto physical storage

### levels of fs (highest to lowest)

- apps
- logical fs
- file organization module
- basic fs
- i/o control
- devices

### i/o control consists of device drivers

- transfers info between main mem & disk
- receives high-level commands (e.g. retrieve block 123)
- outputs low-level, hardware-specific instructions
  - used by hardware controller
  - writes bit patterns into specific locations of i/o controller

### basic fs issues commands to device driver

- read & write physical blocks on disk (e.g. drive 1, cylinder 73, sector 10)
- manages buffers & caches
  - to hold fs, dir & data blocks
  - improves performance

### file organization module

- knows about files, both logical & physical blocks
- translate logical addrs to physical ones
  - needed for every block
- includes free space manager
  - tracks unallocated blocks & allocates as needed

### logical fs

- manages metadata info - data describing data
- manages file structure via file control blocks
  - info about file
    - ownership & permissions
    - location of file contents
  - inode in unix fs

### several fs are in use

- cd-roms written in iso 9660 format designed by cd manufacturers
- unix
  - unix fs (ufs)
  - bsd fast fs (ffs)
- windows
  - fat
  - fat32
  - ntfs
- linux
  - supports 40 different fs
  - extended fs: ext2, ext3, ext4

## anatomy of disk

### using magnet & nail for instant messaging

- message: "see you later, or not"
- drop nail in friend's mailbox
  - if nail magnetized, you'll see the friend
  - if nail not magnetized, you won't
- your friend comes home & picks up nail
  - use nail to pick up paperclip
    - if sticks, friend will expect to see you
- magnetism can be used to store info

### using magnetism to store info

- store info even when you turn power off
- storing 10001
  - magnetize 1st bit
  - demagnetize next 3
  - magnetize next bit

### anatomy of disk

- disk comprises set of platters
  - flat, circular shape
  - made of glass or aluminum
- both surfaces of platter covered with magnetic material
  - store info by recording it magnetically
- platter logically divided into circular tracks
  - subdivided into sectors

### rates & times associated with disks

- transfer rate: rate of data movement between disk & mem
- positioning time
  - seek time: time to move disk arm to necessary cylinder
  - rotational latency: time for desired sector to rotate to disk head

### how about cd-roms, dvds, & blu-rays?

- data written with help of high intensity laser that makes pits on reflecting surface
- when reading
  - use lower intensity laser
  - mirrors & focusing lens used to shine light on a specific portion of disk
  - amount of light that is reflected back depends on presence of pit (used to interpret 1 or 0)
- shorter wavelength, smaller pit, greater density of what can be stored
- dvd uses 650 nm wavelength laser diode
  - 780 nm for cd
  - pit sizes: dvd = 0.74 um, cd = 1.6 um
- blu-ray
  - 405 nm wavelength, 0.13 um pit size
  - 50 gb storage possible on 1 disk
- next
  - archival disk (sony/panasonic) 79.5 nm with 300gb of data storage

## fs implementation

### boot: etymology tidbit

- pull yourself up by your (own) bootstraps
- bring at least a portion of kernel into main mem, then have processor execute it

### on-disk structures used to implement fs

- boot control block
  - info needed to boot os from that volume
- volume control block: volume info
  - num blocks in partition
  - size of blocks
  - free block count/pointers
  - free file control block count/pointers
  - ufs: super block
  - windows: master file table
- dir structure to organize files (1 per fs)
- per-file file control block
  - contains details about individual files

### in mem structures used to improve performance via caching

- mount table
  - info about each mounted volume
- dir structure cache
  - holds info about recently accessed dirs
- system-wide open file table
  - file control block of each open file
- buffers to hold fs blocks
  - to read & write to storage

### creation of new file

- allocation of file control block (fcb)
- read appropriate dir into mem
  - dir is just a file in unix
    - special type field
- update dir with new file name & fcb
- write dir back to disk

### partitions: a disk can be sliced into multiple partitions

- cooked: has a fs
- raw: no fs
  - unix swap spaces use this
  - hold info needed by disk raid (redundant array of independent disks) systems

### boot info can be stored in separate partition

- usually sequential series of blocks loaded as image into mem
- image execution starts at predefined location

## dir implementation

### dir implementation

- linear list
- hash table

### linear list

- file names with pointers to data blocks
- simple to program, inefficient & slow execution
- finding file requires linear search
- sorted list
  - complicates creation & deletion
- tree data structures might help here
  - e.g. b-tree

### dir implementation: hash table

- linear list & hash table maintained
- key computed from file name
  - hash table value returns pointer to entry in linear list
- things to consider
  - account for collisions in hash space
  - need to rehash table when num entries exceed limit

### allocation methods: objective & approaches

- how to allocate space for files such that
  - disk space utilized effectively
  - file accessed quickly
- major methods
  - contiguous
  - linked
  - indexed

## contiguous allocations

### contiguous allocation

- each file occupies a set of contiguous blocks on disk
  - if file has n blocks & starts at location b, occupies blocks b, b + 1, ..., b + n - 1
- disk head movements
  - none for moving from block b to b + 1
  - only when moving to different track

### sequential & direct access in contiguous allocations

- sequential access
  - remember disk addr of last referenced block
  - when needed, read next block
- direct access to block i of file that starts at block b: b + i

### contiguous allocations suffer from external fragmentation

- free space broken up into chunks
  - space fragmented into holes
- largest continuous chunk may be insufficient for meeting request
- compaction very slow on large disks - needs several hours

### determining how much space needed for a file is another problem

- preallocate if eventual size of file is known
  - inefficient if file grows slowly
    - much of allocated space unused for long time
- modified contiguous allocation scheme
  - allocate space in a continuous chunk initially
  - when space runs out, allocate another set of chunks (extent)

## linked allocations

### linked allocation: each file is linked list of disk blocks

- each file block has a pointer to the next block
- file blocks, like with logical mem, do not need to be in physical order

### linked list allocations

- advantages
  - every disk block can be used
    - no space lost in external fragmentation
  - sufficient for directory entry to merely store disk addr of 1st block
    - the rest can be found starting there
- disadvantages
  - used effectively only for sequential accesses - extremely slow random access
  - space in each block set aside for pointers - each file requires slightly more space
  - reliability - what if pointer lost or damaged?

### linked list allocations: reading/writing files much less efficient

- amount of data storage in block is no longer a power of 2
  - pointer takes up some space
- peculiar size less efficient
  - programs read/write in blocks that are powers of 2

### linked list allocation using an index

- entire disk block available for data
- random access much easier
  - chain must be followed, but chain can be cached in mem
- ms-dos & os/2 operating systems use such a file allocation table (fat)

### linked list allocation using an index: disadvantages

- table must be cached in mem for efficient access
- large disk will have large num data blocks
  - table consumes large amount of physical mem

## indexed allocations

### indexed allocations

- bring all pointers together in 1 location called an index block
- each file has its own index block
  - ith entry points to ith block of file
  - dir contains index block's addr

### indexed allocation supports direct access without external fragmentation

- every disk block can be utilized -> no external fragmentation
- space wasted by pointers is generally higher than linked list allocations
  - e.g. file has 2 blocks
    - linked list allocations: 2 pointers
    - indexed allocations: entire index block must be allocated

## inodes

### inode

- fixed-length data structure (1 per file)
- contains info about
  - file attributes
    - size, owner, creation/modification time, &c.
  - disk addrs
    - file blocks that comprise file
- used to encapsulate info about large num file blocks
- e.g.
  - block size = 8 kb, file size = 8 gb
  - 1 million file blocks
    - inode stores info about pointers to these blocks
  - inode allows us to access info for all these blocks
    - storing pointers to these file blocks also takes up storage

### managing info about data blocks in inode

- 1st few data blocks of file stored in inode
- if file is large
  - indirect pointer to block of pointers pointing to additional data blocks
- if file is larger
  - double indirect pointer (pointer to block of indirect pointers)
- if file is huge
  - triple indirect pointer (pointer to block of double indirect pointers)

### schematic structure of inode

- file attributes: size (bytes), owner uid/gid, relevant times, link & block counts, permissions
- addr of disk block
  - direct pointers to 1st few file blocks
  - single indirect pointer -> pointers to next file blocks
  - double indirect pointer
  - triple indirect pointer

### disk layout in traditional unix systems

- boot block
- super block
- inodes
- data blocks
- note: integral num inodes fit in single data block

### super block describes state of fs

- total size of partition
- block size & num disk blocks
- num inodes
- list of free blocks
- inode num of root dir
- destruction of superblock -> fs unreadable

### linear array of inodes follows data block

- inodes numbered from 1 to max
- each inode identified by inode num
  - inode num contains info needed to locate inode on disk
  - users think of files as filenames
  - unix thinks of files in terms of inodes

### unix dir structure

- contains only filenames & corresponding inode numbers
- use `ls -i` to retrieve inode nums of files in dir

### advantages of dir entries that have name & inode info

- changing filename only requires changing dir entry
- only 1 physical copy of file needs to be on disk
  - file may have several names (or same name) in different dirs
- dir entries are small
  - most file info kept in inode

### hard vs. symbolic links

- hard links point to the same inode as the original file
- symbolic links have different inode num, but that num points to the original file

### max size of hard disk (8 kb blocks, 32-bit pointers)

- 32-bit pointers can access 2^32 blocks
- at 8 kb per block, hard disk can be 2^13 \* 2^32 = 2^45 bytes (32 tb)

### case for larger block sizes

- larger partitions for fixed pointer size
- retrieval more efficient -> better system throughput
- problem: internal fragmentation

### limitations of fs based on inodes

- file must fit in single disk partition
- partition size & num files are fixed when system set up

### inode preallocation & distribution

- inodes preallocated on volume
  - even on empty disks, %age of space lost to inodes
- preallocating inodes & spreading them improves performance
- keep file's data block close to inode -> reduce seek times

### checking up on inodes: `df -i`

- inode stats for given set of fs (total, free, & used inodes)

## free space mgmt

### free space mgmt

- disk space limited -> reuse spcae from deleted files
- keep track of free disk space
  - maintain free space list
  - record all free disk blocks
- metadata i/o can impact performance

### free space mgmt using free space list

- creating new file
  - search free space list for requisite space
  - allocate that to file
- file deletion
  - add file blocks of deleted file to free space list

### using bit vectors to implement free space list

- each file block represented with bit
  - block is free: 1
  - block allocated: 0
- hdd with n blocks requires n-bit vector

### advantages of using bit vector

- simplicity
- efficiency in finding 1st free block or n consecutive free blocks
- most cpus have bit manipulation operators
  - allows us to compute free blocks fairly fast

### finding free blocks using bit vector

- 0 valued word represented allocated blocks
- 1st nonzero word is scanned for 1st 1 bit
  - location of 1st free block
- free block num = bits per word \* num 0 value words + offset of 1st 1 bit

### problems with bit vector approach

- for efficiency purposes, bit vector must be mem resident
  - difficult for larger disks
  - 1 tb hard disk with 4kb blocks
    - bit vector = 32 mb
  - 1 pb disk = 32 gb bit vector
- freeing 1 gb of data on a 1 tb disk
  - thousands of blocks of bit maps need to be updated
    - blocks could be scattered all over disk

### linked list approach to free space mgmt

- link free blocks
- pointer to 1st free block stored in special location on disk & cached in mem
- problems
  - to traverse list, we must read each block - substantial i/o
  - finding large num free blocks expensive

### grouping to augment linked list approach

- set aside 1 block for tracking a portion of chain
  - 1st n - 1 entries are free blocks
  - last entry points to another set of free blocks
- if tracker block has 512 entries
  - after linked list block is loaded in mem, next 510 blocks do not need i/o operations
  - 512th entry points to another tracker block

### free space mgmt using counting

- several contiguous blocks free/allocated simultaneously
- keep addr of 1st free block & num contiguous blocks that follow it

### space maps used in zfs (zettabyte fs)

- 1 zb = 2^70 bytes
- controls size of i/o data structures
  - minimize i/o needed to manage them
- metaslabs divide space on disk into chunks
  - a volume has hundreds of metaslabs
- a metaslab has a space map
  - counting algorithm to store info on space maps

### zfs free space mgmt

- does not write i/o metadata directly to disk
  - free space list updated on disk using transactional techniques
- when space (de)allocated from metaslab
  - corresponding space map loaded into mem
    - b-tree structure indexed by block offsets

## mem mapped files

### mem mapped files

- `open()`, `read()`, `write()` require system calls & disk access
- mem mapping - allow part of virt addr space to be logically associated with the file

### mem mapping maps a disk block to page(s) in mem

- manipulate files through mem
  - multiple procs may map file concurrently
    - enables data sharing
  - since jvm 1.4, java supports mem mapped files (FileChannel)
- writes to files in mem not necessarily immediate

### mem mapped files: things to watch for

- make sure that 2 procs don't see inconsistent views of same file
- file may be larger than entire virt addr space -> map portions of file

## performance

### improving reads from disk

- disk controllers have onboard cache that can store multiple racks
- when seek performed
  - track read into disk cache
  - starting at sector under disk head
    - reduces latency
- controller then transfers sectors to os

### buffering data read from disks since they may be used again

- maintain a separate section of mem for buffer cache
- cache file data using page cache
  - use vram techniques
  - cache as pages rather than file blocks
  - access interfaces with vram, not fs

### unified buffer cache

- mem mapping & `read()`/`write()` system calls use same page cache
- allows vram to manage fs data

### need to make distinction between pages allocated to procs & page cache, else...

- procs performing i/o will use most of mem set aside for caching pages
- pages may be reclaimed from procs
- solution: fixed limit for proc pages & fs page cache
  - prevent one from forcing out other
  - e.g. solaris 8

### synchronous writes

- writes not buffered
- occurs in order of receival
- calling routine waits for data to reach disk, blocking call
- metadata writes tend to be synchronous
- databases use this for atomic transactions

### async writes

- data stored in cacche
- control returns to caller immediately
- done majority of time

### page cache, disk drivers, & async disk writes

- when data written to disk, pages buffered in cache
- disk driver sorts its output queue based on disk addrs
  - minimize disk head seeks
  - write at times optimized for disk rotations

### page cache & page replacement algorithms

- replacement algorithm depends on file access type
- files being read/written sequentially
  - pages should not be replaced in lru order
    - most recently used page may never be used again
- free behind
  - remove page from buffer whennext one requested
- read ahead
  - requested page & subsequent pages cached
