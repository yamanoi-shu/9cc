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
  int len; // トークンの長さ
};

char *user_input;

Token *token;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


// tokenが記号の場合にtokenを次のトークンへ進めて、trueを返す
// それ以外はfalseを返す
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    return false;
  }
  token = token->next;
  return true;
}

// tokenが記号の場合にtokenを次のトークンへ進めて、trueを返す
// それ以外はerror()を呼ぶ
void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    error_at(token->str, "'%c'ではありません", op);
  }
  token = token->next;
}

// tokenが数値の場合にtokenを次のトークンへ進めて、その数値を返す
// それ以外はerror()を呼ぶ
int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません。");
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
bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを生成してcurの次に連結する
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (cmpMultiOpe(p, "==") || cmpMultiOpe(p, "!=") ||
        cmpMultiOpe(p, ">=") || cmpMultiOpe(p, "<=")) {
      cur =  new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (strchr("+-*/()<>", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
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

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM,
  ND_EQ, // ==
  ND_NE, // !=
  ND_LT, // <
  ND_LE, // <=
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind; // Nodeの種類
  Node *lhs; // 左側のノード
  Node *rhs; // 右側のノード
  int val; // ND_NUMの場合の値
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr() {
  return equality();
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node  *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<="))  {
      node = new_node(ND_LE, node, add());
    } else if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume(">=")) {
      node = new_node(ND_LE, add(), node);
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {
  if (consume("+")) {
    return unary();
  }
  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), unary());
  }
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  } 

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  sete al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  setne al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  setl al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  setle al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];
  // トークナイズする
  token = tokenize();
  // パースする
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
}
