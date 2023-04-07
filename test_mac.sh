TEST_DIR="./"
RESULTFILE=$TEST_DIR"result.txt"
EXEC="./lsls"

function compare_tty() {
	diff -u ${REAL_OUT_FILE} ${MINE_OUT_FILE}
}

function compare_file() {
	diff -u ${REAL_OUT_FILE} ${MINE_OUT_FILE} && diff -u ${REAL_ERR_FILE} ${MINE_ERR_FILE}
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
	cp /bin/ls ${EXEC}
	script -q ${REAL_OUT_FILE} ${EXEC} $P > /dev/null
	cp ./ft_ls ${EXEC}
	script -q ${MINE_OUT_FILE} ${EXEC} $P > /dev/null
	compare_tty
	print_result "tty: $P"
}

function run_file() {
	P=$1
	cp /bin/ls ${EXEC}
	(${EXEC} $P; echo $?) > ${REAL_OUT_FILE} 2> ${REAL_ERR_FILE}
	cp ./ft_ls ${EXEC}
	(${EXEC} $P; echo $?) > ${MINE_OUT_FILE} 2> ${MINE_ERR_FILE}
	compare_file
	print_result "file: $P"
}

rm -rf	$RESULTFILE

EXEC=`mktemp`
chmod a+x ${EXEC}
REAL_OUT_FILE=`mktemp`
MINE_OUT_FILE=`mktemp`
REAL_ERR_FILE=`mktemp`
MINE_ERR_FILE=`mktemp`

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

run_tty ""
run_tty "-R"
run_tty "-l ./srcs"
run_tty "-l ./includes"
run_tty "-lf ccc aaa bbb"
run_tty "-l ccc aaa bbb"

run_tty "test_field1/link_dir_1"
run_tty "-l test_field1/link_dir_1"

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

run_tty "-l@ test_field3"

run_file ""
run_file "-R"
run_file "-l ./srcs"
run_file "-l ./includes"
run_file "-lf ccc aaa bbb"
run_file "-l ccc aaa bbb"

run_file "test_field1/link_dir_1"
run_file "-l test_field1/link_dir_1"

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


run_tty "test_field"
run_tty "-d test_field"
run_tty "-l test_field"
run_tty "-R test_field"
run_tty "-a test_field"
run_tty "-lR test_field"
run_tty "-laR test_field"
run_tty "-laR --col test_field"
run_tty "-laRu test_field"
run_tty "-laRr test_field"
run_tty "-laRt test_field"
run_tty "-laRf test_field"


run_file "test_field"
run_file "-d test_field"
run_file "-l test_field"
run_file "-R test_field"
run_file "-a test_field"
run_file "-lR test_field"
run_file "-laR test_field"
run_file "-laR --col test_field"
run_file "-laRu test_field"
run_file "-laRr test_field"
run_file "-laRt test_field"
run_file "-laRf test_field"
