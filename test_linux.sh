MINISHELL="./minishell"
TEST_DIR="./"
RESULTFILE=$TEST_DIR"result.txt"
REAL_FILE="real.txt"
MINE_FILE="mine.txt"

function compare_evidence() {
	diff -u <(head -n-1 ${TEST_DIR}${REAL_FILE} | tail -n+1) <(head -n-1 ${TEST_DIR}${MINE_FILE} | tail -n+1)
}

function print_result() {
	{ test_stat=$?; } 2>/dev/null
	# { set +x; } 2>/dev/null
	if [ $test_stat -eq 0 ]; then
		echo -e "\033[32m[ OK ]\033[m $1"
		echo [ OK ] "$(date -R)	$1" >> $RESULTFILE
	else
		echo -e "\033[31m[FAIL] $1\033[m"
		echo "[FAIL] "$(date -R)"	$1" >> $RESULTFILE
	fi
	# set -x
}

function run_case() {
	P=$1
	script -q -c "ls --color=none $P" ${TEST_DIR}${REAL_FILE} > /dev/null
	script -q -c "./ft_ls $P" ${TEST_DIR}${MINE_FILE} > /dev/null
	compare_evidence
	print_result "$P"
}

run_case "./"
run_case "./srcs"
run_case "./includes"
run_case "./srcs ./includes"
run_case ". ./srcs ./includes"
run_case ". ."
run_case ".. .. ."

run_case "/usr"
run_case "/usr/local"
run_case "/usr/bin"
run_case "/usr/bin /usr"
run_case "/usr/bin /usr/bin"

run_case "-l ./srcs"
run_case "-l ./includes"
run_case "-l ./srcs ./includes"

run_case "srcs/"
run_case "srcs/*"
run_case "./includes/../srcs/"
run_case "./includes/../srcs/*"

run_case "-l srcs/"
run_case "-l srcs/*"
run_case "-l ./includes/../srcs/"
run_case "-l ./includes/../srcs/*"
