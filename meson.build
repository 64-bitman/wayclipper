project('stashman', 'c',
  version: '0.0.1',
  license: 'MIT',
  license_files: 'LICENSE',
  default_options: ['c_std=gnu11', 'warning_level=3'],
  meson_version: '>= 1.1.0'
  )

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language : 'c')
endif

deps = []
link_with = []
sources = []
includes = []

glib_dep = dependency('glib-2.0')
wl_dep = dependency('wayland-client', required: get_option('wayland'))
x11_dep = dependency('xcb', required: get_option('x11'))

deps += [glib_dep, wl_dep, x11_dep]

cc = meson.get_compiler('c')
cfg_data = configuration_data()

if wl_dep.found()
  cfg_data.set('FEAT_WAYLAND', true)
  subdir('src/wl_protocols')
endif

if x11_dep.found()
    cfg_data.set('FEAT_X11', true)
endif

configure_file(output: 'project_config.h', configuration: cfg_data)
includes += include_directories('.')

subdir('src')

executable('stashman',
  sources,
  include_directories: includes,
  dependencies: deps,
  link_with: link_with
  )
