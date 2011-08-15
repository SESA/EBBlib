
function locked_unchecked_tupleCreateType
{
     local type=$1
     local typedir=$KHTUPLEDIR/$type

     if [[ -d $typedir ]]
     then
         echo "ERROR: $FUNCNAME: $typedir already exists" >&2
         return -1
     fi

     if  ! mkdir -p $typedir
     then
        echo "ERROR: $FUNCNAME: could not create typedir" >&2
        return -1
     fi

}

function tupleCreateType
{
   local type=$1
 
   if [[ -z $type ]]
   then
     echo "USAGE: $FUNCNAME <type>"  >&2
     return -1
   fi
   
   lock tuples
      locked_unchecked_tupleCreateType $type
   unlock tuples
   return 0
}

locked_unchecked_tupleTypeExists()
{
  local type=$1
  local typedir=$KHTUPLEDIR/$type
  if [[ -d $typedir ]]
  then
    return 0
  else
    return -1
  fi
}

function tupleTypeExists
{
  local type=$1
  local rc=0

  if [[ -z $type ]]
  then
    echo "USAGE: $FUNCNAME <type>"  >&2
    return -1
  fi

  lock tuples
      locked_unchecked_tupleTypeExists $type
      rc=$?
  unlock tuples

  return $rc
}

locked_unchecked_tupleCmd()
{
  local cmd=$1
  local type=$2
  local tuplepattern=$3
  local rc=0

  cd $KHTUPLEDIR/$type

  if eval $cmd $tuplepattern 2>/dev/null
  then 
    rc=0
  else
#    eval echo "ERROR: $FUNCNAME: $cmd $typedir/$tuplepattern: FAILED"  >&2
    rc=-1
  fi

  return $rc
}

function tupleCmd
{
  local cmd=$1
  local type=$2
  local tuplepattern=$3
  local rc=0

  if [[ -z $cmd || -z $type || -z $tuplepattern ]]
  then
    echo "USAGE: $FUNCNAME <cmd> <type> <tuplepattern>" >&2
    return -1
  fi

  type -a $cmd >/dev/null 2>&1

  if (( $? != 0 ))
  then
    echo "ERROR: $FUNCNAME: $cmd is not a executable command"  >&2
    return -1
  fi

  lock tuples 
     if locked_unchecked_tupleTypeExists $type
     then
       locked_unchecked_tupleCmd $cmd $type "$tuplepattern"
       rc=$?
     else 
       echo "ERROR: $FUNCNAME: Tuple type $type does not exist"
       rc=-1
      fi
  unlock tuples 
  return $rc
}
