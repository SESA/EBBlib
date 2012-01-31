if { $env(SIMOSSCACHETRACE) == "ON" } {
    set SPDAS {}

    annotation set simos enter {
	annotation disable strace
    }
    
    annotation set scache instr -tag tst {
	set index [lsearch $SPDAS $ACTIVEPDA($ScacheCPU)]
	if { $index != -1 } { 
	    log "**** SCACHETRACE: $CYCLES ${ScacheCPU}: PC ${pc}: ScacheCPU=$ScacheCPU pda=$ACTIVEPDA($ScacheCPU)\n"
	}
    }
    proc scachetraceon {pda s} {
	global SPDAS 
	set index [lsearch $SPDAS $pda]
	if { $index == -1 } {
	    lappend SPDAS $pda
	    log "$s : scachetraceon\n"
	}
    }
    proc scachetraceoff {pda s} {
	global SPDAS
	set index [lsearch $SPDAS $pda]
	if { $index != -1 } {
	    set SPDAS [lreplace $SPDAS $index $index]
	    log "$s : scachetraceoff\n"
	}
    }
} else {
    proc scachetraceon {cpu s} {}
    proc scachetraceoff {cpu s} {}
}

