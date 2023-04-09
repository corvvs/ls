# `ft_ls`

`ls`コマンドの機能限定クローン. macOS / Linux 対応.

## Usage

`make`した後、 `./ft_ls` を実行する。

### macOS

```
$ ./ft_ls -l srcs
total 192
-rw-r--r--  1 corvvs  staff      0  4  5 13:49 a
-rw-r--r--  1 corvvs  staff   4232  4  7 02:58 acl.c
-rw-r--r--  1 corvvs  staff   3209  4  7 21:03 arguments.c
-rw-r--r--  1 corvvs  staff    470  4  5 13:22 assert.c
d-w-------  2 corvvs  staff     64  4  5 13:49 b
-rw-r--r--  1 corvvs  staff   1199  4  5 00:33 basic_utils.c
-rw-r--r--  1 corvvs  staff      0  4  5 13:49 c
-rw-r--r--  1 corvvs  staff   1655  3 28 11:32 cache.c
-rw-r--r--  1 corvvs  staff    118  3 29 12:41 debug.c
-rw-r--r--  1 corvvs  staff   9223  4  9 18:39 ls.c
-rw-r--r--  1 corvvs  staff   2796  4  7 03:55 ls_utils.c
-rw-r--r--  1 corvvs  staff   1109  4  7 01:14 main.c
-rw-r--r--  1 corvvs  staff   3772  4  9 18:39 out_dirs.c
-rw-r--r--  1 corvvs  staff   1541  4  7 14:48 out_files.c
-rw-r--r--  1 corvvs  staff   3970  4  7 14:40 print_column_linux.c
-rw-r--r--  1 corvvs  staff   2974  4  7 02:58 print_column_macos.c
-rw-r--r--  1 corvvs  staff  11655  4  9 17:22 print_long_format.c
-rw-r--r--  1 corvvs  staff   3607  4  9 18:35 print_utils.c
-rw-r--r--  1 corvvs  staff   6873  4  8 01:27 printf.c
-rw-r--r--  1 corvvs  staff   2076  4  7 14:38 time.c
-rw-r--r--  1 corvvs  staff   1462  4  7 13:01 xattr.c
$ 
```

### Linux

```
# ./ft_ls -l srcs
total 96
-rw-r--r-- 1 root root     0 Apr  5 04:49 a
-rw-r--r-- 1 root root  4232 Apr  6 17:58 acl.c
-rw-r--r-- 1 root root  3209 Apr  7 12:03 arguments.c
-rw-r--r-- 1 root root   470 Apr  5 04:22 assert.c
d-w------- 2 root root    64 Apr  5 04:49 b
-rw-r--r-- 1 root root  1199 Apr  4 15:33 basic_utils.c
-rw-r--r-- 1 root root     0 Apr  5 04:49 c
-rw-r--r-- 1 root root  1655 Mar 28 02:32 cache.c
-rw-r--r-- 1 root root   118 Mar 29 03:41 debug.c
-rw-r--r-- 1 root root  9223 Apr  9 09:39 ls.c
-rw-r--r-- 1 root root  2796 Apr  6 18:55 ls_utils.c
-rw-r--r-- 1 root root  1109 Apr  6 16:14 main.c
-rw-r--r-- 1 root root  3772 Apr  9 09:39 out_dirs.c
-rw-r--r-- 1 root root  1541 Apr  7 05:48 out_files.c
-rw-r--r-- 1 root root  3970 Apr  7 05:40 print_column_linux.c
-rw-r--r-- 1 root root  2974 Apr  6 17:58 print_column_macos.c
-rw-r--r-- 1 root root 11655 Apr  9 08:22 print_long_format.c
-rw-r--r-- 1 root root  3607 Apr  9 09:35 print_utils.c
-rw-r--r-- 1 root root  6873 Apr  7 16:27 printf.c
-rw-r--r-- 1 root root  2076 Apr  7 05:38 time.c
-rw-r--r-- 1 root root  1462 Apr  7 04:01 xattr.c
# 
```

## Options

- `-l`
  - ロングフォーマットで表示する。
- `-R`
  - サブディレクトリを再帰的に表示する。
- `-t`
  - 同一ディレクトリ内のファイルを、**最終修正時刻**の降順にソートして表示する。
- `-r`
  - ソートが有効な場合、ソートを逆順に行う。
    - 「ソートが有効な場合」なので、`-f`オプションには効力を持たない。
- `-a`
  - `.`で始まるファイルを表示する。
    - コマンドラインから明示的に与える場合は`-a`がなくても表示される。
- `-u`
  - **時刻によるソートが有効な場合**、最終修正時刻のかわりに**最終アクセス時刻**でソートする。
- `-f`
  - **ソートを行わない。** このオプションは`-t`を無効化する。
- `-g`
  - ロングフォーマット時、ユーザ名を表示しない。
- `-d`
  - ディレクトリの展開を行わない。
- `-G`
  - 色付けを行う。`--color=auto`と同等。
- `--color`
  - `--color=none`
    - 色付けを行わない。
  - `--color=auto`
    - 出力先が端末の場合、色付けを行う。
  - `--color=always`
  - `--color`
    - 出力先を問わずに色付けを行う。
- `-@`
  - ロングフォーマットにおいて、ファイルに拡張属性が指定されている場合はそれを表示する。
- `-e`
  - ロングフォーマットにおいて、ファイルにACLが指定されている場合はそれを表示する。
  - macOSのみサポート。



