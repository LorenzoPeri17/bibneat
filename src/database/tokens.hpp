#pragma once

enum Token{
// EOF
TOK_EOF = -1,

// Error Handling
TOK_ERROR = 101,

// keep parsing
TOK_KEEP_PARSING=0,

// String
TOK_STR=1,

// entries
TOK_NEW_ENTRY=2,
TOK_END_ENTRY=-2,
TOK_KEY=5,

// new attribute
TOK_NEW_ATTR=3,
TOK_END_ATTR=-3,
TOK_ONEWORD_ATTR=-4,

// Dealing with LaTeX nonsense
TOK_MATH=7,
TOK_ESCAPE=9,

};