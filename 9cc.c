#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>


typedef enum {
  TK_RESERVED, // 記号
  TK_NUM, // 数字
  TK_EOF, // 入力の終わり
} TokenKind;


typedef struct Token Token;

struct Token {
  TokenKind kind; // トークンの型
  Token *next; // 次のトークン
  int val; // kindがTK_NUMの場合、その数値
  char *str; // トークン文字列
};

Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  exit(1);
}


// tokenが記号の場合にtokenを次のトークンへ進めて、trueを返す
// それ以外はfalseを返す
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    return false;
  }
  token = token->next;
  return true;
}

// tokenが記号の場合にtokenを次のトークンへ進めて、trueを返す
// それ以外はerror()を呼ぶ
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    error("'%c'ではありません", op);
  }
  token = token->next;
}

// tokenが数値の場合にtokenを次のトークンへ進めて、その数値を返す
// それ以外はerror()を呼ぶ
int expect_number() {
  if (token->kind != TK_NUM) {
    error("数ではありません。");
  }
  int val = token->val;
  token = token->next;
  token->val = val;
  return val;
}

//tokenが入力の終わりかを返す
bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを生成してcurの次に連結する
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // トークナイズする
  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", expect_number());

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
    }
    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}
