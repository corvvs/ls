TEST_DIR="./"
RESULTFILE=$TEST_DIR"result.txt"
REAL_OUT_FILE="test_real.out.txt"
MINE_OUT_FILE="test_mine.out.txt"
REAL_ERR_FILE="test_real.err.txt"
MINE_ERR_FILE="test_mine.err.txt"
EXEC="./lsls"

function compare_tty() {
	diff -u <(head -n-1 ${TEST_DIR}${REAL_OUT_FILE} | tail -n+2) <(head -n-1 ${TEST_DIR}${MINE_OUT_FILE} | tail -n+2)
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
	script -q -c "${EXEC} $P; echo \$?" ${TEST_DIR}${REAL_OUT_FILE} > /dev/null
	rm -rf ${EXEC}
	ln -s ./ft_ls ${EXEC}
	script -q -c "${EXEC} $P; echo \$?" ${TEST_DIR}${MINE_OUT_FILE} > /dev/null
	compare_tty
	print_result "tty: $P"
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
	print_result "file: $P"
}

rm -rf	$RESULTFILE

# argv が存在する場合は, それだけでテストする
if [ $# -eq 1 ]; then
	run_tty "$1"
	exit 0
fi

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
run_tty "-R test1"

run_tty "--col"
run_tty "--col srcs"
run_tty "--col includes"
run_tty "--col minimini"
run_tty "--col /usr"
run_tty "--col /usr/bin"

run_tty "-l /usr/bin"
run_tty "-l --col pocket"
run_tty "-l --col /usr/bin"

run_tty "/dev"
run_tty "--col /dev"
run_tty "-l --col /dev"
run_tty "/var"
run_tty "--col /var"
run_tty "-l --col /var"

run_tty "xxx x"

run_tty ""
run_tty "-R"
run_tty "-l ./srcs"
run_tty "-l ./includes"
run_tty "-lf ccc aaa bbb"
run_tty "-l ccc aaa bbb"

run_tty "test_field/link_dir_1"
run_tty "-l test_field/link_dir_1"

run_tty "test_field3"
run_tty "-l test_field3"
run_tty "-R test_field3"
run_tty "-lR test_field3"
run_tty "test_field3/*"
run_tty "-l test_field3/*"
run_tty "-R test_field3/*"
run_tty "-lR test_field3/*"

run_tty "-g srcs includes"
run_tty "-gl srcs includes"

run_tty "-d"
run_tty "-dR"
run_tty "-d .."
run_tty "-dR .."
run_tty "-d test_field*"
run_tty "-Rd test_field*"
run_tty "-lRd test_field*"

run_tty "-a .."
run_tty "-a ."
run_tty "-a"
run_tty "-aR pocket"
run_tty "-aR pocket/dir"
run_tty "-aR pocket/dir/."
run_tty "-aR pocket/dir/.."

run_file ""
run_file "-R"
run_file "-l ./srcs"
run_file "-l ./includes"
run_file "-lf ccc aaa bbb"
run_file "-l ccc aaa bbb"

run_file "test_field/link_dir_1"
run_file "-l test_field/link_dir_1"

run_file "test_field3"
run_file "-l test_field3"
run_file "-R test_field3"
run_file "-lR test_field3"
run_file "test_field3/*"
run_file "-l test_field3/*"
run_file "-R test_field3/*"
run_file "-lR test_field3/*"

run_file "-g srcs includes"
run_file "-gl srcs includes"
