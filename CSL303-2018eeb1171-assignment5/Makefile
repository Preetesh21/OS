COMPILER = gcc
FILESYSTEM_FILES = code.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -lcurl -o code `pkg-config fuse --cflags --libs`
	echo 'To Mount: ./code -f [mount point]'

clean:
	rm ssfs
