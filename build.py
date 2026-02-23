
import sbs

exec = sbs.Executable('test')\
	.add_src('src/**/*.c')

exec.compile()