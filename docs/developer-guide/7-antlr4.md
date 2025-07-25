# ANTLR4 grammar
Some of the code uses ANTLR generated code to parse expressions at runtime. The grammar files are located in
* src/expressions/antlr-interface/Expr.g4
* src/libs/antares/additionalConstraintRhsExpression/HoursField.g4


## How to generate code from the grammar
[ANTLR4 4.13.2](https://www.antlr.org/download/antlr-4.13.2-complete.jar) is used to generate the code. Note that using a different version results in a different code, possibly incompatible with antlr4-runtime.

You may use the convenience CMake target `antlr_generate_all` to re-generate from *all* grammars.

```
cmake -B <build> -DANTLR_JAR_PATH=path/to/antlr.jar [other options]
cmake --build <build> --target antlr_generate_all
```

See [here](https://github.com/antlr/antlr4/blob/master/doc/getting-started.md) for some general documentation about generating C++ code from an ANTLR4 grammar.

## Formatting
Please *DO NOT* format the generated code.

## Checksum
For reference, the sha256sum for file antlr-4.13.2-complete.jar is

```
eae2dfa119a64327444672aff63e9ec35a20180dc5b8090b7a6ab85125df4d76  antlr-4.13.2-complete.jar
```
Since ANTLR maintainers may choose to re-release a version without a warning, please check this control sum if you notice any difference in the generated code.
