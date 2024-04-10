# Output files

1. The outputs of the Antares time-series analyzer are not printed in the general output files but kept within
   the input files structure (the reason being that they are input data for the Antares time - series generators).
   The associated data may nonetheless be accessed to be reviewed, updated and deleted at any time through the GUI.
4. The times-series analyzer requires the use of a temporary directory in which intermediate files are created
   in the course of the analysis and deleted in the end, unless the user wishes to keep them for further examination.
   Its location is user-defined and should usually be the "user" subfolder if files are to be kept, otherwise any
   proper temporary space such as `C..../Temp`.