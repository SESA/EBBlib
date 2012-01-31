proc functrace {function tag}{
    
    
    set annotation pc $function:START -tag $tag {
         
    }

    set annotation simos exit {
	log "**** STATS: Number of times executed $function=\n"
        log "**** STATS: Cycles spent in $function:Cycles=\n"
        log "**** STATS: Instructions spent in $function:Inst
    }