%module spotless

%{
  //Function exposed from C
  extern int verify_json(char *s);
%}

%typemap(out) int verify_json {
  $result = PyBool_FromLong($1);
}

// Function exposed to Python
extern int verify_json(char *s);
