build-fifo:
		copy Makefile Makefile.orig
		echo "cacheLab: FIFO"
		copy Makefile-cacheLab Makefile

build-lru:
		copy Makefile Makefile.orig
		echo "cacheEC: LRU"
		copy Makefile-cacheEC Makefile