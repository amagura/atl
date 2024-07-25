# -- Path Variables -- #
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mydir := $(abspath $(lastword $(dir $(MAKEFILE_LIST))))
cwd := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
rootdir := $(shell ./here)

# -- File Variables -- #

SRC = tl/user_tools.tl tl/navigation-ng
OBJS = tl/user_tools.o tl/navigation-ng.o

C_SRC = $(wildcard $(rootdir)/aux/c/*.c)
C_OBJS = $(C_SRC:.c=.o)
C_BINS = $(C_OBJS:.o=)

BATS_SRCS = $(wildcard $(rootdir)/t/*.bat)
BATS_OBJS = $(BATS_SRCS:.bat=.bats)

TEST_SRCS = $(wildcard $(rootdir)/t/*.t)
TESTS = $(TEST_SRCS:.t=.bats)

BINS = $(wildcard apple*)$(wildcard grape*)

__STARTDIR := '/mnt/c/Users/sickh/code/utl'
M4 := m4 $(STARTDIR)/tl/macro.m4
BATS := $(BATS) -t
GPP := $(GPP) \
  -U '' '' '(' ',' ')' '(' ')' "\#" "\\" +c "//" "\n" +c "\*" "*/" +c "\\\n" "" \
  -M '!\w' '\n' ' ' ' ' '\n' '' '' \
  --include $(rootdir)/aux/macro.gpp \
  -D__0LF="\n"

# -- END OF VARIABLES --

all: $(OBJS)

tidy:
	cd $(mydir); \
	  test -z "$(wildcard $(rootdir)/aux/c/*.o)" || \
	  $(RM) $(C_BINS); \
	  test -z "$(BINS)" || \
	  $(RM) $(BINS); \
	  test -z "$(wildcard tmp/*.bats*)" || \
	  $(RM) $(wildcard tmp/*.bats*); \
	  test -z "$(TESTS)" || \
	  $(RM) $(TESTS);

clean: tidy

%.bats : %.t
	$(GPP) $< > $@

%.o : %.tl
	/usr/bin/bash <(echo "source $(rootdir)/user_tools; tl-apple")

expand-tests:
	$(foreach file,$(TEST_SRCS), printf "Compiling $(file)" 1>&2; $(GPP) $(file);)

expand-gpp:
	$(GPP) $(rootdir)/aux/macro.gpp

expand-vars:
	@printf  "rootdir \t->\t'$(rootdir)'\n"
	@printf  "mkfile_path \t->\t'$(mkfile_path)'\n"
	@printf  "mydir \t\t->\t'$(mydir)'\n"
	@printf  "cwd \t\t->\t'$(cwd)'\n"

test: tidy $(TESTS)
	$(foreach file,$(TESTS), printf '\n- $(file) -\n'; $(BATS) $(file);)

check: test
	@echo :: C Programs ::
	echo $(C_SRCS)
	echo $(C_OBJS)
	echo $(C_BINS)
