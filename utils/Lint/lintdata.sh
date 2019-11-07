#!/bin/bash
# A script to lint all .cpp/.hpp files in a directory for extra data and info,
#   with configured linter. It prints the number of 
#   passed/failed source/header files, the number of 
#   NOLINTS used, and the number of 
#   non-comment, non-empty lines scanned, 
#   as well as how many of them were in passed source/header files

# Note that this significantly slower than just calling 'python presubmit.py'
#   only use this if you need the extra data

# Call with --verbose to print all messages from linter
# Call with --dir={name} to lint files in ./{name}
# Call with --onlyerr to only print error messages when file fails linter

# If both --verbose and --onlyerr are called, the last option passed will be used



### configuration variables ###
lintres=./utils/Lint/lintresults.txt # file where information from linting is stored
lintops=./utils/Lint/lintoptions.txt # file where enabled lint rules are stored
floc=./src                           # directory where files to test lint are located
linter=./utils/Lint/cpplint.py       # linter to be used
optargs=--verbose=1                  # option arguments to be passed to linter
interp=python                        # interpreter to run linter in


### Constants Declaration ### 
kSourceFile=0 # token arg to pass with source files
kHeaderFile=1 # token arg to pass with header files


### Variable Declaration ###
countercpp=0         # number of source files to check
linescpp=0           # number of lines in source files linted, DOES NOT COUNT COMMENTED LINES
counterhpp=0         # number of header files to check
lineshpp=0           # number of lines in header files linted, DOES NOT COUNT COMMENTED LINES
countall=0           # number of files (not dir) in floc
countnolintcpp=0     # number of times the word 'NOLINT' appears in source files
countnolinthpp=0     # number of times the word 'NOLINT' appears in header files

totalerrcpp=0        # number of total errors in source files as reported by linter
totalerrhpp=0        # number of total errors in header files as reported by linter
passedcpp=0          # number of source files passed
passedlinescpp=0     # lines in passed source files, DOES NOT COUNT COMMENTED LINES
passedhpp=0          # number of header files passed
passedlineshpp=0     # lines in passed header files PASSED, DOES NOT COUNT COMMENTED LINES

checked=""           # string containing files checked
ruleson="--filter=-" # string containing all enabled lint rules, other rules disabled
verbosemode=1        # changes to 0 (True) if called with --verbose
onlyerrmode=1        # changes to 1 (True) if called with --onlyerr



# construct a string representing enabled lint options 
while read rule 
do 
    if [[ "${rule: 0:1}" == "#" ]]
    then 
        continue 
    else 
        ruleson="$ruleson,+$rule"
    fi 
done < $lintops


### Function Declarations ### 

#TODO(New-Anu., anyone good with awk) replace this with efficient 'awk' (maybe with some seds,etc) 
# takes three arguments
# first arugment:  count number of NON-COMMENT NON-EMPTY lines in the file passed ($1)
# second argument: specifies whether this is a source (0) or header file (1)
# third argument:  specifies whether this file passed linting (1) or not (0)
function countlines 
{
    incomment=1 # 0 if in /* ... */ comment, 1 otherwise
    while read line 
    do 
        trimmed="$(echo $line | sed -e 's/^[[:space:]]*//')"
        if [[ "$trimmed" == "//"* ]]
        then
            continue 
        elif [[ "$trimmed" == "/*"* ]]
        then 
            incomment=0
            continue
        elif [[ "$trimmed" == "*/"* ]]
        then 
            incomment=1
            continue
        elif [[ "$trimmed" == "" ]] # doesn't count empty lines
        then 
            continue
        fi 

        if [[ "$incomment" -eq 1 ]]
        then
            if [[ $2 -eq 0 ]]
            then 
                ((linescpp++))
                ((passedlinescpp+=$3)) # adds one if file passed linted ($3=1/=0)
            else 
                ((lineshpp++))
                ((passedlineshpp+=$3))
            fi
        fi 
    done < $1
}


# recursively count all files AND source/header files in all dirs
function countalldir 
{
    for fl in $1/*
    do 
        if [[ "${fl: -1}" == "*" ]]
        then  
            break
        elif [[ -d "$fl" ]]
        then
            countalldir $fl
        # ignore PROTOBUF files (automatically generated)
        elif [[ ${#fl} -ge 7 ]] && [[ ${fl: -7} == ".pb."*"pp" ]]
        then 
            echo "FOUND AND SKIPPED PROTOBUF FILE: $fl"
            continue
        elif [[ "${fl: -4}" == ".cpp" ]]
        then 
            ((countercpp++))
            ((countall++))
        elif [[ "${fl: -4}" == ".hpp" ]]
        then 
            ((counterhpp++))
            ((countall++))
        else 
            ((countall++))
        fi
    done 
} 


#TODO(New-Anu., anyone good with awk) replace this with efficient 'awk' (maybe with some seds,etc) 
# Takes two arguments, $1 - file being lint, $2 - 0 if this is a source file
# counts the number of times the word "NOLINT" appears in the source/header file
# second argument states whether this is a source file (0) or header file (1, or anything else) 
function countnolint 
{
    for word in $(cat $1) 
    do 
        # checks if the word contains the substr 'NOLINT'
        if [[ "$word" == *"NOLINT"* ]]
        then 
            if [[ $2 -eq 0 ]]
            then 
                ((countnolintcpp++))
            else
                ((countnolinthpp++))
            fi
        fi 
    done
}


# lint all files in $floc, and recursively lint all files in its subfiles
function lintalldir 
{
    for file in $1/*
    do 
        if [[ "${file: -1}" == '*' ]] # guard against empty
        then 
            break 
        elif [[ -d "$file" ]]
        then 
            lintalldir $file
        elif [[ "${file: -4}" == ".cpp" ]] || [[ "${file: -4}" == ".hpp" ]]
        then 
            # ignore PROTOBUF files (automatically generated)
            if [[ ${#file} -ge 7 ]] && [[ ${file: -7} == ".pb."*"pp" ]]
            then 
                echo "LINTER FOUND AND SKIPPED PROTOBUF FILE: $file"
                continue
            fi

            # extracts both exit code and 'total error' message 
            #   (prevents exit code being overwritten by further pipe ops)
            set -o pipefail

            # calls linter on the file with optional args specified
            # note that the exit code is 0 when the total error = 0
            errmsg=$($interp $linter $optargs $ruleson $file 2>&1)
            
            # Please dont add any code here --might mess up value of $?

            # checks if call to linter was successful (lintfailed = 1 if lint successful)
            ((lintfailed = ! $?))

            # last line: "Total Error: (a number)"
            errline=$(echo $errmsg | tail -n 1) 

            # unsets -o pipefail so other bits of code aren't messed up
            set +o pipefail 

            # extracts number of errors from final line of linter
            errfound=$(echo $errline | awk '{print $NF}')

            if [[ $verbosemode -eq 0 ]]
            then 
                printf "$errmsg\n\n" 
            elif [[ $onlyerrmode -eq 0 ]] && [[ $lintfailed -eq 0 ]]
            then 
                printf "$errmsg\n"
            fi 

            if [[ "${file: -4}" == ".cpp" ]]
            then
                # counts the number of times the word "NOLINT" appears
                countnolint $file kSourceFile

                # adds one if linter did NOT fail
                ((passedcpp += lintfailed))

                # adds error from file to total errors
                ((totalerrcpp += $errfound))

                # counts the number of non-commented lines
                countlines $file kSourceFile lintfailed
            else # then file must end with .hpp
                countnolint $file kHeaderFile
                ((passedhpp += lintfailed))
                ((totalerrhpp += $errfound))
                countlines $file kHeaderFile lintfailed
            fi 
        fi 
    done 
}



### start ###
for opt in $@
do
    if [[ $opt == '--verbose' ]] 
    then 
        verbosemode=0
        onlyerrmode=1
    elif [[ $opt == '--onlyerr' ]]
    then 
        onlyerrmode=0
        verbosemode=1
    elif [[ $opt == '--dir='* ]]
    then 
        floc="./${opt#--dir=}"
    fi
done


countalldir $floc 


echo "Found $countall in $floc dir"
echo "Checking $countercpp source files with linter..."
echo "Checking $counterhpp header files with linter..."
echo 
echo 

lintalldir $floc


echo 
echo 
echo "Found $countall files in directory to lint"
echo "Linted $(($linescpp + $lineshpp)) lines"

echo 
echo "Reporting statistics for linted source files:"
echo "Checked $countercpp source files with linter"
echo "DONE! Exactly $passedcpp source files have passed"
echo "Also: Exactly $((countercpp - passedcpp)) source files have failed"
echo "      The word 'NOLINT' was located $((countnolintcpp)) time(s) in passed source files!"
echo
echo "      There were $linescpp lines in source files in total" 
echo "      $passedlinescpp lines are in source files that passed"
echo "      $((linescpp - passedlinescpp)) lines are in source files that failed"
echo "      A total amount of $totalerrcpp errors were reported"

echo 
echo
echo "Reporting statistics for linted header files"
echo "Checked $counterhpp header files with linter"
echo "DONE! Exactly $passedhpp header files have passed"
echo "Also: Exactly $((counterhpp - passedhpp)) header files have failed"
echo "      The word 'NOLINT' was located $((countnolinthpp)) time(s) in passed header files!"
echo 
echo "      There were $lineshpp lines in header files in total" 
echo "      $passedlineshpp lines are in header files that passed"
echo "      $((lineshpp - passedlineshpp)) lines are in header files that failed"
echo "      A total amount of $totalerrhpp errors were reported"



# writes lint results info to $lintres
echo "Directory linted:        $floc"                                 > $lintres # overwrites
echo "Files found:             $countall"                            >> $lintres 
echo "Source files linted:     $countercpp"                          >> $lintres 
echo "Header files linted:     $counterhpp"                          >> $lintres

echo "Files passed:            $((passedcpp + passedhpp))"           >> $lintres
echo "Source files passed:     $passedcpp"                           >> $lintres
echo "Header files passed:     $passedhpp"                           >> $lintres

echo "Lines linted:            $((linescpp + lineshpp))"             >> $lintres
echo "Source lines linted:     $linescpp"                            >> $lintres
echo "Header lines linted:     $lineshpp"                            >> $lintres 

echo "Lines passed:            $((passedlinescpp + passedlineshpp))" >> $lintres
echo "Source lines passed:     $passedlinescpp"                      >> $lintres
echo "Header lines passed:     $passedlineshpp"                      >> $lintres

echo "Errors found:            $((totalerrcpp + totalerrhpp))"       >> $lintres
echo "Source files errors:     $totalerrcpp"                         >> $lintres
echo "Header files errors:     $totalerrhpp"                         >> $lintres

echo "NOLINTS found:           $((countnolintcpp + countnolinthpp))" >> $lintres
echo "NOLINTS in source files: $countnolintcpp"                      >> $lintres 
echo "NOLINTS in header files: $countnolinthpp"                      >> $lintres



