sources = files('i8kctl.c', 'i8k-generic.c', 'i8k-sysfs.c')

add_project_arguments('-DVERSION="@0@"'.format(meson.project_version()), language : 'c')

if meson.get_compiler('c').has_header('linux/i8k.h')
  add_project_arguments('-DHAVE_I8K_PROCFS', language : 'c')
  sources += files('i8k-procfs.c')
endif

executable('i8kctl', sources, include_directories : incdir, install : true)
