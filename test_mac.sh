TEST_DIR="./"
RESULTFILE=$TEST_DIR"result.txt"
REAL_OUT_FILE="test_real.out.txt"
MINE_OUT_FILE="test_mine.out.txt"
REAL_ERR_FILE="test_real.err.txt"
MINE_ERR_FILE="test_mine.err.txt"
EXEC="./lsls"

function compare_tty() {
	diff -u ${TEST_DIR}${REAL_OUT_FILE} ${TEST_DIR}${MINE_OUT_FILE}
}

function compare_file() {
	diff -u ${TEST_DIR}${REAL_OUT_FILE} ${TEST_DIR}${MINE_OUT_FILE} && diff -u ${TEST_DIR}${REAL_ERR_FILE} ${TEST_DIR}${MINE_ERR_FILE}
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

function run_tty() {
	P=$1
	rm -rf ${EXEC}
	ln -s /bin/ls ${EXEC}
	script -q ${TEST_DIR}${REAL_OUT_FILE} ${EXEC} $P > /dev/null
	rm -rf ${EXEC}
	ln -s ./ft_ls ${EXEC}
	script -q ${TEST_DIR}${MINE_OUT_FILE} ${EXEC} $P > /dev/null
	compare_tty
	print_result "$P"
}

function run_file() {
	P=$1
	rm -rf ${EXEC}
	ln -s /bin/ls ${EXEC}
	(${EXEC} $P; echo $?) > ${TEST_DIR}${REAL_OUT_FILE} 2> ${TEST_DIR}${REAL_ERR_FILE}
	rm -rf ${EXEC}
	ln -s ./ft_ls ${EXEC}
	(${EXEC} $P; echo $?) > ${TEST_DIR}${MINE_OUT_FILE} 2> ${TEST_DIR}${MINE_ERR_FILE}
	compare_file
	print_result "$P"
}

rm -rf	$RESULTFILE

run_tty "./"
run_tty "./srcs"
run_tty "./includes"
run_tty "./srcs ./includes"
run_tty ". ./srcs ./includes"
run_tty ". ."
run_tty ".. .. ."

run_tty "/usr"
run_tty "/usr/local"
run_tty "/usr/bin"
run_tty "/usr/bin /usr"
run_tty "/usr/bin /usr/bin"

run_tty "-l ./srcs"
run_tty "-l ./includes"
run_tty "-l ./srcs ./includes"

run_tty "srcs/"
run_tty "srcs/*"
run_tty "./includes/../srcs/"
run_tty "./includes/../srcs/*"

run_tty "-l srcs/"
run_tty "-l srcs/*"
run_tty "-l ./includes/../srcs/"
run_tty "-l ./includes/../srcs/*"

run_tty "-l ./includes/../srcs/*"

run_tty "Makefile"
run_tty "Makefile src/"

run_tty "libft libft"
run_tty "-l libft libft"
run_tty "-R libft libft"
run_tty "-lR libft libft"

run_tty "-R"
run_tty "."
run_tty "-R ."

run_tty "-G"
run_tty "-G srcs"
run_tty "-G includes"
run_tty "-G minimini"
run_tty "-G /usr"
run_tty "-G /usr/bin"

run_tty "-l /usr/bin"
run_tty "-lG pocket"
run_tty "-lG /usr/bin"

run_tty " /dev/"
run_tty "-G /dev/"

run_tty "-l@"
run_tty "-l@e"

run_tty "xxx x"
