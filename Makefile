MESON_FLAGS :=

ifeq ($(RELEASE), 1)
	MESON_FLAGS += --buildtype=release
else
	MESON_FLAGS += --buildtype=debug -Db_sanitize=undefined,address
endif

all: build/
	meson compile -C build/

build/:
	meson setup $(MESON_FLAGS) build/

reset:
	rm -rf build/

clean:
	meson compile -C build/ --clean

run:
	build/stashman --daemon

.PHONY: all reset clean run
