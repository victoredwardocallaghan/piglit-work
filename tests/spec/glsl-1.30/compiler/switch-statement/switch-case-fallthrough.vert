// [config]
// expect_result: pass
// glsl_version: 1.30
// [end config]
//
// From page 57 (page 63 of the PDF) of the GLSL 1.30 spec:
//
//     "The type of init-expression in a switch statement must be a scalar
//     integer.  If a case label has a constant- expression of equal value,
//     then execution will continue after that label...Fall through labels are
//     allowed, but it is an error to have no statement between a label and
//     the end of the switch statement."
//
// The spec doesn't actually say anything about the type of cases.  It only
// says "constant-expression".

#version 130

void main() {
   int tmp = 0;
   switch (1) {
   case 1:
      tmp = 1;
   default:
      tmp = 0;
   }

   gl_Position = vec4(0.0);
}
