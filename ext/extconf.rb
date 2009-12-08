require 'mkmf'

extension_name = 'mr_prime'
dir_config(extension_name)
create_makefile(extension_name, '.')
