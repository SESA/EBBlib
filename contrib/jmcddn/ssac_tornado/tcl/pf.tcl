annotation type pageFault enum {
   startPF    endPF
}

set PFTYPE("NOCPU") "NOPF"

set PFTRACEIDS {}
set NUMTLBREFILLS("NOBADGE:NOPDA") 0
set NUMPFS("NOBADGE:NOPDA") 0
#set NUMHANDLETLBEXCEPTS("NOBADGE:NOPDA") 0

annotation set simos enter {
    annotation disable pf
    annotation disable epf
}

proc PFtraceon {} {
    global CPU ACTIVEBADGE ACTIVEPDA PFTRACEIDS NUMTLBREFILLS
    global NUMPFS CYCLESPFS INSTSPFS 
#   global NUMHANDLETLBEXCEPTS
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    lappend PFTRACEIDS $id
    set  NUMTLBREFILLS($id) 0
    set  NUMPFS($id) 0 
    set  CYCLESPFS($id) 0
    set  SCYCLESPFS($id) 0
    set  INSTSPFS($id) 0 
    set  SINSTSPFS($id) 0
#    set  NUMHANDLETLBEXCEPTS($id) 0
    if { [llength $PFTRACEIDS] == 1 }  {
	annotation enable pf
    }
    log "**** DEBUG: PF: Start page fault tracing for BADGE:PDA = $id\n"
}

proc PFtraceoff {} {
    global CPU ACTIVEBADGE ACTIVEPDA PFTRACEIDS NUMTLBREFILLS 
    global NUMPFS CYCLESPFS INSTSPFS 
#   global NUMHANDLETLBEXCEPTS
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    set doneindex [lsearch $PFTRACEIDS $id]
    set PFTRACEIDS [lreplace $PFTRACEIDS $doneindex $doneindex]
    set avgcycles [expr  $CYCLESPFS($id) / $NUMPFS($id) ]
    set avginsts  [expr  $INSTSPFS($id) / $NUMPFS($id) ]

    if { [llength $PFTRACEIDS] == 0 } {
	annotation disable pf
    }

    log "**** DEBUG: PF: End page fault tracing for BADGE:PDA = $id\n"   
    log "**** STATS: PF: $id : Number of page faults=$NUMPFS($id)\n"
    log "**** STATS: PF: $id : total cycles in page faults=$CYCLESPFS($id)\n"
    log "**** STATS: PF: $id : total instructions in page faults=$INSTSPFS($id)\n"
    log "**** STATS: PF: $id : avg cycles in page faults=$avgcycles\n"
    log "**** STATS: PF: $id : avg instructions in page faults=$avginsts\n"
    log "**** STATS: PF: $id : Number of tlb misses=$NUMTLBREFILLS($id)\n"
#    log "**** STATS: PF $id : Number of times executed handle_tlbexcept=$NUMHANDLETLBEXCEPTS($id)\n"
}


#annotation set pc kernel::handle_tlbexcept:START -tag pf {
#    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
#    if { $foundid != -1 } {
#	set NUMHANDLETLBEXCEPTS($id) [expr $NUMHANDLETLBEXCEPTS($id) + 1]
#	log "**** PF handle_tlbexcept invocation: cpu=$CPU epc=$epc BadVaddr=$bad cycles=$CYCLES insts=$INSTS\n"
#    }
#}

# Annotation to catch TLB refill misses
annotation set utlb -tag pf {
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    set foundid [lsearch $PFTRACEIDS $id]
    if { $foundid != -1 } {
	set NUMTLBREFILLS($id) [expr $NUMTLBREFILLS($id) + 1]
#	log "**** PF TLBMiss: cpu=$CPU epc=$epc BadVaddr=$bad cycles=$CYCLES insts=$INSTS\n"
    }
#    log "**** DEBUG: PF: utlb: cpu=$CPU epc=$epc BadVaddr=$bad cycles=$CYCLES insts=$INSTS\n"
}

# Annotations to catch TLB exceptions

# Write to page indicated by the TLB to be READONLY
annotation set exc mod -tag pf {
#    log "**** DEBUG: PF: mod cpu=$CPU epc=$epc BadVaddr=$bad cycles=$CYCLES insts=$INSTS\n"
    set PFTYPE($CPU) "MODIFYFAULT"
    set CURPFID($CPU) "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    annotation exec pageFault startPF
    annotation enable epf $CPU
}

# Read from a page with an InValid TLB entry
annotation set exc rmiss -tag pf {
#    log "**** DEBUG: PF: rmiss cpu=$CPU epc=$epc BadVaddr=$bad cycles=$CYCLES insts=$INSTS\n"
    set PFTYPE($CPU) "READFAULT"
    set CURPFID($CPU) "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    annotation exec pageFault startPF
    annotation enable epf $CPU
}

# Write to a page with an InValid TLB entry
annotation set exc wmiss -tag pf {
#    log "**** DEBUG: PF: wmiss cpu=$CPU epc=$epc BadVaddr=$bad cycles=$CYCLES insts=$INSTS\n"
    set PFTYPE($CPU) "WRITEFAULT"
    set CURPFID($CPU) "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    annotation exec pageFault startPF
    annotation enable epf $CPU
}

annotation set osEvent endExceptionLevel -tag epf {
#   log "**** DEBUG epf on rfe fired\n"
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    if { $id == $CURPFID($CPU) } {
	annotation exec pageFault endPF
	annotation disable epf $CPU
    }
}

annotation set pageFault startPF {
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    set foundid [lsearch $PFTRACEIDS $id]
    if { $foundid != -1 } {
	set NUMPFS($id) [expr $NUMPFS($id) + 1]
	set SCYCLESPFS($id) $CYCLES
	set SINSTSPFS($id) $INSTS
#	log "**** PF PageFault START: $PFTYPE($CPU) cpu=$CPU id=$id epc=$epc BadVaddr=$bad scycles=$CYCLES sinsts=$INSTS\n"
    }
}

annotation set pageFault endPF {
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
   set foundid [lsearch $PFTRACEIDS $id]
    if { $foundid != -1 } {
	set tcycles [expr $CYCLES - $SCYCLESPFS($id)]
	set tinsts  [expr $INSTS - $SINSTSPFS($id)]
	set CYCLESPFS($id) [expr $CYCLESPFS($id) + $tcycles]
	set INSTSPFS($id)  [expr $INSTSPFS($id) + $tinsts]
    }
#    log "**** PF PageFault END: $PFTYPE($CPU) cpu=$CPU id=$id epc=$epc BadVaddr=$bad cycles=$tcycles insts=$tinsts\n"
}

