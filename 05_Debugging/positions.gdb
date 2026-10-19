set pagination off
set confirm off
set $index = 0
break element
commands
  silent
  set $index = $index + 1
  if $index >= 28 && $index <= 35
    printf "@@@ start=%lld stop=%lld step=%lld current=%lld\n", start, stop, step, current
  end
  continue
end
run -100 100 3 > /dev/null
