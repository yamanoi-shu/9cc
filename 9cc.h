#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT, // 識別子
  TK_NUM, // 数字
  TK_EOF, // 入力の終わり
} TokenKind;


typedef struct Token Token;

struct Token {
  TokenKind kind; // トークンの型
  Token *next; // 次のトークン
  int val; // kindがTK_NUMの場合、その数値
  char *loc; // トークン文字列
  int len; // トークンの長さ
};

void error_at(char *loc, char *fmt, ...);
void error_token(Token *token, char *fmt, ...);
void verror_at(char *loc, char *fmt, va_list ap);

bool equal(Token *token, char *op);

void expect(Token *token, char *op);

bool cmpMultiOpe(char *p, char *q);

bool at_eof(Token *token);

Token *new_token(TokenKind kind, Token *cur, char *str, int len);

Token *tokenize();


typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_ASSIGN, // =
  ND_LVAR, // ローカル変数
  ND_NUM, // 整数
  ND_EQ, // ==
  ND_NE, // !=
  ND_LT, // <
  ND_LE, // <=
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind; // Nodeの種類
  Node *next;
  Node *lhs; // 左側のノード
  Node *rhs; // 右側のノード
  int val; // ND_NUMの場合の値
  int offset; // ND_LVARの場合のベースポインタからのオフセット
};

Node *new_node(NodeKind  kind, Node *lhs, Node *rhs);

Node *new_node_num(int val);

Node *parse(Token *token);

Node *program(Token **rest, Token *token);
Node *stmt(Token **rest, Token *token);
Node *expr(Token **rest, Token *token);
Node *assign(Token **rest, Token *token);
Node *equality(Token **rest, Token *token);
Node *relational(Token **rest, Token *token);
Node *add(Token **rest, Token *token);
Node *mul(Token **rest, Token *token);
Node *unary(Token **rest, Token *token);
Node *primary(Token **rest, Token *token);

void gen(Node *node);
