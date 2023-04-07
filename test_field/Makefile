SUBDIRS		:=\
				exotic_names\
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

# 大量のファイルを含む平坦なディレクトリ
massive_plain:
	perl make_massive_plain.pl

