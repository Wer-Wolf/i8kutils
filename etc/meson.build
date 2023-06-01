dep_systemd = dependency('systemd', required : get_option('systemd_support'))
dep_udev = dependency('udev', required : get_option('systemd_support'))

install_data('i8kmon.conf', install_dir : '/etc', install_mode : ['rw-r--r--', 'root', 'root'])

if dep_systemd.found() and dep_udev.found()
  # Calling dep.get_variable(varname) fails
  install_data('i8kmon.service', install_dir : dep_systemd.get_variable(pkgconfig : 'systemdsystemunitdir'), install_mode : ['rw-r--r--', 'root', 'root'])
  install_data('50-i8kmon.rules', install_dir : dep_udev.get_variable(pkgconfig : 'udevdir') + '/rules.d', install_mode : ['rw-r--r--', 'root', 'root'])
endif

if get_option('sysvinit_support').allowed()
  install_data('i8kmon', install_dir : '/etc/init.d', install_mode : ['rwxr-xr-x', 'root', 'root'])
endif
