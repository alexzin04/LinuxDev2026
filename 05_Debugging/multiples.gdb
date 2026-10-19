set pagination off
set confirm off
break element if current % 5 == 0
commands
  silent
  printf "@@@ start=%lld stop=%lld step=%lld current=%lld\n", start, stop, step, current
  continue
end
run 1 12 > /dev/null
