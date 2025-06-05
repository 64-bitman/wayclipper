project('clippor', 'c',
  version: 'v0.0.1',
  license: 'MIT',
  license_files: 'LICENSE',
  default_options: ['c_std=gnu11', 'warning_level=3'],
  meson_version: '>= 1.1.0'
  )

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG',
    '-DG_LOG_USE_STRUCTURED',
    '-DVERSION="' + meson.project_version() + '"',
    language : 'c')
endif

deps = []
link_with = []
sources = []
includes = []

glib_dep = dependency('glib-2.0')
gio_dep = dependency('gio-2.0')
wl_dep = dependency('wayland-client', required: get_option('wayland'))
x11_dep = dependency('xcb', required: get_option('x11'))

deps += [glib_dep, gio_dep, wl_dep, x11_dep]

cc = meson.get_compiler('c')
cfg_data = configuration_data()

if wl_dep.found()
  cfg_data.set('FEAT_WAYLAND', true)
endif

if x11_dep.found()
    cfg_data.set('FEAT_X11', true)
endif

# gnome_mod = import('gnome')

# enum_files = gnome_mod.mkenums_simple('enum-types', sources: 'src/include/clippor-enums.h')
# sources += enum_files

configure_file(output: 'project_config.h', configuration: cfg_data)
includes += include_directories('.')

subdir('schemas')
subdir('src')

executable('clippor',
  sources,
  include_directories: includes,
  dependencies: deps,
  link_with: link_with
  )
