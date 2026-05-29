# Command-line instructions

**Executable**: antares-modeler

The modeler takes one amndatory argument: the path to the [study directory](02-inputs.md#study-structure).

The other option **--parquet** is optional, it tells to print simulation table (= simulation results) in format parquet.
If this option is not given, simulation table is printed in csv format.
In both cases, simulation table is printed into simulation's output folder.

Example for Windows users:
~~~
antares-modeler.exe C:\path\to\my\study [--parquet]
~~~

For Linux users:
~~~
antares-modeler /path/to/my/study [--parquet]
~~~
