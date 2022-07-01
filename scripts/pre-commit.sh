PATH_TO_SCRIPTS="./scripts"

# Expects one argument, name of the script (without extension)
run_check() {
    echo "Running ${1}"
    "./${PATH_TO_SCRIPTS}/${1}.sh"

    if [[ $? -ne 0 ]]; then
        echo "Failed..."
        exit 1
    fi
    echo "${1} Ok"
}


run_check "check_format"
