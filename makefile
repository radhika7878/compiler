all:
	$(CC) driver.c lexer.c parser.c semantics.c codegen.c -o toycompiler