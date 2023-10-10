wrapper = find_program('all.tcl')

env = environment()

env.append('TCL8_6_TM_PATH', meson.global_source_root() / 'modules')

test('tcltest', wrapper, env : env, is_parallel : false, protocol : 'exitcode', verbose : true)
