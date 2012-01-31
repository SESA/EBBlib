annotation set pc kernel::worker__FPP4SSACP7requesti:START {
    annotation exec tstEvent startworker
}

annotation set pc kernel::worker__FPP4SSACP7requesti:END {
    annotation exec tstEvent endworker
}

annotation set pc kernel::driver__Fv:START {
    annotation exec tstEvent starttest
    annotation exec tstEvent startdriver
}

annotation set pc kernel::driver__Fv:END {
    annotation exec tstEvent enddriver
}

annotation set pc kernel::test__Fv:END {
    annotation exec tstEvent endtest
}

annotation set simos enter {
    annotation disable functrace
}

annotation set tstEvent startworker {
    annotation enable functrace
}

set NUMOFSLEEPS 0

annotation set pc kernel::sleep__16CacheEntrySimpleFv:START -tag functrace {
    set NUMOFSLEEPS [expr $NUMOFSLEEPS + 1]
}

set NUMOFWAKEUPS 0

annotation set pc kernel::wakeup__16CacheEntrySimpleFv:START -tag functrace {
    set NUMOFWAKEUPS [expr $NUMOFWAKEUPS + 1]
}

set NUMOFLRUENTRIES 0

annotation set pc kernel::lruentry__Q2_21SSACSimpleSharedArray10HashQueuesFRCi:START -tag functrace {
    set NUMOFLRUENTRIES [expr $NUMOFLRUENTRIES + 1]
}

set NUMOFDIRTYS 0

annotation set pc kernel::dirty__16CacheEntrySimpleFv:START -tag functrace {
    set NUMOFDIRTYS [expr $NUMOFDIRTYS + 1]
}

annotation set pc kernel::rollover__Q2_21SSACSimpleSharedArray10HashQueuesFv:START {
    log "**** ERROR:  SSACSimpleSharedArray::HashQueues::rollover called\n"
}


# GENERAL STATS FOR ALL SSACS

set NUMOFGETS 0
set SIMPLESHAREDARRAY 0
set SIMPLEREPLICATEDARRAY 0
set SIMPLEPARTITIONEDARRAY 0

annotation set pc kernel::get__21SSACSimpleSharedArrayFR13CacheObjectIdRP10CacheEntryRCQ2_4SSAC7gettype:START -tag functrace {
#    console "***** get a0=$a0 a1=$a1\n"
    set NUMOFGETS [expr $NUMOFGETS + 1]
    set SIMPLESHAREDARRAY 1
}

annotation set pc kernel::get__25SSACSimpleReplicatedArrayFR13CacheObjectIdRP10CacheEntryRCQ2_4SSAC7gettype:START -tag functrace {
#    console "***** get a0=$a0 a1=$a1\n"
    set NUMOFGETS [expr $NUMOFGETS + 1]
    set SIMPLEREPLICATEDARRAY 1
}

annotation set pc kernel::get__26SSACSimplePartitionedArrayFR13CacheObjectIdRP10CacheEntryRCQ2_4SSAC7gettype:START -tag functrace {
#    console "***** get a0=$a0 a1=$a1\n"
    set NUMOFGETS [expr $NUMOFGETS + 1]
    set SIMPLEPARTITIONEDARRAY 1
}

set NUMOFPUTBACKS 0

annotation set pc kernel::putback__21SSACSimpleSharedArrayFRP10CacheEntryRCQ2_4SSAC7putflag:START -tag functrace {
    set NUMOFPUTBACKS [expr $NUMOFPUTBACKS + 1]
}

annotation set pc kernel::putback__25SSACSimpleReplicatedArrayFRP10CacheEntryRCQ2_4SSAC7putflag:START -tag functrace {
    set NUMOFPUTBACKS [expr $NUMOFPUTBACKS + 1]
}

annotation set pc kernel::putback__26SSACSimplePartitionedArrayFRP10CacheEntryRCQ2_4SSAC7putflag:START -tag functrace {
    set NUMOFPUTBACKS [expr $NUMOFPUTBACKS + 1]
}

set NUMOFLOADS 0

annotation set pc kernel::load__19CacheObjectIdSimpleFv:START -tag functrace {
#    console "**** DEBUG: load a0=$a0\n"
    set NUMOFLOADS [expr $NUMOFLOADS + 1]
}

annotation set pc kernel::load__19CacheObjectIdSimpleFv:START -tag functrace {
#    console "**** DEBUG: load a0=$a0\n"
    set NUMOFLOADS [expr $NUMOFLOADS + 1]
}

annotation set pc kernel::load__19CacheObjectIdSimpleFv:START -tag functrace {
#    console "**** DEBUG: load a0=$a0\n"
    set NUMOFLOADS [expr $NUMOFLOADS + 1]
}

set NUMOFSAVES 0

annotation set pc kernel::save__19CacheObjectIdSimpleFPv:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFSAVES [expr $NUMOFSAVES + 1]
}

annotation set pc kernel::save__19CacheObjectIdSimpleFPv:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFSAVES [expr $NUMOFSAVES + 1]
}

annotation set pc kernel::save__19CacheObjectIdSimpleFPv:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFSAVES [expr $NUMOFSAVES + 1]
}

# STATS FOR SSAC SIMPLE REPLICATED ARRAY

set NUMOFFINDDIRLINES 0

annotation set pc kernel::finddirline__25SSACSimpleReplicatedArrayFR19CacheObjectIdSimple:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFFINDDIRLINES [expr $NUMOFFINDDIRLINES + 1]
}

set NUMOFDIRLINELOOKUPIDS 0

annotation set pc kernel::lookupid__Q2_25SSACSimpleReplicatedArray7DirLineFR19CacheObjectIdSimpleRCi:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFDIRLINELOOKUPIDS [expr $NUMOFDIRLINELOOKUPIDS + 1]
}

set NUMOFGETREMOTECOPIES 0

annotation set pc kernel::getremotecopyfromdir__25SSACSimpleReplicatedArrayFPQ2_25SSACSimpleReplicatedArray8DirEntry:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFGETREMOTECOPIES [expr $NUMOFGETREMOTECOPIES + 1]
}


set NUMOFADDDIRENTRIES 0

annotation set pc kernel::adddirentry__Q2_25SSACSimpleReplicatedArray7DirLineFR19CacheObjectIdSimpleRCiT2:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFADDDIRENTRIES [expr $NUMOFADDDIRENTRIES + 1]
}

set NUMOFSETBUSYONALLS 0

annotation set pc kernel::setbusyonallcopiesindir__25SSACSimpleReplicatedArrayFPQ2_25SSACSimpleReplicatedArray8DirEntryP16CacheEntrySimple:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFSETBUSYONALLS [expr $NUMOFSETBUSYONALLS + 1]
}


set NUMOFUPDATEANDCLEARSBUSYONALLS 0

annotation set pc kernel::updateandclearbusyonallcopiesindir__25SSACSimpleReplicatedArrayFPQ2_25SSACSimpleReplicatedArray8DirEntryP16CacheEntrySimple:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFUPDATEANDCLEARSBUSYONALLS [expr $NUMOFUPDATEANDCLEARSBUSYONALLS + 1]
}

set NUMOFREMOVEFROMDIRS 0

annotation set pc kernel::removefromdir__25SSACSimpleReplicatedArrayFR19CacheObjectIdSimpleRCii:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFREMOVEFROMDIRS [expr $NUMOFREMOVEFROMDIRS + 1]
}

set NUMOFDIRENTRYSPINS 0

annotation set pc kernel::spin__Q2_25SSACSimpleReplicatedArray8DirEntryFv:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFDIRENTRYSPINS [expr $NUMOFDIRENTRYSPINS + 1]
}



annotation set simos exit {
    if { $SIMPLESHAREDARRAY == 1 } { 
	log "**** STATS: Number of times executed SSACSimpleSharedArray::HashQueues:: lruentry()=$NUMOFLRUENTRIES\n"
	log "**** STATS: Number of times executed SSACSimpleSharedArray:: get()=$NUMOFGETS\n"
	log "**** STATS: Number of times executed SSACSimpleSharedArray:: putback()=$NUMOFPUTBACKS\n"
    }
    if { $SIMPLEREPLICATEDARRAY == 1 } { 
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray::HashQueues:: lruentry()=$NUMOFLRUENTRIES\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: get()=$NUMOFGETS\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: putback()=$NUMOFPUTBACKS\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: finddirline()=$NUMOFFINDDIRLINES\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: getremotecopyfromdir()=$NUMOFGETREMOTECOPIES\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: setbusyonallcopiesindir()=$NUMOFSETBUSYONALLS\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: updateandclearbusyonallcopiesindir()=$NUMOFUPDATEANDCLEARSBUSYONALLS\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: removefromdir()=$NUMOFREMOVEFROMDIRS\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirLine:: lookupid()=$NUMOFDIRLINELOOKUPIDS\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirLine:: adddirentry()=$NUMOFADDDIRENTRIES\n"
	log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirEntry:: spin()=$NUMOFDIRENTRYSPINS\n"
    }
    if { $SIMPLEPARTITIONEDARRAY == 1 } { 
	log "**** STATS: Number of times executed SSACSimplePartitionedArray::HashQueues:: lruentry()=$NUMOFLRUENTRIES\n"
	log "**** STATS: Number of times executed SSACSimplePartitionedArray:: get()=$NUMOFGETS\n"
	log "**** STATS: Number of times executed SSACSimplePartitionedArray:: putback()=$NUMOFPUTBACKS\n"
    }
    log "**** STATS: Number of times executed CacheEntrySimple:: sleep()=$NUMOFSLEEPS\n"
    log "**** STATS: Number of times executed CacheEntrySimple:: wakeup()=$NUMOFWAKEUPS\n"
    log "**** STATS: Number of times executed CacheEntrySimple:: dirty()=$NUMOFDIRTYS\n"
    log "**** STATS: Number of times executed CacheObjectIdSimple:: load=$NUMOFLOADS\n"
    log "**** STATS: Number of times executed CacheObjectIdSimple:: save=$NUMOFSAVES\n"
}




