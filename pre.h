/* expand_macros: the function gets a file name and a file pointer. It creats a
	a new new-file - identical to the original file which its name passed, but
	with macro-defenitions expanded. After the file created it points the given
	file pointer to the new file, with reading permission. For its operatin, the
	function usesstwo FILE pointers: the input pointer for reading, and another
	for writing. It examines each line separately, and places macros contents
	instead of their names into the new file. */
void expand_macros(char *file_name, FILE **read);
