This tool the names all local method variables in a Java
class' method using the format namex_oldname. Original 
names can thus be retrieved by stripping the text before
(inclusive) the first underscore in the current name.

Suggested Compilation:
$ javac -cp ./jar/javaparser-core-2.0.1-SNAPSHOT.jar -d ./bin src/Main.java src/VariableRenamer.java

Suggested Use: 
$ java -cp "./jar/javaparser-core-2.0.1-SNAPSHOT.jar;./bin" Main /input/file/location > /output/file/location