#!/usr/bin/env bats

assert_ok() {
    printf '%s\n' "$1" | ./spotless
}

assert_fail() {
    ! assert_ok "$1"
}

@test "true keyword"        { assert_ok 'true'; }
@test "false keyword"       { assert_ok 'false'; }
@test "null keyword"        { assert_ok 'null'; }
@test "number '0'"          { assert_ok '0'; }
@test "number '-0'"         { assert_fail '-0'; }
@test "number '1'"          { assert_ok '1'; }
@test "number '2'"          { assert_ok '2'; }
@test "number '3'"          { assert_ok '3'; }
@test "number '4'"          { assert_ok '4'; }
@test "number '5'"          { assert_ok '5'; }
@test "number '6'"          { assert_ok '6'; }
@test "number '7'"          { assert_ok '7'; }
@test "number '8'"          { assert_ok '8'; }
@test "number '9'"          { assert_ok '9'; }
@test "number '12'"         { assert_ok '12'; }
@test "number '123'"        { assert_ok '123'; }
@test "number '1234'"       { assert_ok '1234'; }
@test "number '12345'"      { assert_ok '12345'; }
@test "number '123456'"     { assert_ok '123456'; }
@test "number '1234567'"    { assert_ok '1234567'; }
@test "number '-1'"         { assert_ok '-1'; }
@test "number '-2'"         { assert_ok '-2'; }
@test "number '-3'"         { assert_ok '-3'; }
@test "number '-4'"         { assert_ok '-4'; }
@test "number '-5'"         { assert_ok '-5'; }
@test "number '-6'"         { assert_ok '-6'; }
@test "number '-7'"         { assert_ok '-7'; }
@test "number '-8'"         { assert_ok '-8'; }
@test "number '-9'"         { assert_ok '-9'; }
@test "number '-12'"        { assert_ok '-12'; }
@test "number '-123'"       { assert_ok '-123'; }
@test "number '-1234'"      { assert_ok '-1234'; }
@test "number '-12345'"     { assert_ok '-12345'; }
@test "number '-123456'"    { assert_ok '-123456'; }
@test "number '-1234567'"   { assert_ok '-1234567'; }
@test "number '1.0'"        { assert_ok '1.0'; }
@test "number '1.01'"       { assert_ok '1.01'; }
@test "number '1.012345'"   { assert_ok '1.012345'; }
@test "number '1.012345'"   { assert_ok '1.012345'; }
@test "number '-1.0'"       { assert_ok '-1.0'; }
@test "number '-1.01'"      { assert_ok '-1.01'; }
@test "number '-1.012345'"  { assert_ok '-1.012345'; }
@test "number '-1.012345'"  { assert_ok '-1.012345'; }
@test "number '1e10'"       { assert_ok '1e10'; }
@test "number '1E10'"       { assert_ok '1E10'; }
@test "number '1e+10'"      { assert_ok '1e+10'; }
@test "number '1E+10'"      { assert_ok '1E+10'; }
@test "number '1e-10'"      { assert_ok '1e-10'; }
@test "number '1E-10'"      { assert_ok '1E-10'; }
@test "number '1.1234e10'"  { assert_ok '1.1234e10'; }
@test "number '1.1234E10'"  { assert_ok '1.1234E10'; }
@test "number '1.1234e+10'" { assert_ok '1.1234e+10'; }
@test "number '1.1234E+10'" { assert_ok '1.1234E+10'; }
@test "number '1.1234e-10'" { assert_ok '1.1234e-10'; }
@test "number '1.1234E-10'" { assert_ok '1.1234E-10'; }
@test "number '-0.2'"       { assert_ok '-0.2'; }
@test "number '-0.2e-3'"    { assert_ok '-0.2e-3'; }
@test "number '-0.2e+3'"    { assert_ok '-0.2e+3'; }
@test "number '-0.2e3'"     { assert_ok '-0.2e3'; }
@test "number '12.'"        { assert_fail '12.'; }
@test "number '.12'"        { assert_fail '.12'; }
@test "number '1e'"         { assert_fail '1e'; }
@test "number '1e-'"        { assert_fail '1e-'; }
@test "number '1e+'"        { assert_fail '1e+'; }
@test 'string ""'           { assert_ok '""'; }
@test 'string "abc"'        { assert_ok '"abc"'; }
@test 'string "abc def"'    { assert_ok '"abc def"'; }
@test 'string "jérôme"'     { assert_ok '"jérôme"'; }
@test 'string "abc'         { assert_fail '"abc'; }
@test 'string abc"'         { assert_fail 'abc"'; }
@test 'string escape quotes' {
    assert_ok '" \"quoted\" "'
}
@test 'string escapes'      { assert_ok '" \\ \/ \b \f \n \r \t "'; }
@test 'string escapes (invalid)' { assert_fail '" \v \y \x "'; }
@test 'string escapes unicode' { assert_ok '" \u0123 \u4567 \u89ab \uCDEF "'; }
@test 'array empty'         { assert_ok '[]'; }
@test 'array 1'             { assert_ok '[1]'; }
@test 'array 2'             { assert_ok '[1,2]'; }
@test 'array 3'             { assert_ok '[1,2,"abc"]'; }
@test 'array 4'             { assert_ok '[1,2,"abc",[]]'; }
@test 'array 5'             { assert_ok '[1,2,"abc",[], [2,3,5]]'; }
@test 'object empty'        { assert_ok '{}'; }
@test 'object 1'            { assert_ok '{"x": 1}'; }
@test 'object 2'            { assert_ok '{"x": 1, "y": 2}'; }
@test 'object 3' {
    assert_ok '{ "x": 1, "y": 2, "inner": {"z": 3} }';
}
@test 'object 4' {
    assert_ok '{ "x": 1, "y": 2, "inner": {"z": 3}, "array": [2, 3, 5] }';
}
