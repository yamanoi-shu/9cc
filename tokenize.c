#include "9cc.h"

static char *current_input;

void verror_at(char *loc, char *fmt, va_list ap) {
  int pos = loc - current_input;
  fprintf(stderr, "%s\n", current_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_token(Token *token, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  error_at(token->loc, fmt, ap);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(loc, fmt, ap);
}

// alnum is a-z && A-Z && 0-9
int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

// tokenが記号の場合にtokenを次のトークンへ進めて、trueを返す
// それ以外はfalseを返す
bool equal(Token *token, char *op) {
  return memcmp(token->loc, op, token->len) == 0 && strlen(op) == token->len;
}

// tokenが記号の場合にtokenを次のトークンへ進めて、trueを返す
// それ以外はerror()を呼ぶ
void expect(Token *token, char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->loc, op, token->len)) {
    error_at(token->loc, "'%c'ではありません", op);
  }
}

// tokenが数値の場合にtokenを次のトークンへ進めて、その数値を返す
// それ以外はerror()を呼ぶ
int expect_number(Token *token) {
  if (token->kind != TK_NUM) {
    error_at(token->loc, "数ではありません。");
  }
  int val = token->val;
  token = token->next;
  token->val = val;
  return val;
}

bool cmpMultiOpe(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

//tokenが入力の終わりかを返す
bool at_eof(Token *token) {
  return token->kind == TK_EOF;
}

// 新しいトークンを生成してcurの次に連結する
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->loc = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  current_input = p;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p) {
   if (isspace(*p)) {
      p++;
      continue;
    }

    // strncmp(a, b, n) aとbを先頭からn文字比較する
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (cmpMultiOpe(p, "==") || cmpMultiOpe(p, "!=") ||
        cmpMultiOpe(p, ">=") || cmpMultiOpe(p, "<=")) {
      cur =  new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (strchr("+-*/()<>;=", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      int len = 0;
      char *buf = p;
      while (!strchr("+-*/()<>;=", *p) && ('a' <= *p && *p <= 'z')) {
        len++;
        p++;
      }
      cur = new_token(TK_IDENT, cur, buf, len);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  
  return head.next;
}
