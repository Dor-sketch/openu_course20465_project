# 🛠 Assembler for Custom Assembly Language

## 📌 Overview

This endeavor involved crafting an assembler tailored for a specialized assembly language. The primary objective was to transform assembly instructions, which are readable by humans, into binary machine code executable by computers. This tool serves as a pivotal resource for comprehending the connection between abstract programming concepts and their concrete low-level machine code executions.

The project is implemented in `ANSI C`, demonstrating a strong grasp of foundational programming principles. It was completed as part of the curriculum for the `20465 System Programming Laboratory` course at _The Open University of Israel_, where it achieved a grade of `98`.


---

![image](https://github.com/Dor-sketch/openu_course20465_project/assets/138825033/2b722882-b363-491c-9e1f-db706b2da88a)


## 🧩 Modules

The assembler consists of the following modules:

- `📝 pre.c`: This module handles preprocessing of the assembly language file, including macro expansion and line numbering.
- `🔎 syntax.c`: This module handles syntax checking of the assembly language file, including checking for valid opcodes and operands.
- `🚦 first_pass.c`: This module performs the first pass of the assembly process, which generates a symbol table and calculates the memory addresses of labels.
- `🚀 second_pass.c`: This module performs the second pass of the assembly process, which generates the machine code and data images.
- `🖨️ print_output.c`: This module handles printing the output files, including the machine code file, external data words file, and entry type symbols file.
- `🏁 main.c`: This module is the entry point for the assembler and coordinates the other modules to produce the output files.

## 🤖 Usage

To use the assembler, you will need to provide it with an input file containing assembly code. The assembler will output several files, including a machine code file, an external data words file, and an entry type symbols file.

```bash
$ ./assembler {input - without .as extension}
```

## 👥 Contributing

If you would like to contribute to the project, you can fork the repository and submit a pull request with your changes. Please make sure that your changes are well-tested and adhere to the project's coding standards.

## 📜 License

This project is licensed under the MIT License.


