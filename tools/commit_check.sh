#!/usr/bin/env bash

############# Tests ##############
command -v ./tests > /dev/null 2>&1
status=$?
if [[ ${status} -eq 0 ]];
then
    echo Tests available, running
    output=$(./tests 2>&1 >/dev/null)
    test_status=$?

    # Catch potential log messages
    if [[ ! -z ${output} ]]
    then
        test_status=1
    fi
else
    echo Tests not found, skipping
fi

############ Cppcheck ############
command -v cppcheck > /dev/null 2>&1
status=$?
if [[ ${status} -eq 0 ]];
then
    echo Cppcheck found, running
    cppcheck --error-exitcode=1 --enable=all --suppress=useStlAlgorithm --suppress=missingInclude --suppress=missingIncludeSystem --suppress=unusedFunction --suppress=redundantAssignment ../src >/dev/null
    cppcheck_status=$?
else
    echo Cppcheck not found, skipping
fi


############ Summary #############

echo
echo "##### Summary #####"

if [[ -z ${test_status} ]];
then
    echo     "Tests:    SKIPPED"
else
    if [[ ${test_status} -eq 0 ]];
    then
        echo "Tests:    success"
    else
        # TODO add more verbose output
        echo "Tests:    FAILURE"
    fi
fi

if [[ -z ${cppcheck_status} ]];
then
    echo     "Cppcheck: SKIPPED"
else
    if [[ ${cppcheck_status} -eq 0 ]];
    then
        echo "Cppcheck: success"
    else
        # TODO add more verbose output
        echo "Cppcheck: FAILURE"
    fi
fi

