This A, B example demonstrates the problem with naming conflicts between 
classes. Were we to make our traces thread all events temporally, we would be
able to infer G("this.x == 3" -> XF"this.x == 4"), which is false; A's x is only
ever 0 or 3 and B's x is only ever 0 or 4. The x related in the data-temporal
property is not the same x, and again, this would bring about false 
interpretation problems. 

Our current solution is to separate traces by object; this potentially breaks
some temporality. The other option would be to rename variables with their
object name.



