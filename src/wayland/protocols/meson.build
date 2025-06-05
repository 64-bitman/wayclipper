wayland_scanner = find_program('wayland-scanner', native: true, required: true)

protocols = [
    'ext-data-control-v1',
    'virtual-keyboard-unstable-v1',
    'wlr-data-control-unstable-v1'
    ]

foreach name: protocols
    header = custom_target(name + ' client header',
        input: name + '.xml', output: name + '.h',
        command: [wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@']
        )
    source = custom_target(name + ' private code',
        input: name + '.xml', output: name + '.c',
        command: [wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@']
        )

    sources += header
    link_with += static_library(name, source, header, dependencies: wl_dep)
endforeach
