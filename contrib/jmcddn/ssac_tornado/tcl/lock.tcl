annotation type Lock enum {
  acquireStart   acquireEnd
  releaseStart   releaseEnd
  blockStart     blockEnd
}

set LOCKADDRESS("NOCPU") {}
set LockExitList("NOADDRESS") "NOFUNC"
set LockRtnList {}
set LockBlockRtnList {}
set LockBlockList("NOLOCKID") {}
set blockEndCount 0

annotation set simos enter {
    set LOCKADDRESS($CPU) {}
    annotation disable Lock
}

proc Locktraceon {} {
    annotation enable Lock 
}

proc Locktraceoff {} {
    global CYCLESLOCKACQS CYCLESLOCKRELS NUMLOCKACQS NUMLOCKRELS NUMLOCKACQFAILS
    global NUMLOCKBLOCKS CYCLESLOCKBLOCKS
    global LockNames blockEndCount
    annotation disable Lock
    set locks [array names CYCLESLOCKACQS]
    set knownlocks [array names LockNames]
    set tnumacqs 0
    set tnumrels 0
    set tnumblocks 0
    set tcycacqs 0
    set tcycrels 0
    set tcycblocks 0
    set tnumfails 0

    log "**** STATS: Lock total number of locks accessed: [llength $locks]\n"
    foreach l $locks {
	if { [lsearch $knownlocks $l]==-1 } {
	    set lname "UNKNOWNLOCK"
        } else {
	    set lname $LockNames($l)
	}
	log "**** STATS: Lock: $l: $lname: number of acquires=$NUMLOCKACQS($l)\n"
	set tnumacqs [expr $tnumacqs + $NUMLOCKACQS($l)]
	log "**** STATS: Lock: $l: $lname: cycles in acquires=$CYCLESLOCKACQS($l)\n"
	set tcycacqs [expr $tcycacqs + $CYCLESLOCKACQS($l)]
	log "**** STATS: Lock: $l: $lname: number of releases=$NUMLOCKRELS($l)\n"
	set tnumrels [expr $tnumrels + $NUMLOCKRELS($l)]
	log "**** STATS: Lock: $l: $lname: cycles in releases=$CYCLESLOCKRELS($l)\n"
	set tcycrels [expr $tcycrels + $CYCLESLOCKRELS($l)]
	log "**** STATS: Lock: $l: $lname: number of blocks=$NUMLOCKBLOCKS($l)\n"
	set tnumblocks [expr $tnumblocks + $NUMLOCKBLOCKS($l)]
	log "**** STATS: Lock: $l: $lname: cycles in block=$CYCLESLOCKBLOCKS($l)\n"
	set tcycblocks [expr $tcycblocks + $CYCLESLOCKBLOCKS($l)]
	log "**** STATS: Lock: $l: $lname: number of aquire failures=$NUMLOCKACQFAILS($l)\n"
	set tnumfails [expr $tnumfails + $NUMLOCKACQFAILS($l)]
    }
    log "**** STATS: Lock: Total number of Lock acquires=$tnumacqs\n"
    log "**** STATS: Lock: Total number of cycles in acquires=$tcycacqs\n"
    log "**** STATS: Lock: Total number of Lock releases=$tnumrels\n"
    log "**** STATS: Lock: Total number of cycles in releases=$tcycrels\n"
    log "**** STATS: Lock: Total number of Lock blocks=$tnumblocks\n"
    log "**** STATS: Lock: Total number of cycles in blocks=$tcycblocks\n"
    log "**** STATS: Lock: Total number of Lock blockEnds=$blockEndCount\n"
    log "**** STATS: Lock: Total number of Lock acquire failures=$tnumfails\n"
}

set NMCMD "nm64 -x" 

if [info exists env(CPU)] {
    if { $env(CPU) == "X86" } {
	set NMCMD "ssh sonata /stumm/d0/tornado/tools/sgi/bin/nm64 -x" 
    }
}


proc LockAcquireAnnotateExits {path exitsymbolpat tag} {
   global NMCMD
    set f [open [concat | $NMCMD [pwd]/$path | egrep FUNC.*\\\\\|e1_bs_tryacquire_lock__($exitsymbolpat)]]
    while { [gets $f tmp] >=0 } {
	set values [split $tmp |]
        set addr [lindex $values 1]
        set func [lindex $values 7]
        setAcquireExitFuncAnnotation $addr $func $tag 1
    }
    close $f
    set f [open [concat | $NMCMD [pwd]/$path | egrep FUNC.*\\\\\|e2_bs_tryacquire_lock__($exitsymbolpat)]]
    while { [gets $f tmp] >=0 } {
	set values [split $tmp |]
        set addr [lindex $values 1]
        set func [lindex $values 7]
        setAcquireExitFuncAnnotation $addr $func $tag 0
    }
    close $f
}

proc setAcquireExitFuncAnnotation {addr func tag gotit} {
    global LockExitList
    set LockExitList($addr) $func
    if { $gotit == 1 } {
	annotation set pc $addr -tag $tag {
	    set LockAcquireResult($CPU) 1
	    annotation exec Lock acquireEnd
	    #	log "**** DEBUG : Lock: $CPU $LockExitList($pc) lock=[lindex $LOCKADDRESS($CPU) 0] ra=$ra\n"
	    set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
	}
    } else {
	annotation set pc $addr -tag $tag {
	    set LockAcquireResult($CPU) 0
	    annotation exec Lock acquireEnd
	    #	log "**** DEBUG : Lock: $CPU $LockExitList($pc) lock=[lindex $LOCKADDRESS($CPU) 0] ra=$ra\n"
	    set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
	}
    }
    log "**** DEBUG: Lock: set annotation on bs_tryaquire_lock exit @  $addr ($func)\n"
}

proc LockReleaseAnnotateExits {path exitsymbolpat tag} {
    global NMCMD
    set f [open [concat | $NMCMD [pwd]/$path | egrep FUNC.*\\\\\|e._bs_release_lock__($exitsymbolpat)]]
    while { [gets $f tmp] >=0 } {
	set values [split $tmp |]
        set addr [lindex $values 1]
        set func [lindex $values 7]
        setReleaseExitFuncAnnotation $addr $func $tag
    }
    close $f
}

proc setReleaseExitFuncAnnotation {addr func tag} {
    global LockExitList
    set LockExitList($addr) $func
    annotation set pc $addr -tag $tag {
	annotation exec Lock releaseEnd
#	log "**** DEBUG: Lock: $CPU  $LockExitList($pc) lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
	set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
    }
    log "**** DEBUG: Lock: set annotation on bs_release_lock exit @  $addr ($func)\n"
}

annotation set pc kernel::bs_tryacquire_lock__FUlPUlT1:START -tag Lock {
    set LOCKADDRESS($CPU) [linsert $LOCKADDRESS($CPU) 0 $a1]
    annotation exec Lock acquireStart
#    log "**** DEBUG : Lock: $CPU bs_tryacquire_lock__FUlPUlT1:START lock=[lindex $LOCKADDRESS($CPU) 0] ra=$ra\n"
}


annotation set pc kernel::bs_tryacquire_lock__FUiPUiUl:START -tag Lock {
    set LOCKADDRESS($CPU) [linsert $LOCKADDRESS($CPU) 0 $a1]
    annotation exec Lock acquireStart
#    log "**** DEBUG : Lock: $CPU bs_tryacquire_lock__FUiPUiUl:START lock=[lindex $LOCKADDRESS($CPU) 0] ra=$ra\n"
}

LockAcquireAnnotateExits $KERNEL "FUiPUiUl|FUlPUlT1" Lock


annotation set pc kernel::bs_release_lock__FUlT1PUl:START -tag Lock {
    set LOCKADDRESS($CPU) [linsert $LOCKADDRESS($CPU) 0 $a2]
    annotation exec Lock releaseStart
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUlT1PUl:START lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
}

#annotation set pc kernel::bs_release_lock__FUlT1PUl:END -tag Lock {
#    annotation exec Lock releaseEnd
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUlT1PUl:END lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
#    set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
#}

annotation set pc kernel::bs_release_lock__FUlPUl:START -tag Lock {
    set LOCKADDRESS($CPU) [linsert $LOCKADDRESS($CPU) 0 $a1]
    annotation exec Lock releaseStart
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUlPUl:START lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
}

annotation set pc kernel::bs_release_lock__FUlPUl:END -tag Lock {
    annotation exec Lock releaseEnd
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUlPUl:END lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
    set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
}

annotation set pc kernel::bs_release_lock__FUiT1PUi:START -tag Lock {
    set LOCKADDRESS($CPU) [linsert $LOCKADDRESS($CPU) 0 $a2]
    annotation exec Lock releaseStart
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUiT1PUi:START lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
}

#annotation set pc kernel::bs_release_lock__FUiT1PUi:END -tag Lock {
#    annotation exec Lock releaseEnd
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUiT1PUi:END lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
#    set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
#}
 
annotation set pc kernel::bs_release_lock__FUiPUi:START -tag Lock {
    set LOCKADDRESS($CPU) [linsert $LOCKADDRESS($CPU) 0 $a1]
    annotation exec Lock releaseStart
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUiPUi:START lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
}

annotation set pc kernel::bs_release_lock__FUiPUi:END -tag Lock {
    annotation exec Lock releaseEnd
#    log "**** DEBUG: Lock: $CPU  bs_release_lock__FUiPUi:END lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
    set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
}

annotation set pc kernel::releaseLock__17SimpleSLockNDebugFv:START -tag Lock {
    set LOCKADDRESS($CPU) [linsert $LOCKADDRESS($CPU) 0 $a0]
    annotation exec Lock releaseStart
#    log "**** DEBUG: Lock: $CPU  SimpleSLockNDebug::releaseLock:START lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
}

annotation set pc kernel::releaseLock__17SimpleSLockNDebugFv:END -tag Lock {
    annotation exec Lock releaseEnd
#    log "**** DEBUG: Lock: $CPU  SimpleSLockNDebug::releaseLock:END lock=[lindex $LOCKADDRESS($CPU) 0] a1=$a1 ra=$ra\n"
    set LOCKADDRESS($CPU) [lreplace $LOCKADDRESS($CPU) 0 0]
}

LockReleaseAnnotateExits $KERNEL "FUiT1PUi|FUlT1PUl" Lock



# WARNING this does not handle the case of multiple lockers on the same
# CPU

annotation set Lock acquireStart {
    set currentlock [lindex $LOCKADDRESS($CPU) 0]
    set locklist [array names CYCLESLOCKACQS]
#    log "**** DEBUG: locklist: $locklist\n"

    if { [lsearch $locklist $currentlock]==-1 } {
#	log "**** DEBUG: Lock: Initializing counters for $currentlock\n"
	set CYCLESLOCKACQS($currentlock) 0
	set NUMLOCKACQS($currentlock) 0
	set NUMLOCKACQFAILS($currentlock) 0
	set CYCLESLOCKRELS($currentlock) 0
	set NUMLOCKRELS($currentlock) 0
	set NUMLOCKBLOCKS($currentlock) 0
	set CYCLESLOCKBLOCKS($currentlock) 0
    }
    set SCYCLESLOCKACQS("$CPU.$currentlock") $CYCLES
    set NUMLOCKACQS($currentlock) [expr $NUMLOCKACQS($currentlock) + 1]
#    log "**** DEBUG: Lock: $CPU acquireStart lock=$currentlock\n"
}

annotation set Lock acquireEnd {
    set currentlock [lindex $LOCKADDRESS($CPU) 0]
    set CYCLESLOCKACQS($currentlock) [expr $CYCLESLOCKACQS($currentlock) + ($CYCLES-$SCYCLESLOCKACQS("$CPU.$currentlock"))]
    if { $LockAcquireResult($CPU) == 0 } {
	set NUMLOCKACQFAILS($currentlock) [expr $NUMLOCKACQFAILS($currentlock) + 1]
    }
#    log "**** DEBUG: Lock: $CPU acquireEnd lock=$currentlock\n"
}

annotation set Lock releaseStart {
    set currentlock [lindex $LOCKADDRESS($CPU) 0]
    set SCYCLESLOCKRELS("$CPU.$currentlock") $CYCLES
    set NUMLOCKRELS($currentlock) [expr $NUMLOCKRELS($currentlock) + 1]
#    log "**** DEBUG: Lock: $CPU releaseStart lock=$currentlock\n"
}

annotation set Lock releaseEnd {
    set currentlock [lindex $LOCKADDRESS($CPU) 0]
    set CYCLESLOCKRELS($currentlock) [expr $CYCLESLOCKRELS($currentlock) + ($CYCLES-$SCYCLESLOCKRELS("$CPU.$currentlock"))]
#    log "**** DEBUG: Lock: $CPU releaseEnd  lock=$currentlock\n"
}

annotation set pc kernel::BlockPD__FPUlP12LockHashHashUiT3:START -tag Lock {
    annotation exec Lock blockStart    
}

#annotation set pc kernel::BlockPD__FPUlP12LockHashHashUiT3:END -tag Lock {
#    annotation exec Lock blockEnd
#    set blockCountEnd [expr $blockCountEnd + 1]
#    log "**** DEBUG: Lock: BlockPD(unsigned long*,LockHashHash*,unsigned int,unsigned int):END a0=$a0\n"
#}

annotation set pc kernel::BlockPD__FPUlP12LockHashListUiT3:START -tag Lock {
    annotation exec Lock blockStart
}

#annotation set pc kernel::BlockPD__FPUlP12LockHashListUiT3:END -tag Lock {
#    annotation exec Lock blockEnd
#    set blockCountEnd [expr $blockCountEnd + 1]
#    log "**** DEBUG: Lock: BlockPD(unsigned long*,LockHashList*,unsigned int,unsigned int):END a0=$a0\n"
#}

annotation set Lock blockStart {
    set currentlock $a0
    set blockid "$ra.$ACTIVEBADGE($CPU).$ACTIVEPDA($CPU)"
    set LockBlockList($blockid) [list $currentlock $CYCLES]
    set NUMLOCKBLOCKS($currentlock) [expr $NUMLOCKBLOCKS($currentlock) + 1]
    lappend LockBlockRtnList $blockid

#    log "**** DEBUG: Lock: blockStart: cpu=$CPU blockid=$blockid LockBlockRtnList=$LockBlockRtnList\n"

    if { [lsearch $LockRtnList $ra] == -1 } {
#	log "   **** DEBUG: Lock: blockStart: cpu=$CPU  setting annotation on $blockid\n"
	annotation set pc $ra -tag $blockid {
	    set myblockid "$pc.$ACTIVEBADGE($CPU).$ACTIVEPDA($CPU)"
	    set rtnindx [lsearch $LockBlockRtnList $myblockid]
#	    log "\n**** DEBUG: Lock: blockEnd: cpu=$CPU  @ $pc blockid=$myblockid\n" 
	    if { $rtnindx  != -1 } {
		set myblockinfo $LockBlockList($myblockid)
		set mylock [lindex $myblockinfo 0]
		set mybscyc [lindex $myblockinfo 1]
		set CYCLESLOCKBLOCKS($mylock) [expr $CYCLESLOCKBLOCKS($mylock) + ($CYCLES - $mybscyc)]
#		annotation exec Lock blockEnd
		set LockBlockRtnList [lreplace $LockBlockRtnList $rtnindx $rtnindx]
		annotation disable $myblockid $CPU
#		log "**** DEBUG: Lock: blockEnd: cpu=$CPU cycles=$CYCLES at $pc blockid=$myblockid currentlock=$mylock\n\n"
		set blockEndCount [expr $blockEndCount + 1]
	    }
	}
	lappend LockRtnList $ra
    }
    annotation enable $blockid $CPU

#    log "**** DEBUG: Lock: blockStart currentlock=$currentlock blockid=$blockid\n"
}

