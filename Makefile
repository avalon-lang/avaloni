#
# makefile for the avalon compiler
#
# :author   Ntwali Bashige
# :year     2017
# :email    ntwali.bashige@gmail.com
#

cc          := g++
cflags      := -std=c++11 -g -Wall -pedantic -DDEBUG -fopenmp
ldpaths     := -L/usr/local/lib
rdpaths     := -Wl,-rpath=/usr/local/lib
ldflags     := -lboost_filesystem -lboost_system -fopenmp
src_dir     := src
inc         := -Isrc -Ideps/qpp
sysinc      := -isystem deps/boost -isystem deps/eigen
build_dir   := build
bin_dir     := bin
target      := $(bin_dir)/avaloniq

src_ext     := cpp
sources     := $(shell find $(src_dir) -type f -name *.$(src_ext))
objects     := $(patsubst $(src_dir)/%,$(build_dir)/%,$(sources:.$(src_ext)=.o))

sdk_path    := /usr/lib/avalon-sdk


.PHONY: all
all: setup $(target)

$(target): $(objects)
	$(cc) $^ -o $(target) $(ldpaths) $(ldflags) $(rdpaths)

$(build_dir)/%.o: $(src_dir)/%.$(src_ext)
	@mkdir -p $(dir $@)
	$(cc) $(cflags) $(sysinc) $(inc) -c -o $@ $<

install:
	@echo " Installing..."
	@# Copy the binary for system-wide access into </usr/bin>
	@cp $(target) /usr/bin
	@# Copy the SDK into the default AVALON_HOME folder at </usr/lib/avalon-sdk>
	@mkdir -p $(sdk_path)
	@cp -r sdk/* $(sdk_path)
	@echo " Installation finished."

.PHONY: setup
setup:
	@mkdir -p $(bin_dir)
	@mkdir -p $(build_dir)

.PHONY: clean
clean:
	@find . -type d \( -path ./docs/venv -o -path ./experiments/quil \) -prune -o -exec touch {} \;
	@rm -rf $(build_dir) $(bin_dir)
