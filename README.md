# 🛠 Assembler for Custom Assembly Language

## 📌 Overview

This is an assembler for a custom assembly language that translates assembly code into machine code. The assembler consists of several modules that work together to produce machine code files from assembly language files.

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

## 🙏 Acknowledgments

This project was done as part of a school assignment at The Open University of Israel.

## 📜 License

This project is licensed under the MIT License.


