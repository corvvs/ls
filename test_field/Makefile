SUBDIRS		:=\
				exotic_names\
				exotic_types\
				massive_plain\


all:	$(SUBDIRS)

.PHONY:	clean
clean:
	$(RM) -r $(SUBDIRS)

# Linuxではクオートされるような文字を名前に持つファイル(そうでもないものもある)
exotic_names:
	mkdir $@ && cd $@ &&\
	touch {a..z} && \
	touch {0..9} && \
	touch "^" "_" " " "'" "!" "@" "#" "%" "&" "*" "(" ")" "-" "+" "=" "[" "]" "{" "}" "|" ";" ":" "," "." "<" ">" "?" "~" "\\" && \
	touch '"' "' '" "' , '" "'"'"' '"'"'""'"'"'

exotic_types:
	mkdir $@ && cd $@ &&\
	touch normal_file && \
	mkdir nomal_dir && \
	touch owner_exec && chmod u+x owner_exec &&\
	touch group_exec && chmod g+x group_exec \



# 大量のファイルを含む平坦なディレクトリ
massive_plain:
	mkdir -p $@
	perl make_massive_plain.pl

.PHONY:	re
re:	clean all
