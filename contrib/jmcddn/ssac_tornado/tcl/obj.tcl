set OBJmethodlist("NOADDRESS") "NOMETHOD"
set OBJrtnaddresslist {}
set OBJActivemethods("NOCPU") {}


proc OBJtraceobjectson args {
    if { [llength $args] == 2 } {
	set tag [lindex $args 0]
	set cpu [lindex $args 1]
	annotation enable $tag $cpu
    } else {
	set tag [lindex $args 0]
	annotation enable $tag
    }
}

proc OBJtraceobjectsoff args {
    global OBJCycles
    global OBJNum
    global OBJmethodlist
    if { [llength $args] == 2 } {
	set tag [lindex $args 0]
	set cpu [lindex $args 1]
	annotation disable $tag $cpu
    } else {
	set tag [lindex $args 0]
	annotation disable $tag
    }
    foreach s [array names OBJCycles] {
	set tmp [split $s .]
	set instance [lindex $tmp 0]
	set addr  [lindex $tmp 1]
	set methodname $OBJmethodlist($addr)
	log "**** STATS: OBJ: $instance $methodname number of invocations=$OBJNum($s)\n"
	log "**** STATS: OBJ: $instance $methodname cycles=$OBJCycles($s)\n"
    }
}

set NMCMD "nm64 -x" 

if [info exists env(CPU)] {
    if { $env(CPU) == "X86" } {
	set NMCMD "ssh sonata /stumm/d0/tornado/tools/sgi/bin/nm64 -x" 
    }
}

proc OBJtraceclass {path object tag} {
    global NMCMD
#    log "symbolefile=$symbolfile object=$object\n"
    set f [open [concat | $NMCMD -C [pwd]/$path | egrep FUNC.*\\\\\|($object)::]]
    set p [pid $f]
    while { [gets $f tmp] >=0 } {
	set values [split $tmp |]
        set addr [lindex $values 1]
#        set len [lindex $values 2]
        set method [lindex $values 7]
#        set end [expr $addr + ($len - 0x4)]
#        log "**** $method START:$addr $len $end\n"
#        setMethodannotation $addr $end $method $tag
        setMethodannotation $addr $method $tag
    }
    close $f
}

#proc setMethodannotation {addr end name tag} {
proc setMethodannotation {addr name tag} {
    global OBJmethodlist OBJrtnaddresslist OBJActivemethods
    set OBJmethodlist($addr) $name
#    set OBJmethodlist($end) $name

    log "**** DEBUG: OBJ: Setting annotations for $name\n"

    annotation set pc $addr -tag $tag {
	set this $a0
	set name $OBJmethodlist($pc)
	set objmethodindex "$this.$pc"
	set objmethodlist [array names OBJCycles]
	#log "**** DEBUG: OBJ: objmethodlist: $objmethodlist\n"

	if { [lsearch $objmethodlist $objmethodindex]==-1 } {
	    #log "**** DEBUG: OBJ: Initializing counters for $objmethodindex\n"
	    set OBJCycles($objmethodindex) 0
	    set OBJNum($objmethodindex) 0
	} 

	set OBJActivemethods($CPU) [linsert $OBJActivemethods($CPU) 0 [list $pc $this $CYCLES]]
	set etag "$pc.$ra"

        if { [lsearch $OBJrtnaddresslist $etag] == -1 } {
	    annotation set pc $ra -tag $etag {
		set tmp [lindex $OBJActivemethods($CPU) 0]
		set method [lindex $tmp 0]
		set this   [lindex $tmp 1]
		set smcyc  [lindex $tmp 2]
                set mytag "$method.$pc"
		set myobjindex "$this.$method"
		set OBJCycles($myobjindex) [expr $OBJCycles($myobjindex) + ($CYCLES - $smcyc)]
		set OBJNum($myobjindex) [expr $OBJNum($myobjindex) + 1]
#		log "**** DEBUG: OBJ: $CPU EXITED $OBJmethodlist($method) (this=$this) at return address of $pc\n"
                
	        set OBJActivemethods($CPU) [lreplace $OBJActivemethods($CPU) 0 0]
		annotation disable $mytag $CPU
	    }
#	    log "**** DEBUG: OBJ: $CPU Setting Exit annotation on return address $ra for $name\n"
	    lappend OBJrtnaddresslist $etag
	} 
	annotation enable $etag $CPU
#	log "**** DEBUG: OBJ: $CPU ENTERED $name @ $pc this=$this Active Methods: $OBJActivemethods($CPU) etag=$etag\n"
#	log "**** DEBUG: OBJ: $CPU ENTERED $name @ $pc this=$this etag=$etag\n"
#	log "**** DEBUG: OBJ: $CPU ENTERED $name @ $pc this=$this rtn=$ra\n"
    }

 
    annotation disable $tag
}

annotation set simos enter {
    set OBJActivemethods($CPU) {}
}