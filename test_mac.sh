MINISHELL="./minishell"
TEST_DIR="./"
RESULTFILE=$TEST_DIR"result.txt"
REAL_FILE="real.txt"
MINE_FILE="mine.txt"
EXEC="./lsls"

function compare_evidence() {
	diff -u ${TEST_DIR}${REAL_FILE} ${TEST_DIR}${MINE_FILE}
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
	script -q ${TEST_DIR}${REAL_FILE} ${EXEC} $P > /dev/null
	rm -rf ${EXEC}
	ln -s ./ft_ls ${EXEC}
	script -q ${TEST_DIR}${MINE_FILE} ${EXEC} $P > /dev/null
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

run_case "-G"
run_case "-G srcs"
run_case "-G includes"
run_case "-G minimini"
run_case "-G /usr"
run_case "-G /usr/bin"

run_case "-l /usr/bin"
run_case "-lG pocket"
run_case "-lG /usr/bin"

run_case " /dev/"
run_case "-G /dev/"

run_case "-l@"
run_case "-l@e"
