set TCPUS {}

if { $env(SIMOSINSTTRACE) == "ON" } {
    proc insttraceon s {
	global TCPUS CPU
	set index [lsearch $TCPUS $CPU]
	if { $index == -1 } {
	    lappend TCPUS $CPU
	    instDump $CPU
	    log "$s : TCPUS=$TCPUS\n"
	}
    }
    proc insttraceoff s {
	global TCPUS CPU
	set index [lsearch $TCPUS $CPU]
	if { $index != -1 } {
	    set TCPUS [lreplace $TCPUS $index $index]
	    instDump off
	    foreach cpu $TCPUS {
		instDump $cpu
	    }
	    log "$s : TCPUS=$TCPUS\n"
	}
    }
} else {
    proc insttraceon s {}
    proc insttraceoff s {}
}

