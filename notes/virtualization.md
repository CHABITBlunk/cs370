# virtualization

### toc

- virtualization
- techniques for efficient virtualization
  - virtualizing the unvirtualizable
- cost of virtualization
- mem virtualization
- virtual appliances
- clouds

## virtualization

### what we will examine

- mem & i/o virtualization
- type 1 & type 2 hypervisors
- techniques for efficiency
- why?
- advantages
- brief history
- requirements

## why virtualization

### firms often have multiple dedicated servers: email, ftp, e-commerce, web, &c.

- load: maybe 1 machine cannot handle all that load
- reliability: mgmt doesn't trust os to run 24/7 without failures
- by putting 1 server on a separate computer, if one of the server crashes, at least others not affected
- if someone breaks into web server, at least sensitive emails are still protected - sandboxing

### but

- while this approach achieves isolation & fault tolerance
  - still expensive & hard to manage because so many machines are also involved
- other reasons for having separate machines
  - orgs depend on more than 1 os for their daily operations
    - web server on linux, mail server on windows, e-commerce server on os x, other services on various flavors of unix

### what to do?

- possible & popular solution is to use vm tech
- sounds hip, but has been used since the 1960s
  - even so, we use it very differently today

### main idea

- vmm (vm monitor) creates illusion of multiple (virtual) machines on same physical hardware
  - vmm aka hypervisor
    - we will look at type 1 hypervisors (bare metal) & type 2 hypervisors (use services & abstractions offered by underlying os)
- virtualization allows a single computer to host multiple vms, each running a potentially different os

### if 1 vm fails, it doesn't bring down any others

- different servers run on different vms
  - maintains partial failure model at a lower cost with easier maintainability
- also, we can run different os on same hardware
  - benefit from vm isolation in face of attacks
  - enjoy other good stuff: savings, real estate, &c.
  - convenient for complex software stack with precise system dependencies (think core libs)

### isn't consolidating servers like this putting all eggs in 1 basket?

- if server running vms fails, result is even more catastrophic than crashing of a single dedicated server

### why virtualization works

- service outages are due not to faulty hardware, but due to poor software, emphatically including os
  - ill-designed, unreliable, buggy, poorly configured software
- migration to another machine may be easier
- only software running in highest privilege is hypervisor
- hypervisor has 2 orders of magnitude fewer lines of code than a full os
  - 2 orders of magnitude fewer bugs
- hypervisor is simpler than os because it only does one thing
  - emulate copies of bare metal (most commonly x86)

### advantages to running software in vms besides strong isolation

- few physical machines
  - saves money on hardware & electricity
  - takes up less rack space
- for companies such as amazon/microsoft
  - reducing physical demands on data centers represents huge cost savings
  - companies frequently locate data centers in middle of nowhere to be close to hydroelectric dams

### checkpointing & migration

- for load balancing across multiple servers
- easier with vms than migrating procs running on normal os
- why?
  - in bare metal case, a fair amount of critical state info about each proc kept in os tables
  - when migrating vm, all that has to be moved are mem & disk images
    - all os tables move with it

### other uses of vms

- run legacy apps
- software development: test on different os
  - no need to get a dozen computers & install a dozen os, just install a dozen vms
    - of course you can partition the hard drive & installed different os, but more difficult
      - standard pcs allow only 4 primary disk partitions, no matter how big disk is
      - although a multiboot program can be installed in boot block, it would be necessary to reboot computer to switch os
- with vms, all run at once since they are just procs

### key idea of cloud is straightforward

- outsource computation/storage needs to well-managed data center
- pay for use of resources, but at least you won't have to worry about physical machines, power, cooling, & maintenance

## brief history of virtualization

### 1960s

- early 1960s, ibm experimented with 2 independently developed hypervisors: simmon & cp-40
- cp-40 was research project that was reimplemented as cp-67 to form control program of cp/cms, a vm os for ibm/360

### 1970s

- 1974: gerald popek & robert goldberg published seminal paper
  - listed what conditions a computer architecture should satisfy to support virtualization efficiently
- famously, well-known x86 architecture that originated in 1970s did not meet this for decades
- 1970s were very productive, seeing birth of unix, ethernet, cray-1, microsoft, & apple

### path to vmware

- researchers at stanford developed a new hypervisor called disoc
  - went on to found vmware, which offers type 1 & type 2 hypervisors
- vmware introduced 1st virtualization solution for x86 in 1999
- other products followed in its wake
  - xen, kvm, virtualbox, hyper-v, parallels

## requirements for virtualization

### requirements for virtualization

- vms must act just like real machines
  - must be possible to boot & install arbitrary os
- task of hypervisors is to efficiently provide this illusion

### hypervisors should score well on

- safety
  - hypervisor should have full control of virtualized resourcees
- fidelity
  - behavior of program on vm should be identical to same program running on bare hardware
- efficiency
  - much of code in vm should run without intervention from hypervisor

### safety

- consider each instruction in turn as an interpreter (such as bochs) & perform exactly what is needed
  - may execute some instructions (inc) as is, but other instructions must be simulated
- we cannot allow guest os to disable interrupts for entire machine or modify page-table mappings
  - trick is to make guest os believe it can
- interpreter may be safe, even hi-fi, but performance is abysmal
  - vmms try to execute most code directly

### fidelity

- virtualization has long been a problem on x86
  - defects in 386 carried forward into new cpus for 20 years in the name of backward compatibility
- every cpu with kernel mode & user mode has instructions that behave differently
  - depending on whether executed in kernel/user mode
    - sensitive instructions
  - some instructions cause trap when executed in user mode
    - privileged instructions
- if you do something in user mode that you shouldn't
  - hardware should trap
  - ibm/370 had this property, intel's 386 didn't
- several executable 386 instructions were ignored if executed in user mode
  - or executed with different behavior
  - e.g. popf instruction replaces flags register which changes bit that enables/disables interrupts - in user mode, this bit just wasn't changed
    also, some instructions could read sensitive state in user mode without causing trap

### problem solved in 2005

- when intel & amd introduced virtualization in their cpus
  - intel: vt (virtualization technology)
  - amd: svm (secure vm)
- create containers in which vms can be run
- when guest os started in container, continues to run until causes an exception & traps to hypervisor
  - e.g. by executing an i/o instruction
- set of operations that trap is controlled by hardware bit map set by hypervisor
  - classic trap-and-emulate approach becomes possible

### before

- hypervisors before 2005 didn't really run original guest os
  - rewrote part of code on fly
    - replace instructions that are sensitive but not privileged - binary translation

### full virtualization

- trap all instructions
- fully simulate entire computer
- tradeoff: high overhead
- benefit: can virtualize any os

### paravirtualization

- never aims to present a vm that looks just like actual underlying hardware
- present machine-line software interface that explicitly exposes that it is a vm
  - offers set of hypercalls that allow guest to send explicit requests to hypervisor
    - similar to how system call offers kernel services to apps
- drawback: guest os has to be aware of vm api
- guests use hypercalls for privileged, sensitive operations like updating page tables
  - but they do it in cooperation with hypervisor
  - overall system can be simpler & faster
- paravirtualization offered by ibm since 1972
- idea revived by denali (2002) & xen (2003) hypervisors

### not all virtualization attempts to trick guest into believing it has entire system

- sometimes aim is to allow proc to run that was run on different os and/or architecture
  - proc-level virtualization
- e.g.
  - wine compatibility layer allows windows apps to run on posix-compliant systems
  - proc-level version of qemu allows apps for one architecture to run on another

## type 1 & type 2 hypervisors

### terms

- guest os
  - os running on top of hypervisor
- host os
  - for type 2 hypervisor: os that runs on hardware
- safe executions
  - execute machine's instruction set in safe manner
  - guest os may change or mess up its own page tables, but not those of others

### type 1 hypervisor

- only program running in most privileged mode
- support multiple copies of actual hardware
  - vms
  - similar to procs a normal os would run
- located right above hardware

### control domain in type 1 hypervisor: also known as dom0

- is a vm like guest vms, with 2 functional differences
  - has ability to talk to hypervisor to instruct it to start & stop guest vms
  - by default, contains device drivers needed to address hardware

### type 2 hypervisor

- aka hosted hypervisor
- relies on host os to allocate & schedule resources
- still pretends to be full computer with cpu & other devices
- located side-by-side with host os procs

### type 2: running guest os

- when it starts for first time, acts like newly booted computer
  - expects to find dvd, usb drive, or cd-rom containing os
    - drive could be virtual device
    - store image as iso file on hard drive & have hypervisor pretend it's reading from proper dvd drive
- hypervisor installs os to its virtual disk (just a file) by running installation that it found on dvd
- once guest os installed on virtual disk, can be booted & run

## techniques for efficient virtualization

### type 1 hypervisor

- vm runs as user proc in user mode
  - not allowed to execute sensitive instructions in the popek-goldberg sense
- but vm runs a guest osthat thinks it is in kernel mode - virtual kernel mode
- vm also runs user procs, which think they are in user mode (they really are)

### execution of kernel model instructions

- what if guest os executes an instruction that is allowed only when cpu is really in kernel mode?
  - on cpus without vt (intel virtualization tech)?
    - instruction fails, os crashes
- on cpus with vt
  - trap to hypervisor occurs
    - hypervisor can inspect instruction to see if it was issued
      - by guest os: arrange for instruction to be carried out
      - by user proc in that vm: emulate what hardware would do when confronted with sensitive instruction executed in user mode

## virtualizing the unvirtualizable

### virtualizing x86 before vt (& amd svm)

- virtualizing is straightforward when vt available
- when not available
  - make clever use of
    - binary translation
    - hardware features that exist on x86

### protection rings

- x86 supported 4 protection modes (rings)
- ring 3 is least privileged
  - where normal procs execute
  - cannot execute privileged instructions
- ring 0 is most privileged
  - allows execution of any instruction
  - in normal operation, kernel runs here
- other rings never used by os

### in other words, hypervisors had some room to play with

- many solutions kept hypervisor in kernel mode
- apps in user mode
- guest os is layer of intermediate privilege - ring 1

### how this allows virtualization

- kernel privileged relative to user procs
  - any attempt to access kernel mem from user program leads to an access violation
- guest os' privileged instructions trap to hypervisor
  - hypervisor performs sanity checks, then performs instructions on guest's behalf

### but what about sensitive instructions in guest os' kernel code?

- hypervisor makes sure they no longer exist
  - hypervisor rewrites code one basic block at a time
- basic block
  - short, straight-line sequences that end with branch
  - contain no jump, call, trap, return, or other instructions that alter flow of control
    - except for very last instruction which does precisely that

### executing basic blocks

- prior to executing basic block, hypervisor scans it to se if there are sensitive instructions
  - if so, replace with call to hypervisor procedure that handles them

### dynamic translation & emulation sound very expensive

- typically, they're not
- translated blocks are cached, so no translation needed in future
- after basic block has completed executing, control returned to hypervisor
  - locates block's successor
  - if successor has already been translated, it can be immediately executed

### binary translations

- common to perform binary translation on all guest os code running in ring 1
- replace even privileged, sensitive instructions that could be made to trap
  - traps can be expensive & binary translation leads to better performance

### what about type 2 hypervisors?

- though type 2 hypervisors are conceptually different from type 1, they use the same techniques by & large
  - e.g. vmware esx server (type 1, 2001) used exactly in same binary translation as 1st vmware workstation (type 2, 1999)

### for faithful virtualization

- guest os should also be tricked into thinking it is only king
  - full control of all machine's resources
  - access to entire addr space (4gb on 32-bit machines)
- when king finds another king squatting in its addr space?

### 2 kings problem

- in linux, a user proc has access to 3 gb of 4 gb addr space (32-bit addressing)
  - 1 gb reserved for kernel
  - any access to kernel memory leads to trap
- we could take trap & emulate appropriate actions, but that's expensive

### type 2 hypervisors have kernel module operating in ring 0

- allows manipulation of hardware with privileged instructions
  - allows guest to have full addr space
- fine, but at some point, hypervisor needs to clean up & restore original processor context

### what if guest running & interrupt arrives from external device?

- type 2 hypervisor depends on host's device drivers to handle interrupt
- so, hypervisor reconfigures hardware to run host os system code
  - when device driver runs, it finds everything as expected
- hypervisor behaves like teenagers throwing a party when parents away
  - it's ok to rearrange furniture completely, as long as they put it back as before

### world switch

- going from hardware configuration for host kernel to a configuration for guest os

### why do hypervisors work even on unvirtualizable hardware?

- sensitive instrucitons in guest kernel replaced by calls to procedures that emulate these instructions
- no sensitive instructions issued by guest os ever executed directly by true hardware
  - turned into calls by hypervisor, which emulates them

## cost of virtualization

### cost of virtualization

- we expect cpus with vt would greatly outperform software techniques
- trap-and-emulate approach used by vt hardware generates a lot of expensive traps
  - ruin cpu caches, tlbs, & branch predictions
- in contrast, when sensitive instructions replaced by calls to hypervisor procedures, no context switching overhead incurred
- still, with modern vt hardware, usually hardware beats software

### contrasting virtualization approaches

|                                  | full virtualization with binary translation  | hardware assisted                            | os assisted/paravirtualization                                                                  |
| -------------------------------- | -------------------------------------------- | -------------------------------------------- | ----------------------------------------------------------------------------------------------- |
| technique                        | binary translation & direct execution        | exit to root mode on privileged instructions | hypercalls                                                                                      |
| guest modification/compatibility | unmodified guest os, excellent compatibility | unmodified guest os, excellent compatibility | guest os codified to issue hypercalls so it can't run on native hardware, lacking compatibility |
