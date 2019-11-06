echo set print elements 0 for limitless printing
echo Defining v as print command for giac types\n
set print repeats 0
define v
print ($arg0).dbgprint()
end
echo set print thread-events off
set print thread-events off