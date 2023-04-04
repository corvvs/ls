MINISHELL="./minishell"
TEST_DIR="./"
RESULTFILE=$TEST_DIR"result.txt"
REAL_FILE="real.txt"
MINE_FILE="mine.txt"
EXEC="./lsls"

function compare_evidence() {
	diff -u <(head -n-1 ${TEST_DIR}${REAL_FILE} | tail -n+2) <(head -n-1 ${TEST_DIR}${MINE_FILE} | tail -n+2)
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
	rm -rf ${EXEC}
	ln -s /bin/ls ${EXEC}
	script -q -c "${EXEC} $P" ${TEST_DIR}${REAL_FILE} > /dev/null
	rm -rf ${EXEC}
	ln -s ./ft_ls ${EXEC}
	script -q -c "${EXEC} $P" ${TEST_DIR}${MINE_FILE} > /dev/null
	compare_evidence
	print_result "$P"
}

rm -rf	$RESULTFILE

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

run_case "-l ./includes/../srcs/*"

run_case "Makefile"
run_case "Makefile src/"

run_case "libft libft"
run_case "-l libft libft"
run_case "-R libft libft"
run_case "-lR libft libft"

run_case "-R"
run_case "."
run_case "-R ."
run_case "-R test1"

run_case "--col"
run_case "--col srcs"
run_case "--col includes"
run_case "--col minimini"
run_case "--col /usr"
run_case "--col /usr/bin"

run_case "-l /usr/bin"
run_case "-l --col pocket"
run_case "-l --col /usr/bin"

run_case "/dev"
run_case "--col /dev"
run_case "-l --col /dev"
run_case "/var"
run_case "--col /var"
run_case "-l --col /var"
# run_case "-G /dev/"
# run_case "-lG /dev/"

run_case "xxx x"
