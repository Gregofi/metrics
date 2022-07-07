RED="\033[0;31m"
ENDCOLOR="\033[0m"

if [ "$#" -ne 1 ]; then
    echo "usage: run_tests.sh metrics - Where 'metrics' is the program executable"
    exit 1
fi

# For now, only folders named by two decimals are considered tests.
for file in `ls | egrep "^[0-9]{2}$"`; do
    python3 test.py ${1} ${file}
    if [ $? -ne 0 ]; then
        echo "${RED}${file} failed${ENDCOLOR}";
    fi
done
